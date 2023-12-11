#ifndef GFX_QT_MATHPLOT_H
#define GFX_QT_MATHPLOT_H

#include <QOpenGLTexture>
#include <QColor>

#include "ygfx/gfxincludes.h"
#include "ygfx/gfxuniforms.h"
#include "ygfx/gfxshader.h"


#ifdef ENABLE_STOPWATCHES
#define STOPWATCH(text) OneTimeStopWatch stop_watch_(text)
#else
#define STOPWATCH(text) ;
#endif

class OneTimeStopWatch {
private:
    std::chrono::system_clock::time_point tp_start_;
    std::string output_prefix_;

public:
    OneTimeStopWatch(std::string const & output_prefix = "ELAPSED TIME: ") :
        tp_start_(std::chrono::system_clock::now()),
        output_prefix_(output_prefix) {}

    ~OneTimeStopWatch() {
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>
                           (std::chrono::system_clock::now() - tp_start_).count();
        auto microseconds = nanoseconds / 1000;
        double milliseconds = microseconds / 1000.0;
        double seconds = milliseconds / 1000.0;
        std::cout << output_prefix_ << std::endl;
        std::cout << "[" << nanoseconds << " ns][";
        std::cout << microseconds << " mys][";
        std::cout << milliseconds << " ms][";
        std::cout << seconds << " s]" << std::endl;
    }
};

namespace ygfxqt {
    class YMathGfxPlot {
    protected:
        glm::dvec2 bounding_box_min = glm::dvec2(-10, -10);
        glm::dvec2 bounding_box_max = glm::dvec2(10, 10);

        GLenum drawmode = GL_LINES;
        GLint draw_offset = 0;
        GLsizei draw_size = std::numeric_limits<GLsizei>::max();
        QColor color_base = Qt::blue;
        QOpenGLTexture * texture = nullptr;
        ygfx::spShader customshader;
        bool visible = true;

    public:
        void drawPlot(ygfx::GlobalUniforms * u_globals, ygfx::MatrixUniforms * u_matrices, ygfx::GuiUniforms * u_gui);

        GLenum getDrawMode()    const { return drawmode; }
        bool hasCustomShader()  const { return customshader != nullptr && customshader->isValid(); }
        bool isVisible()        const { return visible; }
        double getMaxX()        const { return bounding_box_max.x; }
        double getMaxY()        const { return bounding_box_max.y; }
        double getMinX()        const { return bounding_box_min.x; }
        double getMinY()        const { return bounding_box_min.y; }
        glm::dvec2 getMin()     const { return bounding_box_min; }
        glm::dvec2 getMax()     const { return bounding_box_max; }

        YMathGfxPlot * setMinX(double x)                        { bounding_box_min.x = x; return this; }
        YMathGfxPlot * setMinY(double y)                        { bounding_box_min.y = y; return this; }
        YMathGfxPlot * setMaxX(double x)                        { bounding_box_max.x = x; return this; }
        YMathGfxPlot * setMaxY(double y)                        { bounding_box_max.y = y; return this; }
        YMathGfxPlot * setVisible(bool vis)                     { visible = vis; return this; }
        YMathGfxPlot * setDrawMode(GLenum new_drawmode)         { drawmode = new_drawmode; return this; }
        YMathGfxPlot * setTexture(QOpenGLTexture * new_texture) { texture = new_texture; return this; }
        YMathGfxPlot * setCustomShader(ygfx::spShader shader)   { customshader = shader; return this; }
        YMathGfxPlot * setDrawingOffset(GLint offset)           { draw_offset = offset; return this; }
        YMathGfxPlot * setDrawingSize(GLsizei size)             { draw_size = size; return this; }
        YMathGfxPlot * setBaseColor(QColor const & color)       { color_base = color; return this; }

    protected:
        virtual void draw() = 0;
        virtual ~YMathGfxPlot() {}
    };
}
#endif // GFX_QT_MATHPLOT_H
