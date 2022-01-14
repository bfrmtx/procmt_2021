#include "dualplot.h"
#include "ui_dualplot.h"

std::vector<QPen> const plot_qpens = {
    QPen(Qt::red, 0.75),
    QPen(Qt::blue, 0.75),
    QPen(Qt::darkGreen, 0.5),
    QPen(Qt::cyan, 0.5),
    QPen(Qt::yellow, 0.5),
    QPen(Qt::magenta, 0.5)
};

DualPlot::DualPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DualPlot)
{
    ui->setupUi(this);

    // rho - setup //
    // /////////// //
    ui->plot_rho->yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
    ui->plot_rho->xAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
    ui->plot_rho->xAxis->setRangeReversed(true);

    ui->plot_rho->yAxis->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog));
    ui->plot_rho->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog));

    // phi - setup //
    // /////////// //
    ui->plot_phi->xAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
    ui->plot_phi->xAxis->setRangeReversed(true);
    ui->plot_phi->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog));

    QCPMarginGroup *margin_group = new QCPMarginGroup(ui->plot_rho);
    ui->plot_rho->axisRect(0)->setMarginGroup(QCP::MarginSide::msLeft | QCP::MarginSide::msRight, margin_group);
    ui->plot_phi->axisRect(0)->setMarginGroup(QCP::MarginSide::msLeft | QCP::MarginSide::msRight, margin_group);

    connect(this->ui->plot_phi, &EdiPlot::signal_set_x_axis_label, this, &DualPlot::signal_set_x_axis_label);
    connect(this->ui->plot_phi, &EdiPlot::signal_set_y_axis_label, this, &DualPlot::signal_set_y_axis_label);
    connect(this->ui->plot_rho, &EdiPlot::signal_set_x_axis_label, this, &DualPlot::signal_set_x_axis_label);
    connect(this->ui->plot_rho, &EdiPlot::signal_set_y_axis_label, this, &DualPlot::signal_set_y_axis_label);

    // new set graph number for cross hair cursor
    connect(this, &DualPlot::signal_line_for_crosshair, this->ui->plot_phi, &EdiPlot::slot_line_for_crosshair);
    connect(this, &DualPlot::signal_line_for_crosshair, this->ui->plot_rho, &EdiPlot::slot_line_for_crosshair);


}

DualPlot::~DualPlot() {
    delete ui;
}

void DualPlot::fit_rho_ranges() {
    ui->plot_rho->xAxis->rescale(true);
    ui->plot_rho->yAxis->rescale(true);
    ui->plot_rho->xAxis->setRange(ui->plot_rho->xAxis->range().lower * 0.95, ui->plot_rho->xAxis->range().upper * 1.05);
    ui->plot_rho->replot();
    ui->plot_phi->replot();
}

void DualPlot::fit_phi_ranges() {
    ui->plot_phi->xAxis->rescale(true);
    ui->plot_phi->yAxis->rescale(true);
    ui->plot_phi->xAxis->setRange(ui->plot_phi->xAxis->range().lower * 0.95, ui->plot_phi->xAxis->range().upper * 1.05);
    ui->plot_phi->replot();
    ui->plot_rho->replot();
}

void DualPlot::fit_ranges() {
    fit_phi_ranges();
    fit_rho_ranges();
}

void DualPlot::set_phi_range(double min, double max) {
    ui->plot_phi->yAxis->setRange(min, max);
    ui->plot_phi->replot();
    ui->plot_rho->replot();
}

void DualPlot::set_rho_range(double min, double max) {
    ui->plot_rho->yAxis->setRange(min, max);
    ui->plot_rho->replot();
    ui->plot_phi->replot();
}

void DualPlot::add_plots(const QString & name, mt_data_res<std::complex<double>> const & data, size_t a, size_t b) {
    ui->plot_phi->add_edi_plot(name, data, a, EdiPlotType::phi, plot_qpens[0]);
    ui->plot_phi->add_edi_plot(name, data, b, EdiPlotType::phi, plot_qpens[1]);
    ui->plot_rho->add_edi_plot(name, data, a, EdiPlotType::rho, plot_qpens[0]);
    ui->plot_rho->add_edi_plot(name, data, b, EdiPlotType::rho, plot_qpens[1]);
    fit_ranges();
}

void DualPlot::remove_plots(const QString & name) {
    ui->plot_phi->remove_edi_plots(name);
    ui->plot_rho->remove_edi_plots(name);
    fit_ranges();
}

void DualPlot::remove_all_plots() {
    ui->plot_phi->remove_all_plots();
    ui->plot_rho->remove_all_plots();
}

void DualPlot::set_plots_visible(const QString & name, bool value) {
    ui->plot_phi->set_edi_plots_visible(name, value);
    ui->plot_rho->set_edi_plots_visible(name, value);
    fit_ranges();
}

void DualPlot::slot_line_for_crosshair(const int graph_number)
{
    emit this->signal_line_for_crosshair(std::abs(graph_number));
}

