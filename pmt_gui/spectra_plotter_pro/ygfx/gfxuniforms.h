#ifndef YGFX_GL_UNIFORMS_H
#define YGFX_GL_UNIFORMS_H

// stl-includes
#include <unordered_set>

// ygfx-includes
#include "gfxshader.h"
#include "gfxarray.h"

namespace ygfx {
    static constexpr GLint uniform_bp_matrices = 0;
    static constexpr GLint uniform_bp_globals = 1;
    static constexpr GLint uniform_bp_gui = 2;

    static constexpr GLint uniform_matrices_size = 48;
    static constexpr GLint uniform_globals_size = 20;
    static constexpr GLint uniform_gui_size = 16;

    static const char * uniform_matrices_name = "uniforms_matrices";
    static const char * uniform_globals_name = "uniforms_global";
    static const char * uniform_gui_name = "uniforms_gui";

    class MatrixUniforms {
    private:
        gfxArray<GLfloat, uniform_matrices_size, BufferTarget::UNIFORM_BUFFER> memory;
        std::unordered_set<GLuint> supported_shaders;

    public:
        MatrixUniforms& uploadMatrixView(const glm::mat4& matView);
        MatrixUniforms& uploadMatrixProjection(const glm::mat4& matProj);
        MatrixUniforms& uploadMatrixViewProjection(const glm::mat4& matVP);

        void activate();
        void ready_for_bind();
    };

    class GlobalUniforms {
    private:
        gfxArray<GLfloat, uniform_globals_size, BufferTarget::UNIFORM_BUFFER> memory;
        std::unordered_set<GLuint> supported_shaders;

    public:
        GlobalUniforms& uploadVectorResolution(const glm::vec2& resolution);
        GlobalUniforms& uploadVectorMousePosition(const glm::vec2& position);
        GlobalUniforms& uploadVectorMouseClickPosition(const glm::vec2& position);
        GlobalUniforms& uploadVectorGLMousePosition(const glm::vec2& glPosition);
        GlobalUniforms& uploadVectorRandom(const glm::vec2& random);
        GlobalUniforms& uploadPixelSize(const glm::vec2& size);
        GlobalUniforms& uploadPixelSize(const float & size_x, const float & size_y);
        GlobalUniforms& uploadBottomLeft(const glm::vec2& min);
        GlobalUniforms& uploadTopRight(const glm::vec2& max);
        GlobalUniforms& uploadResolution2d(const glm::vec2& res);
        GlobalUniforms& uploadMouseTime(GLfloat timer);
        GlobalUniforms& uploadTimer(GLfloat timer);

        void activate();
    };

    class GuiUniforms {
    private:
        gfxArray<GLfloat, uniform_gui_size, BufferTarget::UNIFORM_BUFFER> memory;
        std::unordered_set<GLuint> supported_shaders;

    public:
        GuiUniforms& uploadPosition(const glm::vec2& position);
        GuiUniforms& uploadSize(const glm::vec2& size);
        GuiUniforms& uploadCornerSize(const glm::vec2& cornerSize);
        GuiUniforms& uploadColorBorder(const glm::vec4& color);
        GuiUniforms& uploadColorBackground(const glm::vec4& color);
        GuiUniforms& uploadBorderWidth(GLfloat borderwidth);
        GuiUniforms& uploadType(GLint borderwidth);

        void activate();
    };
}

#endif
