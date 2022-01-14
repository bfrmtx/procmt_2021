#include "gfx_qt_widget_maths.h"

#include <QOpenGLDebugLogger>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QKeyEvent>
#include <algorithm>
#include <random>

const double bounding_box_margin = 0.000125;
const int delta_pixel_mousejump = 100;
const int axis_offset_max = 80;
const int axis_offset_min = 25;

ygfxqt::YMathOpenGLWidget::YMathOpenGLWidget(QWidget * parent) : QOpenGLWidget(parent) {
    setMouseTracking(true);
}

// change that whole position stuff later to view-matrices
glm::dvec2 ygfxqt::YMathOpenGLWidget::plotPosition(const QPoint & point) {
    glm::dvec2 ret;
    auto px = point.x() - axes_offset.x();
    if(x_axis_order == AxisOrder::REVERSED) {
        px = current_plot_width - px;
    }
    auto py = height() - point.y() - axes_offset.y();
    auto pWidth = static_cast<double>(width() - axes_offset.x());
    auto pHeight = static_cast<double>(height() - axes_offset.y());

    ret.x = (px / pWidth) * plot_size.x + bounds_min.x;
    ret.y = (py / pHeight) * plot_size.y + bounds_min.y;

    return ret;
}

QPoint ygfxqt::YMathOpenGLWidget::widgetPosition(const glm::dvec2 & vec) {
    QPoint ret;
    ret.ry() = height() - static_cast<int>(((vec.y - bounds_min.y) / plot_size.y) * current_plot_height) - axes_offset.y();
    ret.rx() = static_cast<int>(((vec.x - bounds_min.x) / plot_size.x) * current_plot_width) + axes_offset.x();
    if(x_axis_order == AxisOrder::REVERSED) {
        ret.rx() -= axes_offset.x();
        ret.rx() = current_plot_width - ret.rx();
        ret.rx() += axes_offset.x();
    }
    return ret;
}

size_t ygfxqt::YMathOpenGLWidget::addPlot(ygfxqt::YMathGfxPlot * plot) {
    plots.push_back(plot);
    return plots.size() - 1;
}


ygfxqt::YMathOpenGLWidget * ygfxqt::YMathOpenGLWidget::moveBoundingRectangle(double dx, double dy) {
    setNewBoundingRectangle(bounds_min.x + dx, bounds_min.y + dy, bounds_max.x + dx, bounds_max.y + dy);
    return this;
}

ygfxqt::YMathOpenGLWidget * ygfxqt::YMathOpenGLWidget::moveBoundingRectangle(const glm::dvec2 & vec) {
    setNewBoundingRectangle(bounds_min.x + vec.x, bounds_min.y + vec.y, bounds_max.x + vec.x, bounds_max.y + vec.y);
    return this;
}

ygfxqt::YMathOpenGLWidget * ygfxqt::YMathOpenGLWidget::scaleBoundingRectangle(double sx, double sy, double cx, double cy) {
    cx = std::min(std::max(0.0, cx), 1.0);
    cy = std::min(std::max(0.0, cy), 1.0);
    double width = bounds_max.x - bounds_min.x;
    double height = bounds_max.y - bounds_min.y;
    double newWidth = width * sx;
    double newHeight = height * sy;
    double deltaWidth = newWidth - width;
    double deltaHeight = newHeight - height;
    setNewBoundingRectangle(bounds_min.x - deltaWidth * (1 - cx), bounds_min.y - deltaHeight * (1 - cy),
                            bounds_max.x + deltaWidth * cx, bounds_max.y + deltaHeight * cy);
    return this;
}

ygfxqt::YMathOpenGLWidget * ygfxqt::YMathOpenGLWidget::setAxesOffsets(int offset_x, int offset_y) {
    offset_x = std::max(axis_offset_min, std::min(axis_offset_max, offset_x));
    offset_y = std::max(axis_offset_min, std::min(axis_offset_max, offset_y));
    axes_offset = QPoint(offset_x, offset_y);
    return setNewBoundingRectangle(bounds_min.x, bounds_min.y, bounds_max.x, bounds_max.y);
}

ygfxqt::YMathOpenGLWidget *ygfxqt::YMathOpenGLWidget::setSelectionTool(ygfxqt::YMathWidgetSelector * new_selector) {
    if(selector_tool != nullptr) {
        selector_tool->disconnect(selector_connection);
    }
    selector_tool = new_selector;
    return this;
}

ygfxqt::YMathOpenGLWidget *ygfxqt::YMathOpenGLWidget::fitScaleToAllVisiblePlots() {
    glm::dvec2 bbox_min( 1e30,  1e30);
    glm::dvec2 bbox_max(-1e30, -1e30);
    size_t visible_plot_count = 0;

    for(auto plot : plots) {
        if(plot->isVisible()) {
            ++visible_plot_count;
            auto upd_min = plot->getMin();
            auto upd_max = plot->getMax();
            bbox_min.x = std::min(upd_min.x, bbox_min.x);
            bbox_min.y = std::min(upd_min.y, bbox_min.y);
            bbox_max.x = std::max(upd_max.x, bbox_max.x);
            bbox_max.y = std::max(upd_max.y, bbox_max.y);
        }
    }

    if(bbox_max.x - bbox_min.x < std::numeric_limits<double>::epsilon()) {
        bbox_max.x -= bounding_box_margin;
        bbox_min.x += bounding_box_margin;
    }
    if(bbox_max.y - bbox_min.y < std::numeric_limits<double>::epsilon()) {
        bbox_max.y -= bounding_box_margin;
        bbox_min.y += bounding_box_margin;
    }

    if(visible_plot_count > 0) {
        setNewBoundingRectangle(bbox_min.x, bbox_min.y, bbox_max.x, bbox_max.y);
        scaleBoundingRectangle(1.05, 1.05, 0.5, 0.5);
    } else {
        setNewBoundingRectangle(-10, -10, 10, 10);
    }
    return this;
}

ygfxqt::YMathOpenGLWidget *ygfxqt::YMathOpenGLWidget::fitScaleToPlot(size_t id) {
    if(id < plots.size()) {
        fitScaleToPlot(plots[id]);
    }
    return this;
}

ygfxqt::YMathOpenGLWidget *ygfxqt::YMathOpenGLWidget::fitScaleToPlot(ygfxqt::YMathGfxPlot * plot) {
    if(plot != nullptr) {
        auto bbox_min = plot->getMin();
        auto bbox_max = plot->getMax();
        setNewBoundingRectangle(bbox_min.x, bbox_min.y, bbox_max.x, bbox_max.y);
        scaleBoundingRectangle(1.05, 1.05, 0.5, 0.5);
        update();
    }
    return this;
}

ygfxqt::YMathOpenGLWidget *ygfxqt::YMathOpenGLWidget::hideAllPlotsExceptIndex(size_t id) {
    if(id < plots.size()) {
        for(size_t i = 0; i < plots.size(); ++i) {
            plots[i]->setVisible(i == id);
        }
        update();
    }
    return this;
}


ygfxqt::YMathOpenGLWidget * ygfxqt::YMathOpenGLWidget::setNewBoundingRectangle(double minx, double miny, double maxx, double maxy) {
    if(!(options_plot.testFlag(PlotOption::EnableMaximumXValue) && maxx > maximum_plot_position.x + plot_size.x * 0.01) ||
        (options_plot.testFlag(PlotOption::EnableMinimumXValue) && minx < minimum_plot_position.x - plot_size.x * 0.01)) {
        bounds_min.x = minx;
        bounds_max.x = maxx;
    }
    if(!(options_plot.testFlag(PlotOption::EnableMaximumYValue) && maxy > maximum_plot_position.y + plot_size.y * 0.01) ||
        (options_plot.testFlag(PlotOption::EnableMinimumYValue) && miny < minimum_plot_position.y - plot_size.y * 0.01)) {
        bounds_min.y = miny;
        bounds_max.y = maxy;
    }

    emit boundingRectangleChanged(bounds_min.x, bounds_min.y, bounds_max.x, bounds_max.y);
    emit xBoundingChanged(bounds_min.x, bounds_max.x);
    emit yBoundingChanged(bounds_min.y, bounds_max.y);

    plot_size = bounds_max - bounds_min;

    if(selector_tool != nullptr)
        selector_tool->handle_dimension_change(bounds_min, bounds_max, QPoint(current_plot_width, current_plot_height), axes_offset);
    update();

    return this;
}

ygfxqt::YMathOpenGLWidget *ygfxqt::YMathOpenGLWidget::setNewXRange(double minx, double maxx) {
    this->blockSignals(true);
    setNewBoundingRectangle(minx, bounds_min.y, maxx, bounds_max.y);
    this->blockSignals(false);
    return this;
}

ygfxqt::YMathOpenGLWidget *ygfxqt::YMathOpenGLWidget::setNewYRange(double miny, double maxy) {
    this->blockSignals(true);
    setNewBoundingRectangle(bounds_min.x, miny, bounds_max.x, maxy);
    this->blockSignals(false);
    return this;
}



void ygfxqt::YMathOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

#ifdef OGLDEBUGENABLED
    qDebug() << "*************** initializeGL - " << this->objectName() << "***************";
    if(context() != nullptr && context()->format().options().testFlag(QSurfaceFormat::DebugContext)) {
        if(context()->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
            QOpenGLDebugLogger * debug_logger = new QOpenGLDebugLogger(this);
            bool worked = debug_logger->initialize();
            if(worked) {
                qDebug() << "---> debugging is enabled for this OpenGL-Context";
                connect(debug_logger, &QOpenGLDebugLogger::messageLogged, this, [](QOpenGLDebugMessage const & msg) {
                    qWarning() << msg;
                });
                debug_logger->startLogging();
            } else {
                delete debug_logger;
            }
        }
    }

    qDebug() << "THIS GL CONTEXT ADR         :" << reinterpret_cast<void*>(context());
    qDebug() << "GLOBAL CONTEXT ADR          :" << reinterpret_cast<void*>(QOpenGLContext::globalShareContext());
    qDebug() << "SHARED CONTEXT ADR          :" << reinterpret_cast<void*>(context()->shareContext());
    { std::stringstream sstream; sstream << "GL_VENDOR                   : " << glGetString(GL_VENDOR);
        qDebug().noquote() << QString::fromStdString(sstream.str()); }
    { std::stringstream sstream; sstream << "GL_RENDERER                 : " << glGetString(GL_RENDERER);
        qDebug().noquote() << QString::fromStdString(sstream.str()); }
    { std::stringstream sstream; sstream << "GL_VERSION                  : " << glGetString(GL_VERSION);
        qDebug().noquote() << QString::fromStdString(sstream.str()); }
    { std::stringstream sstream; sstream << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION);
        qDebug().noquote() << QString::fromStdString(sstream.str()); }


    {
        std::stringstream sstream; sstream << glGetString(GL_EXTENSIONS);
        qDebug().noquote() << endl << "THIS GL IMPLEMENTATIONS SUPPORTS THE FOLLOWING EXTENSIONS: ";
        auto ext = QString::fromStdString(sstream.str()).split(" ");
        for(auto & extension : ext) {
            qDebug().noquote() << "- " << extension;
        }
    }
#endif

    u_matrices = new ygfx::MatrixUniforms();
    u_globals = new ygfx::GlobalUniforms();
    u_gui = new ygfx::GuiUniforms();

    emit initialized();
}

void ygfxqt::YMathOpenGLWidget::resizeGL(int, int) {
    current_plot_width = width() - axes_offset.x();
    current_plot_height = height() - axes_offset.y();
    setNewBoundingRectangle(bounds_min.x, bounds_min.y, bounds_max.x, bounds_max.y);
}

void ygfxqt::YMathOpenGLWidget::paintGL() {
    glClearColor(GLfloat(color_background.redF()),
                 GLfloat(color_background.greenF()),
                 GLfloat(color_background.blueF()), 1.0);
    glClearDepth(0.0);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    QPainter painter(this);
    //painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    paintAxes(painter);

    painter.beginNativePainting();
        {
            #ifdef OGLDEBUGENABLED
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);
                qDebug().noquote() << "****** " << this->objectName() << " Viewport INFO ******";
                qDebug().noquote() << "opengl-viewport: " << "x:" << viewport[0] << "|y:" << viewport[1] << "|w:" << viewport[2] << "|h:" << viewport[3];
                qDebug().noquote() << "widget size/pos: " << "x:" << this->x() << "|y:" << this->y() << "|w:" << this->width() << "|h:" << this->height();
                qDebug().noquote() << "axes offsets   : " << "x:" << axes_offset.x() << "|y:" << axes_offset.y();
                qDebug().noquote() << "***************************";
            #endif
        }

        if(width() - axes_offset.x() - 1 > 0 && height() - axes_offset.y() - 2 > 0) {
            glViewport(axes_offset.x(), axes_offset.y(), width() - axes_offset.x() - 1, height() - axes_offset.y() - 2);
        }

        {
            #ifdef OGLDEBUGENABLED
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);
                qDebug().noquote() << "****** " << this->objectName() << " Viewport INFO ******";
                qDebug().noquote() << "opengl-viewport: " << "x:" << viewport[0] << "|y:" << viewport[1] << "|w:" << viewport[2] << "|h:" << viewport[3];
                qDebug().noquote() << "widget size/pos: " << "x:" << this->x() << "|y:" << this->y() << "|w:" << this->width() << "|h:" << this->height();
                qDebug().noquote() << "axes offsets   : " << "x:" << axes_offset.x() << "|y:" << axes_offset.y();
                qDebug().noquote() << "***************************";
            #endif
        }


        u_globals->uploadPixelSize(2.0f / (width() - axes_offset.x()), 2.0f / (height() - axes_offset.y()));
        u_globals->uploadResolution2d(glm::vec2(plot_size.x, plot_size.y));
        u_gui->uploadType(x_axis_order == AxisOrder::FORWARD ? 1 : 2);

        u_globals->uploadTopRight(glm::vec2(bounds_max.x, bounds_max.y));
        u_globals->uploadBottomLeft(glm::vec2(bounds_min.y, bounds_min.y));

        glEnable(GL_BLEND);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if(current_shader != nullptr) {
            current_shader->activate();
        }

        for(auto & plot : plots) {
            if(plot->isVisible()) {
                plot->drawPlot(u_globals, u_matrices, u_gui);
            }
        }

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glDisable(GL_DEPTH_TEST);
    painter.endNativePainting();

    if(selector_tool != nullptr && state_info.testFlag(InternalInfo::MouseAboveWidget)) {
        if(selector_tool->isActive()) {
            selector_tool->paint_selection(painter);
        }
    }
    paintAxesValues(painter);
}

void ygfxqt::YMathOpenGLWidget::paintAxesValues(QPainter & painter) {
    auto zero = widgetPosition(glm::dvec2(0, 0));

    painter.setPen(color_axis);
    if(options_plot.testFlag(PlotOption::DrawXAxis) && zero.y() < height() - axes_offset.y()) {
        painter.drawLine(axes_offset.x() + 1, zero.y(), width(), zero.y());
    }
    if(options_plot.testFlag(PlotOption::DrawYAxis) && zero.x() > axes_offset.x()) {
        painter.drawLine(zero.x(), 0, zero.x(), height() - axes_offset.y() - 1);
    }

    // paint ruler boxes
    painter.setPen(Qt::white);
    painter.fillRect(0, 0, axes_offset.x() - 1, height() - axes_offset.y(), QBrush(color_ruler_bg));
    painter.fillRect(axes_offset.x(), height() - axes_offset.y() + 1, width(), axes_offset.y(), QBrush(color_ruler_bg));

    painter.setPen(color_ruler_text);
    painter.drawLine(0, height() - axes_offset.y(), width(), height() - axes_offset.y());
    painter.drawLine(axes_offset.x(), 0, axes_offset.x(), height());

    // paint axis text / ruler lines
    int min_text_exp_x = 1000;
    int min_text_x = 1000;
    for(auto current_y = std::floor(bounds_min.y / plot_stepsize.y) * plot_stepsize.y;
        current_y < bounds_max.y; current_y += plot_stepsize.y) {
        QString yValue = QString::number(current_y);
        // Qt6 .width to horizontalAdvance
        min_text_x = std::min(axes_offset.x() - 6 - painter.fontMetrics().horizontalAdvance(yValue), min_text_x);
        min_text_exp_x = std::min(axes_offset.x() - 6 - painter.fontMetrics().horizontalAdvance(yValue) - painter.fontMetrics().horizontalAdvance("10"), min_text_exp_x);
    }

    // calc / paint ticks and texts
    for(auto current_y = std::floor(bounds_min.y / plot_stepsize.y) * plot_stepsize.y;
        current_y < bounds_max.y; current_y += plot_stepsize.y) {
        int plot_y = widgetPosition(glm::vec2(0, current_y)).y();
        for(int i = 1; i <= 4; ++i) {
            int plot_yy = widgetPosition(glm::vec2(0, current_y + (plot_stepsize.y / 5.0) * i)).y();
            painter.drawLine(axes_offset.x(), plot_yy, axes_offset.x() - 2, plot_yy);
        }

        if(plot_y < height() - axes_offset.y()) {
            QString yValue = QString::number(current_y);
            painter.drawLine(axes_offset.x(), plot_y, axes_offset.x() - 4, plot_y);
            if(y_axis_scale == AxisScale::LOGARITHMIC && std::abs(current_y) > std::numeric_limits<double>::epsilon()) {
                painter.drawText(min_text_exp_x, plot_y + painter.fontMetrics().height() / 2 - 1, "10");
                painter.drawText(min_text_exp_x + painter.fontMetrics().horizontalAdvance("10"), plot_y + painter.fontMetrics().height() / 2 - 8, yValue);
            } else {
                painter.drawText(axes_offset.x() - 6 - painter.fontMetrics().horizontalAdvance(yValue), plot_y + painter.fontMetrics().height() / 2, yValue);
            }
        }
    }
    for(auto current_x = std::floor(bounds_min.x / plot_stepsize.x) * plot_stepsize.x;
        current_x < bounds_max.x; current_x += plot_stepsize.x) {
        int plot_x = widgetPosition(glm::vec2(current_x, 0)).x();

        for(int i = 1; i <= 4; ++i) {
            int plot_xx = widgetPosition(glm::vec2(current_x + (plot_stepsize.x / 5.0) * i, 0)).x();
            painter.drawLine(plot_xx, height() - axes_offset.y(), plot_xx, height() - axes_offset.y() + 2);
        }

        if(plot_x > axes_offset.x()) {
            QString xValue = QString::number(current_x);
            painter.drawLine(plot_x, height() - axes_offset.y(), plot_x, height() - axes_offset.y() + 4);
            if(x_axis_scale == AxisScale::LOGARITHMIC && std::abs(current_x) > std::numeric_limits<double>::epsilon()) {
                painter.drawText(plot_x + painter.fontMetrics().horizontalAdvance("10") / 2, height() - axes_offset.y() + 5 + painter.fontMetrics().height(), xValue);
                painter.drawText(plot_x - painter.fontMetrics().horizontalAdvance("10") / 2, height() - axes_offset.y() + 12 + painter.fontMetrics().height(), "10");
            } else {
                painter.drawText(plot_x - painter.fontMetrics().horizontalAdvance(xValue) / 2, height() - axes_offset.y() + 4 + painter.fontMetrics().height(), xValue);
            }
        }
    }

    // paint small bottom left corner
    painter.fillRect(0, height() - axes_offset.y() + 1, axes_offset.x() - 1, axes_offset.y() - 1, QBrush(color_ruler_bg));
    painter.setPen(Qt::black);
    painter.drawRect(1, 0, this->width() - 1, this->height() - 1);

    QFont bFont("Arial", painter.font().pointSize() + 2);
    bFont.setBold(true);
    bFont.setPointSize(bFont.pointSize() + 3);
    bFont.setStyleStrategy(QFont::StyleStrategy::PreferAntialias);
    painter.setFont(bFont);
    // Qt6
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    QFontMetrics bMetrics(bFont);
    // Qt6
    auto x1 = (width() + axes_offset.x() - bMetrics.horizontalAdvance(label_x_axis)) / 2;
    auto y1 = height() - (painter.font().pointSize() + 2);
    auto x2 = 16;
    auto y2 = (height() - axes_offset.y() + 1 + bMetrics.horizontalAdvance(label_y_axis)) / 2;

    painter.setPen(Qt::black);
    painter.drawText(x1, y1, label_x_axis);
    painter.translate(x2, y2);
    painter.rotate(-90);
    painter.drawText(0, 0, label_y_axis);
    painter.rotate(90.0);
}

void ygfxqt::YMathOpenGLWidget::paintAxes(QPainter & painter) {
    bool xfound = false;
    bool yfound = false;

    for(int j = 3; j > 0; --j) {
        auto rx = std::pow(10.0, std::floor(std::log10(plot_size.x) - j));
        auto ry = std::pow(10.0, std::floor(std::log10(plot_size.y) - j));

        for(int i = 2; i >= 0 && !xfound; --i) {
            plot_stepsize.x = (rx * 10) / std::pow(2.0, i);
            auto lines_x = plot_size.x / plot_stepsize.x;
            if(lines_x < width() / 50) { xfound = true; break; }
        }
        for(int i = 2; i >= 0 && !yfound; --i) {
            plot_stepsize.y = (ry * 10) / std::pow(2.0, i);
            auto lines_y = plot_size.y / plot_stepsize.y;
            if(lines_y < height() / 50) { yfound = true; break; }
        }
    }

    auto current_x = std::floor(bounds_min.x / plot_stepsize.x) * plot_stepsize.x;
    auto current_y = std::floor(bounds_min.y / plot_stepsize.y) * plot_stepsize.y;

    while(current_x < bounds_max.x) {
        int plot_x = widgetPosition(glm::vec2(current_x, 0)).x();

        if(plot_x > axes_offset.x()) {
            painter.setPen(QPen(QBrush(color_grid), 1.0, Qt::DotLine));
            painter.drawLine(plot_x, height() - axes_offset.y() - 1, plot_x, 0);
        }
        current_x += plot_stepsize.x;
    }

    while(current_y < bounds_max.y) {
        int plot_y = widgetPosition(glm::vec2(0, current_y)).y();
        if(plot_y < height() - axes_offset.y()) {
            painter.setPen(QPen(QBrush(color_grid), 1.0, Qt::DotLine));
            painter.drawLine(axes_offset.x() + 1, plot_y, width(), plot_y);
        }
        current_y += plot_stepsize.y;
    }
}


void ygfxqt::YMathOpenGLWidget::mousePressEvent(QMouseEvent * event) {
    if(event->button() == mouse_button_axis_drag) {
        if(event->pos().x() <= axes_offset.x() && event->pos().y() < height() - axes_offset.y()) {
            state_info.setFlag(InternalInfo::InAxisYDragMode);
            setCursor(Qt::ClosedHandCursor);
        } else if(event->pos().y() >= height() - axes_offset.y() && event->pos().x() > axes_offset.x()) {
            state_info.setFlag(InternalInfo::InAxisXDragMode);
            setCursor(Qt::ClosedHandCursor);
        }
    }
    if(event->button() == mouse_button_area_drag) {
        if(event->pos().x() > axes_offset.x() && event->pos().y() < height() - axes_offset.y()) {
            state_info.setFlag(InternalInfo::InAreaDragMode);
            setCursor(Qt::ClosedHandCursor);
        }
    }

    if(!state_info.testFlag(InternalInfo::InAreaDragMode) &&
       !state_info.testFlag(InternalInfo::InAxisXDragMode) &&
       !state_info.testFlag(InternalInfo::InAxisYDragMode)) {
        if(selector_tool != nullptr)  {
            selector_tool->handle_mouse_press_event(event, plotPosition(event->pos()));

            if(selector_tool->select_on_press()) {

                if(event->button() == selector_tool->selectionButton()) {
                    for(auto plot : plots) {
                        if(plot->isVisible()) {
                            auto plot2d = static_cast<YMathGfxPlotSet2D*>(plot);
                            selector_tool->select(plot2d->getXData(), plot2d->getYData(), plot2d);
                        }
                    }
                    update();
                }
            }
        }
    }
}

void ygfxqt::YMathOpenGLWidget::mouseReleaseEvent(QMouseEvent * event) {
    if(selector_tool != nullptr) {
        if(selector_tool->select_on_release())  {
            if(event->button() == selector_tool->selectionButton()) {
                for(auto plot : plots) {
                    if(plot->isVisible()) {
                        auto plot2d = static_cast<YMathGfxPlotSet2D*>(plot);
                        if(x_axis_order == AxisOrder::REVERSED) selector_tool->T_xaxis_reversed = true;
                        selector_tool->select(plot2d->getXData(), plot2d->getYData(), plot2d);
                        selector_tool->T_xaxis_reversed = false;
                    }
                }
                update();
            }
        }
        selector_tool->handle_mouse_release_event(event, plotPosition(event->pos()));
    }

    if(event->button() == mouse_button_axis_drag) {
        state_info.setFlag(InternalInfo::InAxisXDragMode, false);
        state_info.setFlag(InternalInfo::InAxisYDragMode, false);
    }
    if(event->button() == mouse_button_area_drag) {
        state_info.setFlag(InternalInfo::InAreaDragMode, false);
    }

    auto new_cursor = Qt::ArrowCursor;
    if(state_info.testFlag(InternalInfo::MouseAboveXAxis) &&
       options_plot.testFlag(PlotOption::EnableXAxisMove)) {
        new_cursor = Qt::OpenHandCursor;
    } else if(state_info.testFlag(InternalInfo::MouseAboveYAxis) &&
              options_plot.testFlag(PlotOption::EnableYAxisMove)) {
        new_cursor = Qt::OpenHandCursor;
    } else if(options_mouse.testFlag(MouseOption::HideCursorOnPlot)) {
        new_cursor = Qt::BlankCursor;
    }
    if(new_cursor != cursor()) {
        setCursor(new_cursor);
    }
}

void ygfxqt::YMathOpenGLWidget::mouseMoveEvent(QMouseEvent * event) {
    if(selector_tool != nullptr) {
        selector_tool->handle_mouse_move_event(event, plotPosition(event->pos()));
        if(selector_tool->select_on_pressed_move() && event->buttons().testFlag(selector_tool->selectionButton())) {
            for(auto plot : plots) {
                if(plot->isVisible()) {
                    auto plot2d = static_cast<YMathGfxPlotSet2D*>(plot);
                    selector_tool->select(plot2d->getXData(), plot2d->getYData(), plot2d);
                }
            }
            update();
        }
    }

    auto event_plot_pos = plotPosition(event->pos());
    auto delta_position = event->pos() - mouse_position_current_widget;
    auto mouse_plot_position = plotPosition(mouse_position_current_widget);
    auto plot_delta = event_plot_pos - mouse_plot_position;

    state_info.setFlag(InternalInfo::MouseAboveYAxis, event->pos().x() <= axes_offset.x());
    state_info.setFlag(InternalInfo::MouseAboveXAxis, event->pos().y() >= height() - axes_offset.y());

    if(state_info.testFlag(InternalInfo::InAreaDragMode)) {
        moveBoundingRectangle(options_plot.testFlag(PlotOption::EnableXAxisMove) ? -plot_delta.x : 0,
                              options_plot.testFlag(PlotOption::EnableYAxisMove) ? -plot_delta.y : 0);
    }
    if(state_info.testFlag(InternalInfo::InAxisXDragMode) || state_info.testFlag(InternalInfo::InAxisYDragMode)) {
        if(event->buttons().testFlag(mouse_button_axis_drag)) {

            if(state_info.testFlag(InternalInfo::InAxisXDragMode) && options_plot.testFlag(PlotOption::EnableXAxisMove)) {
                if(std::abs(delta_position.x()) < delta_pixel_mousejump) {
                    moveBoundingRectangle(-plot_delta.x, 0);
                    if(event->pos().x() > this->width()) {
                        QCursor::setPos(mapToGlobal(QPoint(2 + axes_offset.x(), event->y())));
                    } else if(event->pos().x() < axes_offset.x()) {
                        QCursor::setPos(mapToGlobal(QPoint(width() - 2, event->y())));
                    }
                }
            } else if(state_info.testFlag(InternalInfo::InAxisYDragMode) && options_plot.testFlag(PlotOption::EnableYAxisMove)) {
                if(std::abs(delta_position.y()) < delta_pixel_mousejump) {
                    moveBoundingRectangle(0, -plot_delta.y);
                    if(event->pos().y() > this->height() - axes_offset.y()) {
                        QPoint point(event->x(), 2);
                        QCursor::setPos(mapToGlobal(point));
                    } else if(event->pos().y() < 0) {
                        QPoint point(event->x(), height() - axes_offset.y() - 2);
                        QCursor::setPos(mapToGlobal(point));
                    }
                }
            }
        }
    }

    if(event->buttons().testFlag(Qt::NoButton)) {
        auto new_cursor = Qt::ArrowCursor;
        if(state_info.testFlag(InternalInfo::MouseAboveXAxis) &&
           options_plot.testFlag(PlotOption::EnableXAxisMove)) {
            new_cursor = Qt::OpenHandCursor;
        } else if(state_info.testFlag(InternalInfo::MouseAboveYAxis) &&
                  options_plot.testFlag(PlotOption::EnableYAxisMove)) {
            new_cursor = Qt::OpenHandCursor;
        } else if(options_mouse.testFlag(MouseOption::HideCursorOnPlot)) {
            new_cursor = Qt::BlankCursor;
        }
        if(new_cursor != cursor()) {
            setCursor(new_cursor);
        }
    }


    mouse_position_current_widget = event->pos();
    update();
    emit mousePlotPositionChanged(mouse_plot_position);
}

void ygfxqt::YMathOpenGLWidget::keyPressEvent(QKeyEvent * event) {
    if(selector_tool != nullptr)
        selector_tool->handle_key_press_event(event);

    update();
}

void ygfxqt::YMathOpenGLWidget::keyReleaseEvent(QKeyEvent * event) {
    if(selector_tool != nullptr)
        selector_tool->handle_key_release_event(event);

    if(options_plot.testFlag(PlotOption::EnableMoveKeys)) {
        bool shiftMod = event->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier);
        double movex = 0;
        double movey = 0;
        double sizex = (bounds_max.x - bounds_min.x) / 10.0;
        double sizey = (bounds_max.y - bounds_min.y) / 10.0;

        if(options_plot.testFlag(PlotOption::EnableXAxisMove)) {
            if(event->key() == Qt::Key_Left) {
                movex = -sizex;
            } else if(event->key() == Qt::Key_Right) {
                movex = sizex;
            }
        }
        if(options_plot.testFlag(PlotOption::EnableYAxisMove)) {
            if(event->key() == Qt::Key_Up) {
                movey = sizey;
            } else if(event->key() == Qt::Key_Down) {
                movey = -sizey;
            }
        }

        if(shiftMod) {
            movex *= 10.0;
            movey *= 10.0;
        }
        moveBoundingRectangle(movex, movey);
    }

    if(options_plot.testFlag(PlotOption::EnableZoomKeys)) {
        if(options_plot.testFlag(PlotOption::EnableXAxisZoom) &&
           options_plot.testFlag(PlotOption::EnableYAxisZoom)) {
            if(event->key() == Qt::Key_Plus) {
                scaleBoundingRectangle(0.9, 0.9, 0.5, 0.5);
            } else if(event->key() == Qt::Key_Minus) {
                scaleBoundingRectangle(1.1, 1.1, 0.5, 0.5);
            }
        }
    }

    update();
}

void ygfxqt::YMathOpenGLWidget::enterEvent(QEvent * event) {
    Q_UNUSED(event);
    if(selector_tool != nullptr) {
        selector_tool->handle_dimension_change(
                    bounds_min, bounds_max,
                    QPoint(current_plot_width, current_plot_height), axes_offset);
    }

    setCursor(Qt::BlankCursor);
    setFocus();
    state_info.setFlag(InternalInfo::MouseAboveWidget, true);
}

void ygfxqt::YMathOpenGLWidget::leaveEvent(QEvent * event) {
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
    state_info.setFlag(InternalInfo::MouseAboveWidget, false);
    update();
}

void ygfxqt::YMathOpenGLWidget::wheelEvent(QWheelEvent *event) {
    if(state_info.testFlag(InternalInfo::MouseAboveXAxis) || state_info.testFlag(InternalInfo::MouseAboveYAxis)) {
        double zoom_x = options_plot.testFlag(PlotOption::EnableXAxisZoom) ? 0.05 : 0.0;
        double zoom_y = options_plot.testFlag(PlotOption::EnableYAxisZoom) ? 0.05 : 0.0;
        // Qt6
        if(event->angleDelta().y() < 0) {
            scaleBoundingRectangle(state_info.testFlag(InternalInfo::MouseAboveXAxis) ? 1.0 + zoom_x : 1.0,
                                   state_info.testFlag(InternalInfo::MouseAboveYAxis) ? 1.0 + zoom_y : 1.0, 0.5, 0.5);
        } else {
            scaleBoundingRectangle(state_info.testFlag(InternalInfo::MouseAboveXAxis) ? 1.0 - zoom_x : 1.0,
                                   state_info.testFlag(InternalInfo::MouseAboveYAxis) ? 1.0 - zoom_y : 1.0, 0.5, 0.5);
        }
    } else {
        bool dont_ignore_mouse_wheel = true;

        if(selector_tool != nullptr) {
            if(event->modifiers().testFlag(selector_tool->get_mwheel_pass_key_modifier())) {
                selector_tool->handle_mouse_wheel_event(event);
                dont_ignore_mouse_wheel = false;
                update();
            }
        }

        if(dont_ignore_mouse_wheel) {
            // Qt6
            auto plot_pos = plotPosition(event->position().toPoint());
            auto sx = (plot_pos.x - bounds_min.x) / plot_size.x;
            auto sy = (plot_pos.y - bounds_min.y) / plot_size.y;
            // Qt6
            double delta = event->angleDelta().y() > 0 ? -0.1 : 0.1;
            double delta_x = options_plot.testFlag(PlotOption::EnableXAxisZoom) ? delta : 0;
            double delta_y = options_plot.testFlag(PlotOption::EnableYAxisZoom) ? delta : 0;

            scaleBoundingRectangle(1.0 + delta_x, 1.0 + delta_y, 1 - sx, 1 - sy);
        }
    }
}
