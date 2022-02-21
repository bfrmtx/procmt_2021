#include "plot.h"
#include "qthelper.h"

QString const layer_selection = "layer_selection";

std::vector<QPen> const readout_pens = {QPen(Qt::blue, 0.8),
                                        QPen(Qt::darkRed, 0.8),
                                        QPen(Qt::darkGreen, 0.8),
                                        QPen(Qt::cyan, 0.8),
                                        QPen(Qt::yellow, 0.8),
                                        QPen(Qt::magenta, 0.8)};

QColor const color_ival_selection   = QColor(0, 255, 0, 55);
QColor const color_ival_remove      = QColor(255, 0, 0, 55);
QColor const color_ival             = QColor(0, 0, 0, 35);

double const rectangle_min  = -1e10;
double const rectangle_max  = 1e10;

using namespace TS_NAMESPACE_NAME;

bool is_interval_mode(CursorMode mode) {
    return mode == CursorMode::SELECTION_ADD ||
           mode == CursorMode::SELECTION_REMOVE;
}

QCPItemRect * create_selection_rectangle(TSPlotterPlot * plot, SelectionInterval const & interval) {
    auto ret = new QCPItemRect(plot);
    ret->topLeft->setCoords(interval.begin, rectangle_max);
    ret->bottomRight->setCoords(interval.end, rectangle_min);
    ret->setLayer(layer_selection);
    ret->setPen(QPen(Qt::black));
    ret->setBrush(QBrush(color_ival));
    ret->setVisible(true);

    plot->layer(layer_selection)->replot();
    return ret;
}

TSPlotterPlot::TSPlotterPlot(QWidget * parent) : QCustomPlot(parent) {
    addLayer(layer_selection);

    m_vertical_line = new QCPItemLine(this);
    m_vertical_line->setLayer(layer_selection);
    m_vertical_line->setPen(readout_pens.front());

    m_mouse_rect = new QCPItemRect(this);
    m_mouse_rect->setLayer(layer_selection);

    set_selection_coords(rectangle_min, rectangle_min);
    set_cursor_coords(rectangle_min, rectangle_min);
    set_cursor_mode(CursorMode::DEFAULT);
    setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
}

void TSPlotterPlot::set_cursor_mode(CursorMode new_mode) {
    m_cursor_mode = new_mode;
    switch(m_cursor_mode) {
        case CursorMode::DEFAULT: {
            for(auto hline : m_horizontal_lines) hline->setVisible(true);
            m_vertical_line->setVisible(true);
            m_mouse_rect->setVisible(false);
            break;
        }
        case CursorMode::SELECTION_ADD: {
            set_selection_color(color_ival_selection);
            for(auto hline : m_horizontal_lines) hline->setVisible(false);
            m_vertical_line->setVisible(false);
            m_mouse_rect->setVisible(true);
            break;
        }
        case CursorMode::SELECTION_REMOVE: {
            set_selection_color(color_ival_remove);
            for(auto hline : m_horizontal_lines) hline->setVisible(false);
            m_vertical_line->setVisible(false);
            m_mouse_rect->setVisible(true);
            break;
        }
    }
    layer(layer_selection)->replot();
}

void TSPlotterPlot::set_plot_type(TSPlotType new_type) {
    m_plot_type = new_type;
}

void TSPlotterPlot::mousePressEvent(QMouseEvent * event) {
    QCustomPlot::mousePressEvent(event);
    if(event->button() == Qt::LeftButton) {
        m_pressed_pos_x = xAxis->pixelToCoord(event->position().x());
        layer(layer_selection)->replot();
    } else if(event->button() == Qt::MiddleButton) {
        event->ignore();
    }
}

void TSPlotterPlot::mouseReleaseEvent(QMouseEvent * event) {
    QCustomPlot::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton) {
        if(is_interval_mode(m_cursor_mode)) {
            handle_current_interval_dropped(m_pressed_pos_x, xAxis->pixelToCoord(event->position().x()));
            set_selection_coords(rectangle_min, rectangle_min);
        } else if(m_cursor_mode == CursorMode::READOUT) {
            set_cursor_coords(rectangle_min, rectangle_min);
        }
    }
    layer(layer_selection)->replot();
}

void TSPlotterPlot::mouseMoveEvent(QMouseEvent * event) {
    QCustomPlot::mouseMoveEvent(event);
    if(m_cursor_mode == CursorMode::READOUT && event->buttons().testFlag(Qt::LeftButton)) {
        handle_readout(event->position().x(), event->position().y());
    } else if(is_interval_mode(m_cursor_mode)) {
        handle_current_interval_moved(event->position().x(), event->buttons().testFlag(Qt::LeftButton));
    }
    layer(layer_selection)->replot();
}

void TSPlotterPlot::wheelEvent(QWheelEvent * event) {
    QCustomPlot::wheelEvent(event);
    event->ignore();
}

void TSPlotterPlot::enterEvent(QEnterEvent * event) {
    QCustomPlot::enterEvent(event);
}

void TSPlotterPlot::leaveEvent(QEvent * event) {
    QCustomPlot::leaveEvent(event);
    set_selection_coords(rectangle_min, rectangle_min);
    set_cursor_coords(rectangle_min, rectangle_min);
    layer(layer_selection)->replot();
}

void TSPlotterPlot::add_selection_interval(int64_t a, int64_t b) {
    SelectionInterval new_interval(a, b);

    for(auto iterator = m_intervals.begin(); iterator != m_intervals.end(); ) {
        auto & interval = (*iterator).first;
        auto & rectangle = (*iterator).second;

        if(SelectionInterval::overlap(interval, new_interval)) {
            new_interval += interval;
            removeItem(rectangle);
            iterator = m_intervals.erase(iterator);
        } else {
            ++iterator;
        }
    }

    auto new_rectangle = create_selection_rectangle(this, new_interval);
    m_intervals.insert(std::make_pair(new_interval, new_rectangle));
}

void TSPlotterPlot::remove_selection_interval(int64_t a, int64_t b) {
    std::vector<SelectionInterval> new_intervals;
    SelectionInterval remover(a, b);

    for(auto iterator = m_intervals.begin(); iterator != m_intervals.end(); ) {
        auto & interval = (*iterator).first;
        auto & rectangle = (*iterator).second;

        if(SelectionInterval::overlap(interval, remover)) {
            auto intervals = SelectionInterval::difference(interval, remover);
            new_intervals.insert(new_intervals.end(), intervals.begin(), intervals.end());
            removeItem(rectangle);
            iterator = m_intervals.erase(iterator);
        } else {
            ++iterator;
        }
    }
    for(auto & new_interval : new_intervals) {
        auto new_rectangle = create_selection_rectangle(this, new_interval);
        m_intervals.insert(std::make_pair(new_interval, new_rectangle));
    }
    layer(layer_selection)->replot();
}

void TSPlotterPlot::clear_selection() {
    for(auto & interval : m_intervals) {
        delete interval.second;
    }
    m_intervals.clear();
}

void TSPlotterPlot::set_selection_coords(double min, double max) {
    m_mouse_rect->topLeft->setCoords(min, rectangle_max);
    m_mouse_rect->bottomRight->setCoords(max, rectangle_min);
}

void TSPlotterPlot::set_selection_color(const QColor & color) {
    m_mouse_rect->setBrush(QBrush(color));
    m_mouse_rect->setPen(QPen(color));
}

void TSPlotterPlot::set_cursor_coords(double x, double y) {
    m_vertical_line->start->setCoords(x, rectangle_min);
    m_vertical_line->end->setCoords(x, rectangle_max);
    for(auto horz : m_horizontal_lines) {
        horz->start->setCoords(xAxis->range().lower, y);
        horz->end->setCoords(xAxis->range().upper, y);
    }
}

void TSPlotterPlot::set_cursor_coords(double x, const std::vector<double> & values) {
    for(auto i = m_horizontal_lines.size(); i < values.size(); ++i) {
        auto horizontal_line = new QCPItemLine(this);
        horizontal_line->setLayer(layer_selection);
        horizontal_line->setPen(readout_pens[i % readout_pens.size()]);
        m_horizontal_lines.push_back(horizontal_line);
    }
    m_vertical_line->start->setCoords(x, yAxis->range().lower);
    m_vertical_line->end->setCoords(x, yAxis->range().upper);
    for(size_t i = 0; i < values.size(); ++i) {
        auto y = values[i];
        auto & hLine = m_horizontal_lines[i];
        hLine->start->setCoords(xAxis->range().lower, y);
        hLine->end->setCoords(xAxis->range().upper, y);
        hLine->setVisible(true);
    }
}

void TSPlotterPlot::handle_current_interval_moved(int new_x, bool pressed) {    
    auto ax = std::round(m_pressed_pos_x / m_interval_cursor_step) * m_interval_cursor_step;
    auto bx = std::round(xAxis->pixelToCoord(new_x) / m_interval_cursor_step) * m_interval_cursor_step;
    double min = std::min(ax, bx);
    double max = std::max(ax, bx);
    double cur = std::round(bx);

    set_selection_coords(pressed ? min : cur, pressed ? max : cur);
    emit interval_selection_changed(static_cast<int64_t>(pressed ? min : cur),
                                    static_cast<int64_t>(pressed ? max : cur));
}

void TSPlotterPlot::handle_current_interval_dropped(double a, double b) {
    auto ax = std::round(a / m_interval_cursor_step) * m_interval_cursor_step;
    auto bx = std::round(b / m_interval_cursor_step) * m_interval_cursor_step;
    auto min = static_cast<int64_t>(std::round(std::min(ax, bx)));
    auto max = static_cast<int64_t>(std::round(std::max(ax, bx)));

    //! @todo check YASC : min can be negative --- maybe plot/screen precision; set to 0 in case

    if (min < 0) min = 0;

    if(m_cursor_mode == CursorMode::SELECTION_REMOVE) {
        remove_selection_interval(min, max);
        emit interval_removed(min, max);
    } else if(m_cursor_mode == CursorMode::SELECTION_ADD) {
        add_selection_interval(min, max);
        emit interval_added(min, max);
    }
}

void TSPlotterPlot::handle_readout(int x, int y) {
    Q_UNUSED(y);
    double xval = xAxis->pixelToCoord(x);
    std::vector<double> coords;
    QCPGraphData p;
    for(int graphno = 0; graphno < graphCount(); ++graphno) {
        auto it = std::lower_bound(graph(graphno)->data()->begin(), graph(graphno)->data()->end(),
                                   xval, [](QCPGraphData & d, const double val) { return d.key < val; });
        p = (*it);
        if(it != graph(graphno)->data()->begin()) {
            auto p2 = *(it -= 1);
            if(std::abs(p.key - xval) > std::abs(p2.key - xval)) p = p2;
        }
        coords.push_back(p.value);

        emit plot_position_changed(p.key, p.value);
    }
    set_cursor_coords(p.key, coords);
}
