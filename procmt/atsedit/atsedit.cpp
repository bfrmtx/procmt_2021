#include "atsedit.h"
#include "ui_atsedit.h"

atsedit::atsedit(const QString &dbname, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::atsedit)
{
    this->dbname = dbname;
    ui->setupUi(this);

    this->setLocale(QLocale::c());


    this->site_table_includes << "site_name" << "client" << "contractor" << "area" << "survey_id"
                              << "operator" << "site_name" << "survey_header_name" << "site_name_rr" << "site_name_emap";

    this->excludes << "sample_freq" << "num_samples" << "start_date" <<
                      "start_time" << "start_date_time" << "dipole_length" << "dipole_angle"
                   << "stop_date" << "stop_time" << "run_number"
                   << "latitude" << "longitude" <<  "elevation" << "UTMZone" "GaussRefMeridian" ;


    this->ui->plainTextEdit_comment->setAcceptDrops(false);

    QStringList EWitems;
    QStringList NSitems;
    EWitems << "E" << "W";
    NSitems << "N" << "S";
    this->ui->comboBox_EW->addItems(EWitems);
    this->ui->comboBox_NS->addItems(NSitems);
    this->ui->dateTimeEdit_startsat->setTimeSpec(Qt::UTC);
    this->ui->dateTimeEdit_stopsat->setTimeSpec(Qt::UTC);
    this->ui->dateTimeEdit_startsat->setDateTime(QDateTime::fromString("2000-01-01 12:00:00", "yyyy-MM-dd HH:mm:ss"));
    this->ui->dateTimeEdit_stopsat->setDateTime(QDateTime::fromString("2000-01-01 12:00:01", "yyyy-MM-dd HH:mm:ss"));


    this->calcf = std::make_unique<calcfilesize>(this->dbname, this->ui->selfreq, this);
    this->calcf->start_time_changed(this->ui->dateTimeEdit_startsat->dateTime());
    this->calcf->stop_time_changed(this->ui->dateTimeEdit_stopsat->dateTime());
    this->calcf->channels_value_changed(this->ui->spinBox_channels->value());
    this->calcf->frequency_index_changed(this->ui->selfreq->currentIndex());

    this->allsensors = std::make_unique<sensors_combo>(this->dbname, QChar(' '), this->ui->comboBox_sensortype);



}
atsedit::~atsedit()
{
    if (this->atsh != nullptr) this->atsh.reset();
    if (this->site_table != nullptr) this->site_table.reset();
    if (this->measdoc != nullptr) {
        this->measdoc->close();
        this->measdoc.reset();
    }

    delete ui;
}

void atsedit::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void atsedit::dropEvent(QDropEvent *event)
{

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    this->open_urls(urls, "");
}

void atsedit::open_urls(const QList<QUrl> urls, const QString otherwise_absolute_path)
{

    this->ui->plainTextEdit_comment->clear();
    if (this->atsh != nullptr) this->atsh.reset();
    if (this->measdoc != nullptr) {
        this->measdoc->close();
        this->measdoc.reset();
    }

    if (urls.size()) {
        if (urls.at(0).isValid()) {
            this->qfi.setFile(urls.at(0).toLocalFile());
        }
        else {
            this->setWindowTitle("FAILED");
            return;
        }
    }

    else if (otherwise_absolute_path.size()) {
        qfi.setFile(otherwise_absolute_path);
    }
    size_t check;
    if (!qfi.absoluteFilePath().endsWith("ats", Qt::CaseInsensitive)) return;
    if (!qfi.exists()) {
        this->setWindowTitle("FAILED");
    }
    this->atsh = std::make_unique<atsheader>(this->qfi, 0, this);
    check = this->atsh->scan_header_close();
    if (check == SIZE_MAX) {
        this->atsh.reset();
        this->setWindowTitle("FAILED");
    }

    this->atsh->set_change_measdir(false);

    this->starts = this->atsh->get_start_datetime();
    this->stops = this->atsh->get_stop_datetime();



    this->ui->dateTimeEdit_starts->setDateTime(this->starts);
    this->ui->dateTimeEdit_stops->setDateTime(this->stops);
    this->ui->dateTimeEdit_stops->setEnabled(false);


    this->ui->lineEdit_num_samples->setText(QString::number(this->starts.get_num_samples()));
    this->ui->lineEdit_time_stamp->setText(this->starts.timestamp_to_str());
    this->ui->doubleSpinBox_sample_freq->setRange(DBL_MIN, DBL_MAX);
    this->ui->doubleSpinBox_sample_freq->setValue(this->starts.get_sample_freq());
    if (this->starts.get_sample_freq() < 1.0) {
        this->ui->sample_seconds_label->setText("(" + QString::number(1./this->starts.get_sample_freq()) + "s)");
    }


    ////////////////////// SENSOR POS SER  comments////////////////////////////////////////////
    if (this->sens_pos_ser != nullptr) this->sens_pos_ser.reset();
    this->sens_pos_ser = std::make_unique<prc_com>();
    this->sens_pos_ser->insert("pos_x1", double(0.0));
    this->sens_pos_ser->insert("pos_x2", double(0.0));
    this->sens_pos_ser->insert("pos_y1", double(0.0));
    this->sens_pos_ser->insert("pos_y2", double(0.0));
    this->sens_pos_ser->insert("pos_z1", double(0.0));
    this->sens_pos_ser->insert("pos_z2", double(0.0));
    this->sens_pos_ser->insert("sensor_type", QString(""));
    this->sens_pos_ser->insert("sensor_sernum",  int(0));
    this->sens_pos_ser->insert("xml_header", QString(""));
    this->sens_pos_ser->insert("comments", QString(""));


    this->sens_pos_ser->set_data(this->atsh->get_data(), false);
    this->ui->plainTextEdit_comment->insertPlainText(this->sens_pos_ser->svalue("comments"));


    this->ui->doubleSpinBox_x1->setRange(-DBL_MAX, DBL_MAX);
    this->ui->doubleSpinBox_x1->setValue(this->sens_pos_ser->dvalue("pos_x1"));

    this->ui->doubleSpinBox_x2->setRange(-DBL_MAX, DBL_MAX);
    this->ui->doubleSpinBox_x2->setValue(this->sens_pos_ser->dvalue("pos_x2"));

    this->ui->doubleSpinBox_y1->setRange(-DBL_MAX, DBL_MAX);
    this->ui->doubleSpinBox_y1->setValue(this->sens_pos_ser->dvalue("pos_y1"));

    this->ui->doubleSpinBox_y2->setRange(-DBL_MAX, DBL_MAX);
    this->ui->doubleSpinBox_y2->setValue(this->sens_pos_ser->dvalue("pos_y2"));

    this->ui->doubleSpinBox_z1->setRange(-DBL_MAX, DBL_MAX);
    this->ui->doubleSpinBox_z1->setValue(this->sens_pos_ser->dvalue("pos_z1"));

    this->ui->doubleSpinBox_z2->setRange(-DBL_MAX, DBL_MAX);
    this->ui->doubleSpinBox_z2->setValue(this->sens_pos_ser->dvalue("pos_z2"));

    this->ui->lineEdit_sernum->setText(this->sens_pos_ser->svalue("sensor_sernum"));
    this->ui->lineEdit_xml_header->setText(this->sens_pos_ser->svalue("xml_header"));

    if (this->sens_pos_ser->svalue("xml_header").size() && this->sens_pos_ser->svalue("xml_header").endsWith(".xml", Qt::CaseInsensitive) ) {

        QString pth = this->qfi.absolutePath();
        this->qfi_measdoc.setFile(pth + "/" + this->sens_pos_ser->svalue("xml_header"));
        if (this->qfi_measdoc.exists()) {
            this->measdoc = std::make_unique<measdocxml>(&this->qfi_measdoc, nullptr, nullptr, this);
        }
    }


    // todo the info sqlite -> combobox; and select the sensor_type



    if (this->site_table != nullptr) this->site_table.reset();
    this->site_table = std::make_unique<prc_com_table>(this);
    this->site_table->set_includes(this->site_table_includes);
    this->site_table->set_map(this->atsh->get_data());
    this->ui->tableView_site->setModel(this->site_table.get());
    this->ui->tableView_site->setAlternatingRowColors(true);
    this->ui->tableView_site->resizeColumnToContents(0);
    this->ui->tableView_site->resizeColumnToContents(1);
    this->ui->tableView_site->resizeRowsToContents();
    this->ui->tableView_site->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->tableView_site->horizontalHeader()->setStretchLastSection(true);



    // lastly the system_table which uses the excludes
    if (this->system_table != nullptr) this->system_table.reset();
    this->system_table = std::make_unique<prc_com_table>(this);
    this->system_table->set_excludes(this->site_table_includes);
    this->system_table->set_excludes(this->excludes);
    this->system_table->set_excludes(this->sens_pos_ser->keys());

    QStringList used_filters = this->atsh->get_filter();

    this->tmp_header.clear();
    this->tmp_header = this->atsh->get_data();
    this->tmp_header.remove("LF_filters");
    this->tmp_header.remove("HF_filters");

    int nums = 1;
    for (auto &str : used_filters) {
        QString fil ("Filter_");
        fil.append(QString::number(nums++));
        this->tmp_header.insert(fil, str);
    }
    // I always need an empty second filter in the GUI otherwise I can not set him
    this->tmp_header.insert("Filter_" + QString::number(nums++), "off");

    this->system_table->set_map(this->tmp_header);

    //this->system_table->set_map(this->atsh->get_data());
    this->ui->tableView_system->setModel(this->system_table.get());
    this->ui->tableView_system->setAlternatingRowColors(true);
    this->ui->tableView_system->resizeColumnToContents(0);
    this->ui->tableView_system->resizeColumnToContents(1);
    this->ui->tableView_system->resizeRowsToContents();
    this->ui->tableView_system->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->tableView_system->horizontalHeader()->setStretchLastSection(true);


    this->ui->doubleSpinBox_easting->setRange(DBL_MIN, DBL_MAX);
    this->ui->doubleSpinBox_northing->setRange(DBL_MIN, DBL_MAX);
    this->ui->doubleSpinBox_elev_m->setRange(DBL_MIN, DBL_MAX);
    this->ui->doubleSpinBox_easting->setDecimals(3);
    this->ui->doubleSpinBox_northing->setDecimals(3);
    this->ui->doubleSpinBox_elev_m->setDecimals(3);

    this->ui->spinBox_lat_msecs->setRange(INT_MIN, INT_MAX);
    this->ui->spinBox_lon_msecs->setRange(INT_MIN, INT_MAX);

    this->allsensors->set_atsheader_sensor_name(this->atsh->svalue("sensor_type"));

    this->coord.set_lat_lon_msec(this->atsh->ivalue("latitude"), this->atsh->ivalue("longitude"), (long double)(this->atsh->dvalue("elevation")/100.));
    this->update_gui_vars();
    this->update_coord_gui();

    if (this->atsh != nullptr) this->setWindowTitle(this->atsh->baseName());


    //this->ui->tableView_site->setEditTriggers(QAbstractItemView::SelectedClicked);
    // see also for edit behaviour the prc_com_table class

}

void atsedit::on_buttonBox_accepted()
{
    if (this->atsh == nullptr) return;
    QFileInfo oldfi(this->atsh->absoluteFilePath());
    QFileInfo actfi;

    this->atsh->set_data(this->site_table->get_map(), false);

    this->sens_pos_ser->set_key_value("comments", this->ui->plainTextEdit_comment->toPlainText());
    qDebug() << this->ui->plainTextEdit_comment->toPlainText();
    this->atsh->set_data(this->sens_pos_ser->get_data(), false);

    this->atsh->set_start_date_time(this->starts);
    this->atsh->set_key_value("sample_freq", this->starts.get_sample_freq());
    this->atsh->set_key_value("num_samples", this->starts.get_num_samples());



    this->tmp_header.clear();
    this->tmp_header = this->system_table->get_map();
    this->tmp_header.remove("LF_filters");
    this->tmp_header.remove("HF_filters");

    this->atsh->set_data(this->tmp_header, false);

    QStringList act_filters;
    act_filters << tmp_header.value("Filter_1").toString();
    act_filters << tmp_header.value("Filter_2").toString();
    this->tmp_header.remove("Filter_1");
    this->tmp_header.remove("Filter_2");
    this->atsh->set_filter(act_filters);


    this->atsh->set_key_value("sensor_type", this->allsensors->get_atsheader_sensor_name());
    this->atsh->prc_com_to_classmembers();

    // editor must change in place
    actfi.setFile(this->atsh->get_new_filename(false));
    if (oldfi.absoluteFilePath() != actfi.absoluteFilePath()) {
        qDebug() <<" new file must generated!";
        qDebug() << oldfi.baseName() << " to " << atsh->baseName();
        this->atsh->close(false);
        this->ovr = new overwrite_dialog(this);
        int doit = 0;
        doit =this->ovr->exec();
        if (doit) {
            QFile newfile(oldfi.absoluteFilePath());
            newfile.rename(actfi.absoluteFilePath());
            std::unique_ptr<atsheader> atshnew = nullptr;
            atshnew = std::make_unique<atsheader>(actfi, 0, this);
            atshnew->scan_header_close();
            atshnew->set_change_measdir(false);

            // repeat above now
            atshnew->set_data(this->site_table->get_map(), false);

            this->sens_pos_ser->set_key_value("comments", this->ui->plainTextEdit_comment->toPlainText());
            qDebug() << this->ui->plainTextEdit_comment->toPlainText();
            atshnew->set_data(this->sens_pos_ser->get_data(), false);

            atshnew->set_start_date_time(this->starts);
            atshnew->set_key_value("sample_freq", this->starts.get_sample_freq());
            atshnew->set_key_value("num_samples", this->starts.get_num_samples());

            atshnew->set_data(this->system_table->get_map(), false);
            atshnew->set_key_value("sensor_type", this->allsensors->get_atsheader_sensor_name());
            if (atshnew->get_new_filename(false) != actfi.absoluteFilePath()) {
                qDebug() << "failed to creat new file -> atsedit::on_buttonBox_accepted";
                exit(1);
            }
            atshnew->prc_com_to_classmembers();
            atshnew->write_header(true);
            atshnew->close(false);

        }
        delete this->ovr;


    }

    else {
        this->atsh->write_header(true);
        this->atsh->close(false);
        if (this->qfi_measdoc.exists() && (this->measdoc != nullptr)) {
            this->measdoc->slot_update_atswriter(atsh->get_atswriter_section(), atsh->ivalue("channel_number"), this->atsh->absolutePath(), atsh->get_measdoc_name());
            this->measdoc->set_date_time(this->atsh->get_start_datetime(), this->atsh->get_stop_datetime());
            //this->measdoc->set_simple_section("HwStatus", "GPS", atsh->get_gps_section());
            this->measdoc->set_lat_lon_elev(atsh->ivalue("latitude"), atsh->ivalue("longitude"), atsh->ivalue("elevation"));
            this->measdoc->save(&this->qfi_measdoc);
        }
    }

    QList<QUrl> urlis;
    this->open_urls(urlis, actfi.absoluteFilePath());


    qDebug() << "header re-written";

}

void atsedit::on_buttonBox_rejected()
{

    if (this->atsh != nullptr) this->atsh.reset();
    this->close();

}

void atsedit::on_doubleSpinBox_x2_valueChanged(double arg1)
{
    this->sens_pos_ser->set_key_value("pos_x2", arg1);
}

void atsedit::on_doubleSpinBox_y1_valueChanged(double arg1)
{
    this->sens_pos_ser->set_key_value("pos_y1", arg1);
}

void atsedit::on_doubleSpinBox_y2_valueChanged(double arg1)
{
    this->sens_pos_ser->set_key_value("pos_y2", arg1);

}

void atsedit::on_doubleSpinBox_x1_valueChanged(double arg1)
{
    this->sens_pos_ser->set_key_value("pos_x1", arg1);

}

void atsedit::on_doubleSpinBox_z1_valueChanged(double arg1)
{
    this->sens_pos_ser->set_key_value("pos_z1", arg1);

}

void atsedit::on_doubleSpinBox_z2_valueChanged(double arg1)
{
    this->sens_pos_ser->set_key_value("pos_z2", arg1);

}

void atsedit::on_comboBox_sensortype_editTextChanged(const QString &arg1)
{
    this->sens_pos_ser->set_key_value("sensor_type", arg1);

}

void atsedit::on_lineEdit_sernum_textChanged(const QString &arg1)
{
    this->sens_pos_ser->set_key_value("sensor_sernum", arg1);
}

void atsedit::on_lineEdit_xml_header_textChanged(const QString &arg1)
{
    this->sens_pos_ser->set_key_value("xml_header", arg1);
}

void atsedit::on_lineEdit_time_stamp_textChanged(const QString &arg1)
{
    if (this->time_updt) {
        this->time_updt = false;
        this->starts.update_time_stamp_from_string(arg1);
        this->ui->dateTimeEdit_starts->setDateTime(this->starts);

        this->stops = this->starts.sample_time(this->starts.get_num_samples());
        this->ui->dateTimeEdit_stops->setDateTime(this->stops);

        this->time_updt = true;
    }
}

void atsedit::on_lineEdit_num_samples_textChanged(const QString &arg1)
{
    if (this->time_updt) {
        this->time_updt = false;
        quint64 sam = arg1.toULongLong();
        this->starts.set_num_samples(sam);

        this->stops = this->starts.sample_time(this->starts.get_num_samples());
        this->ui->dateTimeEdit_stops->setDateTime(this->stops);

        this->time_updt = true;
    }
}

void atsedit::on_dateTimeEdit_starts_dateTimeChanged(const QDateTime &dateTime)
{
    if (this->time_updt) {
        this->time_updt = false;
        this->starts = dateTime;
        this->ui->lineEdit_time_stamp->setText(this->starts.timestamp_to_str());
        this->stops = this->starts.sample_time(this->starts.get_num_samples());
        this->ui->dateTimeEdit_stops->setDateTime(this->stops);

        this->time_updt = true;
    }
}

void atsedit::update_gui_vars()
{

    int gk;


    this->coord.coordinates(this->latitude, this->longitude, this->elevation);
    this->coord.get_lat_lon_msec(this->msec_lat, this->msec_lon, this->elevation);
    this->coord.get_lat_lon(this->deg_la,   this->mi_la,   this->se_la,  this->LatH,
                            this->deg_lo,   this->mi_lo,   this->se_lo,   this->LonH,
                            this->elevation,   this->utm_meridian,   gk);

    this->coord.get_UTM_ZONE(this->northing, this->easting, this->UTMzone);




}

void atsedit::update_coord_gui()
{
    this->coo_update = false;

    this->ui->doubleSpinBox_easting->setValue((double)this->easting);
    this->ui->doubleSpinBox_northing->setValue((double)this->northing);
    this->ui->lineEdit_zone->setText(this->UTMzone);


    this->ui->spinBox_lat_deg->setValue(this->deg_la);
    this->ui->spinBox_lat_min->setValue(this->mi_la);
    this->ui->doubleSpinBox_lat_sec->setValue(this->se_la);


    this->ui->spinBox_lon_deg->setValue(this->deg_lo);
    this->ui->spinBox_lon_min->setValue(this->mi_lo);
    this->ui->doubleSpinBox_lon_sec->setValue(this->se_lo);

    this->ui->spinBox_meridan->setValue(this->utm_meridian);


    this->ui->spinBox_lat_msecs->setValue(this->msec_lat);
    this->ui->spinBox_lon_msecs->setValue(this->msec_lon);
    this->ui->doubleSpinBox_elev_m->setValue((double)this->elevation);

    if (0 == this->LatH.compare("N")) ui->comboBox_NS->setCurrentIndex(0);
    else ui->comboBox_NS->setCurrentIndex(1);
    if (0 == this->LonH.compare("E")) ui->comboBox_EW->setCurrentIndex(0);
    else ui->comboBox_EW->setCurrentIndex(1);


    QString as,bs;
    long double la, lo, elev;
    this->coord.coordinates(la,lo,elev);
    this->ui->coord_lineEdit->setText(this->coord.get_lat_lon_str(as, bs));
    this->ui->maps_lineEdit->setText(this->coord.get_lat_lon_str_google_maps());
    this->ui->coord_lineEdit->setReadOnly(false);
    this->ui->maps_lineEdit->setReadOnly(false);


    this->coo_update = true;


}

void atsedit::recalc_coordinates()
{
    if (this->b_from_deg) {
        this->coord.set_lat_lon(this->deg_la, this->mi_la, this->se_la, this->LatH,
                                this->deg_lo, this->mi_lo, this->se_lo, this->LonH,
                                this->elevation);
    }

    if (this->b_from_mesc) {
        this->coord.set_lat_lon_msec(this->msec_lat, this->msec_lon, this->elevation);
    }

    if (this->b_from_utm) {
        this->coord.set_utm(this->northing, this->easting, this->UTMzone, true);
    }

    this->b_from_deg = this->b_from_mesc = this->b_from_utm;
    this->update_gui_vars();
    this->update_coord_gui();

}

void atsedit::on_spinBox_lat_deg_valueChanged(int arg1)
{
    if (coo_update == true) {
        this->deg_la = arg1;
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_spinBox_lat_min_valueChanged(int arg1)
{
    if (coo_update == true) {
        this->mi_la = arg1;
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_doubleSpinBox_lat_sec_valueChanged(double arg1)
{
    if (coo_update == true) {
        this->se_la = arg1;
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_spinBox_lon_deg_valueChanged(int arg1)
{
    if (coo_update == true) {
        this->deg_lo = arg1;
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_spinBox_lon_min_valueChanged(int arg1)
{
    if (coo_update == true) {
        this->mi_lo = arg1;
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_doubleSpinBox_lon_sec_valueChanged(double arg1)
{
    if (coo_update == true) {
        this->se_lo = arg1;
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_doubleSpinBox_northing_valueChanged(double arg1)
{
    if (coo_update == true) {
        this->northing = arg1;
        this->b_from_utm = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_doubleSpinBox_easting_valueChanged(double arg1)
{
    if (coo_update == true) {
        this->easting = arg1;
        this->b_from_utm = true;
        this->recalc_coordinates();
    }
}


void atsedit::on_spinBox_lon_msecs_valueChanged(int arg1)
{

    if (coo_update == true) {
        this->msec_lon = arg1;
        this->b_from_mesc = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_spinBox_lat_msecs_valueChanged(int arg1)
{
    if (coo_update == true) {
        this->msec_lat = arg1;
        this->b_from_mesc = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_doubleSpinBox_elev_m_valueChanged(double arg1)
{
    if (coo_update == true) {
        this->elevation = arg1;
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}




void atsedit::on_comboBox_NS_currentIndexChanged(const QString &arg1)
{
    if (coo_update == true) {
        this->LatH = arg1.at(0);
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

void atsedit::on_comboBox_EW_currentIndexChanged(const QString &arg1)
{
    if (coo_update == true) {
        this->LonH = arg1.at(0);
        this->b_from_deg = true;
        this->recalc_coordinates();
    }
}

calcfilesize::calcfilesize(const QString &dbname, QComboBox *box, atsedit *gui) : QObject(gui), box(box)
{
    this->selectfreq = std::make_unique<frequencies_combo>(dbname, box);
    this->gui = gui;
    this->box = box;


}

quint64 calcfilesize::calc_filesizes()
{
    if (this->gui->ui->checkBox_CEA->isChecked()) this->headersize_used = this->headersize_cea;
    else this->headersize_used = this->headersize;
    this->total_size = ((this->freq * this->sample_size * this->samplesecs)  + this->headersize_used ) * this->nchannels;

    // std::cerr << "total / 1024            freq  sample size samplesescs channels" << std::endl;
    // std::cerr << total_size / 1024 << "  "  << this->freq << "  " << this->sample_size << "  "  << this->samplesecs << "  "  << this->nchannels << "  "  << std::endl;

    this->gui->ui->byte_size_lineEdit->setText(QString::number(this->total_size));
    this->gui->ui->Mbyte_size_lineEdit->setText(QString::number(this->total_size/ (1024 * 1024)));
    this->gui->ui->kbyte_size_lineEdit->setText(QString::number(this->total_size / 1024));

    return this->total_size;
}

void calcfilesize::start_time_changed(const QDateTime &startime)
{
    this->starts = startime.toMSecsSinceEpoch();
    if (this->starts == this->stops) this->samplesecs = 0;
    if (this->starts > this->stops) this->samplesecs = 0;
    else this->samplesecs = (this->stops - this->starts) / 1000;
    this->calc_filesizes();

}

void calcfilesize::stop_time_changed(const QDateTime &stoptime)
{
    this->stops = stoptime.toMSecsSinceEpoch();
    if (this->starts == this->stops) this->samplesecs = 0;
    if (this->starts > this->stops) this->samplesecs = 0;
    else this->samplesecs = (this->stops - this->starts) / 1000;
    this->calc_filesizes();
}

void calcfilesize::channels_value_changed(const int nchan)
{
    this->nchannels = nchan;
    this->calc_filesizes();
}

void calcfilesize::frequency_index_changed(const int index)
{
    this->freq = this->box->itemData(index).toDouble();
    this->calc_filesizes();
}


void atsedit::on_selfreq_currentIndexChanged(int index)
{
    if (this->calcf != nullptr)
        this->calcf->frequency_index_changed(index);
}

void atsedit::on_spinBox_channels_valueChanged(int arg1)
{
    if (this->calcf != nullptr)
        this->calcf->channels_value_changed(arg1);
}

void atsedit::on_dateTimeEdit_startsat_dateTimeChanged(const QDateTime &dateTime)
{
    if (this->calcf != nullptr)
        this->calcf->start_time_changed(dateTime);
}

void atsedit::on_dateTimeEdit_stopsat_dateTimeChanged(const QDateTime &dateTime)
{
    if (this->calcf != nullptr)
        this->calcf->stop_time_changed(dateTime);
}

void atsedit::on_comboBox_sensortype_currentIndexChanged(const QString &arg1)
{
    if (this->allsensors != nullptr)
        this->allsensors->set_name(arg1);

}

void atsedit::on_pushButton_openstreetmap_clicked()
{
    if (this->atsh == nullptr) return;
    long double lat_rad, lon_rad, elev;
    this->coord.coordinates(lat_rad,lon_rad, elev);
    // site_name
    lat_rad = lat_rad / M_PI * 180.;
    lon_rad = lon_rad / M_PI * 180.;
    QUrl url;
    //https://www.j-berkemeier.de/osm_marker.html?lat=52.518493,lon=13.376026,info=Reichstagsgeb%C3%A4ude;lat=52.516195,lon=13.377442,info=Brandenburger%20Tor;lat=52.519981,lon=13.36916,info=Bundeskanzleramt
    url.setUrl("https://www.openstreetmap.org/?mlat=" + QString::number(lat_rad, 'g') + "&mlon=" + QString::number(lon_rad, 'g'));
    //url.setUrl("https://geo-metronix.de/osm_marker.html?lat=" + QString::number(lat_rad, 'g') + ",lon=" + QString::number(lon_rad, 'g') + ",info=" + this->atsh->svalue("site_name"));

    QDesktopServices::openUrl(url);

}

void atsedit::on_pushButton_google_maps_clicked()
{
    if (this->atsh == nullptr) return;
    long double lat_rad, lon_rad, elev;
    this->coord.coordinates(lat_rad,lon_rad, elev);
    lat_rad = lat_rad / M_PI * 180.;
    lon_rad = lon_rad / M_PI * 180.;
    QUrl url;
    // https://www.google.com/maps/search/?api=1&query=47.5951518,-122.3316393
     url.setUrl("https://www.google.com/maps/search/?api=1&query=" +  QString::number(lat_rad, 'g') + "," + QString::number(lon_rad, 'g'));
    QDesktopServices::openUrl(url);

}
