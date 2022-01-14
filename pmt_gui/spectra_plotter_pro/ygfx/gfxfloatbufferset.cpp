#include "gfxfloatbufferset.h"
#include <QOpenGLFunctions_3_1>

using namespace ygfx;

void FloatBufferSet::recreateVertexArrayObject() {
    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
#ifdef OGLDEBUGENABLED
            std::cout << "recreating VertexArrayObject..." << std::endl;
#endif
            OGLFUNCS glGenVertexArrays(1, &vertexarrayobject);
            OGLFUNCS glBindVertexArray(vertexarrayobject);
            for(auto & iterator : floatbufferdata) {
                auto & fbuffer = iterator.second;
                auto & rules = iterator.first;
                OGLFUNCS glBindBuffer(GL_ARRAY_BUFFER, fbuffer->getVBO());
                for(auto & rule : rules) {
                    size_t offset = size_t(rule.data_offset * GLint(sizeof(GLfloat)));
                    auto stride = rule.data_stride * GLint(sizeof(GLfloat));

#ifdef OGLDEBUGENABLED
                    std::cout << "glVertexAttribPointer(" << rule.target_location << ", " << rule.data_set_length
                              << ", GL_FLOAT, GL_FALSE, " << stride << ", " << offset << ")" << std::endl;
#endif

                    OGLFUNCS glVertexAttribPointer(rule.target_location, rule.data_set_length, GL_FLOAT, GL_FALSE,
                                                   stride, reinterpret_cast<void*>(offset));
                    OGLFUNCS glEnableVertexAttribArray(rule.target_location);
                    OGLFUNCS glBindBuffer(GL_ARRAY_BUFFER, 0);
                }
            }
            if(indexbuffer != nullptr) {
#ifdef OGLDEBUGENABLED
                std::cout << "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, " << indexbuffer->getVBO() << ")" << std::endl;
#endif
                OGLFUNCS glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer->getVBO());
            }
            OGLFUNCS glBindBuffer(GL_ARRAY_BUFFER, 0);
            OGLFUNCS glBindVertexArray(0);
            internal_vao_created = true;
            internal_attribute_added = false;
            context_ptr = QOpenGLContext::currentContext();
        }
    }
}

void FloatBufferSet::draw(GLenum mode) {
    OGL_IF_CONTEXT {
        if(QOpenGLContext::currentContext() != context_ptr || internal_attribute_added) {
#ifdef OGLDEBUGENABLED
            std::cout << "current-ctx : " << QOpenGLContext::currentContext() << std::endl;
            std::cout << "ctx-ptr     : " << context_ptr << std::endl;
            std::cout << "attr-added  : " << internal_attribute_added << std::endl;
#endif
            internal_vao_created = false;
            recreateVertexArrayObject();
        }

        if(internal_vao_created) {
            for(auto & data : floatbufferdata) {
                data.second->enableMapping(false);
            }

            OGLFUNCS glBindVertexArray(vertexarrayobject);
            OGLFUNCS glDrawArrays(mode, 0, 12);
            OGLFUNCS glBindVertexArray(0);
        }
    }
}

FloatBufferSet::FloatBufferSet() {

}

void FloatBufferSet::addFloatBuffer(std::vector<FloatBufferRule> rules, gfxVector<GLfloat> * floatbuffer) {
    floatbufferdata.push_back(std::make_pair(rules, floatbuffer));
    capacities.push_back(floatbuffer->capacity());
    internal_attribute_added = true;
}

void FloatBufferSet::addFloatBuffer(FloatBufferRule rule, gfxVector<GLfloat> * floatbuffer) {
    std::vector<FloatBufferRule> rules; rules.push_back(rule); rules.shrink_to_fit();
    addFloatBuffer(rules, floatbuffer);
}

void FloatBufferSet::setIndexBuffer(gfxVector<GLushort> * indexbuffer) {
    this->indexbuffer = indexbuffer;
    internal_attribute_added = true;
}

void FloatBufferSet::setBreakIndexForIndexBuffer(GLushort index) {
    has_break_index = true;
    break_index = index;
}

void FloatBufferSet::removeBreakIndexForIndexBuffer() {
    has_break_index = false;
}

void FloatBufferSet::draw(GLenum mode, GLint start, GLsizei count) {
    OGL_IF_CONTEXT {
        if(QOpenGLContext::currentContext() != context_ptr || internal_attribute_added) {
#ifdef OGLDEBUGENABLED
            std::cout << "current-ctx : " << QOpenGLContext::currentContext() << std::endl;
            std::cout << "ctx-ptr     : " << context_ptr << std::endl;
            std::cout << "attr-added  : " << internal_attribute_added << std::endl;
#endif
            internal_vao_created = false;
            recreateVertexArrayObject();
        }
        bool force_recreation = false;
        for(size_t i = 0; i < floatbufferdata.size(); ++i) {
            if(floatbufferdata[i].second->capacity() != capacities[i] || (hasIndexBuffer() && indexbuffer->capacity() != idx_capacity)) {
                if(hasIndexBuffer()) {
                    idx_capacity = indexbuffer->capacity();
                }
                internal_vao_created = false;
                capacities[i] = floatbufferdata[i].second->capacity();
                force_recreation = true;
            }
        }
        if(force_recreation) {
#ifdef OGLDEBUGENABLED
            std::cout << "vertexbuffer capacity changed -> need a re-creation of VertexArrayObject" << std::endl;
#endif
            recreateVertexArrayObject();
        }

        if(internal_vao_created) {
            for(auto & data : floatbufferdata) {
                data.second->enableMapping(false);
            }
            if(indexbuffer != nullptr) indexbuffer->enableMapping(false);

            OGLFUNCS glBindVertexArray(vertexarrayobject);
            if(indexbuffer != nullptr) {
                // Qt 6
                // auto funcs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_1>();
                auto funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_1>();
                if(has_break_index) {
                    funcs->glEnable(GL_PRIMITIVE_RESTART);
                    funcs->glPrimitiveRestartIndex(GLuint(break_index));
                }
                OGLFUNCS glDrawElements(mode, count, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid const *>(size_t(start)));
                if(has_break_index) {
                    funcs->glDisable(GL_PRIMITIVE_RESTART);
                }
            } else {
                OGLFUNCS glDrawArrays(mode, start, count);
                OGLFUNCS glBindVertexArray(0);
            }
        }
    }
}
