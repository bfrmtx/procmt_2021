#include "mestplotlib.h"
#include "ui_mestplotlib.h"

mest_plotlib::mest_plotlib(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mest_plotlib)
{
    ui->setupUi(this);
    this->setWindowTitle("m estimator");


    if (this->plot_tr != nullptr) this->plot_tr.reset();
    this->plot_tr = std::make_unique<QCustomPlot>(Q_NULLPTR);

//    this->ui->vl_plots->addWidget(this->plot_tr.get());
//    this->plot_tr->show();
}

mest_plotlib::~mest_plotlib()
{
    delete ui;
    if (this->plot_tr != nullptr) this->plot_tr.reset();
    if (this->plot_ti != nullptr) this->plot_ti.reset();
}

void mest_plotlib::set_realpart_estimator(const std::vector<double> &mhxr, const std::vector<double> &mhyr, const std::vector<double> &d_itersr)
{


    if (this->plot_tr != nullptr)  {
        this->ui->vl_plots->removeWidget(this->plot_tr.get());
        this->plot_tr.reset();
    }
    this->plot_tr = std::make_unique<QCustomPlot>(Q_NULLPTR);
    this->ui->vl_plots->addWidget(this->plot_tr.get());


    this->plot_tr->xAxis->setLabel("N iterations");
    this->plot_tr->yAxis->setLabel("m estimator, REAL");

    this->plot_tr->addGraph();
    this->plot_tr->graph(0)->setData(d_itersr, mhxr, true);
    this->plot_tr->graph(0)->setPen((QPen(Qt::blue)));
    this->plot_tr->graph(0)->setLineStyle((QCPGraph::LineStyle)1);
    this->plot_tr->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

    this->plot_tr->addGraph();
    this->plot_tr->graph(1)->setData(d_itersr, mhyr, true);
    this->plot_tr->graph(1)->setPen((QPen(Qt::red)));
    this->plot_tr->graph(1)->setLineStyle((QCPGraph::LineStyle)1);
    this->plot_tr->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));


    auto x_minmax = std::minmax_element(d_itersr.begin(), d_itersr.end());
    auto y_minmax = min_max2v(mhxr, mhyr);


    this->plot_tr->xAxis->setRange(*x_minmax.first -0.5, *x_minmax.second + 0.5);
    this->plot_tr->yAxis->setRange(y_minmax.first - std::abs(y_minmax.first) * 0.1, y_minmax.second + std::abs(y_minmax.second) * 0.1 );
    //this->plot_tr->setMinimumSize(1024,768);

    this->plot_tr->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    this->plot_tr->replot();
    this->plot_tr->show();


}

void mest_plotlib::set_imagpart_estimator(const std::vector<double> &mhxi, const std::vector<double> &mhyi, const std::vector<double> &d_itersi)
{

    if (this->plot_ti != nullptr)  {
            this->ui->vl_plots->removeWidget(this->plot_ti.get());
            this->plot_ti.reset();
        }
        this->plot_ti = std::make_unique<QCustomPlot>(Q_NULLPTR);
        this->ui->vl_plots->addWidget(this->plot_ti.get());


        this->plot_ti->xAxis->setLabel("N iterations");
        this->plot_ti->yAxis->setLabel("m estimator, IMAG");

        this->plot_ti->addGraph();
        this->plot_ti->graph(0)->setData(d_itersi, mhxi, true);
        this->plot_ti->graph(0)->setPen((QPen(Qt::blue)));
        this->plot_ti->graph(0)->setLineStyle((QCPGraph::LineStyle)1);
        this->plot_ti->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

        this->plot_ti->addGraph();
        this->plot_ti->graph(1)->setData(d_itersi, mhyi, true);
        this->plot_ti->graph(1)->setPen((QPen(Qt::red)));
        this->plot_ti->graph(1)->setLineStyle((QCPGraph::LineStyle)1);
        this->plot_ti->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));

        auto x_minmax = std::minmax_element(d_itersi.begin(), d_itersi.end());
        auto y_minmax = min_max2v(mhxi, mhyi);


        this->plot_ti->xAxis->setRange(*x_minmax.first -0.5, *x_minmax.second + 0.5);
        this->plot_ti->yAxis->setRange(y_minmax.first - std::abs(y_minmax.first) * 0.1, y_minmax.second + std::abs(y_minmax.second) * 0.1 );
        //this->plot_ti->setMinimumSize(1024,768);

        this->plot_ti->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

        this->plot_ti->replot();
        this->plot_ti->show();


}
