#include "plotwidget.h"

static const char * qcpplot_layer_overlay = "overlay";

void PlotWidget::mouseMoveEvent(QMouseEvent * event) {
    QCustomPlot::mouseMoveEvent(event);

    if(selector != nullptr) {
        auto plotPosition = QPointF(xAxis->pixelToCoord(event->pos().x()), yAxis->pixelToCoord(event->pos().y()));
        selector->handle_mouse_move_event(event, plotPosition);
        if(selector->select_on_pressed_move() && event->buttons().testFlag(Qt::MouseButton::RightButton)) {
            size_t counter = 0;
            for(auto & graphIt : graph_map) {
                if(graphIt.second->visible()) {
                    auto f = graphIt.first;
                    if(graph_data.find(f) != graph_data.end()) {
                        selector->select(counter, &graph_data[f]);
                    }
                }
                counter++;
            }
        }

        layer(qcpplot_layer_overlay)->replot();
    }
}

void PlotWidget::mousePressEvent(QMouseEvent * event) {
    QCustomPlot::mousePressEvent(event);
    if(selector != nullptr)  {
        auto plotPosition = QPointF(xAxis->pixelToCoord(event->pos().x()), yAxis->pixelToCoord(event->pos().y()));
        selector->handle_mouse_press_event(event, plotPosition);

        if(selector->select_on_press()) {
            if(event->button() == Qt::MouseButton::RightButton) {
                size_t counter = 0;
                for(auto & graphIt : graph_map) {
                    if(graphIt.second->visible()) {
                        auto f = graphIt.first;
                        if(graph_data.find(f) != graph_data.end()) {
                            selector->select(counter, &graph_data[f]);
                        }
                    }
                    counter++;
                }
            }
        }

        layer(qcpplot_layer_overlay)->replot();
    }
}

void PlotWidget::mouseReleaseEvent(QMouseEvent * event) {
    QCustomPlot::mouseReleaseEvent(event);
    if(selector != nullptr) {
        if(selector->select_on_release())  {
            if(event->button() == Qt::MouseButton::RightButton) {
                size_t counter = 0;
                for(auto & graphIt : graph_map) {
                    if(graphIt.second->visible()) {
                        auto f = graphIt.first;
                        if(graph_data.find(f) != graph_data.end()) {
                            selector->select(counter, &graph_data[f]);
                        }
                    }
                    counter++;
                }
            }
        }

        auto plotPosition = QPointF(xAxis->pixelToCoord(event->pos().x()), yAxis->pixelToCoord(event->pos().y()));
        selector->handle_mouse_release_event(event, plotPosition);
        layer(qcpplot_layer_overlay)->replot();
    }
}

void PlotWidget::enterEvent(QEvent * event) {
    Q_UNUSED(event)
    setFocus();
    selector_item->setVisible(true);
    replot();
}

void PlotWidget::leaveEvent(QEvent * event) {
    Q_UNUSED(event)
    selector_item->setVisible(false);
    replot();
}

void PlotWidget::keyPressEvent(QKeyEvent * event) {
    QCustomPlot::keyPressEvent(event);
    if(selector != nullptr) {
        selector->handle_key_press_event(event);
        layer(qcpplot_layer_overlay)->replot();
    }
}

void PlotWidget::keyReleaseEvent(QKeyEvent * event) {
    QCustomPlot::keyReleaseEvent(event);
    if(selector != nullptr) {
        selector->handle_key_release_event(event);
        layer(qcpplot_layer_overlay)->replot();
    }
}


void PlotWidget::rescale_axes_with_graph_data(double frequency) {
    if(graph_data.find(frequency) != graph_data.end()) {
        auto const & additional_data = graph_data[frequency];

        xAxis->blockSignals(true);
        yAxis->blockSignals(true);

        double width = additional_data.plot_max_data.x() - additional_data.plot_min_data.x();
        double height = additional_data.plot_max_data.y() - additional_data.plot_min_data.y();
        if(width < std::numeric_limits<double>::epsilon()) width = 1.0;
        if(height < std::numeric_limits<double>::epsilon()) height = 1.0;
        xAxis->setRange(additional_data.plot_min_data.x() - width * 0.125, additional_data.plot_max_data.x() + width * 0.125);
        yAxis->setRange(additional_data.plot_min_data.y() - height * 0.125, additional_data.plot_max_data.y() + height * 0.125);

        yAxis->blockSignals(false);
        xAxis->blockSignals(false);
    }
}

void PlotWidget::rescale_axes_with_margin(double percentage, bool scale_x_axis, bool scale_y_axis) {
    percentage = percentage <= 0.000125 ? 0.000125 : percentage;
    Q_UNUSED(scale_x_axis);
    Q_UNUSED(scale_y_axis);
    xAxis->blockSignals(true);
    yAxis->blockSignals(true);
    rescaleAxes();

//    double width = xAxis->range().size();
//    double height = yAxis->range().size();
//    auto range = xAxis->range();
//    qDebug() << "RESCALING...";
//    qDebug() << "xAxis: " << xAxis->range() << "|" << xAxis->range().lower << "|" << xAxis->range().upper;
//    qDebug() << "yAxis: " << yAxis->range() << "|" << yAxis->range().lower << "|" << yAxis->range().upper;
//    qDebug() << "width: " << width << "|height: " << height;
//    qDebug() << "width * perc: " << width * percentage << "|height * perc: " << height * percentage;
//    double width = std::abs(xAxis->range().upper - xAxis->range().lower);
//    double height = std::abs(yAxis->range().upper - yAxis->range().lower);
//    if(scale_x_axis) {
//        xAxis->setRange(xAxis->range().lower - width * percentage, xAxis->range().upper + width * percentage);
//    }
//    if(scale_y_axis) {
//        yAxis->setRange(yAxis->range().lower - height * percentage, yAxis->range().upper + height * percentage);
//    }
//    qDebug() << xAxis->range();
//    qDebug() << yAxis->range();
    yAxis->blockSignals(false);
    xAxis->blockSignals(false);
}

void PlotWidget::set_adaptive_sampling(bool value) {
    for(int i = 0; i < graphCount(); ++i) {
        graph(i)->setAdaptiveSampling(value);
    }
}

PlotWidget::PlotWidget(QWidget * parent) :
    QCustomPlot(parent), selector(nullptr),
    selector_item(new QCPSelectorItem(this)) {

    //this->setOpenGl(true, 0);
    selector_item->setLayer(qcpplot_layer_overlay);

    setMouseTracking(true);
    setInteractions(QCP::Interaction::iRangeZoom | QCP::Interaction::iRangeDrag);
    setNotAntialiasedElements(QCP::aePlottables);
}

void PlotWidget::set_selection(size_t graph_id, QCPDataSelection const & selection) {
    this->graph(static_cast<int>(graph_id))->setSelection(selection);
}

QCPDataSelection PlotWidget::add_and_get_selection(size_t graph_id, const std::unordered_set<size_t> & indices) {
    QCPGraph * graph = this->graph(static_cast<int>(graph_id));
    auto sel = graph->selection();
    for(size_t index : indices) {
        sel.addDataRange(QCPDataRange(static_cast<int>(index), static_cast<int>(index + 1)), false);
    }
    sel.simplify();
    graph->setSelection(sel);
    this->replot();
    return sel;
}

QCPDataSelection PlotWidget::remove_and_get_selection(size_t graph_id, const std::unordered_set<size_t> & indices) {
    QCPGraph * graph = this->graph(static_cast<int>(graph_id));
    auto sel = graph->selection();
    for(size_t index : indices) {
        sel -= QCPDataRange(static_cast<int>(index), static_cast<int>(index + 1));
    }
    sel.simplify();
    graph->setSelection(sel);
    this->replot();
    return sel;
}

void PlotWidget::add_to_selection(size_t graph_id, const std::unordered_set<size_t> & indices) {
    if(!indices.empty()) {
        QCPGraph * graph = this->graph(static_cast<int>(graph_id));
        auto sel = graph->selection();
        for(size_t index : indices) {
            sel.addDataRange(QCPDataRange(static_cast<int>(index), static_cast<int>(index + 1)), false);
        }
        sel.simplify();
        graph->setSelection(sel);
        this->replot();
    }
}

void PlotWidget::remove_from_selection(size_t graph_id, const std::unordered_set<size_t> & indices) {
    if(!indices.empty()) {
        QCPGraph * graph = this->graph(static_cast<int>(graph_id));
        auto sel = graph->selection();
        for(size_t index : indices) {
            sel -= QCPDataRange(static_cast<int>(index), static_cast<int>(index + 1));
        }
        sel.simplify();
        graph->setSelection(sel);
        this->replot();
    }
}

void PlotWidget::set_graph_data(double freq, const std::vector<double> & keys,
                                const std::vector<double> & values,
                                std::vector<bool> * selection,
                                bool visible, bool adaptivesampling) {
    auto data_size = keys.size();

    if(graph_map.find(freq) == graph_map.end()) {
        graph_map[freq] = addGraph();
        graph_data[freq] = AdditionalPlotData();
    }

    auto & additional_data = graph_data[freq];
    auto & graph = graph_map[freq];
    graph->setVisible(visible);
    QVector<double> keys_q;
    QVector<double> values_q;
    for(auto k : keys) keys_q.push_back(k);
    for(auto v : values) values_q.push_back(v);
    graph->setData(keys_q, values_q, true);

    // calculating grid-data etc
    {
        additional_data.plot = this;
        additional_data.plot_min_data = QPointF(1e100, 1e100);
        additional_data.plot_max_data = QPointF(-1e100, -1e100);
        additional_data.data_x = keys;
        additional_data.data_y = values;
        additional_data.selection_data = selection;

        for(size_t i = 0; i < keys.size(); ++i) {
            additional_data.plot_min_data.rx() = std::min(keys[i], additional_data.plot_min_data.x());
            additional_data.plot_max_data.rx() = std::max(keys[i], additional_data.plot_max_data.x());
            additional_data.plot_min_data.ry() = std::min(values[i], additional_data.plot_min_data.y());
            additional_data.plot_max_data.ry() = std::max(values[i], additional_data.plot_max_data.y());
        }

        size_t sizex = 1, sizey = 1;

        double width = (additional_data.plot_max_data.x() - additional_data.plot_min_data.x());
        double height = (additional_data.plot_max_data.y() - additional_data.plot_min_data.y());
        additional_data.x_one_dimensional = width < std::numeric_limits<double>::epsilon();
        additional_data.y_one_dimensional = height < std::numeric_limits<double>::epsilon();

        QPointF max_data, min_data;
        max_data.rx() = additional_data.plot_max_data.x() + width * 0.01;
        max_data.ry() = additional_data.plot_max_data.y() + height * 0.01;
        min_data.rx() = additional_data.plot_min_data.x() - width * 0.01;
        min_data.ry() = additional_data.plot_min_data.y() - height * 0.01;

        width = (max_data.x() - min_data.x());
        height = (max_data.y() - min_data.y());

        if(additional_data.y_one_dimensional) {
            min_data.ry() -= 0.0025;
            max_data.ry() += 0.0025;
            additional_data.selection_grid_step.ry() = max_data.y() - min_data.y();
        } else {
            additional_data.selection_grid_step.ry() = additional_data.x_one_dimensional ? height / std::sqrt(data_size)
                                                                                         : height / std::sqrt(std::sqrt(data_size));
            sizey = static_cast<size_t>(std::ceil(height / additional_data.selection_grid_step.y()));
        }
        if(additional_data.x_one_dimensional) {
            min_data.rx() -= 0.0025;
            max_data.rx() += 0.0025;
            additional_data.selection_grid_step.rx() = max_data.x() - min_data.x();
        } else {
            additional_data.selection_grid_step.rx() = additional_data.y_one_dimensional ? width / std::sqrt(data_size)
                                                         : width / std::sqrt(std::sqrt(data_size));
            sizex = static_cast<size_t>(std::ceil(width / additional_data.selection_grid_step.x()));
        }

        additional_data.selection_grid_size = QPoint(static_cast<int>(sizex), static_cast<int>(sizey));
        additional_data.selection_grid_min = min_data;

        additional_data.selection_grid = grid_3d_vector(sizex, std::vector<std::vector<size_t>>(sizey, std::vector<size_t>()));


        for(size_t index = 0; index < keys.size(); ++index) {
            size_t grid_x = additional_data.x_one_dimensional ? 0 : static_cast<size_t>(((keys[index] - additional_data.selection_grid_min.x())
                                                                                         / additional_data.selection_grid_step.x()));
            size_t grid_y = additional_data.y_one_dimensional ? 0 : static_cast<size_t>(((values[index] - additional_data.selection_grid_min.y())
                                                                                         / additional_data.selection_grid_step.y()));
            additional_data.selection_grid[grid_x][grid_y].push_back(static_cast<size_t>(index));
        }

        graph->setAdaptiveSampling(adaptivesampling);
    }

    {
        QCPScatterStyle scatterStyle;
        scatterStyle.setPen(QPen(Qt::blue));

        scatterStyle.setSize(4);
        scatterStyle.setShape(QCPScatterStyle::ssCross);
        graph->setLineStyle(QCPGraph::LineStyle::lsNone);
        graph->setScatterStyle(scatterStyle);

        QCPScatterStyle selectionScatterStyle;
        selectionScatterStyle.setPen(QPen(Qt::red, 1.0));
        scatterStyle.setSize(4);
        scatterStyle.setShape(QCPScatterStyle::ssCross);

        graph->selectionDecorator()->setPen(QPen(Qt::red));
        graph->selectionDecorator()->setScatterStyle(selectionScatterStyle);
        graph->setSelectable(QCP::SelectionType::stMultipleDataRanges);
    }
}

void PlotWidget::show_all_graphs() {
    for(auto & graphIterator : graph_map) {
        graphIterator.second->setVisible(true);
    }
}

void PlotWidget::show_single_graph(double freq) {
    if(graph_map.find(freq) == graph_map.end()) return;

    for(auto & graphIterator : graph_map) {
        graphIterator.second->setVisible(false);
    }
    graph_map[freq]->setVisible(true);
    rescale_axes_with_graph_data(freq);
    replot();
}

void PlotWidget::clear_all_plots() {
    clearGraphs();
    graph_data.clear();
    graph_map.clear();
}
