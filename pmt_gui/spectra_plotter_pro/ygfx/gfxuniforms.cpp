#include "gfxuniforms.h"
#include "gfxshader.h"

using namespace ygfx;

MatrixUniforms& MatrixUniforms::uploadMatrixView(const glm::mat4& matView) {
    reinterpret_cast<glm::mat4&>(memory[0]) = matView;
    return *this;
}

MatrixUniforms& MatrixUniforms::uploadMatrixProjection(const glm::mat4& matProj) {
    reinterpret_cast<glm::mat4&>(memory[16]) = matProj;
    return *this;
}

MatrixUniforms& MatrixUniforms::uploadMatrixViewProjection(const glm::mat4& matVP) {
    reinterpret_cast<glm::mat4&>(memory[32]) = matVP;
    return *this;
}

void MatrixUniforms::activate() {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    if(f != nullptr) {
        memory.enableMapping(false);

        auto sID = Shader::currentActivatedShaderID();
        auto uniformFinder = supported_shaders.find(sID);

        if(uniformFinder == supported_shaders.end()) {
            auto sID = Shader::currentActivatedShaderID();
            auto location = f->glGetUniformBlockIndex(sID, uniform_matrices_name);
            f->glUniformBlockBinding(sID, location, uniform_bp_matrices);
        }
        f->glBindBufferBase(GL_UNIFORM_BUFFER, uniform_bp_matrices, memory.getVBO());
    }
}





GlobalUniforms& GlobalUniforms::uploadVectorResolution(const glm::vec2& resolution) {
    reinterpret_cast<glm::vec2&>(memory[0]) = resolution;
    return *this;
}

GlobalUniforms& GlobalUniforms::uploadVectorMousePosition(const glm::vec2& position) {
    reinterpret_cast<glm::vec2&>(memory[2]) = position;
    return *this;
}

GlobalUniforms& GlobalUniforms::uploadVectorGLMousePosition(const glm::vec2& glPosition) {
    reinterpret_cast<glm::vec2&>(memory[4]) = glPosition;
    return *this;
}

GlobalUniforms& GlobalUniforms::uploadVectorRandom(const glm::vec2& random) {
    reinterpret_cast<glm::vec2&>(memory[6]) = random;
    return *this;
}

GlobalUniforms& GlobalUniforms::uploadVectorMouseClickPosition(const glm::vec2& position) {
    reinterpret_cast<glm::vec2&>(memory[8]) = position;
    return *this;
}

GlobalUniforms &GlobalUniforms::uploadPixelSize(const glm::vec2 & size) {
    reinterpret_cast<glm::vec2&>(memory[10]) = size;
    return *this;
}

GlobalUniforms &GlobalUniforms::uploadPixelSize(const float & size_x, const float & size_y) {
    return uploadPixelSize(glm::vec2(size_x, size_y));
}

GlobalUniforms &GlobalUniforms::uploadBottomLeft(const glm::vec2 & min) {
    reinterpret_cast<glm::vec2&>(memory[12]) = min;
    return *this;
}

GlobalUniforms &GlobalUniforms::uploadTopRight(const glm::vec2 & max) {
    reinterpret_cast<glm::vec2&>(memory[14]) = max;
    return *this;
}

GlobalUniforms &GlobalUniforms::uploadResolution2d(const glm::vec2 & res) {
    reinterpret_cast<glm::vec2&>(memory[16]) = res;
    return *this;
}

GlobalUniforms& GlobalUniforms::uploadMouseTime(GLfloat timer) {
    memory[18] = timer;
    return *this;
}

GlobalUniforms& GlobalUniforms::uploadTimer(GLfloat timer) {
    memory[19] = timer;
    return *this;
}

void GlobalUniforms::activate() {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    if(f != nullptr) {
        memory.enableMapping(false);

        auto sID = Shader::currentActivatedShaderID();
        auto uniformFinder = supported_shaders.find(sID);

        if(uniformFinder == supported_shaders.end()) {
            auto sID = Shader::currentActivatedShaderID();
            auto location = f->glGetUniformBlockIndex(sID, uniform_globals_name);
            f->glUniformBlockBinding(sID, location, uniform_bp_globals);
        }
        f->glBindBufferBase(GL_UNIFORM_BUFFER, uniform_bp_globals, memory.getVBO());
    }
}





GuiUniforms& GuiUniforms::uploadColorBorder(const glm::vec4& color) {
    reinterpret_cast<glm::vec4&>(memory[0]) = color;
    return *this;
}

GuiUniforms& GuiUniforms::uploadColorBackground(const glm::vec4& color) {
    reinterpret_cast<glm::vec4&>(memory[4]) = color;
    return *this;
}

GuiUniforms& GuiUniforms::uploadPosition(const glm::vec2& position) {
    reinterpret_cast<glm::vec2&>(memory[8]) = position;
    return *this;
}

GuiUniforms& GuiUniforms::uploadSize(const glm::vec2& size) {
    reinterpret_cast<glm::vec2&>(memory[10]) = size;
    return *this;
}

GuiUniforms& GuiUniforms::uploadCornerSize(const glm::vec2& cornerSize) {
    reinterpret_cast<glm::vec2&>(memory[12]) = cornerSize;
    return *this;
}

GuiUniforms& GuiUniforms::uploadBorderWidth(GLfloat borderwidth) {
    memory[14] = borderwidth;
    return *this;
}

GuiUniforms& GuiUniforms::uploadType(GLint type) {
    reinterpret_cast<GLint&>(memory[15]) = type;
    return *this;
}

void GuiUniforms::activate() {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    if(f != nullptr) {
        memory.enableMapping(false);

        auto sID = Shader::currentActivatedShaderID();
        auto uniformFinder = supported_shaders.find(sID);

        if(uniformFinder == supported_shaders.end()) {
            auto sID = Shader::currentActivatedShaderID();
            auto location = f->glGetUniformBlockIndex(sID, uniform_gui_name);
            f->glUniformBlockBinding(sID, location, uniform_bp_gui);
        }
        f->glBindBufferBase(GL_UNIFORM_BUFFER, uniform_bp_gui, memory.getVBO());
    }
}
