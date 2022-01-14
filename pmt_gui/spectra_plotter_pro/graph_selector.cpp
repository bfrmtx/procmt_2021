#include "graph_selector.h"

enum class VectorOrientation {
    COLINEAR,
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

static VectorOrientation orientation(QPointF const & p, QPointF const & q, QPointF const & r) {
    auto val = (q.y() - p.y()) * (r.x() - q.x()) -
               (q.x() - p.x()) * (r.y() - q.y());

    if(val < std::numeric_limits<double>::epsilon()) {
        return VectorOrientation::COLINEAR;
    } else {
        return val > 0 ? VectorOrientation::CLOCKWISE : VectorOrientation::COUNTER_CLOCKWISE;
    }
}

static bool doIntersectIgnoreFPointPrecision(QPointF const & s1a, QPointF const & s1b, QPointF const & s2a, QPointF const & s2b) {
    auto o1 = orientation(s1a, s1b, s2a);
    auto o2 = orientation(s1a, s1b, s2b);
    if(o1 == o2) return false;

    auto o3 = orientation(s2a, s2b, s1a);
    auto o4 = orientation(s2a, s2b, s1b);
    if(o3 == o4) return false;

    return true;
}

void GraphWidgetSelector::select(size_t graph_id, AdditionalPlotData * plot) {
    before_selection_calculation(plot);
    QPointF sel_min = selection_min(plot->plot);
    QPointF sel_max = selection_max(plot->plot);

    auto const & grid = plot->selection_grid;
    auto grid_min = plot->selection_grid_min;
    auto cellsize = plot->selection_grid_step;
    auto gridsize = plot->selection_grid_size;

    int index_min_x = static_cast<int>(std::floor((sel_min.x() - grid_min.x()) / cellsize.x()));
    int index_min_y = static_cast<int>(std::floor((sel_min.y() - grid_min.y()) / cellsize.y()));
    int index_max_x = static_cast<int>(std::floor((sel_max.x() - grid_min.x()) / cellsize.x()));
    int index_max_y = static_cast<int>(std::floor((sel_max.y() - grid_min.y()) / cellsize.y()));

    if(index_min_x <= gridsize.x() - 1 && 0 <= index_max_x) {
        index_min_x = std::max(0, index_min_x);
        index_max_x = std::min(gridsize.x() - 1, index_max_x);
    }
    if(index_min_y <= gridsize.y() - 1 && 0 <= index_max_y) {
        index_min_y = std::max(0, index_min_y);
        index_max_y = std::min(gridsize.y() - 1, index_max_y);
    }

    std::unordered_set<size_t> indices;

    for(int index_x = index_min_x; index_x <= index_max_x; ++index_x) {
        for(int index_y = index_min_y; index_y <= index_max_y; ++index_y) {
            if(index_x >= 0 && index_y >= 0 && index_x < gridsize.x() && index_y < gridsize.y()) {
                auto ix = static_cast<unsigned int>(index_x);
                auto iy = static_cast<unsigned int>(index_y);

                if(ix < grid.size() && iy < grid[ix].size()) {
                    for(size_t i = 0; i < grid[ix][iy].size(); ++i) {
                        auto index = grid[ix][iy][i];
                        if(selection_function(plot, index)) {
                            if(selection_mode_union) {                               
                                if(current_selected_indices[plot].find(index) == current_selected_indices[plot].end()) {
                                    if(plot->select_datapoint(index)) {
                                        current_selected_indices[plot].insert(index);
                                        indices.insert(index);
                                    }
                                }
                            } else if(selection_mode_difference) {
                                if(current_deselected_indices[plot].find(index) == current_deselected_indices[plot].end()) {
                                    if(plot->deselect_datapoint(index)) {
                                        current_deselected_indices[plot].insert(index);
                                        indices.insert(index);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(indices.size() > 0) {
        if(selection_mode_difference) {
            emit indices_deselected(graph_id, indices);
        } else if(selection_mode_union) {
            emit indices_selected(graph_id, indices);
        }
    }
    after_selection_calculation();
}

void GraphWidgetSelector::handle_mouse_press_event(QMouseEvent *event, QPointF plot_pos) {
    mouse_position_on_click = event->pos();
    mouse_position_plot_on_click = plot_pos;
    if(event->button() == mousebutton_use) {
        mouse_usebutton_pressed = true;
    }
}

void GraphWidgetSelector::handle_mouse_release_event(QMouseEvent *event, QPointF plot_pos) {
    Q_UNUSED(plot_pos);
    for(auto & iterator : current_selected_indices) {
        auto plot = iterator.first;
        if(iterator.second.size() > 0) {
            current_selected_indices[plot].clear();
        }
    }
    for(auto & iterator : current_deselected_indices) {
        auto plot = iterator.first;
        if(iterator.second.size() > 0) {
            current_deselected_indices[plot].clear();
        }
    }
    current_selected_indices.clear();
    current_deselected_indices.clear();
    short_time_memory.clear();

    if(event->button() == mousebutton_use) {
        mouse_usebutton_pressed = false;
    }
}

void GraphWidgetSelector::handle_mouse_move_event(QMouseEvent *event, QPointF plot_pos) {
    mouse_position_widget = event->pos();
    mouse_position_plot = plot_pos;
}

void GraphWidgetSelector::handle_mouse_wheel_event(QWheelEvent *event) {
    Q_UNUSED(event);
}

void GraphWidgetSelector::handle_key_press_event(QKeyEvent *event) {
    if(key_modifier_mode == GraphWidgetSelectionModifierMode::HOLD_KEY_TO_SET_MODE) {
        if(event->key() == key_selection_union) {
            set_selection_mode(GraphWidgetSelectionMode::UNION);
        } else if(event->key() == key_selection_difference) {
            set_selection_mode(GraphWidgetSelectionMode::DIFF);
        }
    }
}

void GraphWidgetSelector::handle_key_release_event(QKeyEvent *event) {
    if(event->key() == key_selection_cancel) {
        cancel_action();
    }
    if(key_modifier_mode == GraphWidgetSelectionModifierMode::PRESS_KEY_TO_TOGGLE_MODE) {
        if(event->key() == key_selection_union) {
            set_selection_mode(GraphWidgetSelectionMode::UNION);
        } else if(event->key() == key_selection_difference) {
            set_selection_mode(GraphWidgetSelectionMode::DIFF);
        }
    } else if(key_modifier_mode == GraphWidgetSelectionModifierMode::HOLD_KEY_TO_SET_MODE) {
        if(event->key() == key_selection_union) {
            selection_mode_union = false;
        } else if(event->key() == key_selection_difference) {
            selection_mode_difference = false;
        }

        if(!(selection_mode_union || selection_mode_difference)) {
            set_selection_mode(GraphWidgetSelectionMode::NORMAL_MODE);
        }
    }
}

void GraphWidgetSelector::handle_dimension_change(QPointF bounds_min, QPointF bounds_max, QPoint new_widget_size, QPoint offset_size) {
    plot_bounds_min = bounds_min;
    plot_bounds_max = bounds_max;
    widget_size = new_widget_size;
    axes_size = offset_size;
}

void GraphWidgetSelector::set_selection_mode(GraphWidgetSelectionMode mode) {
    if(mode == GraphWidgetSelectionMode::NORMAL_MODE) {
            selection_mode_union = false;
            selection_mode_difference = false;
    } else if(mode == GraphWidgetSelectionMode::UNION) {
        selection_mode_union = true;
        selection_mode_difference = false;
    } else if(mode == GraphWidgetSelectionMode::DIFF) {
        selection_mode_union = false;
        selection_mode_difference = true;
    }
    selection_mode = mode;
    emit selectionModeChanged(mode);
}

void GraphWidgetSelectorBrushEllipse::handle_key_press_event(QKeyEvent *event) {
    GraphWidgetSelector::handle_key_press_event(event);
    if(event->key() == key_size_decrease) {
        radius = std::max(radius_min, radius - 1);
    } else if(event->key() == key_size_increase) {
        radius = std::min(radius_max, radius + 1);
    }
}

void GraphWidgetSelectorBrushEllipse::handle_mouse_wheel_event(QWheelEvent *event) {
    GraphWidgetSelector::handle_mouse_wheel_event(event);
    if(event->delta() > 0) {
        radius = std::max(radius_min, radius - 2);
    } else {
        radius = std::min(radius_max, radius + 2);
    }
}

void GraphWidgetSelectorBrushEllipse::paint_selection(QCPPainter & painter) {
    painter.setPen(get_current_selector_color());
    painter.drawEllipse(mouse_position_widget.x() - radius, mouse_position_widget.y() - radius, radius * 2, radius * 2);
}

void GraphWidgetSelectorBrushEllipse::cancel_action() {
    set_selection_mode(GraphWidgetSelectionMode::NORMAL_MODE);
}

bool GraphWidgetSelectorBrushEllipse::selection_function(AdditionalPlotData * plot, size_t index) {
    auto px = plot->data_x[index];
    auto py = plot->data_y[index];
    return ((px - cx) * (px - cx)) / (rx * rx) +
           ((py - cy) * (py - cy)) / (ry * ry) <= 1.0;
}

void GraphWidgetSelectorBrushEllipse::before_selection_calculation(AdditionalPlotData * plot) {
    rx = plot->plot->xAxis->pixelToCoord(mouse_position_widget.rx()) - plot->plot->xAxis->pixelToCoord(mouse_position_widget.rx() - radius);
    ry = plot->plot->yAxis->pixelToCoord(mouse_position_widget.ry()) - plot->plot->yAxis->pixelToCoord(mouse_position_widget.ry() + radius);
    cx = mouse_position_plot.x();
    cy = mouse_position_plot.y();
}

QPointF GraphWidgetSelectorBrushEllipse::selection_min(QCustomPlot * plot) {
    return QPointF(plot->xAxis->pixelToCoord(mouse_position_widget.rx() - radius),
                   plot->yAxis->pixelToCoord(mouse_position_widget.ry() + radius));
}

QPointF GraphWidgetSelectorBrushEllipse::selection_max(QCustomPlot * plot) {
    return QPointF(plot->xAxis->pixelToCoord(mouse_position_widget.rx() + radius),
                   plot->yAxis->pixelToCoord(mouse_position_widget.ry() - radius));
}

void GraphWidgetSelectorRectangle::paint_selection(QCPPainter & painter) {
    auto color = get_current_selector_color();

    QRect rectangle(mouse_position_on_click, mouse_position_widget);

    color.setAlpha(color == color_selection_default ? 0 : 100);

    painter.fillRect(rectangle, QBrush(color));
    painter.setPen(Qt::black);
    painter.drawRect(rectangle);
}

void GraphWidgetSelectorRectangle::cancel_action() {
    set_selection_mode(GraphWidgetSelectionMode::NORMAL_MODE);
}

bool GraphWidgetSelectorRectangle::selection_function(AdditionalPlotData * plot, size_t index) {
    auto sel_min = selection_min(plot->plot);
    auto sel_max = selection_max(plot->plot);
    auto px = plot->data_x[index];
    auto py = plot->data_y[index];
    if(px >= sel_min.x() && px <= sel_max.x()) {
        if(py >= sel_min.y() && py <= sel_max.y()) {
            return true;
        }
    }
    return false;
}

QPointF GraphWidgetSelectorRectangle::selection_min(QCustomPlot * plot) {
    Q_UNUSED(plot);
    return QPointF(std::min(mouse_position_plot_on_click.x(), mouse_position_plot.x()),
                                    std::min(mouse_position_plot_on_click.y(), mouse_position_plot.y()));
}

QPointF GraphWidgetSelectorRectangle::selection_max(QCustomPlot * plot) {
    Q_UNUSED(plot);
    return QPointF(std::max(mouse_position_plot_on_click.x(), mouse_position_plot.x()),
                                    std::max(mouse_position_plot_on_click.y(), mouse_position_plot.y()));
}

void GraphWidgetSelectorFreeHand::paint_selection(QCPPainter & painter) {
    current_selection_points.push_back(mouse_position_widget);

    auto color = get_current_selector_color();

    painter.setPen(QPen(color, 2.0));
    for(size_t i = 1; i < current_selection_points.size(); ++i) {
        painter.drawLine(current_selection_points[i - 1], current_selection_points[i]);
    }

    current_selection_points.pop_back();
}

void GraphWidgetSelectorFreeHand::cancel_action() {
    current_selection_points.clear();
}

bool GraphWidgetSelectorFreeHand::selection_function(AdditionalPlotData * plot, size_t index) {
    QPointF point(plot->data_x[index], plot->data_y[index]);
    QPointF point_inf(1e25, plot->data_y[index]);

    int count = 0;
    for(size_t i = 0; i < selection_world_coords.size(); ++i) {
        QPointF segment_begin = selection_world_coords[i];
        QPointF segment_end = selection_world_coords[(i + 1) % selection_world_coords.size()];

        if (doIntersectIgnoreFPointPrecision(segment_begin, segment_end, point, point_inf))
        {
            ++count;
        }
    }

    return (count % 2 == 1);
}

void GraphWidgetSelectorFreeHand::after_selection_calculation() {
    selection_world_coords.clear();
    current_selection_points.pop_back();
}

void GraphWidgetSelectorFreeHand::before_selection_calculation(AdditionalPlotData * plot) {
    Q_UNUSED(plot)
    current_selection_points.push_back(mouse_position_widget);

    for(auto & wpos : current_selection_points) {
        selection_world_coords.push_back(QPointF(plot->plot->xAxis->pixelToCoord(wpos.x()),
                                                 plot->plot->yAxis->pixelToCoord(wpos.y())));
    }
}

QPointF GraphWidgetSelectorFreeHand::selection_min(QCustomPlot * plot) {
    return QPointF(plot->xAxis->pixelToCoord(min_p.x()), plot->yAxis->pixelToCoord(max_p.y()));
}

QPointF GraphWidgetSelectorFreeHand::selection_max(QCustomPlot * plot) {
    return QPointF(plot->xAxis->pixelToCoord(max_p.x()), plot->yAxis->pixelToCoord(min_p.y()));
}

void GraphWidgetSelectorFreeHand::handle_mouse_press_event(QMouseEvent * event, QPointF plot_pos) {
    GraphWidgetSelector::handle_mouse_press_event(event, plot_pos);
    current_selection_points.push_back(event->pos());
    max_p = event->pos();
    min_p = event->pos();
}

void GraphWidgetSelectorFreeHand::handle_mouse_move_event(QMouseEvent * event, QPointF plot_pos) {
    GraphWidgetSelector::handle_mouse_move_event(event, plot_pos);
    if(mouse_usebutton_pressed) {
        auto vec_a = current_selection_points.back();
        auto vec_b = event->pos();

        auto a = vec_a.x() - vec_b.x();
        auto b = vec_a.y() - vec_b.y();
        double dist = std::sqrt(a * a + b * b);

        if(dist > line_length_threshold) {
            current_selection_points.push_back(event->pos());
            max_p.rx() = std::max(current_selection_points.back().x(), max_p.x());
            max_p.ry() = std::max(current_selection_points.back().y(), max_p.y());
            min_p.rx() = std::min(current_selection_points.back().x(), min_p.x());
            min_p.ry() = std::min(current_selection_points.back().y(), min_p.y());
        }
    }
}

void GraphWidgetSelectorFreeHand::handle_mouse_release_event(QMouseEvent * event, QPointF plot_pos) {
    GraphWidgetSelector::handle_mouse_release_event(event, plot_pos);
    current_selection_points.clear();
}

void GraphWidgetSelectorRange::paint_selection(QCPPainter & painter) {
    auto color = get_current_selector_color();
    painter.setPen(color);
    if(current_axis == X_AXIS) {
        if(mouse_usebutton_pressed) {
            color.setAlpha(100);
            painter.drawRect(mouse_position_on_click.x(), -100, mouse_position_widget.x() - mouse_position_on_click.x(), 5000);
            painter.fillRect(mouse_position_on_click.x(), -100, mouse_position_widget.x() - mouse_position_on_click.x(), 5000, QBrush(color));
        } else {
            painter.drawLine(QPoint(mouse_position_widget.x(), -10000), QPoint(mouse_position_widget.x(), 10000));
        }
    } else {
        if(mouse_usebutton_pressed) {
            color.setAlpha(100);
            painter.drawRect(-100, mouse_position_on_click.y(), 5000, mouse_position_widget.y() - mouse_position_on_click.y());
            painter.fillRect(-100, mouse_position_on_click.y(), 5000, mouse_position_widget.y() - mouse_position_on_click.y(), QBrush(color));
        } else {
            painter.drawLine(QPoint(-10000, mouse_position_widget.y()), QPoint(10000, mouse_position_widget.y()));
        }
    }

}

void GraphWidgetSelectorRange::cancel_action() {

}

void GraphWidgetSelectorRange::before_selection_calculation(AdditionalPlotData * plot) {
    selection_point_min.rx() = std::min(mouse_position_plot.x(), mouse_position_plot_on_click.x());
    selection_point_min.ry() = std::min(mouse_position_plot.y(), mouse_position_plot_on_click.y());
    selection_point_max.rx() = std::max(mouse_position_plot.x(), mouse_position_plot_on_click.x());
    selection_point_max.ry() = std::max(mouse_position_plot.y(), mouse_position_plot_on_click.y());

    // ### replace 0.25 by some smarter constant later (bs)
    if(current_axis == X_AXIS) {
        double miny = 1e30;
        double maxy = -1e30;
        for(size_t i = 0; i < plot->data_y.size(); ++i) {
            miny = std::min(plot->data_y[i], miny);
            maxy = std::max(plot->data_y[i], maxy);
        }
        selection_point_min.ry() = miny - 0.25;
        selection_point_max.ry() = maxy + 0.25;
    } else {
        double minx = 1e30;
        double maxx = -1e30;
        for(size_t i = 0; i < plot->data_x.size(); ++i) {
            minx = std::min(plot->data_x[i], minx);
            maxx = std::max(plot->data_x[i], maxx);
        }
        selection_point_min.rx() = minx - 0.25;
        selection_point_max.rx() = maxx + 0.25;
    }
}

QPointF GraphWidgetSelectorRange::selection_min(QCustomPlot * plot) {
    Q_UNUSED(plot);
    return selection_point_min;
}

QPointF GraphWidgetSelectorRange::selection_max(QCustomPlot * plot) {
    Q_UNUSED(plot);
    return selection_point_max;
}

bool GraphWidgetSelectorRange::selection_function(AdditionalPlotData * plot, size_t index) {
    bool ret = false;
    if(current_axis == X_AXIS) {
        auto x = plot->data_x[index];
        ret = (x >= selection_point_min.x() && x <= selection_point_max.x());
    } else {
        auto y = plot->data_y[index];
        ret = (y >= selection_point_min.y() && y <= selection_point_max.y());
    }
    return ret;
}

QColor GraphWidgetSelector::get_current_selector_color() {
    QColor color = color_selection_default;
    if(selection_mode_difference)   color = color_selection_difference;
    if(selection_mode_union)        color = color_selection_union;
    return color;
}
