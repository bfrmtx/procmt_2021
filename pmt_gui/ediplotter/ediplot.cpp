#include "ediplot.h"

QString const layer_selection = "layer_selection";
double const coords_min = -1e20;
double const coords_max = 1e20;

EdiPlot::EdiPlot(QWidget * parent) : QCustomPlot(parent) {
    setMouseTracking(true);
    addLayer(layer_selection);

    m_vertical_line = new QCPItemLine(this);
    m_vertical_line->setLayer(layer_selection);
    m_vertical_line->setPen(QPen(Qt::red, 0.5));

    m_horizontal_line = new QCPItemLine(this);
    m_horizontal_line->setLayer(layer_selection);
    m_horizontal_line->setPen(QPen(Qt::red, 0.5));

    set_cursor_coords(coords_max, coords_max);
}

void EdiPlot::add_edi_plot(const QString & name, mt_data_res<std::complex<double>> const & data, size_t elem, EdiPlotType type, QPen const & pen) {
    auto new_graph = addGraph();
    auto error_bars = new QCPErrorBars(xAxis, yAxis);
    error_bars->setDataPlottable(new_graph);

    new_graph->setPen(pen);
    new_graph->setScatterStyle(QCPScatterStyle::ssSquare);

    if(data.freqs.size() == data.d[elem].size()) {
        QVector<double> errors;
        for(size_t index = 0; index < data.freqs.size(); ++index) {

            auto const & f = data.freqs[index];
            auto y = type == EdiPlotType::phi ? data.phi_deg(elem, index, this->phase_is_mapped) : data.rho(elem, index);
            auto err = type == EdiPlotType::phi ? data.phi_deg_err(elem, index, true) : data.rho_err(elem, index, true);
            new_graph->addData(f, y);

            errors.push_front(err);
        }
        error_bars->addData(errors);
        error_bars->setPen(pen);
    }

    if(!m_mt_data.contains(name)) {
        m_mt_data[name] = data;
    }
    m_error_bars[name].push_back(error_bars);
    m_graphs[name].push_back(new_graph);

    reset_edi_data(name);


}

void EdiPlot::remove_edi_plots(const QString & name) {
    if(m_graphs.contains(name)) {
        for(auto p : m_graphs[name]) {
            removeGraph(p);
        }
        m_error_bars.remove(name);
        m_graphs.remove(name);
        m_mt_data.remove(name);
    }
}

void EdiPlot::reset_edi_data(QString const & name) {
    if(m_graphs.contains(name) && m_mt_data.contains(name) && m_error_bars.contains(name)) {
        auto graph = m_graphs[name];

    }
}

void EdiPlot::set_edi_plots_visible(const QString & name, bool value) {
    if(m_graphs.contains(name)) {
        for(auto p : m_graphs[name]) {
            p->setVisible(value);
        }
    }
}

void EdiPlot::remove_all_plots() {
    for(auto plotList : m_graphs) {
        for(auto plot : plotList) {
            removeGraph(plot);
        }
    }
    m_error_bars.clear();
    m_graphs.clear();
    m_mt_data.clear();
}

void EdiPlot::map_phase(bool map_phase) {
    this->phase_is_mapped = map_phase;

    ///@todo YASC re-read data and replot
}

void EdiPlot::slot_line_for_crosshair(const int graph_number)
{
    this->graph_number_chc = std::abs(graph_number);
    if (this->graph_number_chc > this->graphCount()) this->graph_number_chc = this->graphCount()-1;
    if (this->graph_number_chc < 0) this->graph_number_chc = 0;
}

void EdiPlot::handle_readout(int x, int y) {
    Q_UNUSED(y);
    double xval = xAxis->pixelToCoord(x);

    ///@todo YASC es wird immer nur der erste Plot genutzt ??

    //    std::vector<double> coords;
    QCPGraphData p;

    if (this->graph_number_chc < graphCount() ) {
        auto it = std::lower_bound(graph(this->graph_number_chc)->data()->begin(), graph(this->graph_number_chc)->data()->end(),
                                   xval, [](QCPGraphData & d, const double val) { return d.key < val; });
        p = (*it);
        if(it != graph(this->graph_number_chc)->data()->begin()) {
            auto p2 = *(it -= 1);
            if(std::abs(p.key - xval) > std::abs(p2.key - xval)) p = p2;
        }
    }

    // that would be the irection for many cursers
    //    for(int graphno = 0; graphno < graphCount(); ++graphno) {
    //        auto it = std::lower_bound(graph(graphno)->data()->begin(), graph(graphno)->data()->end(),
    //                                   xval, [](QCPGraphData & d, const double val) { return d.key < val; });
    //        p = (*it);
    //        if(it != graph(graphno)->data()->begin()) {
    //            auto p2 = *(it -= 1);
    //            if(std::abs(p.key - xval) > std::abs(p2.key - xval)) p = p2;
    //        }
    //        coords.push_back(p.value);
    //    }

    set_cursor_coords(p.key, p.value);
}

void EdiPlot::set_cursor_coords(double x, double y) {
    m_vertical_line->start->setCoords(x, coords_min);
    m_vertical_line->end->setCoords(x, coords_max);
    m_horizontal_line->start->setCoords(coords_min, y);
    m_horizontal_line->end->setCoords(coords_max, y);

    qDebug() << x << y;
    if ((x > coords_min) && (x < coords_max)) {
        double s = 1. / x;

        QString label_x_axis = "f: " + QString::number(x) + "[Hz],  "  + QString::number(s) + "[s]";
        emit signal_set_x_axis_label(label_x_axis);

        // if rho phi plot
        QString label_y_axis;

        qDebug() << this->objectName();
        if (this->objectName().contains("rho", Qt::CaseInsensitive)) {

            label_y_axis = "rho: " + QString::number(y) + "[ohm/m]";

        }
        else if (this->objectName().contains("phi", Qt::CaseInsensitive)) {

            label_y_axis = "phi: " + QString::number(y) + "[deg]";
        }

        // coherency

        // anisotropy

        // rhos_zs

        emit signal_set_y_axis_label(label_y_axis);


    }
}

void EdiPlot::mousePressEvent(QMouseEvent * event) {
    if(event->button() == Qt::LeftButton) {
        handle_readout(event->x(), event->y());
        layer(layer_selection)->replot();
    }
}

void EdiPlot::mouseMoveEvent(QMouseEvent * event) {
    if(event->buttons().testFlag(Qt::LeftButton)) {
        handle_readout(event->x(), event->y());
        layer(layer_selection)->replot();
    }
}

void EdiPlot::mouseReleaseEvent(QMouseEvent * event) {
    if(event->button() == Qt::LeftButton) {
        set_cursor_coords(coords_min, coords_min);
        layer(layer_selection)->replot();
    }
}
