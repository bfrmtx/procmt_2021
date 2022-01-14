#include "gfx_qt_mathplot.h"

void ygfxqt::YMathGfxPlot::drawPlot(ygfx::GlobalUniforms * u_globals, ygfx::MatrixUniforms * u_matrices, ygfx::GuiUniforms * u_gui) {
    if(hasCustomShader()) {
        this->customshader->activate();
        this->customshader->uploadColor4("color_normal", color_base);
    }
    u_globals->activate();
    u_matrices->activate();
    u_gui->activate();
    if(this->texture != nullptr) {
        OGLFUNCS glBindTexture(GL_TEXTURE_2D, this->texture->textureId());
    }
    this->draw();
}
