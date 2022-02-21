#ifndef YGFX_GL_ARRAY_H
#define YGFX_GL_ARRAY_H

// ygfx-includes
#include "gfxarraybasecontainer.h"

// gl-includes
#include "gfxincludes.h"

// stl-includes
#include <array>
#include <sstream>

namespace ygfx {
    template<typename T, GLuint N, 
        BufferTarget TARGET = BufferTarget::ARRAY_BUFFER,
        BufferDataUsage USAGE = BufferDataUsage::DYNAMIC_DRAW>
    class gfxArray : public gfxArrayBasedContainer<T, TARGET, USAGE> {
    public:
        explicit gfxArray() :
            gfxArrayBasedContainer<T, TARGET, USAGE>(N)
        {
            this->buffercapacity = N;
        }

        gfxArray(const std::array<T, N>& stlarray) :
            gfxArray<T, N, TARGET>()
        {
            for (int i = 0; i < N; ++i) {
                if (i < stlarray.size()) (*this)[i] = stlarray[i];
            }
        }
    };


    template<typename T, GLuint N, BufferTarget TGT, BufferDataUsage USE>
    std::ostream& operator << (std::ostream& ostr, gfxArray<T, N, TGT, USE>& arr) {
        ostr << "[gfxArray|size:" << arr.gfxSize() << "|vbo:" << arr.getVBO() << "]" << Qt::endl;
        ostr << "[TARGET: " << TGT << "|USAGE: " << USE << Qt::endl;
        ostr << "{";
        auto size = arr.gfxSize() / sizeof(T);
        for (GLuint i = 0; i < size; ++i) {
            ostr << arr[i];
            if (i < size - 1) ostr << ", ";
        }
        ostr << "}";
        return ostr;
    }

#ifdef QT_CORE_LIB
    template<typename T, GLuint N, BufferTarget TGT, BufferDataUsage USE>
    QDebug operator << (QDebug ostr, gfxArray<T, N, TGT, USE>& arr) {
        ostr.noquote().nospace() << "[gfxArray|size:" << arr.gfxSize() << "|vbo:" << arr.getVBO() << "]";
        ostr.noquote().nospace() << "[TARGET: " << TGT << "|USAGE: " << USE;
        ostr << Qt::endl;
        ostr.noquote().nospace() << "{";
        auto size = arr.gfxSize() / sizeof(T);
        for (GLuint i = 0; i < size; ++i) {
            ostr.noquote().nospace() << arr[i];
            if (i % 15 == 0 && i > 0) ostr << Qt::endl;
            if (i < size - 1) ostr.noquote().nospace() << ", ";
        }
        ostr.noquote().nospace() << "}";
        return ostr;
    }
#endif
}

#endif
