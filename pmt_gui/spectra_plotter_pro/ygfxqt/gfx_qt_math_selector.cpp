#include "gfx_qt_math_selector.h"
#include "gfx_qt_helper.h"

#include <QDebug>
#include <iostream>
#include <future>

using namespace ygfxqt;

inline int orient(glm::dvec2 const & p, glm::dvec2 const & q, glm::dvec2 const & r) {
    auto val = (q.y - p.y) * (r.x - q.x) -
               (q.x - p.x) * (r.y - q.y);

    if(val < std::numeric_limits<double>::epsilon()) {
        return 0;
    } else {
        return val > 0 ? -1 : 1;
    }
}

inline bool intersect(glm::dvec2 const & s1a, glm::dvec2 const & s1b, glm::dvec2 const & s2a, glm::dvec2 const & s2b) {
    return (!(orient(s1a, s1b, s2a) == orient(s1a, s1b, s2b) ||
              orient(s2a, s2b, s1a) == orient(s2a, s2b, s1b)));
}


void YMathWidgetSelector::select(const std::vector<double> * xdata,
                                 const std::vector<double> * ydata, YMathGfxPlot * plot) {
    std::vector<size_t> indices; indices.reserve(std::min(xdata->size(), ydata->size()) / 3);

    if(selection_mode_difference || selection_mode_union) {
        STOPWATCH("time_selection_calculation: ");
        before_selection_calculation(xdata, ydata);

        size_t const max_threads = 12;

        auto min = std::min(xdata->size(), ydata->size());
        auto step = std::max(size_t(1), min / max_threads);

        std::vector<std::future<std::vector<size_t>>> futures;
        futures.reserve(max_threads + 1);

        for(size_t index = 0; index < min; index += step) {
            size_t end_index = std::min(min - 1, index + min / max_threads - 1);

            futures.push_back(std::async([index, end_index, this, xdata, ydata]()->std::vector<size_t> {
                std::vector<size_t> l_indices; l_indices.reserve(1250);
                for(auto i = index; i <= end_index; ++i) {
                    if(this->selection_function(xdata, ydata, i)) {
                        l_indices.push_back(i);
                    }
                }
                return l_indices;
            }));
        }

        for(auto & f : futures) {
            std::vector<size_t> const & gIndices = f.get();
            indices.insert(indices.end(), gIndices.begin(), gIndices.end());
        }

        after_selection_calculation();
    }

    if(indices.size() > 0) {
        if(selection_mode_union) {
            emit selectionOccured(indices, SelectionAction::ADDED_TO_SELECTION, plot);
        } else if(selection_mode_difference) {
            emit selectionOccured(indices, SelectionAction::REMOVED_FROM_SELECTION, plot);
        }
    }
}

void YMathWidgetSelector::handle_mouse_press_event(QMouseEvent *event, glm::dvec2 plot_pos) {
    mouse_position_on_click = event->pos();
    mouse_position_plot_on_click = plot_pos;
    if(event->button() == mousebutton_use) {
        mouse_usebutton_pressed = true;
    }
}

void YMathWidgetSelector::handle_mouse_release_event(QMouseEvent *event, glm::dvec2 plot_pos) {
    Q_UNUSED(plot_pos);

    if(event->button() == mousebutton_use) {
        mouse_usebutton_pressed = false;
    }
}

void YMathWidgetSelector::handle_mouse_move_event(QMouseEvent *event, glm::dvec2 plot_pos) {
    mouse_position_widget = event->pos();
    mouse_position_plot = glm::dvec2(plot_pos.x, plot_pos.y);
}

void YMathWidgetSelector::handle_mouse_wheel_event(QWheelEvent *event) {
    Q_UNUSED(event);
}

void YMathWidgetSelector::handle_key_press_event(QKeyEvent *event) {
    if(key_modifier_mode == YMathSelectorModifierMode::HOLD_KEY_TO_SET_MODE) {
        if(event->key() == key_selection_union) {
            set_selection_mode(YMathSelectionMode::UNION);
        } else if(event->key() == key_selection_difference) {
            set_selection_mode(YMathSelectionMode::DIFF);
        }
    }
}

void YMathWidgetSelector::handle_key_release_event(QKeyEvent *event) {
    if(event->key() == key_selection_cancel) {
        cancel_action();
    }
    if(key_modifier_mode == YMathSelectorModifierMode::PRESS_KEY_TO_TOGGLE_MODE) {
        if(event->key() == key_selection_union) {
            set_selection_mode(YMathSelectionMode::UNION);
        } else if(event->key() == key_selection_difference) {
            set_selection_mode(YMathSelectionMode::DIFF);
        }
    } else if(key_modifier_mode == YMathSelectorModifierMode::HOLD_KEY_TO_SET_MODE) {
        if(event->key() == key_selection_union) {
            selection_mode_union = false;
        } else if(event->key() == key_selection_difference) {
            selection_mode_difference = false;
        }

        if(!(selection_mode_union || selection_mode_difference)) {
            set_selection_mode(YMathSelectionMode::NORMAL_MODE);
        }
    }
}

void YMathWidgetSelector::handle_dimension_change(glm::dvec2 bounds_min, glm::dvec2 bounds_max, QPoint new_widget_size, QPoint offset_size) {
    plot_bounds_min = bounds_min;
    plot_bounds_max = bounds_max;
    widget_size = new_widget_size;
    axes_size = offset_size;
}

void YMathWidgetSelector::set_selection_mode(YMathSelectionMode mode) {
    if(mode == YMathSelectionMode::UNION) {
        selection_mode_union = true;
        selection_mode_difference = false;
    } else if(mode == YMathSelectionMode::NORMAL_MODE) {
        selection_mode_union = false;
        selection_mode_difference = false;
    } else if(mode == YMathSelectionMode::DIFF) {
        selection_mode_union = false;
        selection_mode_difference = true;
    }
    selection_mode = mode;
    emit selectionModeChanged(mode);
}

void YMathWidgetSelectorEllipse::handle_key_press_event(QKeyEvent *event) {
    YMathWidgetSelector::handle_key_press_event(event);
    if(event->key() == key_size_decrease) {
        radius = std::max(radius_min, radius - 1);
    } else if(event->key() == key_size_increase) {
        radius = std::min(radius_max, radius + 1);
    }
}

void YMathWidgetSelectorEllipse::handle_mouse_wheel_event(QWheelEvent *event) {
    YMathWidgetSelector::handle_mouse_wheel_event(event);
    // Qt6
    //if(event->delta() > 0) {
    if(event->angleDelta().y() > 0) {
        radius = std::max(radius_min, radius - 2);
    } else {
        radius = std::min(radius_max, radius + 2);
    }
}

void YMathWidgetSelectorEllipse::paint_selection(QPainter &painter) {
    glm::vec4 color(get_current_selector_color(), 1.0);

    painter.setPen(helper::glm_vec4_to_qcolor(color));
    painter.drawEllipse(mouse_position_widget.x() - radius, mouse_position_widget.y() - radius, radius * 2, radius * 2);
}

void YMathWidgetSelectorEllipse::cancel_action() {
    set_selection_mode(YMathSelectionMode::NORMAL_MODE);
}

bool YMathWidgetSelectorEllipse::selection_function(const std::vector<double> * xdata,
                                const std::vector<double> * ydata, size_t index) {
    auto & px = xdata->at(index);
    auto & py = ydata->at(index);
    return ((px - cx) * (px - cx)) / (rx * rx) +
           ((py - cy) * (py - cy)) / (ry * ry) <= 1.0;
}

void YMathWidgetSelectorEllipse::before_selection_calculation(std::vector<double> const * xdata,
                                                              std::vector<double> const * ydata) {
    Q_UNUSED(xdata); Q_UNUSED(ydata);
    rx = (plot_bounds_max.x - plot_bounds_min.x) * (radius / static_cast<double>(widget_size.x()));
    ry = (plot_bounds_max.y - plot_bounds_min.y) * (radius / static_cast<double>(widget_size.y()));
    cx = mouse_position_plot.x;
    cy = mouse_position_plot.y;
}

glm::dvec2 YMathWidgetSelectorEllipse::selection_min() {
    return glm::dvec2(mouse_position_plot.x - rx, mouse_position_plot.y - ry);
}

glm::dvec2 YMathWidgetSelectorEllipse::selection_max() {
    return glm::dvec2(mouse_position_plot.x + rx, mouse_position_plot.y + ry);
}

void YMathWidgetSelectorRectangle::paint_selection(QPainter & painter) {
    glm::vec3 color = get_current_selector_color();

    QRect rectangle(mouse_position_on_click, mouse_position_widget);

    auto brush_color = helper::glm_vec3_to_qcolor(color);
    brush_color.setAlpha(color == color_selection_default ? 0 : 100);

    painter.fillRect(rectangle, QBrush(brush_color));
    painter.setPen(Qt::black);
    painter.drawRect(rectangle);
}

void YMathWidgetSelectorRectangle::cancel_action() {
    set_selection_mode(YMathSelectionMode::NORMAL_MODE);
}

bool YMathWidgetSelectorRectangle::selection_function(const std::vector<double> * xdata,
                                const std::vector<double> * ydata, size_t index) {
    auto sel_min = selection_min();
    auto sel_max = selection_max();
    auto & px = xdata->at(index);
    auto & py = ydata->at(index);
    if(px >= sel_min.x && px <= sel_max.x) {
        if(py >= sel_min.y && py <= sel_max.y) {
            return true;
        }
    }
    return false;
}

glm::dvec2 YMathWidgetSelectorRectangle::selection_min() {
    return glm::dvec2(std::min(mouse_position_plot_on_click.x, mouse_position_plot.x),
                                    std::min(mouse_position_plot_on_click.y, mouse_position_plot.y));
}

glm::dvec2 YMathWidgetSelectorRectangle::selection_max() {
    return glm::dvec2(std::max(mouse_position_plot_on_click.x, mouse_position_plot.x),
                                    std::max(mouse_position_plot_on_click.y, mouse_position_plot.y));
}

void YMathWidgetSelectorFreeHand::paint_selection(QPainter & painter) {
    current_selection_points.push_back(mouse_position_widget);

    glm::vec3 color = get_current_selector_color();

    painter.setPen(QPen(helper::glm_vec3_to_qcolor(color), 2.0));
    for(size_t i = 1; i < current_selection_points.size(); ++i) {
        painter.drawLine(current_selection_points[i - 1], current_selection_points[i]);
    }
    if(current_selection_points.size() > 1) {
        painter.drawLine(current_selection_points.front(), current_selection_points.back());
    }

    current_selection_points.pop_back();
}

void YMathWidgetSelectorFreeHand::cancel_action() {
    current_selection_points.clear();
}


bool YMathWidgetSelectorFreeHand::selection_function(const std::vector<double> * xdata,
                                                     const std::vector<double> * ydata, size_t index) {
    glm::dvec2 point(xdata->at(index), ydata->at(index));
    glm::dvec2 point_inf(1e25, ydata->at(index));

    int count = 0;
    for(size_t i = 0; i < selection_world_coords.size(); ++i) {
        glm::dvec2 segment_begin = selection_world_coords[i];
        glm::dvec2 segment_end = selection_world_coords[(i + 1) % selection_world_coords.size()];

        if (intersect(segment_begin, segment_end, point, point_inf)) {
            ++count;
        }
    }

    return (count % 2 == 1);
}

void YMathWidgetSelectorFreeHand::after_selection_calculation() {
    selection_world_coords.clear();
    current_selection_points.pop_back();
}

void YMathWidgetSelectorFreeHand::before_selection_calculation(const std::vector<double> * xdata,
                                                               const std::vector<double> * ydata) {
    Q_UNUSED(xdata); Q_UNUSED(ydata);
    current_selection_points.push_back(mouse_position_widget);

    auto plot_size = plot_bounds_max - plot_bounds_min;
    for(auto & widget_position : current_selection_points) {
        double relx = ((widget_position.x() - axes_size.x()) / static_cast<double>(widget_size.x()));
        double rely = ((widget_size.y() - widget_position.y()) / static_cast<double>(widget_size.y()));
        auto plotx = plot_bounds_min.x + plot_size.x * relx;
        auto ploty = plot_bounds_min.y + plot_size.y * rely;
        if(T_xaxis_reversed) {
            plotx = plot_bounds_max.x - plot_size.x * relx;
        }

        selection_world_coords.push_back(glm::dvec2(plotx, ploty));
    }
}

glm::dvec2 YMathWidgetSelectorFreeHand::selection_min() {
    auto plot_size = plot_bounds_max - plot_bounds_min;
    auto plotx = plot_bounds_min.x + plot_size.x * ((min_p.x() - axes_size.x()) / static_cast<double>(widget_size.x()));
    auto ploty = plot_bounds_min.y + plot_size.y * ((widget_size.y() - max_p.y()) / static_cast<double>(widget_size.y()));
    return glm::dvec2(plotx, ploty);
}

glm::dvec2 YMathWidgetSelectorFreeHand::selection_max() {
    auto plot_size = plot_bounds_max - plot_bounds_min;
    auto plotx = plot_bounds_min.x + plot_size.x * ((max_p.x() - axes_size.x()) / static_cast<double>(widget_size.x()));
    auto ploty = plot_bounds_min.y + plot_size.y * ((widget_size.y() - min_p.y()) / static_cast<double>(widget_size.y()));
    return glm::dvec2(plotx, ploty);
}

void YMathWidgetSelectorFreeHand::handle_mouse_press_event(QMouseEvent * event, glm::dvec2 plot_pos) {
    YMathWidgetSelector::handle_mouse_press_event(event, plot_pos);
    current_selection_points.push_back(event->pos());
    max_p = event->pos();
    min_p = event->pos();
}

void YMathWidgetSelectorFreeHand::handle_mouse_move_event(QMouseEvent * event, glm::dvec2 plot_pos) {
    YMathWidgetSelector::handle_mouse_move_event(event, plot_pos);
    if(mouse_usebutton_pressed) {
        auto vec_a = helper::qpoint_to_glm_dvec2(current_selection_points.back());
        auto vec_b = helper::qpoint_to_glm_dvec2(event->pos());
        double dist = glm::length(vec_a - vec_b);
        if(dist > line_length_threshold) {
            current_selection_points.push_back(event->pos());
            max_p.rx() = std::max(current_selection_points.back().x(), max_p.x());
            max_p.ry() = std::max(current_selection_points.back().y(), max_p.y());
            min_p.rx() = std::min(current_selection_points.back().x(), min_p.x());
            min_p.ry() = std::min(current_selection_points.back().y(), min_p.y());
        }
    }
}

void YMathWidgetSelectorFreeHand::handle_mouse_release_event(QMouseEvent * event, glm::dvec2 plot_pos) {
    YMathWidgetSelector::handle_mouse_release_event(event, plot_pos);
    current_selection_points.clear();
}

void YMathWidgetSelectorRange::paint_selection(QPainter & painter) {
    glm::vec3 color = get_current_selector_color();
    if(mouse_usebutton_pressed) {
        QColor qColor = helper::glm_vec3_to_qcolor(color);

        painter.setPen(qColor);
        qColor.setAlpha(100);
        if(current_axis == RangeSelectorAxis::X_AXIS) {
            painter.drawRect(mouse_position_on_click.x(), -100, mouse_position_widget.x() - mouse_position_on_click.x(), 3000);
            painter.fillRect(mouse_position_on_click.x(), -100, mouse_position_widget.x() - mouse_position_on_click.x(), 3000, QBrush(qColor));
        } else {
            painter.drawRect(-100, mouse_position_on_click.y(), 3000, mouse_position_widget.y() - mouse_position_on_click.y());
            painter.fillRect(-100, mouse_position_on_click.y(), 3000, mouse_position_widget.y() - mouse_position_on_click.y(), QBrush(qColor));
        }
    } else {
        painter.setPen(helper::glm_vec3_to_qcolor(color));
        if(current_axis == RangeSelectorAxis::X_AXIS) {
            painter.drawLine(mouse_position_widget.x(), -10000, mouse_position_widget.x(), 10000);
        } else {
            painter.drawLine(-10000, mouse_position_widget.y(), 10000, mouse_position_widget.y());
        }
    }
}

void YMathWidgetSelectorRange::cancel_action() {

}

void YMathWidgetSelectorRange::before_selection_calculation(const std::vector<double> * xdata,
                                                            const std::vector<double> * ydata) {
    selection_point_min.x = std::min(mouse_position_plot.x, mouse_position_plot_on_click.x);
    selection_point_min.y = std::min(mouse_position_plot.y, mouse_position_plot_on_click.y);
    selection_point_max.x = std::max(mouse_position_plot.x, mouse_position_plot_on_click.x);
    selection_point_max.y = std::max(mouse_position_plot.y, mouse_position_plot_on_click.y);

    // ### replace 0.0025 by some smarter constant later (bs)
    if(current_axis == X_AXIS) {
        double miny = 1e30;
        double maxy = -1e30;
        for(size_t i = 0; i < std::min(xdata->size(), ydata->size()); ++i) {
            miny = std::min(ydata->at(i), miny);
            maxy = std::max(ydata->at(i), maxy);
        }
        selection_point_min.y = miny - 0.0025;
        selection_point_max.y = maxy + 0.0025;
    } else {
        double minx = 1e30;
        double maxx = -1e30;
        for(size_t i = 0; i < std::min(xdata->size(), ydata->size()); ++i) {
            minx = std::min(xdata->at(i), minx);
            maxx = std::max(xdata->at(i), maxx);
        }
        selection_point_min.x = minx - 0.0025;
        selection_point_max.x = maxx + 0.0025;
    }
}

void YMathWidgetSelectorRange::handle_key_release_event(QKeyEvent * event) {
    YMathWidgetSelector::handle_key_release_event(event);
    if(!mouse_usebutton_pressed) {
        if(event->key() == key_selection_special_key) {
            toggle_range_axis();
        }
    }
}

glm::dvec2 YMathWidgetSelectorRange::selection_min() {
    return glm::dvec2(selection_point_min.x, selection_point_min.y);
}

glm::dvec2 YMathWidgetSelectorRange::selection_max() {
    return glm::dvec2(selection_point_max.x, selection_point_max.y);
}

bool YMathWidgetSelectorRange::selection_function(std::vector<double> const * xdata,
                                                  std::vector<double> const * ydata, size_t index) {
    bool ret = false;
    if(current_axis == X_AXIS) {
        auto x = xdata->at(index);
        ret = (x >= selection_point_min.x && x <= selection_point_max.x);
    } else {
        auto y = ydata->at(index);
        ret = (y >= selection_point_min.y && y <= selection_point_max.y);
    }
    return ret;
}

glm::vec3 YMathWidgetSelector::get_current_selector_color() {
    glm::vec3 color = color_selection_default;
    if(selection_mode_difference)   color = color_selection_difference;
    if(selection_mode_union)        color = color_selection_union;
    return color;
}
