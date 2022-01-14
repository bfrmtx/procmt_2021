#include "cal_stat_display_lib.h"
#include "ui_cal_stat_display_lib.h"

cal_stat_display_lib::cal_stat_display_lib(const QFileInfo &info_db, const QFileInfo &master_cal_db, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cal_stat_display_lib)
{
    ui->setupUi(this);

    this->plots.emplace_back(std::make_unique<plotlib>("on_ampl", this));
    this->plots.emplace_back(std::make_unique<plotlib>("on_phz", this));
    this->plots.emplace_back(std::make_unique<plotlib>("off_ampl", this));
    this->plots.emplace_back(std::make_unique<plotlib>("off_phz", this));
    // prepare the types
    this->plots[0]->chopper = cal::chopper_on; this->plots[0]->plot_type = cal::amplitude;
    this->plots[1]->chopper = cal::chopper_on; this->plots[1]->plot_type = cal::phase_grad;
    this->plots[2]->chopper = cal::chopper_off; this->plots[2]->plot_type = cal::amplitude;
    this->plots[3]->chopper = cal::chopper_off; this->plots[3]->plot_type = cal::phase_grad;


    for (auto &plot : plots) {
        if (plot->plot_type == cal::amplitude) {
            // this adds a plot, e.g. 0
            if (plot->chopper == cal::chopper_on) {
                plot->set_ScaleType_xy("amplitude ON [V/(nT*Hz)]" );
            }
            else if (plot->chopper == cal::chopper_off) {
                plot->set_ScaleType_xy("amplitude OFF [V/(nT*Hz)]");
            }
            plot->graph(0)->setPen(QPen(Qt::blue));
            plot->graph(0)->setLineStyle((QCPGraph::LineStyle)0);
            plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 15));

        }

        if (plot->plot_type == cal::phase_grad) {
            // this adds a plot, e.g. 0
            if (plot->chopper == cal::chopper_on) {
                plot->set_ScaleType_xy("phase ON [deg]" );
            }
            else if (plot->chopper == cal::chopper_off) {
                plot->set_ScaleType_xy("phase OFF [deg]");
            }

            plot->graph(0)->setPen(QPen(Qt::red));
            plot->graph(0)->setLineStyle((QCPGraph::LineStyle)0);
            plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 15));
        }

        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
        // there seem to be two signals rangeChanged with one and two args, so use this way

        QObject::connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)),
                         this, SLOT(xrange_changed(QCPRange)));

        this->ui->pll->addWidget(plot.get());
    }





    //    this->amp_plt_on =  std::make_unique<plotlib>("", this);
    //    this->phz_plt_on =  std::make_unique<plotlib>("", this);
    //    this->amp_plt_off = std::make_unique<plotlib>("", this);
    //    this->phz_plt_off = std::make_unique<plotlib>("", this);

    //    this->amp_plt_on->setObjectName("on_ampl");
    //    this->phz_plt_on->setObjectName("on_phz");
    //    this->amp_plt_off->setObjectName("off_ampl");
    //    this->phz_plt_off->setObjectName("off_phz");



    //    connect(amp_plt_on->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xrange_changed(QCPRange)));
    //    connect(phz_plt_on->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xrange_changed(QCPRange)));
    //    connect(amp_plt_off->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xrange_changed(QCPRange)));
    //    connect(phz_plt_off->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xrange_changed(QCPRange)));

    this->sumcal = std::make_unique<calibration>(info_db, -1, -1, true, "cal_stat_display_lib_info", this);
    this->sumcal->open_master_cal(master_cal_db, "cal_stat_display_lib_master");

    // this->prepare_graphs();


    //    this->ui->pll->addWidget(this->amp_plt_on.get());
    //    this->ui->pll->addWidget(this->phz_plt_on.get());

    //    this->ui->pll->addWidget(this->amp_plt_off.get());
    //    this->ui->pll->addWidget(this->phz_plt_off.get());




}

cal_stat_display_lib::~cal_stat_display_lib()
{

    for (auto &plot : plots) {
        plot->clearGraphs();
        plot.reset();
    }

    this->sumcal.reset();
    delete ui;
}

int cal_stat_display_lib::set_calstat(const QList<calstat> &in)
{

    for (auto &ins : in) this->calstats.push_back(calstat(ins));

    std::vector<double> vfon;
    std::vector<double> vfoff;


    for (auto &cals : calstats) {
        // only amplitude is ok, phase use same freqs
        if (cals.chopper == cal::chopper_on) vfon.push_back(cals.f);
        if (cals.chopper == cal::chopper_off) vfoff.push_back(cals.f);

    }
    this->sumcal->interploate(vfon, -1, cal::chopper_on);
    this->sumcal->interploate(vfoff, -1, cal::chopper_off);

    // and create the on and off frequency range buttons
    for (auto &plot : plots) {
        if ((plot->chopper == cal::chopper_on) && (plot->plot_type == cal::amplitude)) {

            plot->create_fon_ranges(vfon);
            plot->fon_ranges_area->setObjectName("fll_on");
            this->ui->fl->addWidget(plot->fon_ranges_area.data());
            QObject::connect(plot.get(),  &plotlib::f_on_selected,  this, &cal_stat_display_lib::slot_plot);
        }
        if ((plot->chopper == cal::chopper_off) && (plot->plot_type == cal::amplitude)) {

            plot->create_foff_ranges(vfoff);
            plot->foff_ranges_area->setObjectName("fll_off");
            this->ui->fl->addWidget(plot->foff_ranges_area.data());
            QObject::connect(plot.get(),  &plotlib::f_off_selected,  this, &cal_stat_display_lib::slot_plot);

        }
    }
    // we start with on
    this->ui->chopper_chk->setChecked(true);

    //    // on
    //    for (auto &on : this->ons) vf.push_back(on.f);
    //    this->amp_plt_on->create_fon_ranges(vf);
    //    this->sumcal->interploate(vf, -1, cal::chopper_on);
    //    vf.clear();

    //    // off
    //    for (auto &off : this->offs) vf.push_back(off.f);
    //    this->sumcal->interploate(vf, -1, cal::chopper_off);
    //    this->amp_plt_off->create_foff_ranges(vf);




    //    this->amp_plt_on->fon_ranges_area->setObjectName("fll_on");

    //    this->amp_plt_off->foff_ranges_area->setObjectName("fll_off");
    //    // not add widget now
    //    this->ui->fl->addWidget(this->amp_plt_on->fon_ranges_area.data());

    //    this->ui->fl->addWidget(this->amp_plt_off->foff_ranges_area.data());

    //    QObject::connect(this->amp_plt_on.get(),  &plotlib::f_on_selected,  this, &cal_stat_display_lib::slot_plot);
    //    QObject::connect(this->amp_plt_off.get(), &plotlib::f_off_selected, this, &cal_stat_display_lib::slot_plot);

    this->on_chopper_chk_clicked(true);
    this->adjustSize();

    return 1;
}

void cal_stat_display_lib::slot_plot(const double &f)
{

    // this->prepare_graphs();

    // clear the graphs except first
    for (auto &plot : plots) {
        plot->clear_downto(1);
    }



    std::vector<double> sers;

    double median;
    double offsetu;
    double offsetl;
    double border;

    int j = 0;

    QPen pen(Qt::green);
    QPen penll(Qt::darkMagenta);
    QPen entrypen(Qt::black);



    // AMPLITUDES
    // fetch the iterators and pointers
    this->anyplot = nullptr;
    for (auto &plot : plots) {
        if ((plot->chopper == this->chopper) && (plot->plot_type == cal::amplitude)) {
            this->anyplot = plot.get();
        }
    }
    if (this->anyplot == nullptr) return;

    this->f_tmp = f;

    for (this->citb = this->calstats.cbegin(); this->citb != this->calstats.cend(); ++this->citb) {
        if ((this->citb->f == f) && (this->citb->chopper == this->chopper)  && (citb->variance_ampls_data.size() == g_stat::xstat_size)) {
            sers = citb->serials_to_double();

            this->anyplot->graph(0)->vaddData(sers.cbegin(), sers.cend(), citb->ampls.cbegin(), citb->ampls.cend());
            if (chopper == cal::chopper_on) this->anyplot->graph(this->anyplot->graphCount()-1)->setName("amplitude chopper on");
            else this->anyplot->graph(this->anyplot->graphCount()-1)->setName("amplitude chopper off");

            median = citb->variance_ampls_data.at(g_stat::median_x);
            offsetu = median + citb->variance_ampls_data.at(g_stat::stddev_x);
            this->anyplot->add_hz_line(offsetu, sers.front(), sers.back(), pen);
            this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

            offsetl = median - citb->variance_ampls_data.at(g_stat::stddev_x);
            this->anyplot->add_hz_line(offsetl, sers.front(), sers.back(), pen);
            this->anyplot->graph(this->anyplot->graphCount()-1)->setName("1 sigma local");

            if (chopper == 1) {
                j = this->sumcal->f_index(this->sumcal->f_on_ipl, f);
                border = this->sumcal->ampl_on_ipl.at(j);
            }
            else {
                j = this->sumcal->f_index(this->sumcal->f_off_ipl, f);
                border = this->sumcal->ampl_off_ipl.at(j);
            }
            offsetu = border + citb->variance_ampls_data.at(g_stat::stddev_x) * 3.;
            this->anyplot->add_hz_line(offsetu, sers.front(), sers.back(), penll);
            this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

            offsetl = border - citb->variance_ampls_data.at(g_stat::stddev_x) * 3.;
            this->anyplot->add_hz_line(offsetl, sers.front(), sers.back(), penll);
            this->anyplot->graph(this->anyplot->graphCount()-1)->setName("3 sigma master");


            if (chopper == 1) {
                // cal entry points for only chopper on
                if (this->sumcal->sensortype == "MFS-10e" && (f == 0.1) ) {
                    this->anyplot->add_hz_line(0.20 + 0.20 * 0.02,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

                    this->anyplot->add_hz_line(0.20 - 0.20 * 0.02,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->setName("entry levels");

                }
                if (this->sumcal->sensortype == "MFS-06e" && (f == 0.1) ) {
                    this->anyplot->add_hz_line(0.20 + 0.20 * 0.02,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

                    this->anyplot->add_hz_line(0.20 - 0.20 * 0.02,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->setName("entry levels");

                }
                if (this->sumcal->sensortype == "MFS-07e" && (f == 0.4) ) {
                    this->anyplot->add_hz_line(0.020 + 0.20 * 0.02,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

                    this->anyplot->add_hz_line(0.020 - 0.20 * 0.02,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->setName("entry levels");

                }

            }

            this->anyplot->rescaleAxes();
            this->anyplot->yAxis->setRange(offsetl - offsetl * 0.7, offsetu + offsetu * 1.3);
            this->anyplot->xAxis->setNumberPrecision(0);
            this->anyplot->yAxis->setNumberPrecision(3);
            this->anyplot->legend->setVisible(true);
            this->anyplot->replot();

        }
    }

    // PHASES

    // fetch the iterators and pointers
    this->anyplot = nullptr;
    for (auto &plot : plots) {
        if ((plot->chopper == this->chopper) && (plot->plot_type == cal::phase_grad)) {
            this->anyplot = plot.get();
        }
    }
    if (this->anyplot == nullptr) return;

    j = 0;
    for (this->citb = this->calstats.cbegin(); this->citb != this->calstats.cend(); ++this->citb) {
        if ((this->citb->f == f) && (this->citb->chopper == this->chopper) && (citb->variance_phzs_data.size() == g_stat::xstat_size)) {

            this->anyplot->graph(0)->vaddData(sers.cbegin(), sers.cend(), citb->phzs.cbegin(), citb->phzs.cend());
            //this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();
            if (chopper == 1) this->anyplot->graph(this->anyplot->graphCount()-1)->setName("phase chopper on");
            else this->anyplot->graph(this->anyplot->graphCount()-1)->setName("phase chopper off");

            median = citb->variance_phzs_data.at(g_stat::median_x);
            offsetu = median + citb->variance_phzs_data.at(g_stat::stddev_x);

            this->anyplot->add_hz_line(offsetu, sers.front(), sers.back(), pen);
            this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

            offsetl = median - citb->variance_phzs_data.at(g_stat::stddev_x);
            this->anyplot->add_hz_line(offsetl, sers.front(), sers.back(), pen);
            this->anyplot->graph(this->anyplot->graphCount()-1)->setName("1 sigma local");

            if (chopper == 1) {
                j = this->sumcal->f_index(this->sumcal->f_on_ipl, f);
                border = this->sumcal->phase_grad_on_ipl.at(j);
            }
            else {
                j = this->sumcal->f_index(this->sumcal->f_off_ipl, f);
                border = this->sumcal->phase_grad_off_ipl.at(j);
            }
            offsetu = border + citb->variance_phzs_data.at(g_stat::stddev_x) * 3.;
            this->anyplot->add_hz_line(offsetu, sers.front(), sers.back(), penll);
            this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

            offsetl = border - citb->variance_phzs_data.at(g_stat::stddev_x) * 3.;
            this->anyplot->add_hz_line(offsetl, sers.front(), sers.back(), penll);
            this->anyplot->graph(this->anyplot->graphCount()-1)->setName("3 sigma master");

            if (chopper == 1) {
                // cal entry points for only chopper on

                if (this->sumcal->sensortype == "MFS-10e" && (f == 0.1) ) {
                    this->anyplot->add_hz_line(8.85700e+01 + 0.3,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

                    this->anyplot->add_hz_line(8.85700e+01 - 0.3,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->setName("entry levels");

                }
                if (this->sumcal->sensortype == "MFS-06e" && (f == 0.1) ) {
                    this->anyplot->add_hz_line(8.85700e+01 + 0.3,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

                    this->anyplot->add_hz_line(8.85700e+01 - 0.3,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->setName("entry levels");

                }
                if (this->sumcal->sensortype == "MFS-07e" && (f == 0.4) ) {
                    this->anyplot->add_hz_line(8.928e+01 + 0.3,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->removeFromLegend();

                    this->anyplot->add_hz_line(8.928e+01 - 0.3,  sers.front(), sers.back(), entrypen);
                    this->anyplot->graph(this->anyplot->graphCount()-1)->setName("entry levels");

                }
            }

            this->anyplot->rescaleAxes();
            this->anyplot->yAxis->setRange(offsetl - 3., offsetu + 3.);
            this->anyplot->xAxis->setNumberPrecision(0);
            this->anyplot->yAxis->setNumberPrecision(3);
            this->anyplot->legend->setVisible(true);
            this->anyplot->replot();
        }
    }

    // clear to avoid un-controlled access
    this->anyplot = nullptr;

}

void cal_stat_display_lib::xrange_changed(const QCPRange &newRange)
{

    for ( auto &plot : plots) {

        if ( (plot->chopper == this->chopper) && (newRange != plot->xAxis->range()) ) {
            plot->xAxis->setRange(newRange);
            plot->replot();
        }

    }

}

void cal_stat_display_lib::on_chopper_chk_clicked(bool checked)
{


    if (checked) this->chopper = cal::chopper_on;
    else this->chopper = cal::chopper_off;

    // save the window size
    this->wsize = this->size();


    for ( auto &plot : plots) {
        if (plot->chopper != this->chopper) plot->hide();

        // exchange the frequency buttons - which were taken from amplitude only
        // and generate the plot ONCE - slot pot plots plot always both
        if (plot->plot_type == cal::amplitude) {

            if ((plot->chopper == cal::chopper_off) && (this->chopper == cal::chopper_on) ) {
                plot->foff_ranges_area->hide();
            }

            if (this->chopper == cal::chopper_on) {
                if (plot->chopper == cal::chopper_off) plot->foff_ranges_area->hide();
                if (plot->chopper == cal::chopper_on) plot->fon_ranges_area->show();
                this->slot_plot(this->sumcal->f_on_ipl.at(0));

            }
            if (this->chopper == cal::chopper_off) {
                if (plot->chopper == cal::chopper_on)  plot->fon_ranges_area->hide();
                if (plot->chopper == cal::chopper_off) plot->foff_ranges_area->show();
                this->slot_plot(this->sumcal->f_off_ipl.at(0));
            }
        }
        if (plot->chopper == this->chopper) plot->show();
    }


    if (!this->count_me )this->adjustSize();
    else this->resize(this->wsize.width(), this->wsize.height());
    this->count_me++;


}

void cal_stat_display_lib::on_pushButton_rescale_clicked()
{
    if (this->f_tmp > 0.0000000000000000001)
    this->slot_plot(double(this->f_tmp));
}
