#include "filter.h"
#include "ui_filter.h"

filter::filter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::filter)
{
    ui->setupUi(this);
    this->setLocale(QLocale::c());
    //    this->ui->centralWidget->acceptDrops();
    //    this->setAcceptDrops(true);
    this->allButtons = ui->centralWidget->findChildren<QRadioButton *>();

    for (auto btn : this->allButtons) {
        connect(btn, &QRadioButton::clicked, this, &filter::slot_set_filter_state);
    }

    // this->helpme = new QKeyEvent(QEvent::KeyPress,Qt::Key_F1, Qt::NoModifier);

    connect(this, &filter::signal_start_filter, this, &filter::slot_gui_fir_filter);
    connect(this, &filter::signal_start_sub_notch_filter, this, &filter::slot_single_sub_fir_notch_filter);
    connect(this, &filter::signal_start_sub_fir_filter, this, &filter::slot_single_sub_fir_filter);
    connect(this, &filter::signal_start_ascii_output, this, &filter::slot_to_ascii_ts);


    this->cmdline = std::make_shared<prc_com>(false);

    this->ui->progressBar->setRange(0,4096);

    this->ui->progressBar->setValue(0);

    this->qbg = new QButtonGroup(this);

    for (auto btn : this->allButtons) {
        this->qbg->addButton(btn);
    }

    this->qbg->setExclusive(true);

    this->ui->rb_4x->setChecked(true);

    this->ui->runbuton->setEnabled(false);
    this->ui->check_fullseconds->setChecked(true);

    this->set_waiting_for_files(true);

    this->remap_btn_names << "Ex" << "Ey" << "Hx" << "Hy" << "Hz";

    int i = 0;
    for (auto &nam : this->remap_btn_names) {
        this->chmap_low.insert(nam, i++);
    }
    for (auto &nam : this->remap_btn_names) {
        this->chmap_high.insert(nam, i++);
    }


    //    for (auto &child : findChildren<QWidget*>() ) {
    //        child->acceptDrops();
    //    }

    //    QList<QWidget*> lstChildren = findChildren<QWidget*>();
    //    foreach(QWidget* pWidget, lstChildren) {
    //         PackXML(pWidget->text());
    //    }



}

filter::~filter()
{
    delete this->qdi;
    delete ui;
}

void filter::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
    qDebug() << "drag enter";
}

void filter::clear()
{
    this->qdirs.clear();
    this->qxml.clear();
    this->qats.clear();
    this->qfis.clear();
    this->notch_filter_settings.clear();
    this->cmdline->clear();
    this->qfisxmlats_sub.clear();
    this->delete_atsfiles.clear();
    this->delete_xmlfiles.clear();
    this->ui->progressBar->setValue(0);
    this->set_waiting_for_files(true);

}

void filter::open()
{
    if (this->qdirs.size() || this->qxml.size()) this->qats.clear();


    for (int i = 0; i < this->qdirs.size(); ++i) {

        // absolute dir would truncate the last entry (aka file)
        // becasue qdirs is already dir we take all complete
        QDir dir(this->qdirs.at(i).absoluteFilePath());
        QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            if (!iterator.fileInfo().isDir()) {
                QFileInfo measinfo(iterator.filePath());
                if (measinfo.fileName().endsWith(".xml", Qt::CaseInsensitive) && (measinfo.absoluteFilePath().contains("meas_")) ) {
                    qDebug() << "in dir" << measinfo.absolutePath();
                    qDebug() << "-> " << measinfo.fileName();
                    this->qxml.append(measinfo);
                }
            }
        }
    }

    if (!this->qats.size()) qfis.append(this->qxml);
    else this->qfis.append(this->qats);

    this->count_files();
    this->build_remapper();

}

void filter::set_waiting_for_files(const bool wait)
{
    if (wait) {
        this->ui->label_files->setText("waiting for files");
        this->ui->tabascii->setDisabled(true);
        this->ui->tab_remap->setDisabled(true);
        this->ui->start_stop->setDisabled(true);
        this->ui->runbuton->setDisabled(true);
    }
    else {
        this->ui->tabascii->setEnabled(true);
        this->ui->tab_remap->setEnabled(true);
        this->ui->start_stop->setEnabled(true);
        this->ui->runbuton->setEnabled(true);

    }


}


void filter::set_xmlfiles(const QList<QFileInfo> &qxml)
{
    this->clear();
    this->qxml = qxml;
    qfis.append(this->qxml);

    this->count_files();
    this->build_remapper();


}

void filter::set_decimation_filter(const QString &str_x)
{
    if (!str_x.size()) return;
    this->uncheck_btns();
    std::cout << "filter decimation choosen " << str_x.toStdString() << std::endl;

    if (str_x == "4x") this->ui->rb_4x->setChecked(true);
    else if (str_x == "2x") this->ui->rb_2x->setChecked(true);
    else if (str_x == "8x") this->ui->rb_8x->setChecked(true);
    else if (str_x == "10x") this->ui->rb_10x->setChecked(true);
    else if (str_x == "15x") this->ui->rb_15x->setChecked(true);
    else if (str_x == "25x") this->ui->rb_25x->setChecked(true);
    else if (str_x == "32x") this->ui->rb_32x->setChecked(true);

}

void filter::set_ascii_output(const QString &str_asc)
{
    if (!str_asc.size()) return;
    this->uncheck_btns();
    std::cout << "ascii out choosen " << str_asc.toStdString() << std::endl;
    if (str_asc == "raw") this->ui->rb_to_ascii->setChecked(true);
    else if (str_asc == "scaled") this->ui->rb_to_ascii_scaled->setChecked(true);
    else if (str_asc == "nt") this->ui->rb_to_ascii_scaled_nt->setChecked(true);
    else if (str_asc == "nt_theo") this->ui->rb_to_ascii_scaled_nt_theo->setChecked(true);

}

void filter::set_notch_filter(const QString &str_x)
{
    if (!str_x.size()) return;
    this->uncheck_btns();
    std::cout << "filter notch choosen " << str_x.toStdString() << std::endl;

    this->ui->rb_notch->setChecked(true);
    if (str_x == "150") this->ui->check150->setChecked(true);
    else if (str_x == "50") this->ui->check50->setChecked(true);
    else if (str_x == "60") this->ui->check60->setChecked(true);
    else if (str_x == "180") this->ui->check180->setChecked(true);
    else if (str_x == "16_23") this->ui->check16_23->setChecked(true);
    else if (str_x == "50_150") {
        this->ui->check150->setChecked(true);
        this->ui->check50->setChecked(true);
    }
    else if (str_x == "16_23_50_150") {
        this->ui->check150->setChecked(true);
        this->ui->check50->setChecked(true);
        this->ui->check16_23->setChecked(true);
    }
    else if (str_x == "60_180") {
        this->ui->check180->setChecked(true);
        this->ui->check60->setChecked(true);
    }
}

void filter::uncheck_btns()
{
    this->ui->rb_4x->setChecked(false);
    this->ui->rb_2x->setChecked(false);
    this->ui->rb_8x->setChecked(false);
    this->ui->rb_10x->setChecked(false);
    this->ui->rb_15x->setChecked(false);
    this->ui->rb_25x->setChecked(false);
    this->ui->rb_32x->setChecked(false);

    this->ui->rb_to_ascii->setChecked(false);
    this->ui->rb_to_ascii_scaled->setChecked(false);
    this->ui->rb_to_ascii_scaled_nt->setChecked(false);
    this->ui->rb_to_ascii_scaled_nt_theo->setChecked(false);

    this->ui->rb_notch->setChecked(false);
    this->ui->check150->setChecked(false);
    this->ui->check50->setChecked(false);
    this->ui->check60->setChecked(false);
    this->ui->check180->setChecked(false);
    this->ui->check16_23->setChecked(false);
}



void filter::dropEvent(QDropEvent *event)
{
    qDebug() << "dr0";
    this->clear();
    qDebug() << "dr1";


    QList<QUrl> urls = event->mimeData()->urls();

    if (urls.isEmpty())
        return;

    for (int i = 0; i < urls.size(); ++i) {
        if (urls.at(i).isValid()) {
            QFileInfo qfx(urls[i].toLocalFile());
            if (qfx.isDir() && qfx.exists()) {
                this->qdirs.append(qfx);
            }
            if (qfx.suffix().contains("xml", Qt::CaseInsensitive) && qfx.exists()) {
                this->qxml.append(qfx);
            }
            if (qfx.suffix().contains("ats", Qt::CaseInsensitive) && qfx.exists() ) {
                this->qats.append(qfx);
            }
        }
    }

    this->open();



}

void filter::keyPressEvent(QKeyEvent *keyevent)
{
    if (keyevent->key() == Qt::Key_F1) {
        this->on_actionHelp_triggered();
    }
}

void filter::on_runbuton_clicked()
{
    qDebug() << "run";

    if (!this->qfis.size()) return;


    // radio buttons

    if (this->ui->rb_1x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "copy");             // want to cut / copy - no filter if 1x
        cmdline->insert("ats_scale", "none");        // filter does not want to scale

    }


    if (this->ui->rb_low_pass->isChecked() && (this->extrafilters_status != nullptr)) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", this->extrafilters_status->value("filter_name").toString());      // with this filter
        cmdline->insert("upper_freq", this->extrafilters_status->value("upper_freq").toDouble());  // filter tuning
        cmdline->insert("lower_freq", this->extrafilters_status->value("lower_freq").toDouble());  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale

    }

    else if (this->ui->rb_high_pass->isChecked() && (this->extrafilters_status != nullptr)) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", this->extrafilters_status->value("filter_name").toString());      // with this filter
        cmdline->insert("upper_freq", this->extrafilters_status->value("upper_freq").toDouble());  // filter tuning
        cmdline->insert("lower_freq", this->extrafilters_status->value("lower_freq").toDouble());  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale

    }

    else if (this->ui->rb_band_pass->isChecked() && (this->extrafilters_status != nullptr)) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", this->extrafilters_status->value("filter_name").toString());      // with this filter
        cmdline->insert("upper_freq", this->extrafilters_status->value("upper_freq").toDouble());  // filter tuning
        cmdline->insert("lower_freq", this->extrafilters_status->value("lower_freq").toDouble());  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale

    }


    else if (this->ui->rb_2x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", "mtx2");      // with this filter
        cmdline->insert("upper_freq", double(0.0));  // filter tuning
        cmdline->insert("lower_freq", double(0.0));  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale


    }
    else if (this->ui->rb_4x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", "mtx4");      // with this filter
        cmdline->insert("upper_freq", double(0.0));  // filter tuning
        cmdline->insert("lower_freq", double(0.0));  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale

    }
    else if (this->ui->rb_8x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", "mtx8");      // with this filter
        cmdline->insert("upper_freq", double(0.0));  // filter tuning
        cmdline->insert("lower_freq", double(0.0));  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale


    }
    else if (this->ui->rb_10x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", "mtx10");      // with this filter
        cmdline->insert("upper_freq", double(0.0));  // filter tuning
        cmdline->insert("lower_freq", double(0.0));  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale


    }
    else if (this->ui->rb_15x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", "mtx15");      // with this filter
        cmdline->insert("upper_freq", double(0.0));  // filter tuning
        cmdline->insert("lower_freq", double(0.0));  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale


    }
    else if (this->ui->rb_25x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", "mtx25");      // with this filter
        cmdline->insert("upper_freq", double(0.0));  // filter tuning
        cmdline->insert("lower_freq", double(0.0));  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale


    }
    else if (this->ui->rb_32x->isChecked()) {
        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("filter_name", "mtx32");      // with this filter
        cmdline->insert("upper_freq", double(0.0));  // filter tuning
        cmdline->insert("lower_freq", double(0.0));  // filter tuning
        cmdline->insert("ats_scale", "none");        // filter does not want to scale


    }

    else if (this->ui->rb_notch->isChecked()) {

        cmdline->insert("ats", "ts");                // wants ats timeseries
        cmdline->insert("task", "filter");           // want to filter
        cmdline->insert("ats_scale", "none");        // filter does not want to scale



    }
    else if (this->ui->rb_to_ascii->isChecked()) {

        cmdline->insert("ascii", "ts");              // want timeseries
        cmdline->insert("task", "convert");          // want to convert
        cmdline->insert("ats_scale", "none");        // filter does not want to scale


    }
    else if (this->ui->rb_to_ascii_scaled->isChecked()) {

        cmdline->insert("ascii", "ts");              // want timeseries
        cmdline->insert("task", "convert");          // want to convert
        cmdline->insert("ats_scale", "auto");        // that is already the default

    }

    else if (this->ui->rb_to_ascii_scaled_nt->isChecked()) {

        cmdline->insert("ascii", "ts");              // want timeseries
        cmdline->insert("task", "convert");          // want to convert
        cmdline->insert("ats_scale", "nT");        //


    }

    else if (this->ui->rb_to_ascii_scaled_nt_theo->isChecked()) {
        cmdline->insert("ascii", "ts");              // want timeseries
        cmdline->insert("task", "convert");          // want to convert
        cmdline->insert("ats_scale", "nT_theo");     // use the theoretical only for saving memory

    }

    // end radio buttons

    // check buttons


    if (this->ui->check50->isChecked()) {
        QMap<QString, QVariant> notch_settings;
        notch_settings.insert("filter_name", "notch");        // with this filter
        notch_settings.insert("upper_freq",  double (51.0));  // filter tuning
        notch_settings.insert("lower_freq",  double (49.0));  // filter tuning
        notch_settings.insert("center_freq", double (50.0));  // filter tuning
        this->notch_filter_settings.append(notch_settings);
    }
    if (this->ui->check60->isChecked()) {
        QMap<QString, QVariant> notch_settings;
        notch_settings.insert("filter_name", "notch");        // with this filter
        notch_settings.insert("upper_freq",  double(61.0));  // filter tuning
        notch_settings.insert("lower_freq",  double(59.0));  // filter tuning
        notch_settings.insert("center_freq", double(60.0));  // filter tuning
        this->notch_filter_settings.append(notch_settings);
    }
    if (this->ui->check150->isChecked()) {
        QMap<QString, QVariant> notch_settings;
        notch_settings.insert("filter_name", "notch");        // with this filter
        notch_settings.insert("upper_freq",  double(151.0));  // filter tuning
        notch_settings.insert("lower_freq",  double(149.0));  // filter tuning
        notch_settings.insert("center_freq", double(150.0));  // filter tuning
        this->notch_filter_settings.append(notch_settings);
    }
    if (this->ui->check180->isChecked()) {
        QMap<QString, QVariant> notch_settings;
        notch_settings.insert("filter_name", "notch");        // with this filter
        notch_settings.insert("upper_freq",  double(181.0));  // filter tuning
        notch_settings.insert("lower_freq",  double(179.0));  // filter tuning
        notch_settings.insert("center_freq", double(180.0));  // filter tuning
        this->notch_filter_settings.append(notch_settings);
    }
    if (this->ui->check16_23->isChecked()) {
        QMap<QString, QVariant> notch_settings;
        notch_settings.insert("filter_name", "notch");        // with this filter
        notch_settings.insert("upper_freq",  double(19.0));  // filter tuning
        notch_settings.insert("lower_freq",  double(15.0));  // filter tuning
        notch_settings.insert("center_freq", double(17.0));  // filter tuning
        this->notch_filter_settings.append(notch_settings);
    }

    // end check buttons filter


    // exec logic


    if ( ((cmdline->svalue("task") == "filter") || ((cmdline->svalue("task") == "copy"))) && ((cmdline->svalue("ats") == "ts")) ) {

        // XML and ats
        emit this->signal_start_filter();
    }

    // ////////////////////////////////////////////////////////////////////////////////


    if ( (cmdline->svalue("task") == "convert") && ((cmdline->svalue("ascii") == "ts")) ) {

        // ats to ascii
        emit this->signal_start_ascii_output();
    }



    this->notch_filter_settings.clear();
    this->cmdline->clear();
    this->qfisxmlats_sub.clear();
    this->qfis.clear();

    this->delete_files();

    this->count_files();

}

void filter::slot_to_ascii_ts()
{
    if ( (this->cmdline->svalue("task") != "convert") && (cmdline->svalue("ascii") != "ts") ) return;
    this->use_samples = quint64 (this->ui->spinBox_use_samples->value());
    int i;
    for (i = 0; i < this->qfis.size(); ++i) {

        this->ui->runbuton->setDisabled(true);
        auto mcd(std::make_unique<mc_data>());

        connect(mcd.get(), &mc_data::signal_get_max_counts_and_last, this, &filter::slot_set_max_counts_and_last);
        connect(mcd.get(), &mc_data::signal_counts_per,  ui->progressBar, &QProgressBar::setValue, Qt::DirectConnection);
        // next does not work .. why
        connect(mcd.get(), &mc_data::signal_read_ascii_finished,  ui->label_files, &QLabel::setText, Qt::DirectConnection);
        connect(mcd.get(), &mc_data::signal_text_message,  ui->label_files, &QLabel::setText, Qt::DirectConnection);
        connect(mcd.get(), SIGNAL(signal_repaint()),  this, SLOT(repaint()), Qt::DirectConnection);


        qDebug() << "main::prepare ascii output";
        // make the options available first
        bool use_theo_only = false;
        if (this->ui->rb_to_ascii_scaled_nt_theo->isChecked()) use_theo_only = true;
        //if (mcd->in_atsfiles.at(0)->get_sample_freq() > 512.01) use_theo_only = false;
        if (use_theo_only == true) cmdline->insert("calibration", "theo");
        mcd->insert_prc_com(cmdline);
        if (this->qfis.at(i).absoluteFilePath().endsWith(".xml", Qt::CaseInsensitive)) {
            QList<QFileInfo> qfisxmlats;
            qfisxmlats.append(this->qfis.at(i));
            mcd->open_files(qfisxmlats, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
        }
        else {
            mcd->open_files(this->qfis, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
            i = INT_MAX - 1; // all ats files
        }

        if (this->ui->rb_to_ascii->isChecked()) {
            mcd->prepare_ascii_outfiles(100, 102400, this->use_samples, atsfileout::atsfileout_unscaled_timeseries ,".tsdata");
        }
        if (this->ui->rb_to_ascii_scaled->isChecked()) {
            mcd->prepare_ascii_outfiles(100, 102400, this->use_samples, atsfileout::atsfileout_scaled_timeseries ,".tsdata");
        }

        if (this->ui->checkBox_have32GB->isChecked()) mcd->slot_set_GB_mem(32);

        if (this->ui->rb_to_ascii_scaled_nt->isChecked() || this->ui->rb_to_ascii_scaled_nt_theo->isChecked()) {
            // will create atsfile_collector(s)


            this->ui->progressBar->setRange(0, 100);
            connect(mcd.get(), &mc_data::signal_guess_100,  ui->progressBar, &QProgressBar::setValue, Qt::DirectConnection);

            this->ui->label_files->setText("processing ....");
            this->update();
            mcd->start_single_spectra_threads(true, true);

            mcd->join_single_spectra_threads();

            this->ui->progressBar->setValue(100);

            this->ui->label_files->setText("flushing ....");
            this->update();


            mcd->close_infiles();
            mcd->close_ascii_outfiles();
            mcd.reset();
            //this->ui->label_files->setText("waiting for files");
            set_waiting_for_files(true);

        }

        else {

            this->ui->label_files->setText("processing ....");
            this->update();

            if (this->ui->spinBox_start_sample->value()) {
                quint64 mv = quint64(this->ui->spinBox_start_sample->value());
                // that does not work why? emit mcd->signal_slide_to_sample(mv);
                for (size_t ii = 0; ii < mcd->in_atsfiles.size(); ++ii) {
                    mcd->in_atsfiles[ii]->slot_slide_to_sample(mv);
                }
            }

            if (this->ui->checkBox_ISO_date->isChecked()) {

                for (size_t ii = 0; ii < mcd->out_ats_ascii_files.size(); ++ii) {
                    mcd->out_ats_ascii_files[ii]->set_eQDatetime(*this->edate_start.get());
                }
            }


            mcd->start_in_ats_channel_threads();
            mcd->start_out_ats_ascii_channel_threads();

            mcd->join_in_ats_channel_threads();

            this->ui->label_files->setText("flushing ....");
            this->update();

            mcd->join_out_ats_ascii_channel_threads();

            mcd->close_infiles();
            mcd->close_ascii_outfiles();
            mcd.reset();
            //this->ui->label_files->setText("waiting for files");
            set_waiting_for_files(true);
            //this->ui->runbuton->setDisabled(false);


        }
    }
}


void filter::slot_gui_fir_filter()
{

    if (this->ui->checkBox_have_copy->isChecked() && this->ui->rb_remap->isChecked()) {
        qDebug() << "channel mapper active";



        for (auto &xmls : this->qxml) {
            this->notch_filter_settings = this->notch_filter_settings_remember;
            auto mcd(std::make_unique<mc_data>());
            QList<QFileInfo> qfisxmlats;
            qfisxmlats.append(xmls);
            mcd->open_files(qfisxmlats, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);

            if (this->in_channels_labels.size() != mcd->in_atsfiles.size()) return;

            QMap<QString, QFileInfo> mapfiles, renamedfiles;

            size_t i = 0;
            size_t expected_size = 0;
            for (auto &act : this->in_channels_labels) {
                if (act->isChecked()) {

                    for (size_t j = 0; j < mcd->in_atsfiles.size(); ++j) {
                        if (act->text() ==  mcd->in_atsfiles.at(j)->svalue("channel_type")) {
                            qDebug() << "remapping" << mcd->in_atsfiles.at(i)->baseName();
                            auto btns = this->out_channels_groups.value( mcd->in_atsfiles.at(j)->svalue("channel_type"))->buttons();

                            for (auto &btn : btns) {
                                if (btn->isChecked()) {
                                    qDebug() << "checked" << btn->text();
                                    mapfiles.insert(btn->text(), QFileInfo(mcd->in_atsfiles.at(j)->absoluteFilePath()));

                                    ++expected_size;
                                }
                            }

                        }
                    }
                }

                ++i;
            }

            if (expected_size != mapfiles.size()) return;

            mcd->close_infiles();

            auto iter = mapfiles.cbegin();
            for ( auto &fs : mapfiles) {
                QFile file(fs.absoluteFilePath());
                QFile::rename(fs.absoluteFilePath(), fs.absoluteFilePath()+"_tmp");
                renamedfiles.insert(iter.key(), QFileInfo(fs.absoluteFilePath()+"_tmp"));
                ++iter;

            }
            measdocxml measdoc(&xmls);
            QMap<int, int> calibration_map;

            iter = renamedfiles.cbegin();
            for (; iter != renamedfiles.cend(); ++iter) {
                atsheader atsh(iter.value());
                atsh.scan_header_close();
                int old_ch, new_ch;
                qDebug() << "working on"  << iter.value().baseName() << " to " << iter.key();
                old_ch = atsh.ivalue("channel_number");
                atsh.set_key_value("channel_type", iter.key());
                if (atsh.ivalue("channel_number") > 4 ) {
                    atsh.set_key_value("channel_number", this->chmap_high.value(iter.key()));
                    new_ch = this->chmap_high.value(iter.key());
                }
                else {
                    atsh.set_key_value("channel_number", this->chmap_low.value(iter.key()));
                    new_ch = this->chmap_low.value(iter.key());
                }
                QString newfilename = atsh.get_new_filename();
                qDebug() << "new filename " << newfilename;

                QFile::rename(iter.value().absoluteFilePath(), newfilename);
                atsh.close();
                atsheader atshnew((QFileInfo(newfilename)));


                connect(&atshnew, &atsheader::filename_changed, &measdoc, &measdocxml::slot_update_atswriter, Qt::DirectConnection);
                connect(&measdoc, &measdocxml::signal_atswriter_data_qmap_created, &atshnew, &atsheader::set_atswriter_section, Qt::DirectConnection );
                measdoc.create_atswriter();
                atshnew.scan_header_close();
                atshnew.set_key_value("channel_type", iter.key());
                if (atshnew.ivalue("channel_number") > 4 ) {
                    atshnew.set_key_value("channel_number", this->chmap_high.value(iter.key()));
                }
                else {
                    atshnew.set_key_value("channel_number", this->chmap_low.value(iter.key()));

                }
                calibration_map.insert(old_ch, new_ch);
                atshnew.write_header(true);

            }
            measdoc.renumber_ids_calentries(calibration_map);
            measdoc.save(&xmls);
        }


        return;
    }


    if ( ((cmdline->svalue("task") != "filter") && (cmdline->svalue("task") != "copy")) || ((cmdline->svalue("ats") != "ts")) ) return;
    int i;





    // else - we have other filter

    // copy the notch filter - it will be emptied step by step
    this->notch_filter_settings_remember = this->notch_filter_settings;

    if (cmdline->svalue("task") == "copy") {
        this->slide_to_samepos_after_open = false;
    }
    else {
        this->slide_to_samepos_after_open = true;
    }

    for (i = 0; i < this->qfis.size(); ++i) {

        // start with all filters again; notch will be deleted step by step
        this->notch_filter_settings = this->notch_filter_settings_remember;

        // remove the first one and copy into the command line
        if (notch_filter_settings.size()) {
            QMap<QString, QVariant>::const_iterator citer = notch_filter_settings.at(0).constBegin();
            while (citer != notch_filter_settings.at(0).constEnd()) {
                this->cmdline->insert(citer.key(), citer.value());
                ++citer;
            }

            notch_filter_settings.removeFirst();
        }

        // the recursive call / cascade / notches will be cleaned
        this->qfisats_sub.clear();
        this->qfisxmlats_sub.clear();



        auto mcd(std::make_unique<mc_data>());

        connect(mcd.get(), &mc_data::signal_get_max_counts_and_last, this, &filter::slot_set_max_counts_and_last);
        connect(mcd.get(), &mc_data::signal_counts_per,  ui->progressBar, &QProgressBar::setValue, Qt::DirectConnection);

        qDebug() << "main::prepare filter" << cmdline->svalue("filter_name");
        mcd->insert_prc_com(cmdline);
        if (this->qfis.at(i).absoluteFilePath().endsWith(".xml", Qt::CaseInsensitive)) {
            QList<QFileInfo> qfisxmlats;
            qfisxmlats.append(this->qfis.at(i));
            mcd->open_files(qfisxmlats, this->slide_to_samepos_after_open, mttype::nomt, threadctrl::threadctrl_continuous_individual);
        }
        else {
            mcd->open_files(this->qfis, this->slide_to_samepos_after_open, mttype::nomt, threadctrl::threadctrl_continuous_individual);
            i = INT_MAX - 1; // all ats files
        }
        if (!mcd->in_atsfiles.size()) return;
        if (cmdline->svalue("task") == "filter") {
            //
            // here the wl is set and in prepare_ats_outfiles we already know that
            //

            if ((cmdline->svalue("filter_name") == "notch") && mcd->in_atsfiles.at(0)->get_sample_freq() * 0.92 < (cmdline->dvalue("upper_freq") )) {
                mcd->close_infiles();
                return;
            }

            if (!mcd->prepare_firfilter()) {
                qDebug() << "filter::on_runbuton_clicked -> fail";
                return;
            }
        }

        size_t fnbuffers, fwl;
        if (cmdline->svalue("task") == "copy") {
            this->use_samples = quint64 (this->ui->spinBox_use_samples->value());

            if (this->ui->spinBox_start_sample->value()) {
                quint64 mv = quint64(this->ui->spinBox_start_sample->value());
                // that does not work why? emit mcd->signal_slide_to_sample(mv);
                for (size_t ii = 0; ii < mcd->in_atsfiles.size(); ++ii) {
                    mcd->in_atsfiles[ii]->slot_slide_to_sample(mv);
                }
            }

            if (this->use_samples < 65536) {
                fnbuffers = 1;
                fwl = (size_t) this->use_samples;
            }
            else {
                fnbuffers = 10;
                fwl = 65536;
            }

        }


        if (cmdline->svalue("task") == "copy") {
            mcd->prepare_ats_outfiles(fnbuffers, fwl, this->use_samples, atsfileout::atsfileout_unscaled_timeseries ,".ats");
        }
        // the window length is meaningless here
        else {
            mcd->prepare_ats_outfiles(10, 1024, 0, atsfileout::atsfileout_unscaled_fir_filtered ,".ats");
        }

        if (this->ui->checkBox_copy_old_files->isChecked()) {
            for (size_t a = 0; a < mcd->in_atsfiles.size(); ++a) {
                mcd->in_atsfiles.at(a)->cp_essentials_to(mcd->out_atsfiles[a], true);
            }
        }


        mcd->start_in_ats_channel_threads();
        mcd->start_out_ats_channel_threads();

        std::cerr << "filter threads started" << std::endl;

        mcd->join_in_ats_channel_threads();
        mcd->join_out_ats_channel_threads();

        std::cerr << "filter threads joined" << std::endl;

        mcd->close_infiles();
        // copy will have no subs
        if (this->ui->checkBox_copy_old_files->isChecked()) {
            for (auto &ats : mcd->out_atsfiles) {
                ats->close();
            }
            this->create_xml_from_ukn(mcd->out_atsfiles);
        }
        else {
            this->qfisats_sub.append(mcd->close_outfiles());
            this->qfisxmlats_sub.append(mcd->close_measdocs());
        }

        std::cerr << "mcd reset now:" << std::endl;
        mcd.reset();
        //this->ui->label_files->setText("waiting for files");
        set_waiting_for_files(true);
        this->ui->progressBar->setValue(this->ui->progressBar->maximum());

        emit this->signal_start_sub_notch_filter();
        this->remaining_cascades = this->ui->spinBoxCascades->value();
        emit this->signal_start_sub_fir_filter();


    }
}



void filter::slot_single_sub_fir_notch_filter()
{
    if (cmdline->svalue("filter_name") != "notch") return;
    if (!this->qfisxmlats_sub.size()) return;

    if (notch_filter_settings.size()) {
        QMap<QString, QVariant>::const_iterator citer = notch_filter_settings.at(0).constBegin();
        while (citer != notch_filter_settings.at(0).constEnd()) {
            this->cmdline->insert(citer.key(), citer.value());
            ++citer;
        }

        notch_filter_settings.removeFirst();
        std::cerr << "adding files to del in slot_single_sub_fir_notch_filter" << std::endl;
        this->delete_atsfiles.append(this->qfisats_sub);
        this->delete_xmlfiles.append(this->qfisxmlats_sub);
    }
    else return;

    auto mcd_sub(std::make_unique<mc_data>());
    mcd_sub->insert_prc_com(cmdline);
    connect(mcd_sub.get(), &mc_data::signal_get_max_counts_and_last, this, &filter::slot_set_max_counts_and_last);
    connect(mcd_sub.get(), &mc_data::signal_counts_per,  ui->progressBar, &QProgressBar::setValue, Qt::DirectConnection);
    mcd_sub->open_files(qfisxmlats_sub, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
    if (!mcd_sub->prepare_firfilter()) {
        qDebug() << "filter::slot_single_sub_fir_filter -> fail";
        exit(1);
    }

    mcd_sub->prepare_ats_outfiles(1000, 1024, 0, atsfileout::atsfileout_unscaled_fir_filtered ,".ats");

    mcd_sub->start_in_ats_channel_threads();
    mcd_sub->start_out_ats_channel_threads();

    mcd_sub->join_in_ats_channel_threads();
    mcd_sub->join_out_ats_channel_threads();

    // after the subfilter runs successfuly we prepare previous file for deletion

    mcd_sub->close_infiles();
    this->qfisats_sub.clear();
    this->qfisxmlats_sub.clear();
    this->qfisats_sub.append(mcd_sub->close_outfiles());
    this->qfisxmlats_sub.append(mcd_sub->close_measdocs());

    mcd_sub.reset();
    //this->ui->label_files->setText("waiting for files");
    set_waiting_for_files(true);

    std::cerr << "notch filter threads finished / signals" << std::endl;

    this->ui->progressBar->setValue(this->ui->progressBar->maximum());
    emit this->signal_start_sub_notch_filter();




}

void filter::slot_single_sub_fir_filter()
{
    if (!cmdline->svalue("filter_name").startsWith("mtx")) return;
    if (!this->qfisxmlats_sub.size()) return;
    if (!this->ui->spinBoxCascades->value() && !this->ui->cb_auto_extend->isChecked()) return;
    if (!this->remaining_cascades &&  !this->ui->cb_auto_extend->isChecked()) return;
    --this->remaining_cascades;

    if (this->qfisats_sub.size() && this->ui->cb_auto_extend->isChecked()) {


        QSqlDatabase dbmcd;
        QMap<QString, double> sens_chop;
        QMap<QString, QString> sensor_aliases;
        QMap<QString, double> chop_result;
        QList<double> filter_result;

        dbmcd = QSqlDatabase::addDatabase("QSQLITE", "get_chopper");
        dbmcd.setConnectOptions("QSQLITE_OPEN_READONLY");
        dbmcd.setDatabaseName(fall_back_default_str("info.sql3").absoluteFilePath());
        if (dbmcd.open()) {
            QSqlQuery query(dbmcd);
            QString query_str("SELECT sensortypes.Name, sensortypes.chopperon_lessequal from sensortypes WHERE sensortypes.chopperon_lessequal != 0;");
            if( query.exec(query_str) ) {
                while (query.next()) {
                    sens_chop.insert(query.value(0).toString(), query.value(1).toDouble());
                }
            }
            else qDebug() << "slot_single_sub_fir_filter -> db err: could not get choppers for filtering" ;


            query_str = "SELECT `alias_or_headername`, `true_name` FROM `sensor_aliases`" ;
            query.clear();
            if (query.exec(query_str)) {
                while (query.next()) {
                    sensor_aliases.insert(query.value(0).toString(), query.value(1).toString() );
                }
            }
        }
        else {
            this->remaining_cascades = 0;
            return;
        }


        double f_sample = 0;
        double dsamples = 0;
        for (int iq = 0; iq < this->qfisats_sub.size(); ++iq) {
            std::unique_ptr<atsheader> atsh = std::make_unique<atsheader>(this->qfisats_sub.at(iq));
            atsh->scan_header_close();
            f_sample = atsh->get_sample_freq();
            dsamples = atsh->get_num_samples();
            uint chopper_status = atsh->value("chopper_status").toUInt();
            QString str_sens = atsh->svalue("sensor_type");
            // so chopper is off; we have a sensor found, which contains a chopper less_equal entry - so note it down!
            if (sensor_aliases.keys().contains(str_sens.toLower()) && !chopper_status) {
                chop_result.insert(sensor_aliases.value(str_sens.toLower()), sens_chop.value(sensor_aliases.value(str_sens.toLower())));

            }
            QStringList filters = atsh->get_filter();
            if (filters.contains("HF-HP-500Hz")) filter_result.append(500.0);
            if (filters.contains("HF-HP-1Hz")) filter_result.append(1.0);
        }


        double fct = 1;
        if (this->ui->rb_4x->isChecked())fct = 4.0;
        if (this->ui->rb_2x->isChecked())fct = 2.0;
        if (this->ui->rb_8x->isChecked())fct = 8.0;
        if (this->ui->rb_10x->isChecked())fct = 10.0;
        if (this->ui->rb_15x->isChecked())fct = 15.0;
        if (this->ui->rb_25x->isChecked())fct = 25.0;
        if (this->ui->rb_32x->isChecked())fct = 32.0;

        f_sample /=  fct;
        dsamples /= fct;
        double xwl = 512;
        double min_stacks = 30;
        // use 1 Hz
        if (f_sample > 4095.0) xwl = f_sample;
        else if (f_sample > 0.999999) xwl = 512;

        // avoid too low frequencies for chopper off; we have one or more sensors with chopper OFF
        if (chop_result.size()) {
            QList<double> cfs(chop_result.values());
            std::sort(cfs.begin(), cfs.end());
            if (f_sample < cfs.last()/2) {
                this->remaining_cascades = 0;
                return;
            }
        }

        if (filter_result.size()) {

            double corner = *std::max_element(filter_result.begin(), filter_result.end());
            if (f_sample < corner * 16.0) {
                this->remaining_cascades = 0;
                return;
            }
        }

        if (( dsamples / xwl ) > min_stacks) this->remaining_cascades = 1;
        else {
            this->remaining_cascades = 0;
            return;
        }
    }
    auto mcd_sub(std::make_unique<mc_data>());
    mcd_sub->insert_prc_com(cmdline);
    connect(mcd_sub.get(), &mc_data::signal_get_max_counts_and_last, this, &filter::slot_set_max_counts_and_last);
    connect(mcd_sub.get(), &mc_data::signal_counts_per,  ui->progressBar, &QProgressBar::setValue, Qt::DirectConnection);
    mcd_sub->open_files(qfisxmlats_sub, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
    if (!mcd_sub->prepare_firfilter()) {
        qDebug() << "filter::slot_single_sub_fir_filter -> fail";
        exit(1);
    }

    mcd_sub->prepare_ats_outfiles(1000, 1024, 0, atsfileout::atsfileout_unscaled_fir_filtered ,".ats");

    mcd_sub->start_in_ats_channel_threads();
    mcd_sub->start_out_ats_channel_threads();

    std::cerr << "sub filter threads started" << std::endl;

    mcd_sub->join_in_ats_channel_threads();
    mcd_sub->join_out_ats_channel_threads();

    std::cerr << "filter threads joined" << std::endl;


    // after the subfileter runs successfuly we prpare previous file for deletion

    mcd_sub->close_infiles();
    this->qfisats_sub.clear();
    this->qfisxmlats_sub.clear();
    this->qfisats_sub.append(mcd_sub->close_outfiles());
    this->qfisxmlats_sub.append(mcd_sub->close_measdocs());
    mcd_sub.reset();
    //this->ui->label_files->setText("waiting for files");
    set_waiting_for_files(true);
    std::cerr << "sub filter threads finished / signals" << std::endl;
    this->ui->progressBar->setValue(this->ui->progressBar->maximum());


    if (this->remaining_cascades) emit this->signal_start_sub_fir_filter();
    return;


}



void filter::slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading)
{

    qDebug() << "Progress bar" << max_counts << "," << last_reading;
    this->ui->progressBar->setRange(0, int(max_counts));
}

void filter::slot_update_progress_bar(const int counts)
{
    this->ui->progressBar->setValue(counts);
}


void filter::count_files()
{
    if (this->qfis.size()) {
        int ixml = 0;
        int iats = 0;
        for (auto &qfi : qfis) {
            if (qfi.fileName().endsWith("xml", Qt::CaseInsensitive)) {
                ++ixml;
            }
            if (qfi.fileName().endsWith("ats", Qt::CaseInsensitive)) {
                ++iats;
            }

        }
        if (ixml && !iats) {
            this->ui->label_files->setText(QString::number(ixml) + " xml files loaded");
        }
        if (!ixml && iats) {
            this->ui->label_files->setText(QString::number(iats) + " ats files loaded");
        }
        if (ixml && iats) {
            this->ui->label_files->setText(QString::number(ixml) + " xml files loaded, " + QString::number(iats) + " ats files loaded");
        }

        //this->ui->runbuton->setEnabled(true);
        this->set_waiting_for_files(false);

        this->is_gui_update = false;

        auto mcd(std::make_unique<mc_data>());
        this->first_header = mcd->scan_header_close(this->qfis);

        if (!this->first_header.size()) {
            this->ui->runbuton->setEnabled(false);
            set_waiting_for_files(true);
            this->ui->label_files->setText("waiting for files (last xml seems to be corrupted");
            this->edate_start.reset();
            this->tmp_edate_start.reset();
            this->tmp_edate_stop.reset();
            this->first_header.clear();
            return;
        }

        // create datetimes
        //     eQDateTime(const qint64 &secs_since_1970, const qint64 &msecs = 0, const double &frac_msecs = 0, const double &sample_freq = 0, const quint64 &num_samples = 0, const quint64 &start_sample = 0, const quint64 &use_samples = ULLONG_MAX);

        this->edate_start = (std::make_unique<eQDateTime>(this->first_header.qint64value("start_date_time"), 0, 0, this->first_header.dvalue("sample_freq"), this->first_header.quint64value("num_samples"), 0 ));
        this->tmp_edate_start = (std::make_unique<eQDateTime>());
        this->tmp_edate_stop = (std::make_unique<eQDateTime>());

        this->ui->dateTimeEdit_start->setDateTime(*this->edate_start);
        *this->tmp_edate_start = *this->edate_start;

        *this->tmp_edate_stop = this->edate_start->sample_time(this->edate_start->get_num_samples());

        this->ui->dateTimeEdit_stop->setDateTime(*this->tmp_edate_stop);

        // init the dattime boxes
        this->ui->dateTimeEdit_start->setMinimumDateTime(*this->edate_start);
        this->ui->dateTimeEdit_stop->setMinimumDateTime(*this->edate_start);
        this->ui->dateTimeEdit_start->setMaximumDateTime(*this->tmp_edate_stop);
        this->ui->dateTimeEdit_stop->setMaximumDateTime(*this->tmp_edate_stop);



        // init the spin boxes
        this->ui->spinBox_use_samples->setRange(1, int(this->edate_start->get_num_samples()));
        this->ui->spinBox_start_sample->setRange(0, int(this->edate_start->get_num_samples()));
        this->ui->spinBox_stop_sample->setRange(1, int(this->edate_start->get_num_samples()));
        // this->ui->spinBox_start_sample->setValue(0);
        // this->ui->spinBox_stop_sample->setValue(this->first_header.ivalue("num_samples"));

        // unix time stamp spin boxes

        this->ui->TimeStampspinBox_start->setRange(this->edate_start->sample_unix_time_stamp(0), this->edate_start->sample_unix_time_stamp( this->edate_start->get_num_samples()));
        this->ui->TimeStampspinBox_stop->setRange(this->edate_start->sample_unix_time_stamp(0), this->edate_start->sample_unix_time_stamp( this->edate_start->get_num_samples() ));



        //this->ui->TimeStampspinBox_start->setValue(this->edate_start->toTime_t());
        //this->ui->TimeStampspinBox_stop->setValue(this->edate_stop->toTime_t());

        // check box
        if (this->ui->check_fullseconds->isChecked()) {
            //this->ui->spinBox_use_samples->setSingleStep(first_header.ivalue("sample_freq"));
            this->edate_start->set_fullsec_mode(true);
        }
        //this->ui->spinBox_use_samples->setValue(this->first_header.ivalue("num_samples") );


        this->check_cut(this->edate_start->get_start_sample(), this->edate_start->get_num_samples());






    }
    else {
        //this->ui->runbuton->setEnabled(false);
        //this->ui->label_files->setText("waiting for files");
        set_waiting_for_files(true);
        this->edate_start.reset();
        this->tmp_edate_start.reset();
        this->tmp_edate_stop.reset();
        this->first_header.clear();

    }

    this->is_gui_update = true;

}


void filter::delete_files()
{
    for (auto &q : this->delete_xmlfiles)    qDebug() << q.absoluteFilePath();
    for (auto &q : this->delete_atsfiles)    qDebug() << q.absoluteFilePath();

    QList<QDir> delete_dirs;
    QStringList strdirs;

    for (auto &q : this->delete_xmlfiles) strdirs.append(q.dir().absolutePath());
    for (auto &q : this->delete_atsfiles) strdirs.append(q.dir().absolutePath());

    strdirs.removeDuplicates();

    for (auto &q : strdirs) delete_dirs.append(QDir(q));

    for (auto &q : delete_dirs) qDebug() << q.absolutePath();

    for (auto const &q : this->delete_xmlfiles) {
        QFile file(q.absoluteFilePath());
        file.remove();
    }

    for (auto const &q : this->delete_atsfiles) {
        QFile file(q.absoluteFilePath());
        file.remove();
    }

    std::cerr << "deleting reamining garbarge" << std::endl;
    for (auto const &q : delete_dirs) {
        QDir dir(q.absolutePath());
        if (dir.exists()) {
            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
            if (!dir.count()) {
                QDir().rmdir(q.absolutePath());
            }
        }
    }


}

void filter::create_xml_from_ukn(std::vector<std::shared_ptr<atsfile> > out_atsfiles)
{

    if (!out_atsfiles.size()) {
        qDebug() << "no ats files exist, can not create XML";
        return;
    }


    QString basedir= procmt_homepath("5chanEHnoise") + "/";
    QString HWConfig =      basedir +      "HwConfig.xml";
    QString HwDatabase =    basedir +    "HwDatabase.xml";
    QString HwStatus =      basedir +      "HwStatus.xml";
    QString ChannelConfig = basedir + "ChannelConfig.xml";
    QString outputjobsdir = "/tmp/aduxml/";
    QFileInfo *qfiHWConfig = new QFileInfo(HWConfig);
    QFileInfo *qfiHwDatabase = new QFileInfo(HwDatabase);
    QFileInfo *qfiChannelConfig = new QFileInfo(ChannelConfig);
    QFileInfo *qfiHwStatus = new QFileInfo(HwStatus);


    std::unique_ptr<adulib>  adujob = std::make_unique<adulib>(1, this);
    adujob->fetch_hwcfg(qfiHWConfig);
    adujob->fetch_hwdb(qfiHwDatabase);
    adujob->build_channels();
    adujob->build_hardware();



    adujob->set_start_time(out_atsfiles.at(0)->get_start_datetime());
    adujob->on_set_duration(qint64( (double(out_atsfiles.at(0)->get_num_samples())) / out_atsfiles.at(0)->get_sample_freq() ),false);

    // add fake sample freqs
    adujob->set("sample_freq", out_atsfiles.at(0)->get_sample_freq(), 1, false);

    adujob->set("system_serial_number", -1, false);
    int i = 0;
    for ( auto & ats : out_atsfiles) {

        adujob->set("ts_lsb", ats->value("lsb"), i, false);
        adujob->set("sensor_type", ats->value("sensor_type"), i, false);
        adujob->set("num_samples", ats->value("num_samples"), i, false);
        adujob->set("sensor_sernum", ats->value("sensor_sernum"), i, false);
        adujob->set("ats_data_file", ats->get_qfile_info().fileName(), i, false);
        adujob->set("sample_freq", ats->value("sample_freq"), i, false);
        adujob->set("ADC_sernum", ats->value("ADC_sernum"), i, false);
        adujob->set("DC_offset", ats->value("DC_offset"), i, false);
        adujob->set("gain_stage1", ats->value("gain_stage1"), i, false);
        adujob->set("gain_stage2", ats->value("gain_stage2"), i, false);

        adujob->set("latitude", ats->value("latitude"), i, false);
        adujob->set("longitude", ats->value("longitude"), i, false);
        adujob->set("elevation", ats->value("elevation"), i, false);
        adujob->set("chopper_status", ats->value("chopper_status"), i, false);

        adujob->set("pos_x1", ats->value("pos_x1"), i, false);
        adujob->set("pos_x2", ats->value("pos_x2"), i, false);
        adujob->set("pos_y1", ats->value("pos_y1"), i, false);
        adujob->set("pos_y2", ats->value("pos_y2"), i, false);
        adujob->set("pos_z1", ats->value("pos_z1"), i, false);
        adujob->set("pos_z2", ats->value("pos_z2"), i, false);

        ++i;
    }
    adujob->on_set_filename( out_atsfiles.at(0)->absolutePath() + "/" + out_atsfiles.at(0)->get_measdoc_name());
    adujob->on_write_job(true);

    delete qfiHwStatus;
    delete qfiChannelConfig;
    delete qfiHwDatabase;
    delete qfiHWConfig;

}

void filter::on_check60_clicked(bool checked)
{
    if (checked) {
        this->ui->check50->setChecked(false);
        this->ui->check150->setChecked(false);
        this->ui->rb_notch->setChecked(true);

    }
}

void filter::on_check150_clicked(bool checked)
{
    if (checked) {
        this->ui->check180->setChecked(false);
        this->ui->check60->setChecked(false);
        this->ui->rb_notch->setChecked(true);

    }
}

void filter::on_check50_clicked(bool checked)
{
    if (checked) {
        this->ui->check180->setChecked(false);
        this->ui->check60->setChecked(false);
        this->ui->rb_notch->setChecked(true);
    }
}

void filter::on_check180_clicked(bool checked)
{
    if (checked) {
        this->ui->check50->setChecked(false);
        this->ui->check150->setChecked(false);
        this->ui->rb_notch->setChecked(true);

    }
}

void filter::on_rb_notch_clicked(bool checked)
{
    if (checked) {
        this->ui->check50->setChecked(true);
    }
    else {
        this->ui->check50->setChecked(false);
    }
}

void filter::on_rb_band_pass_clicked()
{
    if (!this->qxml.size()) return;
    if (this->qdi != nullptr) delete qdi;
    this->extrafilters_status.reset();
    this->extrafilters_status = std::make_shared<QMap<QString, QVariant>>();

    this->extrafilters_status->clear();
    this->extrafilters_status->insert("filter_name", "bandpass");
    this->qdi = new low_high_band_passses(this->extrafilters_status, this);
    auto mcd(std::make_unique<mc_data>());
    mcd->insert_prc_com(cmdline);
    mcd->open_files(this->qxml, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
    double fsample = mcd->in_atsfiles.at(0)->get_sample_freq();
    this->qdi->set_f_min_max_lp(fsample / 32.0, fsample / 2.0);
    this->qdi->set_initValue_lp(fsample / 4.0);
    this->qdi->set_f_min_max_hp(fsample / 16398.0, fsample / 8.0);
    this->qdi->set_initValue_hp(fsample / 4096.0);

    this->qdi->show();
}


void filter::on_rb_low_pass_clicked()
{
    if (!this->qxml.size()) return;
    if (this->qdi != nullptr) delete qdi;
    this->extrafilters_status.reset();
    this->extrafilters_status = std::make_shared<QMap<QString, QVariant>>();
    this->extrafilters_status->clear();
    this->extrafilters_status->insert("filter_name", "lowpass");
    this->qdi = new low_high_band_passses(this->extrafilters_status, this);
    auto mcd(std::make_unique<mc_data>());
    mcd->insert_prc_com(cmdline);
    mcd->open_files(this->qxml, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
    double fsample = mcd->in_atsfiles.at(0)->get_sample_freq();

    // that should be deep enough - you may not see variation at all in the time series
    // so here the data becomes visible again after filtering

    //      this->qdi->set_f_min_max_lp(0.1, 8);
    //      this->qdi->set_initValue_lp( 4.0);
    this->qdi->set_f_min_max_lp(fsample / 32.0, fsample / 2.0);
    this->qdi->set_initValue_lp(fsample / 4.0);
    this->qdi->show();

}

void filter::on_rb_high_pass_clicked()
{
    if (!this->qxml.size()) return;
    if (this->qdi != nullptr) delete qdi;
    this->extrafilters_status.reset();
    this->extrafilters_status = std::make_shared<QMap<QString, QVariant>>();

    this->extrafilters_status->clear();
    this->extrafilters_status->insert("filter_name", "highpass");
    this->qdi = new low_high_band_passses(this->extrafilters_status, this);
    auto mcd(std::make_unique<mc_data>());
    mcd->insert_prc_com(cmdline);
    mcd->open_files(this->qxml, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
    double fsample = mcd->in_atsfiles.at(0)->get_sample_freq();

    // doe not make sense to go above
    this->qdi->set_f_min_max_hp(fsample / 16398.0, fsample / 8.0);
    this->qdi->set_initValue_hp(fsample / 4096.0);


    this->qdi->show();
}

void filter::slot_set_filter_state()
{
    for (auto bt : this->allButtons) {
        if (bt->objectName() == "rb_notch") {
            if (bt->isChecked() == false) {
                this->ui->check50->setChecked(false);
                this->ui->check150->setChecked(false);
                this->ui->check180->setChecked(false);
                this->ui->check60->setChecked(false);
                this->ui->check16_23->setChecked(false);
            }
        }

    }
}



void filter::on_rb_1x_clicked(bool checked)
{
    if (checked) this->ui->check_fullseconds->setChecked(true);
}

void filter::on_spinBox_use_samples_valueChanged(int arg1)
{
    // avoid endless call backs
    if (!is_gui_update) return;
    this->check_cut(this->edate_start->get_start_sample(), (quint64(arg1)));

}

void filter::on_spinBox_start_sample_valueChanged(int arg1)
{
    if (!is_gui_update) return;
    this->check_cut((quint64(arg1)), this->edate_start->get_use_samples(), true);
}

void filter::on_spinBox_stop_sample_valueChanged(int arg1)
{
    if (!is_gui_update) return;
    this->check_cut(this->edate_start->get_start_sample(), (quint64(arg1)) - this->edate_start->get_start_sample(), true);
}

void filter::on_TimeStampspinBox_start_valueChanged(int arg1)
{
    if (!is_gui_update) return;

    qint64 newtime = this->edate_start->check_get_change_start_time(qint64(arg1) * 1000);
    this->tmp_edate_start->setMSecsSinceEpoch(newtime * 1000);
    if (!newtime) return;

    quint64 us = this->edate_start->samples_to(*this->tmp_edate_start);
    this->check_cut(us, this->edate_start->get_use_samples());
}

void filter::on_TimeStampspinBox_stop_valueChanged(int arg1)
{
    if (!is_gui_update) return;

    qint64 newtime = this->edate_start->check_get_change_stop_time(qint64(arg1) * 1000);
    if (!newtime) return;
    this->tmp_edate_stop->setMSecsSinceEpoch(newtime * 1000);


    quint64 us = this->edate_start->samples_to(*this->tmp_edate_stop);
    this->check_cut(this->edate_start->get_start_sample(), us - this->edate_start->get_start_sample());
}

void filter::on_dateTimeEdit_start_dateTimeChanged(const QDateTime &dateTime)
{
    if (!is_gui_update) return;

    qint64 newtime = this->edate_start->check_get_change_start_time(dateTime.toMSecsSinceEpoch());
    if (!newtime) return;
    this->tmp_edate_start->setMSecsSinceEpoch(newtime * 1000);
    eQDateTime old_start(this->edate_start->sample_time(this->edate_start->get_start_sample()));
    quint64 start_sample = this->edate_start->samples_to(*this->tmp_edate_start);
    // if new start time is smaller we want to increase the used samples
    // used samples are the driving force
    //if (*this->tmp_edate_start < old_start) {
    // we should have a valid stop time at this moment
    eQDateTime present_stop(this->ui->dateTimeEdit_stop->dateTime());
    quint64 stop_sample = this->edate_start->samples_to(present_stop);
    //if ((stop_sample - start_sample) <= this->edate_start->get_num_samples()) this->edate_start->set_num_samples(stop_sample - start_sample);
    this->check_cut(start_sample, stop_sample - start_sample);
    //}
    //else this->check_cut(start_sample, this->edate_start->get_use_samples());

}

void filter::on_dateTimeEdit_stop_dateTimeChanged(const QDateTime &dateTime)
{
    if (!is_gui_update) return;

    qint64 newtime = this->edate_start->check_get_change_stop_time(dateTime.toMSecsSinceEpoch());
    if (!newtime) {

        return;
    }
    this->tmp_edate_stop->setMSecsSinceEpoch(newtime * 1000);

    quint64 us = this->edate_start->samples_to(*this->tmp_edate_stop);
    this->check_cut(this->edate_start->get_start_sample(), us - this->edate_start->get_start_sample());

}

void filter::on_check_fullseconds_clicked(bool checked)
{
    // if (checked) this->ui->spinBox_use_samples->setSingleStep(first_header.ivalue("sample_freq"));
    // else this->ui->spinBox_use_samples->setSingleStep(1);
    this->edate_start->set_fullsec_mode(checked);


}

bool filter::check_cut(const quint64 starts, const quint64 uses, const bool force_min_change)
{
    this->is_gui_update = false;

    this->edate_start->cuts(starts, uses, force_min_change);
    this->ui->spinBox_use_samples->setValue(int(this->edate_start->get_use_samples()));
    this->ui->spinBox_start_sample->setValue(int(this->edate_start->get_start_sample()));
    this->ui->spinBox_stop_sample->setValue(int(this->edate_start->get_stop_sample()));
    this->ui->dateTimeEdit_start->setDateTime(this->edate_start->sample_time(this->edate_start->get_start_sample()));
    this->ui->dateTimeEdit_stop->setDateTime(this->edate_start->sample_time(this->edate_start->get_start_sample() + this->edate_start->get_use_samples()));
    this->ui->TimeStampspinBox_start->setValue(int(this->edate_start->sample_unix_time_stamp(this->edate_start->get_start_sample())));
    this->ui->TimeStampspinBox_stop->setValue(int(this->edate_start->sample_unix_time_stamp( this->edate_start->get_start_sample() + this->edate_start->get_use_samples() )));
    this->is_gui_update = true;
    return true;
}


void filter::on_rb_cut_clicked()
{
    this->ui->rb_1x->setChecked(true);
    this->ui->rb_1x->click();
}

void filter::on_rb_remap_clicked()
{
    this->ui->rb_1x->setChecked(true);
    this->ui->rb_1x->click();

}



void filter::on_checkBox_copy_old_files_clicked()
{
    if (this->ui->checkBox_copy_old_files->isChecked()) {
        this->ui->rb_cut->setChecked(true);
        this->on_rb_cut_clicked();
    }
}

void filter::build_remapper()
{
    if (this->has_remapper_built) return;
    else if (this->qxml.size()) {
        this->has_remapper_built = true;
        auto mcd(std::make_unique<mc_data>());

        QList<QFileInfo> qfisxmlats;
        qfisxmlats.append(this->qxml.at(0));
        mcd->open_files(qfisxmlats, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);


        for (auto &ats : mcd->in_atsfiles) {
            ats->scan_header_close();
        }


        for (auto &ats : mcd->in_atsfiles) {
            this->button_id_map.insert(ats->svalue("channel_type"), ats->ivalue("channel_number"));
        }

        for (auto &ats : mcd->in_atsfiles) {
            QString kky(ats->svalue("channel_type"));
            this->in_channels_labels.insert(kky, new QCheckBox(this->ui->tab_remap));
            this->out_channels_widgets.insert(kky, new QWidget(this->ui->tab_remap));
            this->out_channels_layouts.insert(kky, new QHBoxLayout(this->ui->tab_remap));
            this->out_channels_groups.insert(kky, new QButtonGroup(this->ui->tab_remap));
            this->out_channels_groups.value(kky)->setExclusive(true);
            //int bi = this;
            for (auto &nam : this->remap_btn_names) {
                this->out_channels_groups.value(kky)->addButton(new QRadioButton(nam, this->out_channels_widgets.value(kky)), this->button_id_map.value(nam));
            }
            this->out_channels_groups.value(kky)->button(this->button_id_map.value(ats->svalue("channel_type")))->click();
            for (auto &qlbut : this->out_channels_groups.value(kky)->buttons()) {
                this->out_channels_layouts.value(kky)->addWidget(qlbut);
            }
            this->out_channels_widgets.value(kky)->setLayout(this->out_channels_layouts.value(kky));


        }
        QMap<QString, QCheckBox*>::iterator add(this->in_channels_labels.begin());

        for (; add != this->in_channels_labels.end(); ++add) {
            add.value()->setText(add.key());
            this->ui->left_remap->addWidget(add.value());
        }
        for (auto &wadd :  this->out_channels_widgets) {
            this->ui->right_remap->addWidget(wadd);
        }

    }
}

void filter::on_actionHelp_triggered()
{

    this->pmt_help.help("atsfilter", "atsfilter.html", 2);


}

void filter::on_actionopenFolder_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);

    dialog.setModal(false);
    dialog.show();

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles();
    if (fileNames.size()) {
        for (auto &str : fileNames) {

            QFileInfo qfi(str);
            if (qfi.isDir()) {
                QDir dir(qfi.absoluteDir());
                QStringList xmls;
                xmls << "*.xml" << "*.XML";
                QStringList allxmls(dir.entryList(xmls));
                for (auto &str : allxmls) {
                    QFileInfo qfix(str);
                    QFileInfo qfixabs(qfix.absoluteFilePath());
                    if (qfixabs.exists()) {
                        this->qxml.append(qfixabs);
                    }
                }

            }

            if (this->qxml.size()) {
                this->open();
            }
        }
    }


}

void filter::on_actionopenXML_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setFilter(QDir::Files);

    dialog.setModal(false);
    dialog.show();

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles();
    if (fileNames.size()) {
        for (auto &str : fileNames) {
            QFileInfo qfi(str);
            if (qfi.exists() && qfi.fileName().endsWith("xml", Qt::CaseInsensitive)) {
                this->qxml.append(qfi);
            }
            if (this->qxml.size()) {
                this->open();
            }
        }
    }

}

void filter::on_actionopenATS_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setFilter(QDir::Files);

    dialog.setModal(false);
    dialog.show();

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles();
    if (fileNames.size()) {
        for (auto &str : fileNames) {
            QFileInfo qfi(str);
            if (qfi.exists() && qfi.fileName().endsWith("ats", Qt::CaseInsensitive)) {
                this->qxml.append(qfi);
            }
            if (this->qxml.size()) {
                this->open();
            }
        }
    }

}
