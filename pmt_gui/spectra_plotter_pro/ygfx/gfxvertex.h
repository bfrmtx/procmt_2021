#ifndef YASC_GL_VERTEX_H
#define YASC_GL_VERTEX_H

// gl/glm-includes
#include "gfxincludes.h"
#include <glm/glm.hpp>

// stl-includes
#include <type_traits>

#define cy_vecp const glm::vec<P, float, glm::highp>&
#define cy_vecc const glm::vec<C, float, glm::highp>&
#define cy_vect const glm::vec<T, float, glm::highp>&
#define cy_vecn const glm::vec<N, float, glm::highp>&

#define y_vecp glm::vec<P, float, glm::highp>&
#define y_vecc glm::vec<C, float, glm::highp>&
#define y_vect glm::vec<T, float, glm::highp>&
#define y_vecn glm::vec<N, float, glm::highp>&

namespace ygfx {
    template<size_t P, size_t C, size_t T, size_t N>
    class Vertex {
    public:
        GLfloat vertexmemory[P + C + T + N];

        Vertex() {}

        template <typename D = void, typename D2 = std::enable_if_t<(P > 0), D>>
        y_vecp position() { return reinterpret_cast<y_vecp>(vertexmemory[0]); }

        template <typename D = void, typename D2 = std::enable_if_t<(C > 0), D>>
        y_vecc color() { return reinterpret_cast<y_vecc>(vertexmemory[P]); }

        template <typename D = void, typename D2 = std::enable_if_t<(T > 0), D>>
        y_vect texCoord() { return reinterpret_cast<y_vect>(vertexmemory[P + C]); }

        template <typename D = void, typename D2 = std::enable_if_t<(N > 0), D>>
        y_vecn normal() { return reinterpret_cast<y_vecn>(vertexmemory[P + C + T]); }

        template <typename D = void, typename D2 = std::enable_if_t<(P > 0 && C == 0 && T == 0 && N == 0), D>>
        Vertex(cy_vecp position) {
            this->position() = position;
        }

        template <typename D = void, typename D2 = std::enable_if_t<(P > 0 && C > 0 && T == 0 && N == 0), D>>
        Vertex(cy_vecp position, cy_vecc color) {
            this->position() = position;
            this->color() = color;
        }

        template <typename D = void, typename D2 = std::enable_if_t<(P > 0 && C == 0 && T > 0 && N == 0), D>>
        Vertex(cy_vecp position, cy_vect texCoord) {
            this->position() = position;
            this->texCoord() = texCoord;
        }

        template <typename D = void, typename D2 = std::enable_if_t<(P > 0 && C > 0 && T > 0 && N == 0), D>>
        Vertex(cy_vecp position, cy_vecc color, cy_vect texCoord) {
            this->position() = position;
            this->color() = color;
            this->texCoord() = texCoord;
        }

        template <typename D = void, typename D2 = std::enable_if_t<(P > 0 && C > 0 && T > 0 && N > 0), D>>
        Vertex(cy_vecp position, cy_vecc color, cy_vect texCoord, cy_vecn normal) {
            this->position() = position;
            this->color() = color;
            this->texCoord() = texCoord;
            this->normal() = normal;
        }

    };
#undef y_vecp
#undef y_vecc
#undef y_vect
#undef y_vecn
#undef cy_vecp
#undef cy_vecc
#undef cy_vect
#undef cy_vecn
}

#endif
