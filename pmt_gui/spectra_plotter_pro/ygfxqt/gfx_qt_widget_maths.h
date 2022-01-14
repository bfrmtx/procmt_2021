#ifndef GFX_QT_WIDGET_MATHS_H
#define GFX_QT_WIDGET_MATHS_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QPainter>

#include "ygfx/gfxfloatbufferset.h"
#include "ygfx/gfxuniforms.h"

#include "gfx_qt_mathplot.h"
#include "gfx_qt_mathplot_set.h"
#include "gfx_qt_math_selector.h"
#include "gfx_qt_math_selection.h"

#include "glm/glm.hpp"

namespace ygfxqt {

enum class YMathGfxPointMode {
    NONE,
    DOTS,
    CIRCLE,
    FILLED_CIRCLE,
    CROSS,
    SQUARE,
    FILLED_SQUARE,
    BARS_TO_ZERO,
    DIAMONDS,
    FILLED_DIAMONDS,
    DEFAULT = FILLED_CIRCLE
};

enum class YMathGfxLineMode {
    NONE,
    CONNECTED_LINES,
    REGIONS_TO_ZERO,
    DEFAULT = CONNECTED_LINES
};

struct YMathGfxSetRenderSettings {
    YMathGfxPointMode pointMode = YMathGfxPointMode::DEFAULT;
    YMathGfxLineMode lineMode = YMathGfxLineMode::DEFAULT;

    float plotPointSize = 5.0f;
    float plotLineWidth = 1.0f;

};

class YMathGfxSet {
    YMathGfxSetRenderSettings renderSettings;
};


class YMathOpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
private:
    enum InternalInfo {
        InAxisXDragMode         = 1 << 0,
        InAxisYDragMode         = 1 << 2,
        InAreaDragMode          = 1 << 3,
        MouseAboveXAxis         = 1 << 4,
        MouseAboveYAxis         = 1 << 5,
        MouseAboveWidget        = 1 << 6
    };
    Q_DECLARE_FLAGS(InternalInfos, InternalInfo)

public:
    enum class AxisScale {
        LINEAR,
        LOGARITHMIC,
        DEFAULT = LINEAR,
    };

    enum class AxisOrder {
        FORWARD,
        REVERSED,
        DEFAULT = FORWARD,
    };

    enum MouseOption {
        UnlimitedXAxisMove      = 1 << 0,
        UnlimitedYAxisMove      = 1 << 1,
        HideCursorOnPlot        = 1 << 2,
        ShowHandOnAxis          = 1 << 3,
        DefaultMouseOptions     = UnlimitedXAxisMove | UnlimitedYAxisMove | ShowHandOnAxis
    };

    enum MouseSnapOption {
        SnapToGrid              = 1 << 0,
        SnapToVisiblePlotPoints = 1 << 1,
        DefaultMouseSnapOptions = 0
    };

    enum PlotOption {
        DrawXAxis               = 1 << 0,
        DrawYAxis               = 1 << 1,
        EnableXAxisZoom         = 1 << 2,
        EnableYAxisZoom         = 1 << 3,
        EnableXAxisMove         = 1 << 4,
        EnableYAxisMove         = 1 << 5,
        EnableMouseDrag         = 1 << 6,
        EnableMouseZoom         = 1 << 7,
        EnableMinimumXValue     = 1 << 8,
        EnableMinimumYValue     = 1 << 9,
        EnableMaximumXValue     = 1 << 10,
        EnableMaximumYValue     = 1 << 11,
        EnableMoveKeys          = 1 << 12,
        EnableZoomKeys          = 1 << 13,
        DefaultPlotOptions      = DrawXAxis + DrawYAxis + EnableXAxisZoom +
                                  EnableYAxisZoom + EnableXAxisMove + EnableYAxisMove +
                                  EnableMouseDrag + EnableMouseZoom + EnableMoveKeys + EnableZoomKeys
    };

    Q_DECLARE_FLAGS(PlotOptions, PlotOption)
    Q_DECLARE_FLAGS(MouseSnapOptions, MouseSnapOption)
    Q_DECLARE_FLAGS(MouseOptions, MouseOption)

public:
    explicit YMathOpenGLWidget(QWidget * parent = nullptr);
    glm::dvec2 plotPosition(QPoint const & point);
    QPoint widgetPosition(glm::dvec2 const & vec);
    double tickerAspectRatio = 1.0;

    size_t addPlot(YMathGfxPlot * plot);
    YMathGfxPlot * getPlot(size_t id)               { return plots.at(id); }
    std::vector<YMathGfxPlot *> getAllPlots()       { return plots; }
    void removeAllPlots()                           { plots.clear(); }

    YMathOpenGLWidget * setNewBoundingRectangle(double minx, double miny, double maxx, double maxy);
    YMathOpenGLWidget * setNewXRange(double minx, double maxx);
    YMathOpenGLWidget * setNewYRange(double miny, double maxy);
    YMathOpenGLWidget * setAxesOffsets(int offset_x, int offset_y);
    YMathOpenGLWidget * setAxesOrders(AxisOrder x, AxisOrder y)                 { x_axis_order = x; y_axis_order = y; update(); return this; }
    YMathOpenGLWidget * setAxesScales(AxisScale x, AxisScale y)                 { x_axis_scale = x; y_axis_scale = y; update(); return this; }
    YMathOpenGLWidget * setShader(std::shared_ptr<ygfx::Shader> shader)         { current_shader = shader; update(); return this; }
    YMathOpenGLWidget * setRulerBackgroundColor(QColor const & color)           { color_ruler_bg = color; update(); return this; }
    YMathOpenGLWidget * setRulerTextColor(QColor const & color)                 { color_ruler_text = color; update(); return this; }
    YMathOpenGLWidget * setBackgroundColor(QColor const & color)                { color_background = color; update(); return this; }
    YMathOpenGLWidget * setGridColor(QColor const & color)                      { color_grid = color; update(); return this; }
    YMathOpenGLWidget * setAxisColor(QColor const & color)                      { color_axis = color; update(); return this; }
    YMathOpenGLWidget * setPlotOptions(PlotOptions options)                     { options_plot = options; return this; }
    YMathOpenGLWidget * setPlotOption(PlotOption option, bool value = true)     { options_plot.setFlag(option, value); return this; }
    YMathOpenGLWidget * setMouseOptions(MouseOptions options)                   { options_mouse = options; return this; }
    YMathOpenGLWidget * setMouseSnappingOptions(MouseSnapOptions options)       { options_mouse_snap = options; return this; }
    YMathOpenGLWidget * setMaximumPlotPosition(double maxx, double maxy)        { maximum_plot_position = glm::dvec2(maxx, maxy); return this; }
    YMathOpenGLWidget * setMinimumPlotPosition(double minx, double miny)        { minimum_plot_position = glm::dvec2(minx, miny); return this; }
    YMathOpenGLWidget * setXAxisText(QString const & label)                     { label_x_axis = label; return this; }
    YMathOpenGLWidget * setYAxisText(QString const & label)                     { label_y_axis = label; return this; }
    YMathOpenGLWidget * setSelectionTool(YMathWidgetSelector * new_selector);
    YMathOpenGLWidget * fitScaleToAllVisiblePlots();
    YMathOpenGLWidget * fitScaleToPlot(size_t id);
    YMathOpenGLWidget * fitScaleToPlot(YMathGfxPlot * plot);
    YMathOpenGLWidget * hideAllPlotsExceptIndex(size_t id);

    glm::dvec2 getMin()                 const { return bounds_min; }
    glm::dvec2 getMax()                 const { return bounds_max; }
    QColor getRulerBackgroundColor()    const { return color_ruler_bg; }
    QColor getRulerTextColor()          const { return color_ruler_text; }
    QColor getBackgroundColor()         const { return color_background; }
    QColor getGridColor()               const { return color_grid; }
    QColor getAxisColor()               const { return color_axis; }
    PlotOptions getPlotOptions()        const { return options_plot; }
    MouseOptions getMouseOptions()      const { return options_mouse; }


    YMathOpenGLWidget * moveBoundingRectangle(double dx, double dy);
    YMathOpenGLWidget * moveBoundingRectangle(const glm::dvec2 & vec);
    YMathOpenGLWidget * scaleBoundingRectangle(double sx, double sy, double cx, double cy);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    void wheelEvent(QWheelEvent * event);

private:
    void paintAxesValues(QPainter & painter);
    void paintAxes(QPainter & painter);

signals:
    void yBoundingChanged(double miny, double maxy);
    void xBoundingChanged(double minx, double maxx);
    void boundingRectangleChanged(double minx, double miny, double maxx, double maxy);
    void initialized();
    void mousePlotPositionChanged(glm::dvec2 const & position);

protected:
    QMetaObject::Connection selector_connection;

    QString label_x_axis = "";
    QString label_y_axis = "";

    std::vector<YMathGfxPlot *> plots;

    ygfx::MatrixUniforms *  u_matrices = nullptr;
    ygfx::GlobalUniforms *  u_globals = nullptr;
    ygfx::GuiUniforms *  u_gui = nullptr;

    std::shared_ptr<ygfx::Shader> current_shader = nullptr;

    PlotOptions      options_plot        = PlotOption::DefaultPlotOptions;
    MouseSnapOptions options_mouse_snap  = MouseSnapOption::DefaultMouseSnapOptions;
    MouseOptions     options_mouse       = MouseOption::DefaultMouseOptions;
    InternalInfos    state_info;

    glm::dvec2 bounds_min = glm::dvec2(-10.0, -10.0);
    glm::dvec2 bounds_max = glm::dvec2(10.0, 10.0);
    glm::dvec2 plot_size  = glm::dvec2(30.0, 30.0);
    glm::dvec2 plot_stepsize;
    int        current_plot_width = -1;
    int        current_plot_height = -1;
    glm::dvec2 maximum_plot_position;
    glm::dvec2 minimum_plot_position;

    QPoint axes_offset = QPoint(30, 30);

    AxisScale y_axis_scale = AxisScale::DEFAULT;
    AxisScale x_axis_scale = AxisScale::DEFAULT;
    AxisOrder y_axis_order = AxisOrder::DEFAULT;
    AxisOrder x_axis_order = AxisOrder::DEFAULT;

    QColor color_ruler_bg      = QColor(245, 245, 245);
    QColor color_axis          = QColor(195, 195, 195);
    QColor color_ruler_text    = QColor(0, 0, 0);
    QColor color_background    = QColor(255, 255, 255);
    QColor color_grid          = QColor(230, 230, 230);

    QPoint mouse_position_current_widget;
    Qt::MouseButton mouse_button_axis_drag = Qt::LeftButton;
    Qt::MouseButton mouse_button_area_drag = Qt::RightButton;

    YMathWidgetSelector * selector_tool = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(YMathOpenGLWidget::PlotOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(YMathOpenGLWidget::MouseOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(YMathOpenGLWidget::MouseSnapOptions)
}
#endif // GFX_QT_WIDGET_MATHS_H
