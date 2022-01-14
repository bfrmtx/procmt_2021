#ifndef YGFX_GL_ARRAYBASE_H
#define YGFX_GL_ARRAYBASE_H

// stl-includes
#include <iostream>

// gl-includes
#include "gfxincludes.h"

namespace ygfx {
    enum class BufferDataUsage {
        STREAM_DRAW = GL_STREAM_DRAW,
        STREAM_READ = GL_STREAM_READ,
        STREAM_COPY = GL_STREAM_COPY,
        STATIC_DRAW = GL_STATIC_DRAW,
        STATIC_READ = GL_STATIC_READ,
        STATIC_COPY = GL_STATIC_COPY,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        DYNAMIC_READ = GL_DYNAMIC_READ,
        DYNAMIC_COPY = GL_DYNAMIC_COPY
    };

    // spc -> kann auskommentiert werden
    enum class BufferTarget {
        ARRAY_BUFFER = GL_ARRAY_BUFFER,
        //ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER, // spc
        COPY_READ_BUFFER = GL_COPY_READ_BUFFER, // spc
        COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER, // spc
        //DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER, // spc
        DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER, // spc
        ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
        PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER, // spc
        PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER, // spc
        //QUERY_BUFFER = GL_QUERY_BUFFER, // spc
        //SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER, // spc
        TEXTURE_BUFFER = GL_TEXTURE_BUFFER, // spc
        TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER, // spc
        UNIFORM_BUFFER = GL_UNIFORM_BUFFER
    };

    template<typename T,
             BufferTarget TARGET = BufferTarget::ARRAY_BUFFER,
             BufferDataUsage USAGE = BufferDataUsage::DYNAMIC_DRAW>
    class gfxArrayBasedContainer {
    protected:
        T* datapointer;
        bool isMapped;
        GLuint vertexbufferID;
        GLuint buffercapacity;

    protected:        
        ~gfxArrayBasedContainer() {
            this->enableMapping(false);
            OGL_IF_CONTEXT {
                OGL_IF_FUNCS {
                    OGLFUNCS glDeleteBuffers(1, &this->vertexbufferID);
                }
            }
        }

        gfxArrayBasedContainer(GLuint buffercap) :
            datapointer(nullptr),
            isMapped(false),
            vertexbufferID(0),
            buffercapacity(0)
        {
            initGfxBuffer(buffercap);
        }

        void initGfxBuffer(GLuint capacity) {
            OGL_IF_CONTEXT {
                OGL_IF_FUNCS {
                    OGLFUNCS glGenBuffers(1, &vertexbufferID);
                    OGLFUNCS glBindBuffer(static_cast<GLenum>(TARGET), vertexbufferID);
                    OGLFUNCS glBufferData(static_cast<GLenum>(TARGET), sizeof(T) * capacity, nullptr, static_cast<GLenum>(USAGE));
                    buffercapacity = capacity;
                } else {
                    GFX_OUT << "initGfxBuffer(" << capacity << ") failed, couldn't find opengl-functions for current opengl context" << Y_ENDL;
                }
            } else {
                GFX_OUT << "initGfxBuffer(" << capacity << ") failed, couldn't find current opengl context" << Y_ENDL;
            }
        }

    public:
        void enableMapping(bool value) {
            OGL_IF_CONTEXT {
                OGL_IF_FUNCS {
                    if (isMapped != value) {
                        isMapped = value;
                        if (isMapped) {
                            OGLFUNCS glBindBuffer(static_cast<GLenum>(TARGET), vertexbufferID);
                            datapointer = static_cast<T*>(
                                OGLFUNCS glMapBufferRange(static_cast<GLenum>(TARGET), 0,
                                    buffercapacity * sizeof(T), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT));
                        } else {
                            OGLFUNCS glBindBuffer(static_cast<GLenum>(TARGET), vertexbufferID);
                            datapointer = nullptr;
                            OGLFUNCS glUnmapBuffer(static_cast<GLenum>(TARGET));
                        }
                    }
                }
            }
        }

        T& at(GLuint position) {
            if (position >= buffercapacity) throw std::out_of_range("index out of range");
            return (*this)[position];
        }

        T& operator[](GLuint position) {
            Q_ASSERT(position < buffercapacity);
            enableMapping(true);
            return *(datapointer + position);
        }

        T operator[](GLuint position) const {
            Q_ASSERT(position < buffercapacity);
            if (isMapped) return datapointer[position];
            OGLFUNCS glBindBuffer(static_cast<GLenum>(TARGET), vertexbufferID);
            T* tmpptr = static_cast<T*>(OGLFUNCS glMapBufferRange(static_cast<GLenum>(TARGET), 0,
                buffercapacity * sizeof(T), GL_MAP_READ_BIT));
            T ret = tmpptr[position];
            OGLFUNCS glUnmapBuffer(static_cast<GLenum>(TARGET));
            return ret;
        }

        GLuint gfxSize() {
            GLint ret;
            enableMapping(false);
            OGLFUNCS glBindBuffer(static_cast<GLenum>(TARGET), vertexbufferID);
            OGLFUNCS glGetBufferParameteriv(static_cast<GLenum>(TARGET), GL_BUFFER_SIZE, &ret);
            return static_cast<GLuint>(ret);
        }

        GLuint getVBO() const {
            return vertexbufferID;
        }

        GLuint capacity() const {
            return buffercapacity;
        }

    };

#ifdef QT_CORE_LIB
    QDebug& operator<< (QDebug& ostr, const BufferDataUsage vartype);
    QDebug& operator<< (QDebug& ostr, const BufferTarget vartype);
#endif
    std::ostream& operator<< (std::ostream& ostr, const BufferTarget vartype);
    std::ostream& operator<< (std::ostream& ostr, const BufferDataUsage vartype);
}

#endif
