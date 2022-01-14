#include "gfxarraybasecontainer.h"
#include "sstream"

#define Y_GLVAROSTR(outp) case outp: {ostr << #outp; break; }

namespace ygfx {
#ifdef QT_CORE_LIB
    QDebug& operator<< (QDebug& ostr, const BufferTarget vartype) {
        std::stringstream test; test << vartype; ostr << QString::fromStdString(test.str());
        return ostr;
    }

    QDebug& operator<< (QDebug& ostr, const BufferDataUsage vartype) {
        std::stringstream test; test << vartype; ostr << QString::fromStdString(test.str());
        return ostr;
    }
#endif

    std::ostream& operator<< (std::ostream& ostr, const BufferTarget vartype) {
        switch (vartype) {
            Y_GLVAROSTR(BufferTarget::ARRAY_BUFFER)
            //Y_GLVAROSTR(BufferTarget::ATOMIC_COUNTER_BUFFER)
            Y_GLVAROSTR(BufferTarget::COPY_READ_BUFFER)
            Y_GLVAROSTR(BufferTarget::COPY_WRITE_BUFFER)
            //Y_GLVAROSTR(BufferTarget::DISPATCH_INDIRECT_BUFFER)
            Y_GLVAROSTR(BufferTarget::DRAW_INDIRECT_BUFFER)
            Y_GLVAROSTR(BufferTarget::ELEMENT_ARRAY_BUFFER)
            Y_GLVAROSTR(BufferTarget::PIXEL_PACK_BUFFER)
            Y_GLVAROSTR(BufferTarget::PIXEL_UNPACK_BUFFER)
            //Y_GLVAROSTR(BufferTarget::QUERY_BUFFER)
            //Y_GLVAROSTR(BufferTarget::SHADER_STORAGE_BUFFER)
            Y_GLVAROSTR(BufferTarget::TEXTURE_BUFFER)
            Y_GLVAROSTR(BufferTarget::TRANSFORM_FEEDBACK_BUFFER)
            Y_GLVAROSTR(BufferTarget::UNIFORM_BUFFER)
        }
        return ostr;
    }

    std::ostream& operator<< (std::ostream& ostr, const BufferDataUsage vartype) {
        switch (vartype) {
            Y_GLVAROSTR(BufferDataUsage::STREAM_DRAW)
            Y_GLVAROSTR(BufferDataUsage::STREAM_READ)
            Y_GLVAROSTR(BufferDataUsage::STREAM_COPY)
            Y_GLVAROSTR(BufferDataUsage::STATIC_DRAW)
            Y_GLVAROSTR(BufferDataUsage::STATIC_READ)
            Y_GLVAROSTR(BufferDataUsage::STATIC_COPY)
            Y_GLVAROSTR(BufferDataUsage::DYNAMIC_DRAW)
            Y_GLVAROSTR(BufferDataUsage::DYNAMIC_READ)
            Y_GLVAROSTR(BufferDataUsage::DYNAMIC_COPY)
        }
        return ostr;
    }
}

#undef Y_GLVAROSTR

