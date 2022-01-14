#include "xmlcallib.h"
#include "ui_xmlcallib.h"


xmlcallib::xmlcallib(const QFileInfo &in_info_db, const QFileInfo &in_master_cal_db, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xmlcallib)
{

    this->info_db = fall_back_default(in_info_db);
    this->master_cal_db = fall_back_default(in_master_cal_db);


    ui->setupUi(this);
    this->ui->x_y_label->setText("x: , y: (press left mouse button)");

    this->pblist_ampl = this->ui->tab_ampl->findChildren<QPushButton *>();
    this->pblist_phs = this->ui->tab_phase->findChildren<QPushButton *>();

    for (auto &pb : this->pblist_ampl) {
        this->pbmap.insert(pb, "tab_ampl");
    }
    for (auto &pb : this->pblist_phs) {
        this->pbmap.insert(pb, "tab_phase");
    }


    this->setAcceptDrops(true);
    this->ui->tabWidget->setAcceptDrops(true);
    this->ui->tabWidget->acceptDrops();
    // set the status to chopper on
    this->ui->pushButton_Chopper_On->setChecked(true);
    this->ui->pushButton_Chopper_On_2->setChecked(true);
    this->ui->pushButton_Chopper_On->setDown(true);
    this->ui->pushButton_Chopper_On_2->setDown(true);
    // connect the buttons from the phase tab to the amplitude tab
    QObject::connect(this->ui->pushButton_rescale_2, &QPushButton::clicked, this, &xmlcallib::on_pushButton_rescale_clicked );
    QObject::connect(this->ui->pushButton_Chopper_Off_2, &QPushButton::clicked, this, &xmlcallib::on_pushButton_Chopper_Off_clicked );
    QObject::connect(this->ui->pushButton_Chopper_On_2, &QPushButton::clicked, this, &xmlcallib::on_pushButton_Chopper_On_clicked );
    QObject::connect(this->ui->pushButton_Chopper_Both_2, &QPushButton::clicked, this, &xmlcallib::on_pushButton_Chopper_Both_clicked );
    QObject::connect(this->ui->pushButton_show_master_cal_2, &QPushButton::clicked, this, &xmlcallib::on_pushButton_show_master_cal_clicked );
    QObject::connect(this->ui->pushButton_ipl_2, &QPushButton::clicked, this, &xmlcallib::on_pushButton_ipl_clicked );
    QObject::connect(this->ui->pushButton_theo_2, &QPushButton::clicked, this, &xmlcallib::on_pushButton_theo_clicked );

    // QObject::connect(this, &xmlcallib::scale_me, this, &xmlcallib::replot_all);


    this->qdw = new QDockWidget("info", this);
    this->qdw->setAllowedAreas(Qt::LeftDockWidgetArea);
    this->qlw = new QListWidget(this->qdw);
    this->qdw->setWidget(this->qlw);
    addDockWidget(Qt::LeftDockWidgetArea, this->qdw, Qt::Vertical);
    this->qdw->setFloating(true);
    this->qdw->hide();

    this->sel = new select_master_dialog(this->info_db.absoluteFilePath(), QChar('H'), this);


    connect(this->sel, &select_master_dialog::master_sensor_selected, this, &xmlcallib::set_potential_master_cal );
    connect(this->sel, &select_master_dialog::accepted, this, &xmlcallib::load_master_only );

    this->sensor_for_master_str = this->sel->current_master_selection();


    this->manual = new manual_scales(this);
    connect(this->manual, &manual_scales::ayrange_changed, this, &xmlcallib::ayrange_changed);
    connect(this->manual, &manual_scales::pyrange_changed, this, &xmlcallib::pyrange_changed);
    connect(this->manual, &manual_scales::xrange_changed, this, &xmlcallib::xrange_changed);

    this->rlabels = new add_range_labels(this);
    this->rlabels->setModal(false);
    connect(this->rlabels, &add_range_labels::signal_range_label_updated, this, &xmlcallib::slot_range_label_updated);



}

void xmlcallib::set_file(const QFileInfoList &qfx)
{
    QFileInfo qfix(qfx.at(0));
    QFileInfoList qfi_all;
    // take the 1st xml in dir
    if (qfix.isDir()) {
        QDir qdir (qfix.absoluteFilePath());
        QStringList entries;
        entries << "*.xml" << "*.XML";
        QFileInfoList xmlf = qdir.entryInfoList(entries, QDir::Files);
        if (!xmlf.size()) return;
        qfi_all.append(QFileInfo(xmlf.at(0).absoluteFilePath()));
    }
    // or take the given xml
    // we may want to delete the pointer

    bool all_is_xml = true;
    for (auto &test: qfx) {
        if (!test.fileName().endsWith("xml", Qt::CaseInsensitive)) all_is_xml = false;
    }

    if (all_is_xml) {
        this->clear(true);
        qfi_all = qfx;
        for (auto &qfi : qfi_all) {
            if (qfi.fileName().endsWith("xml", Qt::CaseInsensitive)) {
                //this->clear(true);

                // create a calibration for each channel and load base data needed

                int j = int(channels.size());
                for (int i = channels.size(); i < j + 10; ++i) {
                    this->channels.emplace_back(std::make_shared<calibration>(QFileInfo(this->info_db.absoluteFilePath()), i, -1, true, "", this));

                }

                // we additionally load master data
                int i = 0;
                auto it = channels.begin() + j;

                while (it != channels.end() ) {
                    //            chan->read_measdoc(i, cal::chopper_off, qfi);
                    //            chan->read_measdoc(i, cal::chopper_on, qfi);
                    it->get()->read_measdoc(i, cal::chopper_undef, qfi);
                    ++i;
                    ++it;
                }
                //                for (auto &chan : channels) {
                //                    //            chan->read_measdoc(i, cal::chopper_off, qfi);
                //                    //            chan->read_measdoc(i, cal::chopper_on, qfi);
                //                    chan->read_measdoc(i, cal::chopper_undef, qfi);
                //                    ++i;
                //                }

                qDebug() << "add plots";
                //this->channelthreads.clear();


                for (it = channels.begin() + j ; it != channels.end(); ) {
                    if ( (it->get()->f_on.size() == 0) && (it->get()->f_off.size() == 0) ) {
                        // delivers an already incremented pointer!
                        it = channels.erase(it);
                    } else {
                        ++it;
                    }
                }




            }

        }

        // remove duplicates

        size_t i = 0, j = 0;
        for (auto &chan : channels) {
            j = 0;
            for (auto &chan_other : channels) {
                if (j > i) {
                    if ((chan != nullptr) && (chan_other != nullptr)) {
                        if (chan.get() == chan_other.get()) {
                            chan.reset();
                        }
                    }
                }
                ++j;
            }
            ++i;
        }
        if (!channels.size()) {
            QMessageBox msgBox;
            msgBox.setText("The XML does not contain any calibration");
            msgBox.exec();
            return;


        }
        std::vector<std::shared_ptr<calibration>> channels_tmp;
        channels_tmp.reserve(channels.size());
        for (auto &chan : channels) {
            if (chan != nullptr) channels_tmp.push_back(chan);

        }
        std::swap(channels_tmp, channels);


        for (auto &chan : channels) {
            chan->open_master_cal(QFileInfo(this->master_cal_db.absoluteFilePath()));
            chan->get_master_cal();
            chan->chopper = cal::chopper_off;
            if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(chan->f_off);
            else chan->gen_theo_cal(chan->f_off);
            chan->chopper = cal::chopper_on;
            if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(chan->f_on);
            else chan->gen_theo_cal(chan->f_on);

        }

    }
    else if ( this->urls.size() && (qfix.fileName().endsWith("txt", Qt::CaseInsensitive) || qfix.fileName().endsWith("json", Qt::CaseInsensitive))) {
        this->clear(true);
        for (int i = 0; i < urls.size(); ++i) {
            size_t nf = 0;
            auto scal = std::make_shared<calibration>(QFileInfo(this->info_db.absoluteFilePath()), i, -1, true, "", this);
            auto qfiptr = std::make_unique<QFileInfo>(this->urls.at(i).toLocalFile());
            if (scal->read_std_txt_file(i, QFileInfo(this->urls.at(i).toLocalFile())) || scal->from_JSON(i, qfiptr.get())) {
                this->channels.emplace_back(std::make_shared<calibration>(QFileInfo(this->info_db.absoluteFilePath()), i, -1, true, "", this));
                if (qfix.fileName().endsWith("txt", Qt::CaseInsensitive)) {
                    nf = this->channels.back()->read_std_txt_file(i, QFileInfo(this->urls.at(i).toLocalFile()));

                }
                else if (qfix.fileName().endsWith("json", Qt::CaseInsensitive)) {
                    this->channels.back()->from_JSON(i, qfiptr.get());
                }
            }
            qfiptr.reset();
            scal.reset();

        }

        int i = 0;
        QList<int> check;
        for (auto &chan : channels) {
            check.append( chan->read_std_txt_file(i, QFileInfo(this->urls.at(i).toLocalFile())));
            ++i;
        }

        i = 0;
        for (auto &chan : channels) {
            if (check.at(i))   chan->open_master_cal(QFileInfo(this->master_cal_db.absoluteFilePath()));
            if (check.at(i))   chan->get_master_cal();
            if (check.at(i)) {
                chan->chopper = cal::chopper_off;
                if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(chan->f_off);
                else chan->gen_theo_cal(chan->f_off);
                chan->chopper = cal::chopper_on;
                if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(chan->f_on);
                else chan->gen_theo_cal(chan->f_on);
            }
            ++i;

        }
    }

    // for testing only

    //    SQL_vector sqlv(this->info_db.absoluteFilePath());
    //    std::vector<double> default_f;
    //    sqlv.get_vector("default_mt_frequencies", "frequencies", default_f);
    //    std::sort(default_f.begin(), default_f.end());
    //    for (auto &chan : channels) {
    //        chan->interploate(default_f, -1, 1);
    //    }
    //    for (auto &chan : channels) {
    //        chan->interploate(default_f, -1, 0);
    //    }


    if (channels.size()) this->append_plots();

    bool has_chopper_on = false;
    bool has_chopper_off = false;

    for (auto &chan : this->channels) {
        if (chan->f_on.size()) has_chopper_on = true;
        if (chan->f_off.size()) has_chopper_off = true;

    }

    if (!has_chopper_on) this->ui->pushButton_Chopper_Off_2->click();
    else if(!has_chopper_off) this->ui->pushButton_Chopper_On_2->click();

}


void xmlcallib::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();

}

void xmlcallib::dropEvent(QDropEvent *event)
{
    this->urls.clear();
    this->urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    QFileInfoList qfx;
    for (auto &urlls : this->urls) qfx.append(QFileInfo(urlls.toLocalFile()));
    this->qdw->show();
    this->qlw->clear();
    this->set_file(qfx);

}

void xmlcallib::rename_ipl_button(const QString newname)
{
    this->ui->pushButton_ipl->setText(newname);
    this->ui->pushButton_ipl_2->setText(newname);
}


xmlcallib::~xmlcallib()
{
    this->clear();
    delete ui;
}

void xmlcallib::slot_set_cal(std::shared_ptr<calibration> cal_channel, const bool nooverlay)
{


    qDebug() << cal_channel->sensortype << cal_channel->sernum;
    QFileInfo qfxx(cal_channel->get_dbname_info());
    int chno(cal_channel->get_channel_no());
    int slotno(cal_channel->get_slot_no());

    this->clear(nooverlay);
    if (!nooverlay) {
        this->channels.emplace_back(std::make_shared<calibration>(qfxx, chno, slotno, false, "", this));
        this->channels.back()->clone_caldata(cal_channel);
    }
    else {
        this->channels.push_back(cal_channel);
    }

    cal::cal chop;
    for (auto &chan : this->channels) {
        chop = chan->chopper;
        chan->chopper = cal::chopper_off;
        if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(chan->f_off);
        else chan->gen_theo_cal(chan->f_off);
        chan->chopper = cal::chopper_on;
        if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(chan->f_off);
        else chan->gen_theo_cal(chan->f_on);
        chan->chopper = chop;
    }
    this->append_plots();

}

void xmlcallib::slot_set_newfreqs(const std::vector<double> &ff)
{
    std::vector<double> default_f(ff);
    std::sort(default_f.begin(), default_f.end());
    for (auto &chan : channels) {
        chan->interploate(default_f, -1, cal::chopper_on);
    }
    for (auto &chan : channels) {
        chan->interploate(default_f, -1, cal::chopper_off);
    }
}

void xmlcallib::append_plots()
{

    if (!this->channels.size()) return;
    size_t i, j, k;
    // rsistivity and phase plots
    if (!this->ui->checkBox->isChecked()) {
        for (i = 0; i < channels.size(); ++i ) {
            this->amplplots.emplace_back(std::make_shared<plotlib>("amplplots", this));
            this->phaseplots.emplace_back(std::make_shared<plotlib>("phaseplots", this));
        }
    }
    // only one amplitude and phase
    else {
        this->amplplots.emplace_back(std::make_shared<plotlib>("amplplots", this));
        this->phaseplots.emplace_back(std::make_shared<plotlib>("phaseplots", this));
    }


    j = 0;
    for (i = 0; i < this->channels.size(); ++i) {
        if (!this->ui->checkBox->isChecked()) j = 0;
        auto plt = this->amplplots[0];
        if (!this->ui->checkBox->isChecked()) plt = this->amplplots[i];
        plt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
        plt->legend->setVisible(true);

        connect(plt.get(), &plotlib::signal_x_y_pos, this, &xmlcallib::slot_x_y_pos);
        connect(plt.get(), &plotlib::axisClick, this, &xmlcallib::xyaxis_clicked);


        if (this->ui->pushButton_Chopper_Both->isChecked() || this->ui->pushButton_Chopper_On->isChecked() ) {

            plt->set_ScaleType_log_xy("amplitude");
            if (this->ui->checkBox_un->isChecked()) {
                std::vector<double> unn(this->channels.at(i)->ampl_on);
                size_t ff = 0;
                for (auto &u : unn) u *= this->channels.at(i)->f_on[ff++];
                plt->graph(j)->setData(this->channels.at(i)->f_on, unn);
            }
            else plt->graph(j)->setData(this->channels.at(i)->f_on, this->channels.at(i)->ampl_on);
            plt->graph(j)->setPen(QPen(Qt::blue));
            if (j == 0) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 12));
            if (j == 1) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));
            if (j == 2) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 12));
            if (j == 3) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 12));
            if (j == 4) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar, 12));
            if (j == 5) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 6) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 12));
            if (j == 7) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 12));
            if (j == 8) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 12));
            if (j == 9) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusSquare, 12));
            if (j == 10) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 11) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusCircle, 12));
            if (j == 12) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPeace, 12));


            if (this->channels.at(i)->sensortype.contains("ADB")) {
                plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (on)");
            }
            else plt->graph(j)->setName(this->channels.at(i)->sensortype + " " + QString::number(channels.at(i)->sernum) + " (on)");
            if (this->channels.at(i)->f_on.size() == this->channels.at(i)->ampl_on_stddev.size()) {
                // this pointer will be deleted by qcustomplot when deleting the shared pointer
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->ampl_on_stddev);
                errorBars->setData(this->channels.at(i)->ampl_on_stddev.cbegin(), this->channels.at(i)->ampl_on_stddev.cend());
            }

            ++j;

        }

        if (this->ui->pushButton_Chopper_Both->isChecked()  || this->ui->pushButton_Chopper_Off->isChecked()  ) {

            plt->set_ScaleType_log_xy("amplitude");
            if (this->ui->checkBox_un->isChecked()) {
                std::vector<double> unn(this->channels.at(i)->ampl_off);
                size_t ff = 0;
                for (auto &u : unn) u *= this->channels.at(i)->f_off[ff++];
                plt->graph(j)->setData(this->channels.at(i)->f_off, unn);
            }
            else plt->graph(j)->setData(this->channels.at(i)->f_off, this->channels.at(i)->ampl_off);
            if (j == 0) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 12));
            if (j == 1) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));
            if (j == 2) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 12));
            if (j == 3) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 12));
            if (j == 4) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar, 12));
            if (j == 5) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 6) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 12));
            if (j == 7) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 12));
            if (j == 8) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 12));
            if (j == 9) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusSquare, 12));
            if (j == 10) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 11) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusCircle, 12));
            if (j == 12) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPeace, 12));
            plt->graph(j)->setPen(QPen(Qt::red));
            if (this->channels.at(i)->sensortype.contains("ADB")) {
                plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (off)");
            }
            else plt->graph(j)->setName(this->channels.at(i)->sensortype + " " + QString::number(channels.at(i)->sernum) + " (off)");
            if (this->channels.at(i)->f_off.size() == this->channels.at(i)->ampl_off_stddev.size()) {
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->ampl_off_stddev);
                errorBars->setData(this->channels.at(i)->ampl_off_stddev.cbegin(), this->channels.at(i)->ampl_off_stddev.cend());
            }
            ++j;

        }

        if ((this->add_master_plot == 2) || (this->add_master_plot == 3)) {
            plt->set_ScaleType_log_xy("amplitude");
            if (this->ui->checkBox_un->isChecked()) {
                std::vector<double> unn(this->channels.at(i)->ampl_off_master);
                size_t ff = 0;
                for (auto &u : unn) u *= this->channels.at(i)->f_off_master[ff++];
                plt->graph(j)->setData(this->channels.at(i)->f_off_master, unn);
            }
            else plt->graph(j)->setData(this->channels.at(i)->f_off_master, this->channels.at(i)->ampl_off_master);
            plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 10));
            plt->graph(j)->setPen(QPen(Qt::green, 4));
            plt->graph(j)->setName(this->channels.at(i)->sensortype + " master off");
            if (this->channels.at(i)->f_off_master.size() == this->channels.at(i)->ampl_off_master_stddev.size()) {
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->ampl_off_master_stddev);
                errorBars->setData(this->channels.at(i)->ampl_off_master_stddev.cbegin(), this->channels.at(i)->ampl_off_master_stddev.cend());
            }
            ++j;

        }

        if ((this->add_master_plot == 1) || (this->add_master_plot == 3)) {
            plt->set_ScaleType_log_xy("amplitude");
            if (this->ui->checkBox_un->isChecked()) {
                std::vector<double> unn(this->channels.at(i)->ampl_on_master);
                size_t ff = 0;
                for (auto &u : unn) u *= this->channels.at(i)->f_on_master[ff++];
                plt->graph(j)->setData(this->channels.at(i)->f_on_master, unn);
            }
            else plt->graph(j)->setData(this->channels.at(i)->f_on_master, this->channels.at(i)->ampl_on_master);
            plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 10));
            plt->graph(j)->setPen(QPen(Qt::darkGreen));
            plt->graph(j)->setName(this->channels.at(i)->sensortype + " master on");
            if (this->channels.at(i)->f_on_master.size() == this->channels.at(i)->ampl_on_master_stddev.size()) {
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->ampl_on_master_stddev);
                errorBars->setData(this->channels.at(i)->ampl_on_master_stddev.cbegin(), this->channels.at(i)->ampl_on_master_stddev.cend());
            }

            ++j;

        }

        if (this->channels.at(i)->f_off_theo.size()) {
            if ((this->add_theo_plot == 2) || (this->add_theo_plot == 3)) {
                plt->set_ScaleType_log_xy("amplitude");
                if (this->ui->checkBox_un->isChecked()) {
                    std::vector<double> unn(this->channels.at(i)->ampl_off_theo);
                    size_t ff = 0;
                    for (auto &u : unn) u *= this->channels.at(i)->f_off_theo[ff++];
                    plt->graph(j)->setData(this->channels.at(i)->f_off_theo, unn);
                }
                else plt->graph(j)->setData(this->channels.at(i)->f_off_theo, this->channels.at(i)->ampl_off_theo);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 10));
                plt->graph(j)->setPen(QPen(Qt::magenta));
                if (this->channels.at(i)->sensortype.contains("ADB")) {
                    plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (theo off)");
                }
                else plt->graph(j)->setName(this->channels.at(i)->sensortype + " theo off");
                if (this->channels.at(i)->f_off_theo.size() == this->channels.at(i)->ampl_off_theo_stddev.size()) {
                    QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    errorBars->removeFromLegend();
                    errorBars->setDataPlottable(plt->graph(j));
                    //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->ampl_off_theo_stddev);
                    errorBars->setData(this->channels.at(i)->ampl_off_theo_stddev.cbegin(), this->channels.at(i)->ampl_off_theo_stddev.cend());
                }
                ++j;

            }
        }
        if (this->channels.at(i)->f_on_theo.size()) {
            if ((this->add_theo_plot == 1) || (this->add_theo_plot == 3)) {
                plt->set_ScaleType_log_xy("amplitude");
                if (this->ui->checkBox_un->isChecked()) {
                    std::vector<double> unn(this->channels.at(i)->ampl_on_theo);
                    size_t ff = 0;
                    for (auto &u : unn) u *= this->channels.at(i)->f_on_theo[ff++];
                    plt->graph(j)->setData(this->channels.at(i)->f_on_theo, unn);
                }
                else plt->graph(j)->setData(this->channels.at(i)->f_on_theo, this->channels.at(i)->ampl_on_theo);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 10));
                plt->graph(j)->setPen(QPen(Qt::darkMagenta));
                if (this->channels.at(i)->sensortype.contains("ADB")) {
                    plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (theo off)");
                }
                else plt->graph(j)->setName(this->channels.at(i)->sensortype + " theo on");
                if (this->channels.at(i)->f_on_theo.size() == this->channels.at(i)->ampl_on_theo_stddev.size()) {
                    QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    errorBars->removeFromLegend();
                    errorBars->setDataPlottable(plt->graph(j));
                    //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->ampl_on_theo_stddev);
                    errorBars->setData(this->channels.at(i)->ampl_on_theo_stddev.cbegin(), this->channels.at(i)->ampl_on_theo_stddev.cend());
                }

                ++j;

            }
        }


        if (this->channels.at(i)->f_off_ipl.size()) {
            if ((this->add_ipl_plot == 2) || (this->add_ipl_plot == 3)) {
                plt->set_ScaleType_log_xy("amplitude");
                if (this->ui->checkBox_un->isChecked()) {
                    std::vector<double> unn(this->channels.at(i)->ampl_off_ipl);
                    size_t ff = 0;
                    for (auto &u : unn) u *= this->channels.at(i)->f_off_ipl[ff++];
                    plt->graph(j)->setData(this->channels.at(i)->f_off_ipl, unn);
                }
                else plt->graph(j)->setData(this->channels.at(i)->f_off_ipl, this->channels.at(i)->ampl_off_ipl);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 10));
                plt->graph(j)->setPen(QPen(Qt::gray));
                plt->graph(j)->setName(this->channels.at(i)->sensortype + " ipl off");

                ++j;

            }
        }

        if (this->channels.at(i)->f_on_ipl.size()) {
            if ((this->add_ipl_plot == 1) || (this->add_ipl_plot == 3)) {
                plt->set_ScaleType_log_xy("amplitude");
                if (this->ui->checkBox_un->isChecked()) {
                    std::vector<double> unn(this->channels.at(i)->ampl_on_ipl);
                    size_t ff = 0;
                    for (auto &u : unn) u *= this->channels.at(i)->f_on_ipl[ff++];
                    plt->graph(j)->setData(this->channels.at(i)->f_on_ipl, unn);
                }
                else plt->graph(j)->setData(this->channels.at(i)->f_on_ipl, this->channels.at(i)->ampl_on_ipl);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 10));
                plt->graph(j)->setPen(QPen(Qt::darkGray));
                plt->graph(j)->setName(this->channels.at(i)->sensortype + " ipl on");
                ++j;

            }
        }

        connect(plt->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xrange_changed(QCPRange)));
        connect(plt->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(ayrange_changed(QCPRange)));
        plt->rescaleAxes();
        this->ui->plots_ampl->addWidget(plt.get());


    }

    j = 0;
    for (i = 0; i < this->channels.size(); ++i) {
        if (!this->ui->checkBox->isChecked()) j = 0;

        auto plt = this->phaseplots[0];
        if (!this->ui->checkBox->isChecked()) plt = this->phaseplots[i];

        plt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
        plt->legend->setVisible(true);

        connect(plt.get(), &plotlib::signal_x_y_pos, this, &xmlcallib::slot_x_y_pos);
        connect(plt.get(), &plotlib::axisClick, this, &xmlcallib::xyaxis_clicked);




        if (this->ui->pushButton_Chopper_Both->isChecked() || this->ui->pushButton_Chopper_On->isChecked() ) {

            plt->set_ScaleType_log_x_lin_y("phase [deg]");
            if (j == 0) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 12));
            if (j == 1) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));
            if (j == 2) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 12));
            if (j == 3) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 12));
            if (j == 4) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar, 12));
            if (j == 5) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 6) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 12));
            if (j == 7) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 12));
            if (j == 8) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 12));
            if (j == 9) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusSquare, 12));
            if (j == 10) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 11) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusCircle, 12));
            if (j == 12) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPeace, 12));
            plt->graph(j)->setPen(QPen(Qt::blue));
            plt->graph(j)->setData(this->channels.at(i)->f_on, this->channels.at(i)->phase_grad_on);
            if (this->channels.at(i)->sensortype.contains("ADB")) {
                plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (on)");
            }
            else plt->graph(j)->setName(this->channels.at(i)->sensortype + " " + QString::number(channels.at(i)->sernum) + " (on)");
            if (this->channels.at(i)->f_on.size() == this->channels.at(i)->phase_grad_on_stddev.size()) {
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->phase_grad_on_stddev);
                errorBars->setData(this->channels.at(i)->phase_grad_on_stddev.cbegin(), this->channels.at(i)->phase_grad_on_stddev.cend());
            }
            ++j;

        }

        if (this->ui->pushButton_Chopper_Both->isChecked()  || this->ui->pushButton_Chopper_Off->isChecked()  ) {

            plt->set_ScaleType_log_x_lin_y("phase [deg]");
            plt->graph(j)->setData(this->channels.at(i)->f_off, this->channels.at(i)->phase_grad_off);
            if (j == 0) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 12));
            if (j == 1) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));
            if (j == 2) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 12));
            if (j == 3) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 12));
            if (j == 4) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar, 12));
            if (j == 5) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 6) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 12));
            if (j == 7) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 12));
            if (j == 8) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 12));
            if (j == 9) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusSquare, 12));
            if (j == 10) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
            if (j == 11) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlusCircle, 12));
            if (j == 12) plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPeace, 12));
            plt->graph(j)->setPen(QPen(Qt::red));
            if (this->channels.at(i)->sensortype.contains("ADB")) {
                plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (off)");
            }
            else plt->graph(j)->setName(this->channels.at(i)->sensortype + " " + QString::number(channels.at(i)->sernum) + " (off)");
            if (this->channels.at(i)->f_off.size() == this->channels.at(i)->phase_grad_off_stddev.size()) {
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->phase_grad_off_stddev);
                errorBars->setData(this->channels.at(i)->phase_grad_off_stddev.cbegin(), this->channels.at(i)->phase_grad_off_stddev.cend());
            }
            ++j;

        }

        if ((this->add_master_plot == 2) || (this->add_master_plot == 3)) {

            plt->set_ScaleType_log_x_lin_y("phase [deg]");
            plt->graph(j)->setData(this->channels.at(i)->f_off_master, this->channels.at(i)->phase_grad_off_master);
            plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 10));
            plt->graph(j)->setPen(QPen(Qt::green));
            plt->graph(j)->setName(this->channels.at(i)->sensortype + " master off");
            if (this->channels.at(i)->f_off_master.size() == this->channels.at(i)->phase_grad_off_master_stddev.size()) {
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->phase_grad_off_master_stddev);
                errorBars->setData(this->channels.at(i)->phase_grad_off_master_stddev.cbegin(), this->channels.at(i)->phase_grad_off_master_stddev.cend());
            }
            ++j;

        }

        if ((this->add_master_plot == 1) || (this->add_master_plot == 3)) {

            plt->set_ScaleType_log_x_lin_y("phase [deg]");
            plt->graph(j)->setData(this->channels.at(i)->f_on_master, this->channels.at(i)->phase_grad_on_master);
            plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 10));
            plt->graph(j)->setPen(QPen(Qt::darkGreen));
            plt->graph(j)->setName(this->channels.at(i)->sensortype + " master on");
            if (this->channels.at(i)->f_on_master.size() == this->channels.at(i)->phase_grad_on_master_stddev.size()) {
                QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                errorBars->removeFromLegend();
                errorBars->setDataPlottable(plt->graph(j));
                //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->phase_grad_on_master_stddev);
                errorBars->setData(this->channels.at(i)->phase_grad_on_master_stddev.cbegin(), this->channels.at(i)->phase_grad_on_master_stddev.cend());
            }
            ++j;

        }

        if (this->channels.at(i)->f_off_theo.size()) {
            if ((this->add_theo_plot == 2) || (this->add_theo_plot == 3)) {
                plt->set_ScaleType_log_x_lin_y("phase [deg]");
                plt->graph(j)->setData(this->channels.at(i)->f_off_theo, this->channels.at(i)->phase_grad_off_theo);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 10));
                plt->graph(j)->setPen(QPen(Qt::magenta));
                if (this->channels.at(i)->sensortype.contains("ADB")) {
                    plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (theo off)");
                }
                else plt->graph(j)->setName(this->channels.at(i)->sensortype + " theo off");
                if (this->channels.at(i)->f_off_theo.size() == this->channels.at(i)->phase_grad_off_theo_stddev.size()) {
                    QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    errorBars->removeFromLegend();
                    errorBars->setDataPlottable(plt->graph(j));
                    //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->phase_grad_off_theo_stddev);
                    errorBars->setData(this->channels.at(i)->phase_grad_off_theo_stddev.cbegin(), this->channels.at(i)->phase_grad_off_theo_stddev.cend());
                }
                ++j;

            }
        }
        if (this->channels.at(i)->f_on_theo.size()) {
            if ((this->add_theo_plot == 1) || (this->add_theo_plot == 3)) {
                plt->set_ScaleType_log_x_lin_y("phase [deg]");
                plt->graph(j)->setData(this->channels.at(i)->f_on_theo, this->channels.at(i)->phase_grad_on_theo);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 10));
                plt->graph(j)->setPen(QPen(Qt::darkMagenta));
                if (this->channels.at(i)->sensortype.contains("ADB")) {
                    plt->graph(j)->setName(this->channels.at(i)->get_adb_name() + " (theo on)");
                }
                else
                    plt->graph(j)->setName(this->channels.at(i)->sensortype + " theo on");
                if (this->channels.at(i)->f_on_theo.size() == this->channels.at(i)->phase_grad_on_theo_stddev.size()) {
                    QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    //QCPErrorBars *errorBars = new QCPErrorBars(plt->xAxis, plt->yAxis);
                    errorBars->removeFromLegend();
                    errorBars->setDataPlottable(plt->graph(j));
                    //QVector<double> eb = QVector<double>::fromStdVector(this->channels.at(i)->phase_grad_on_theo_stddev);
                    errorBars->setData(this->channels.at(i)->phase_grad_on_theo_stddev.cbegin(), this->channels.at(i)->phase_grad_on_theo_stddev.cend());
                }

                ++j;

            }
        }



        if (this->channels.at(i)->f_off_ipl.size()) {
            if ((this->add_ipl_plot == 2) || (this->add_ipl_plot == 3)) {
                plt->set_ScaleType_log_x_lin_y("phase [deg]");
                plt->graph(j)->setData(this->channels.at(i)->f_off_ipl, this->channels.at(i)->phase_grad_off_ipl);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 10));
                plt->graph(j)->setPen(QPen(Qt::gray));
                plt->graph(j)->setName(this->channels.at(i)->sensortype + " ipl off");
                ++j;

            }
        }

        if (this->channels.at(i)->f_on_ipl.size()) {
            if ((this->add_ipl_plot == 1) || (this->add_ipl_plot == 3)) {
                plt->set_ScaleType_log_x_lin_y("phase [deg]");
                plt->graph(j)->setData(this->channels.at(i)->f_on_ipl, this->channels.at(i)->phase_grad_on_ipl);
                plt->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 10));
                plt->graph(j)->setPen(QPen(Qt::darkGray));
                plt->graph(j)->setName(this->channels.at(i)->sensortype + " ipl on");
                ++j;

            }
        }



        connect(plt->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xrange_changed(QCPRange)));
        connect(plt->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(pyrange_changed(QCPRange)));
        plt->rescaleAxes();
        this->ui->plots_phase->addWidget(plt.get());

    }

    //emit this->scale_me();
    qDebug() << "add plots finished" ;
    //this->repaint();


}

void xmlcallib::clear(bool caldata_also)
{
    for (auto &plt : this->amplplots) {
        if (plt != nullptr) {
            plt.reset();
        }
    }
    for (auto &plt : this->phaseplots) {
        if (plt != nullptr) {
            plt.reset();
        }
    }



    this->amplplots.clear();
    this->phaseplots.clear();

    if (caldata_also) {
        for (auto &chan : this->channels) chan.reset();
        this->channels.clear();
    }

}

void xmlcallib::replot_all()
{
    if (this->phaseplots.size()) {
        for (auto &plt : this->phaseplots) {
            plt->update();
        }
    }
    if (this->amplplots.size()) {
        for (auto &plt : this->amplplots) {
            plt->update();
        }
    }

    qDebug() << "replot all";

}

void xmlcallib::xrange_changed(const QCPRange &range)
{

    for ( auto &plt : this->phaseplots) {
        QCPRange indrange = plt->xAxis->range();
        if (indrange != range)  {
            plt->xAxis->setRange(range);
            plt->replot();
        }
    } for ( auto &plt : this->amplplots) {
        QCPRange indrange = plt->xAxis->range();
        if (indrange != range)  {
            plt->xAxis->setRange(range);
            plt->replot();
        }
    }

}

void xmlcallib::ayrange_changed(const QCPRange &range)
{
    for (auto &plt : this->amplplots) {
        QCPRange indrange = plt->yAxis->range();
        if (indrange != range)  {
            plt->yAxis->setRange(range);
            plt->replot();
        }
    }
}
void xmlcallib::pyrange_changed(const QCPRange &range)
{
    for (auto &plt : this->phaseplots) {
        QCPRange indrange = plt->yAxis->range();
        if (indrange != range)  {
            plt->yAxis->setRange(range);
            plt->replot();
        }
    }
}

void xmlcallib::slot_range_label_updated()
{
    for (auto &plt : this->phaseplots) plt->replot();
    for (auto &plt : this->amplplots) plt->replot();
}


void xmlcallib::rx_cal_message(const int &channel, const int &slot, const QString &message)
{
    if (this->qdw->isHidden()) this->qdw->show();
    qDebug() << channel << message;
    this->qlw->addItem(QString::number(channel) + " -> " + message);
}



void xmlcallib::on_pushButton_rescale_clicked()
{
    if (this->phaseplots.size()) {
        for (auto &plt : this->phaseplots) {
            plt->rescaleAxes();
            plt->replot();
        }
    }
    if (this->amplplots.size()) {
        for (auto &plt : this->amplplots) {
            plt->rescaleAxes();
            plt->replot();
        }
    }

}


void xmlcallib::on_pushButton_Chopper_Off_clicked()
{
    this->clear();
    this->pblcear("pushButton_Chopper_Off");
    if (this->add_master_plot) this->add_master_plot = 2;
    if (this->add_ipl_plot) this->add_ipl_plot = 2;
    if (this->add_theo_plot) this->add_theo_plot = 2;

    this->append_plots();
}

void xmlcallib::on_pushButton_Chopper_On_clicked()
{
    this->clear();
    this->pblcear("pushButton_Chopper_On");
    if (this->add_master_plot) this->add_master_plot = 1;
    if (this->add_ipl_plot) this->add_ipl_plot = 1;
    if (this->add_theo_plot) this->add_theo_plot = 1;

    this->append_plots();

}

void xmlcallib::on_pushButton_theo_clicked()
{
    this->clear();
    QString resched;


    if (this->add_theo_plot) this->add_theo_plot = 0;
    else this->add_theo_plot = 1;

    QMapIterator<QPushButton *, QString> pbi(this->pbmap);
    while (pbi.hasNext()) {
        pbi.next();
        if (pbi.key()->isChecked()) {
            resched = pbi.key()->objectName();
        }
    }
    this->pblcear(resched);

    qDebug() << "------------" << resched;

    if (this->add_theo_plot) {
        if (resched.contains("Off", Qt::CaseInsensitive)) {
            this->add_theo_plot = 2;
        }
        if (resched.contains("Both", Qt::CaseInsensitive)) {
            this->add_theo_plot = 3;
        }
    }
    this->append_plots();
}


void xmlcallib::on_pushButton_ipl_clicked()
{
    this->clear();
    QString resched;

    cal::cal chop;
    std::vector<double> fson;
    std::vector<double> fsoff;

    for (auto &chan : channels) {

        chop = chan->chopper;

        if (!chan->f_on_ipl.size()) {
            fson = chan->gen_cal_freqs(chan->f_on.front()/10. ,chan->f_on.back() * 2., 30 );
        }

        if (!chan->f_off_ipl.size()) {
            fsoff = chan->gen_cal_freqs(chan->f_off.front()/2. ,chan->f_off.back() * 2., 30  );
        }



        chan->chopper = cal::chopper_off;
        chan->interploate(fsoff, chan->get_channel_no(), cal::chopper_off, true, true);
        chan->chopper = cal::chopper_on;
        chan->interploate(fson, chan->get_channel_no(), cal::chopper_on, true, true);
        chan->chopper = chop;
        fson.clear();
        fsoff.clear();

    }

    if (this->add_ipl_plot) this->add_ipl_plot = 0;
    else this->add_ipl_plot = 1;

    QMapIterator<QPushButton *, QString> pbi(this->pbmap);
    while (pbi.hasNext()) {
        pbi.next();
        if (pbi.key()->isChecked()) {
            resched = pbi.key()->objectName();
        }
    }
    this->pblcear(resched);

    qDebug() << "------------" << resched;

    if (this->add_ipl_plot) {
        if (resched.contains("Off", Qt::CaseInsensitive)) {
            this->add_ipl_plot = 2;
        }
        if (resched.contains("Both", Qt::CaseInsensitive)) {
            this->add_ipl_plot = 3;
        }
    }

    this->append_plots();
}


void xmlcallib::on_pushButton_Chopper_Both_clicked()
{
    this->clear();
    this->pblcear("pushButton_Chopper_Both");
    if (this->add_master_plot) this->add_master_plot = 3;
    if (this->add_ipl_plot) this->add_ipl_plot = 3;
    if (this->add_theo_plot) this->add_theo_plot = 3;
    this->append_plots();

}

void xmlcallib::on_pushButton_show_master_cal_clicked()
{
    this->clear();
    QString resched;


    if (this->add_master_plot) this->add_master_plot = 0;
    else this->add_master_plot = 1;

    QMapIterator<QPushButton *, QString> pbi(this->pbmap);
    while (pbi.hasNext()) {
        pbi.next();
        if (pbi.key()->isChecked()) {
            resched = pbi.key()->objectName();
        }
    }
    this->pblcear(resched);

    qDebug() << "------------" << resched;

    if (this->add_master_plot) {
        if (resched.contains("Off", Qt::CaseInsensitive)) {
            this->add_master_plot = 2;
        }
        if (resched.contains("Both", Qt::CaseInsensitive)) {
            this->add_master_plot = 3;
        }
    }
    this->append_plots();
}


void xmlcallib::pblcear(const QString btn_name)
{


    // reset all
    QMapIterator<QPushButton *, QString> pbi(this->pbmap);
    while (pbi.hasNext()) {
        pbi.next();
        pbi.key()->setChecked(false);
        pbi.key()->setDown(false);
        pbi.key()->clearFocus();
        qDebug() << pbi.key()->objectName() << " is un checked";

    }
    pbi.toFront();
    while (pbi.hasNext()) {
        pbi.next();
        QString comp = pbi.key()->objectName();
        comp = comp.left(btn_name.size());
        if (!comp.compare(btn_name)) {
            qDebug() << "in map" << btn_name << comp;
            pbi.key()->setChecked(true);
            pbi.key()->setDown(true);
            pbi.key()->setFocus();
            qDebug() << pbi.key()->objectName() << " is checked now";
        }
    }
    QString cur_tab = this->ui->tabWidget->currentWidget()->objectName();
    pbi.toFront();
    while (pbi.hasNext()) {
        pbi.next();
        QString comp = pbi.key()->objectName();
        comp = comp.left(btn_name.size());
        if (!comp.compare(btn_name) && (cur_tab == pbi.value())) {
            // qDebug() << "in map" << btn_name << comp;
            pbi.key()->setFocus();
        }
    }

    //    pbi.toFront();
    //    while (pbi.hasNext()) {
    //        pbi.next();
    ////        QString comp = pbi.key()->objectName();
    ////        comp = comp.left(btn_name.size());
    ////        if (comp.contains(btn_name) && (cur_tab == pbi.value())) {
    ////            qDebug() << "in map" << btn_name << comp;
    //            if (pbi.key()->isChecked()) {
    //                qDebug() << pbi.key()->objectName() << " is checked checker";
    //            }
    //            else qDebug() << pbi.key()->objectName() << " is NOT checked checker";
    //        //}
    //    }



    qDebug() << this->ui->tabWidget->tabText(this->ui->tabWidget->currentIndex());
    qDebug() << this->ui->tabWidget->currentWidget()->objectName();



}



void xmlcallib::on_actionMTX_Format_triggered()
{
    if (!this->channels.size()) return;
    QString dirname;

    auto dialog = new QFileDialog(this, "select FOLDER for export");
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    dialog->setModal(false);

    dialog->setLabelText( QFileDialog::Accept, "select folder" );
    dialog->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dialog->show();

    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();
    if (fileNames.size()) {
        dirname = fileNames.at(0);
    }

    for (auto &ch : this->channels) {

        QFileInfo qfix;
        QString tmpcoil = ch->sensortype;
        QString serno;
        if (ch->sernum) {
            serno = QString::number(ch->sernum).rightJustified(4, '0');
        }
        tmpcoil.remove("-");
        if (dirname.size() && tmpcoil.size() && serno.size()) {
            qfix.setFile(dirname + "/" + tmpcoil + serno + ".txt");
            qDebug() << "writing" <<  qfix.filePath();
            ch->write_std_txt_file(qfix);
        }

        else if (!qfix.baseName().size()) {
            qfix.setFile(dirname + "/" + ch->input_filename.baseName() + ".txt");
            ch->write_std_txt_file(qfix);

        }
    }
}

void xmlcallib::on_actionXML_Format_triggered()
{
    if (!this->channels.size()) return;
    QString dirname;

    auto dialog = new QFileDialog(this, "select FOLDER for export");
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    dialog->setModal(false);

    dialog->setLabelText( QFileDialog::Accept, "select folder" );
    dialog->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dialog->show();

    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();
    if (fileNames.size()) {
        dirname = fileNames.at(0);
    }
    for (auto &ch : this->channels) {

        QFileInfo qfix;
        QString tmpcoil = ch->sensortype;
        QString serno;
        if (ch->sernum) {
            serno = QString::number(ch->sernum).rightJustified(4, '0');
        }
        tmpcoil.remove("-");
        if (dirname.size() && tmpcoil.size() && serno.size()) {
            qfix.setFile(dirname + "/" + tmpcoil + serno + ".xml");
            qDebug() << "writing" <<  qfix.filePath();
            ch->write_xml(qfix);
        }
        else if (!qfix.baseName().size()) {
            qfix.setFile(dirname + "/" + ch->input_filename.baseName() + ".xml");
            ch->write_xml(qfix);

        }
    }

}

void xmlcallib::on_actionCSV_Format_triggered()
{
    if (!this->channels.size()) return;
    QString dirname;

    auto dialog = new QFileDialog(this, "select FOLDER for export");
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    dialog->setModal(false);

    dialog->setLabelText( QFileDialog::Accept, "select folder" );
    dialog->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dialog->show();

    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();
    if (fileNames.size()) {
        dirname = fileNames.at(0);
    }
    for (auto &ch : this->channels) {

        QFileInfo qfix;
        QString tmpcoil = ch->sensortype;
        QString serno;
        if (ch->sernum) {
            serno = QString::number(ch->sernum).rightJustified(4, '0');
        }
        tmpcoil.remove("-");
        if (dirname.size() && tmpcoil.size() && serno.size()) {
            qfix.setFile(dirname + "/" + tmpcoil + serno + ".csv");
            qDebug() << "writing" <<  qfix.filePath();
            ch->write_csv_table(qfix);
        }
        else if (!qfix.baseName().size()) {
            qfix.setFile(dirname + "/" + ch->input_filename.baseName() + ".csv");
            ch->write_csv_table(qfix);
        }
    }

}

void xmlcallib::on_actionJSON_Format_triggered()
{
    if (!this->channels.size()) return;
    QString dirname;

    auto dialog = new QFileDialog(this, "select FOLDER for export");
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    dialog->setModal(false);

    dialog->setLabelText( QFileDialog::Accept, "select folder" );
    dialog->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dialog->show();

    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();
    if (fileNames.size()) {
        dirname = fileNames.at(0);
    }
    for (auto &ch : this->channels) {

        QFileInfo qfix;
        QString tmpcoil = ch->sensortype;
        QString serno;
        if (ch->sernum) {
            serno = QString::number(ch->sernum).rightJustified(4, '0');
        }
        tmpcoil.remove("-");
        if (dirname.size() && tmpcoil.size() && serno.size()) {
            qfix.setFile(dirname + "/" + tmpcoil + serno + ".json");
            qDebug() << "writing" <<  qfix.filePath();
            ch->write_json(qfix);
        }
        else if (!qfix.baseName().size()) {
            qfix.setFile(dirname + "/" + ch->input_filename.baseName() + ".json");
            ch->write_json(qfix);
        }
    }

}

void xmlcallib::on_actionExtendFull_triggered()
{
    if (!channels.size()) return;
    cal::cal chop;
    std::vector<double> fson;
    std::vector<double> fsoff;

    for (auto &chan : channels) {

        chop = chan->chopper;

        if (chan->sensortype.contains("FGS", Qt::CaseInsensitive)) {
            fson = this->channels.at(0)->gen_cal_freqs(1E-8,1E+6, 11 );
            fsoff = this->channels.at(0)->gen_cal_freqs(1E-8,1E+6, 11 );
        }
        else {
            fson = this->channels.at(0)->gen_cal_freqs(1E-5,1E+4, 11 );
            fsoff = this->channels.at(0)->gen_cal_freqs(1E-2,1E+5, 11 );
        }

        chan->chopper = cal::chopper_off;
        if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(fsoff);
        else chan->gen_theo_cal(fsoff);
        chan->chopper = cal::chopper_on;
        if (chan->sensortype.contains("ADB")) chan->gen_adb_pre_cal(fson);
        else chan->gen_theo_cal(fson);
        chan->chopper = chop;
        fson.clear();
        fsoff.clear();

    }

    int stat = this->add_theo_plot;
    // make vis
    this->on_pushButton_theo_clicked();
    // is was visible press gain
    if (stat) {
        this->on_pushButton_theo_clicked();

    }

}



void xmlcallib::on_checkBox_clicked(bool checked)
{
    this->clear();
    this->append_plots();
}

void xmlcallib::on_checkBox_un_clicked()
{
    this->clear();
    this->append_plots();
}

void xmlcallib::on_actionload_master_triggered()
{
    this->sel->show();

}

void xmlcallib::set_potential_master_cal(const QString &sensor_name)
{
    this->sensor_for_master_str = sensor_name;
}

void xmlcallib::load_master_only()
{
    this->clear(true);
    this->channels.emplace_back(std::make_shared<calibration>(QFileInfo(this->info_db.absoluteFilePath()), 0, -1, true, "", this));
    this->channels[0]->sensortype = this->sensor_for_master_str;
    this->channels[0]->set_sensor(this->sensor_for_master_str);
    this->channels[0]->sernum = 1;

    this->channels[0]->open_master_cal(QFileInfo(this->master_cal_db.absoluteFilePath()));
    this->channels[0]->get_master_cal();
    this->channels[0]->master_cal_to_caldata();

    this->append_plots();

}

void xmlcallib::slot_x_y_pos(const double &x, const double &y)
{
    if (x == DBL_MAX) this->ui->x_y_label->setText("x: , y: (press left mouse button");

    else this->ui->x_y_label->setText("x:" + QString::number(x) + "  y:" + QString::number(y));
}

void xmlcallib::on_pushButton_map_to_zero_clicked()
{
    for (auto &chan : this->channels) {
        chan->map_to_zero(358);
    }
    this->clear();
    this->append_plots();
}

void xmlcallib::on_pushButton_map_to_zero_2_clicked()
{
    this->on_pushButton_map_to_zero_clicked();
}

void xmlcallib::on_actiondiv_Gain_1_triggered()
{
    for (auto &chan : this->channels) {
        chan->div_gain_1();
    }
    this->clear();
    this->append_plots();
}

void xmlcallib::on_actiondiv_Gain_2_triggered()
{
    for (auto &chan : this->channels) {
        chan->div_gain_2();
    }
    this->clear();
    this->append_plots();
}

void xmlcallib::on_actionremove_fixed_2x_Gain_triggered()
{
    for (auto &chan : this->channels) {
        chan->remove_fixed_gain();
    }
    this->clear();
    this->append_plots();
}

void xmlcallib::on_actionremove_DIV_8_triggered()
{
    for (auto &chan : this->channels) {
        chan->remove_div_8();
    }
    this->clear();
    this->append_plots();
}


void xmlcallib::on_actionManual_Scales_triggered()
{
    if (!this->amplplots.size()) return;
    QCPRange ampl_range(this->amplplots.at(0)->yAxis->range());
    QCPRange phz_range(this->phaseplots.at(0)->yAxis->range());
    QCPRange freq_range(this->phaseplots.at(0)->xAxis->range());

    this->manual->set_data(ampl_range, phz_range, freq_range);

    this->manual->show();

}


void xmlcallib::xyaxis_clicked(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event)
{
    qDebug() << "hey";
    //QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    if (this->label_calls == 0) {
        this->dnums_labels = (axis->tickVector());
        this->strnums_labels = (axis->tickVectorLabels());
    }
    this->label_calls++;
    this->rlabels->set_data(axis, this->dnums_labels, this->strnums_labels);
    this->rlabels->show();
}





