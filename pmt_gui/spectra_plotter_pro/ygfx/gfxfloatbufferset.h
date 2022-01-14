#ifndef GFXFBUFFERSET_H
#define GFXFBUFFERSET_H

#include "gfxvector.h"
#include "gfxincludes.h"
#include <unordered_map>
#include <QOpenGLVersionFunctionsFactory>

namespace ygfx {
    struct FloatBufferRule {
        enum FloatBufferRuleType {
            POSITION_DATA,
            NORMAL_DATA,
            COLOR_DATA,
            TEXTURE_DATA,
            DEFAULT = POSITION_DATA
        };

        // FloatBuffer example [P1X P1Y C1R C1G C1B | P2X P2Y C2R C2G C2B]
        // --> rule 1 : length 2 | stride 5 || offset 0
        // --> rule 2 : length 3 | stride 5 || offset 2

        GLint data_set_length = 0;
        GLint data_stride = 0;
        GLint data_offset = 0;
        GLuint target_location = 0;
        FloatBufferRuleType data_type = DEFAULT;

        explicit FloatBufferRule(GLint length, GLint stride, GLint offset, GLuint target = 0) :
            data_set_length(length), data_stride(stride), data_offset(offset), target_location(target) {}
    };

    class FloatBufferSet {
    private:
        bool internal_attribute_added = false;
        bool internal_vao_created = false;
        bool has_break_index = false;
        GLuint vertexarrayobject = 0;
        GLushort break_index = 0;
        QOpenGLContext * context_ptr = nullptr;
        std::vector<std::pair<std::vector<FloatBufferRule>, gfxVector<GLfloat> *>> floatbufferdata;
        gfxVector<GLushort> * indexbuffer = nullptr;

        std::vector<size_t> capacities;
        size_t idx_capacity;

    private:
        void recreateVertexArrayObject();

    public:
        void draw(GLenum mode, GLint start, GLsizei count);
        void draw(GLenum mode);

        explicit FloatBufferSet();

        void addFloatBuffer(std::vector<FloatBufferRule> rules, gfxVector<GLfloat> * floatbuffer);
        void addFloatBuffer(FloatBufferRule rule, gfxVector<GLfloat> * floatbuffer);
        void setIndexBuffer(gfxVector<GLushort> * indexbuffer);
        void setBreakIndexForIndexBuffer(GLushort index);
        void removeBreakIndexForIndexBuffer();
        bool hasIndexBuffer() { return indexbuffer != nullptr; }
    };
}

#endif // GFXFBUFFERSET_H
