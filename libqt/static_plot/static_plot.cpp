#include "static_plot.h"
#include "ui_static_plot.h"

static_plot::static_plot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::static_plot)
{
    ui->setupUi(this);

    this->active_plots.reserve(this->max_plots);
    this->plots.reserve(this->max_plots);
    this->mins_x.reserve(this->max_plots);
    this->maxs_x.reserve(this->max_plots);
    this->mins_y.reserve(this->max_plots);
    this->maxs_y.reserve(this->max_plots);

    for (size_t i = 0; i < this->max_plots; ++i) {
        active_plots.push_back(0);
        this->mins_x.push_back(0);
        this->maxs_x.push_back(0);
        this->mins_y.push_back(0);
        this->maxs_y.push_back(0);
        plots.emplace_back(std::make_unique<QCustomPlot>(Q_NULLPTR));
    }

    for (auto &plot : plots) {
        connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(change_y_axis_and_spins(QCPRange)));
        connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(change_x_axis_and_spins(QCPRange)));
    }

    this->ui->xmax_spin->setRange(-DBL_MAX, DBL_MAX);
    this->ui->xmin_spin->setRange(-DBL_MAX, DBL_MAX);
    this->ui->ymax_spin->setRange(-DBL_MAX, DBL_MAX);
    this->ui->ymin_spin->setRange(-DBL_MAX, DBL_MAX);
}

static_plot::~static_plot()
{
    for (auto &p : this->plots) {
        if (p != nullptr) {
            p->close();
            p.reset(nullptr);
        }
    }

    delete ui;
}

void static_plot::set_data(const int plot_number, const std::vector<double> &x, const std::vector<double> &y, const QPen pen, const QCPScatterStyle::ScatterShape shape)
{
    if (plot_number > int(this->max_plots)) return;
    if (!x.size() || !y.size()) return;
    if (x.size() != y.size()) return;
    this->plots[plot_number]->addGraph();
    this->active_plots[plot_number] = 1;
    int count = this->plots[plot_number]->graphCount() - 1;
    this->plots[plot_number]->graph(count)->setData(x, y, true);

    //this->plots[plot_number]->graph(count)->setPen((QPen(Qt::blue)));
    this->plots[plot_number]->graph(count)->setPen(pen);
    this->plots[plot_number]->graph(count)->setLineStyle((QCPGraph::LineStyle)1);
    this->plots[plot_number]->graph(count)->setScatterStyle(QCPScatterStyle(shape, 5));

    auto y_minmax = std::minmax_element(y.begin(), y.end());
    auto x_minmax = std::minmax_element(x.begin(), x.end());

    if (!count) {
        this->mins_x[plot_number] = *x_minmax.first;
        this->maxs_x[plot_number] = *x_minmax.second;

        this->mins_y[plot_number] = *y_minmax.first;
        this->maxs_y[plot_number] = *y_minmax.second;
    }

    else {
        if (this->mins_x[plot_number] > *x_minmax.first) this->mins_x[plot_number] = *x_minmax.first ;
        if (this->maxs_x[plot_number] < *x_minmax.second) this->maxs_x[plot_number] = *x_minmax.second;

        if (this->mins_y[plot_number] > *y_minmax.first) this->mins_y[plot_number] = *y_minmax.first;
        if (this->maxs_y[plot_number] < *y_minmax.second) this->maxs_y[plot_number] = *y_minmax.second;
    }

    this->act_mins_x = this->mins_x;
    this->act_maxs_x = this->maxs_x;
    this->act_mins_y = this->mins_y;
    this->act_maxs_y = this->maxs_y;


}

void static_plot::set_data_complex(const int plot_number, const std::vector<double> &x, const std::vector<std::complex<double> > &y, const int real_1_imag_2_ampl_3_phase_4, const QPen pen, const QCPScatterStyle::ScatterShape shape)
{
    if (plot_number > int(this->max_plots)) return;
    if (!x.size() || !y.size()) return;
    if (x.size() != y.size()) return;

    std::vector<double> newy(y.size());
    size_t i = 0;
    if (real_1_imag_2_ampl_3_phase_4 == 1) {
        for (auto &v : newy) {
            v = real(y[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 2) {
        for (auto &v : newy) {
            v = imag(y[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 3) {
        for (auto &v : newy) {
            v = std::abs(y[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 4) {
        for (auto &v : newy) {
            v = std::arg(y[i++]) * 180. / M_PI;
        }
    }

    this->set_data(plot_number, x, newy, pen, shape);


}

void static_plot::set_data_ops(const int plot_number, const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &y2, int add_1_mul_2_div_3_sub_4, const QPen pen, const QCPScatterStyle::ScatterShape shape)
{
    if (plot_number > int(this->max_plots)) return;
    if (!x.size() || !y.size()) return;
    if (x.size() != y.size()) return;
    if (y.size() != y2.size()) return;
    std::vector<double> newy(y.size());
    size_t i = 0;
    if (add_1_mul_2_div_3_sub_4 == 1) {
        for (auto &v : newy) v = y[i] + y2[i];
        ++i;
    }
    else if (add_1_mul_2_div_3_sub_4 == 2) {
        for (auto &v : newy) v = y[i] * y2[i];
        ++i;
    }
    else if (add_1_mul_2_div_3_sub_4 == 3) {
        for (auto &v : newy) v = y[i] / y2[i];
        ++i;
    }
    else if (add_1_mul_2_div_3_sub_4 == 4) {
        for (auto &v : newy) v = y[i] - y2[i];
        ++i;
    }

    this->set_data(plot_number, x, newy, pen, shape);
}

void static_plot::set_data_complex_ops(const int plot_number, const std::vector<double> &x, const std::vector<std::complex<double> > &y, const std::vector<std::complex<double> > &y2, int add_1_mul_2_div_3_sub_4, const int real_1_imag_2_ampl_3_phase_4, const QPen pen, const QCPScatterStyle::ScatterShape shape)
{
    if (plot_number > int(this->max_plots)) return;
    if (!x.size() || !y.size()) return;
    if (x.size() != y.size()) return;
    if (y2.size() != y.size()) return;

    std::vector<double> newy(y.size());
    std::vector<double> newy2(y.size());
    std::vector<double> result(y.size());
    size_t i = 0;
    if (real_1_imag_2_ampl_3_phase_4 == 1) {
        for (auto &v : newy) {
            v = real(y[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 2) {
        for (auto &v : newy) {
            v = imag(y[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 3) {
        for (auto &v : newy) {
            v = std::abs(y[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 4) {
        for (auto &v : newy) {
            v = std::arg(y[i++]) * 180. / M_PI;
        }
    }

    if (real_1_imag_2_ampl_3_phase_4 == 1) {
        for (auto &v : newy2) {
            v = real(y2[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 2) {
        for (auto &v : newy2) {
            v = imag(y2[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 3) {
        for (auto &v : newy2) {
            v = std::abs(y2[i++]);
        }
    }
    if (real_1_imag_2_ampl_3_phase_4 == 4) {
        for (auto &v : newy2) {
            v = std::arg(y2[i++]) * 180. / M_PI;
        }
    }

    if (add_1_mul_2_div_3_sub_4 == 1) {
        for (auto &v : result) v = newy[i] + newy2[i];
        ++i;
    }
    else if (add_1_mul_2_div_3_sub_4 == 2) {
        for (auto &v : result) v = newy[i] * newy2[i];
        ++i;
    }
    else if (add_1_mul_2_div_3_sub_4 == 3) {
        for (auto &v : result) v = newy[i] / newy2[i];
        ++i;
    }
    else if (add_1_mul_2_div_3_sub_4 == 4) {
        for (auto &v : result) v = newy[i] - newy2[i];
        ++i;
    }

    newy.clear();
    newy2.clear();

    this->set_data(plot_number, x, result, pen, shape);


}

void static_plot::set_tile(const QString &window_title)
{
    this->setWindowTitle(window_title);
}

void static_plot::set_x_y_label(const int plot_number, const QString &xlabel, const QString &ylabel)
{
    if (plot_number > int(this->max_plots)) return;
    this->plots[plot_number]->xAxis->setLabel(xlabel);
    this->plots[plot_number]->yAxis->setLabel(ylabel);

}

void static_plot::set_x_min_max(const double &xmin, const double &xmax)
{
    size_t i = 0;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->xAxis->setRange(xmin, xmax);
            ++i;
        }
    }
}

void static_plot::replot()
{
    size_t i = 0;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) plot->replot();
        if (this->active_plots.at(i++) == 1) plot->show();
    }
}

void static_plot::plot()
{
    //    for (auto &plot : plots) {
    //        this->ui->vl_plots->removeWidget(plot.get());
    //    }

    size_t i = 0;
    for (auto &plot : plots) {
        if (this->active_plots.at(i++) == 1) this->ui->vl_plots->addWidget(plot.get());
    }

    i = 0;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
            plot->xAxis->setRange(this->mins_x[i] -0.5, this->maxs_x[i] + 0.5);
            plot->yAxis->setRange(this->mins_y[i] - std::abs(this->mins_y[i]) * 0.1 , this->maxs_y[i] + std::abs(this->maxs_y[i]) * 0.1 );
            ++i;
        }
    }

    i = 0;

    this->ui->xmax_spin->setValue(this->maxs_x[0]);
    this->ui->xmin_spin->setValue(this->mins_x[0]);
    this->ui->ymax_spin->setValue(this->maxs_y[0]);
    this->ui->ymin_spin->setValue(this->mins_y[0]);

    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) plot->replot();
        if (this->active_plots.at(i++) == 1) plot->show();
    }

    this->show();


}

void static_plot::clear()
{
    for (auto &plot : plots) {
        this->ui->vl_plots->removeWidget(plot.get());
    }

    for (auto &plot : plots) {
        if (plot != nullptr) plot.reset();
    }
    this->plots.clear();
    this->active_plots.clear();

    this->active_plots.reserve(this->max_plots);
    this->plots.reserve(this->max_plots);

    for (size_t i = 0; i < this->max_plots; ++i) {
        active_plots.push_back(0);
        plots.emplace_back(std::make_unique<QCustomPlot>(Q_NULLPTR));
    }

}

void static_plot::on_pushButton_reset_x_clicked()
{
    size_t i = 0;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->xAxis->setRange(this->mins_x[i], this->maxs_x[i]);
            ++i;
        }
    }

    this->replot();
}

void static_plot::on_pushButton_reset_y_clicked()
{
    size_t i = 0;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->yAxis->setRange(this->mins_y[i], this->maxs_y[i]);
            ++i;
        }
    }

    this->replot();
}

void static_plot::on_pushButton_reset_xy_clicked()
{
    size_t i = 0;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->xAxis->setRange(this->mins_x[i], this->maxs_x[i]);
            plot->yAxis->setRange(this->mins_y[i], this->maxs_y[i]);

            ++i;
        }
    }

    this->replot();
}

void static_plot::on_xmin_spin_valueChanged(double arg1)
{
    size_t i = 0;
    for (auto &v : this->act_mins_x) v = arg1;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->xAxis->setRange(this->act_mins_x[i], this->act_maxs_x[i]);
            ++i;
        }
    }

    this->replot();


}

void static_plot::on_xmax_spin_valueChanged(double arg1)
{

    size_t i = 0;
    for (auto &v : this->act_maxs_x) v = arg1;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->xAxis->setRange(this->act_mins_x[i], this->act_maxs_x[i]);
            ++i;
        }
    }

    this->replot();
}

void static_plot::on_ymin_spin_valueChanged(double arg1)
{
    size_t i = 0;
    for (auto &v : this->act_mins_y) v = arg1;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->yAxis->setRange(this->act_mins_y[i], this->act_maxs_y[i]);
            ++i;
        }
    }

    this->replot();

}

void static_plot::on_ymax_spin_valueChanged(double arg1)
{
    size_t i = 0;
    for (auto &v : this->act_maxs_y) v = arg1;
    for (auto &plot : plots) {
        if (this->active_plots.at(i) == 1) {
            plot->yAxis->setRange(this->act_mins_y[i], this->act_maxs_y[i]);
            ++i;
        }
    }

    this->replot();
}

void static_plot::change_x_axis_and_spins(const QCPRange &newRange)
{
    this->blockSignals(true);
    this->ui->xmin_spin->setValue(newRange.lower);
    this->ui->xmax_spin->setValue(newRange.upper);
    this->blockSignals(false);
}

void static_plot::change_y_axis_and_spins(const QCPRange &newRange)
{
    this->blockSignals(true);
    this->ui->ymin_spin->setValue(newRange.lower);
    this->ui->ymax_spin->setValue(newRange.upper);
    this->blockSignals(false);
}

void static_plot::create_scrollbar()
{
    if (this->scrollbar == nullptr) {

        this->scrollbar = new QScrollBar(Qt::Horizontal, this);
        this->ui->hl_bottom->addWidget(this->scrollbar);
        this->scrollbar->setMinimum(0);
        this->scrollbar->setMaximum(100);
        this->scrollbar->setMinimumHeight(20);
        this->scrollbar->setSingleStep(1);


    }
}




multi_static_plots::multi_static_plots(QObject *parent) :  QObject(parent)
{

}

multi_static_plots::~multi_static_plots() {
    this->clear();
}

void multi_static_plots::insert(const QString &plotname,  QWidget *where_to_add_buttons)
{
    this->map::emplace(plotname, std::make_unique<static_plot>());
    if (this->where_to_add_buttons == Q_NULLPTR) this->where_to_add_buttons = where_to_add_buttons;
    if (where_to_add_buttons != Q_NULLPTR) {
        QPushButton * new_menu_button = new QPushButton(where_to_add_buttons);
        new_menu_button->setProperty(prop_what_to_plot, QVariant(plotname));
        //new_menu_button->setProperty(prop_count_what_to_plot, QVariant(uint(what_to_plot_buttons.size())));
        this->what_to_plot_buttons.push_back(new_menu_button);
        //new_menu_button->setCheckable(true);
        new_menu_button->setText(plotname);

        connect(new_menu_button, &QPushButton::clicked, this, &multi_static_plots::slot_plot_button);
        where_to_add_buttons->layout()->addWidget(new_menu_button);


    }
}

void multi_static_plots::clear()
{
    for (auto plot = this->begin(); plot != this->end(); ++plot) {
        plot->second->close();
        plot->second.reset();
    }
    this->map::clear();
    if (this->where_to_add_buttons != Q_NULLPTR) {
        for (auto widget: where_to_add_buttons->findChildren<QWidget*>()) delete widget;
    }
}

void multi_static_plots::slot_plot_button()
{
    auto * whatbutton = reinterpret_cast<QPushButton*>(sender());
    QString what = whatbutton->property(prop_what_to_plot).toString();
    this->at(what)->plot();
    emit this->plotting(what);
}

void multi_static_plots::slot_plot(const QString &what)
{
    this->at(what)->plot();
    emit this->plotting(what);


}

void multi_static_plots::close()
{
    for (auto plot = this->begin(); plot != this->end(); ++plot) {
        plot->second->close();
    }
}
