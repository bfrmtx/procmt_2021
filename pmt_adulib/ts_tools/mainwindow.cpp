#include "mainwindow.h"
#include "ui_mainwindow.h"

static const char *prop_chan = "-1";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setLocale(QLocale::c());
  this->ui->centralWidget->acceptDrops();
  this->ui->tabWidget->acceptDrops();
  this->ui->tab_ats2xml->acceptDrops();
  this->ui->ats2xml_listWidget->acceptDrops();
  this->ui->pushButton_create_xml->setDisabled(true);

  for (int i = 0; i < 10; ++i) {
    this->chbtns.append(new QCheckBox(this->ui->tab_ats2xml));
  }
  int i = 0;
  for (auto &pb : this->chbtns) {
    pb->setText(QString::number(i));
    pb->setProperty(prop_chan, QVariant(i++));
    pb->setCheckable(true);
    this->ui->hzl_chans->addWidget(pb);
    connect(pb, &QPushButton::clicked, this, &MainWindow::slot_scan_all_measdocs);
  }

  auto pb_all = new QPushButton(this);
  pb_all->setText("ALL channels");
  connect(pb_all, &QPushButton::clicked, this, &MainWindow::set_all_channels_checked);

  this->ui->hzl_chans->addWidget(pb_all);

  this->ui->dirs_base_lineEdit->set_what(2);
  this->ui->dirs_base_lineEdit->set_check_subdir("/ts");

  this->ui->dirs_target_lineEdit->set_what(0);
  this->ui->dirs_source_lineEdit->set_what(0);

  this->ui->dirs_target_lineEdit->setDisabled(true);
  this->ui->dirs_source_lineEdit->setDisabled(true);

  this->ui->pushButton_create_xml->setDisabled(true);

  on_dirs_base_lineEdit_valid_dir(false);

  //    this->Phoenix_tags << "manufacturer" << "file_type" << "file_version" << "empower_version" << "recording_id" << "instrument_type" << "coords" << "data_units"
  //                       << "sampling_freq" << "start_time" << "stop_time" << "decimation_levels_start" << "sensor_serials" << "dipole_length_m";

  // ------------- ascii2ats
  // std::make_unique<sensors_combo>(this->dbname, QChar(' '), this->ui->comboBox_sensortype);
  //    for (size_t i = 0; i < 5; ++i) this->allsensors.emplace_back();
  QString dbnms = procmt_homepath("info.sql3");
  this->allsensors.emplace_back(std::make_unique<sensors_combo>(dbnms, QChar('E'), this->ui->comboBox_Ex_sensor));
  this->allsensors.back()->set_name("EFP-06");

  this->allsensors.emplace_back(std::make_unique<sensors_combo>(dbnms, QChar('E'), this->ui->comboBox_Ey_sensor));
  this->allsensors.back()->set_name("EFP-06");

  this->allsensors.emplace_back(std::make_unique<sensors_combo>(dbnms, QChar('H'), this->ui->comboBox_Hx_sensor));
  this->allsensors.back()->set_name("MFS-06e");

  this->allsensors.emplace_back(std::make_unique<sensors_combo>(dbnms, QChar('H'), this->ui->comboBox_Hy_sensor));
  this->allsensors.back()->set_name("MFS-06e");

  this->allsensors.emplace_back(std::make_unique<sensors_combo>(dbnms, QChar('H'), this->ui->comboBox_Hz_sensor));
  this->allsensors.back()->set_name("MFS-06e");

  this->ui->dateTimeEdit_starts->setTimeZone(QTimeZone::utc());

  this->fcombo = std::make_unique<frequencies_combo>(dbnms, this->ui->comboBox_sample_freq);
  this->fcombo->set_frequency(512);
  this->fcombo_tscat = std::make_unique<frequencies_combo>(dbnms, this->ui->fcombo_tscat_template);
  this->fcombo_tscat->set_frequency(128);

  this->ui->comboBox_NS_Indicator->addItem(QString("N"));
  this->ui->comboBox_NS_Indicator->addItem(QString("S"));
  this->NS_Indicatior = this->ui->comboBox_NS_Indicator->currentText();

  this->ui->comboBox_EW_Indicator->addItem(QString("E"));
  this->ui->comboBox_EW_Indicator->addItem(QString("W"));
  this->ui->comboBox_EW_Indicator->addItem(QString("0-360"));

  this->EW_Indicatior = this->ui->comboBox_NS_Indicator->currentText();

  this->ui->groupBox_0_sensor->hide();
  this->ui->groupBox_1_sensor->hide();
  this->ui->groupBox_2_sensor->hide();

  this->setup_group_boxes();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::dropEvent(QDropEvent *event) {
  this->clear();

  QList<QUrl> urls = event->mimeData()->urls();
  if (urls.isEmpty()) {
    return;
  }
  QString currentTabName = this->ui->tabWidget->currentWidget()->objectName();
  if (currentTabName == "tab_ats2xml") {
    this->ui->ats2xml_listWidget->clear();
    QList<QFileInfo> allfiles;
    this->mxcds.emplace_back(std::make_unique<mc_data>());
    for (int i = 0; i < urls.size(); ++i) {
      QString strfile = urls.at(i).toLocalFile();
      if (strfile.endsWith(".ats", Qt::CaseInsensitive))
        allfiles.append(QFileInfo(strfile));
    }
    if (allfiles.size()) {
      this->mxcds.back()->open_files(allfiles, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
      for (auto &atsf : this->mxcds.back()->in_atsfiles) {
        this->ui->ats2xml_listWidget->addItem(atsf->fileName());
      }
      this->ui->pushButton_create_xml->setDisabled(false);
    } else {
      this->clear();
    }
  }
  if (currentTabName == "tab_tsdiff") {
    this->ui->tsdiff_listWidget->clear();
    QList<QFileInfo> allfiles;
    for (int i = 0; i < urls.size(); ++i) {
      QString strfile = urls.at(i).toLocalFile();
      if (strfile.endsWith(".ats", Qt::CaseInsensitive))
        allfiles.append(QFileInfo(strfile));
    }

    if (allfiles.size()) {
      if (this->mxcd != nullptr)
        this->mxcd.reset();
      this->mxcd = std::make_unique<mc_data>();
      this->mxcd->open_files(allfiles, true, mttype::nomt, threadctrl::threadctrl_continuous_join);

      size_t ovr_samples = this->mxcd->in_atsfiles.at(0)->get_override_use_samples();
      size_t samples = this->mxcd->in_atsfiles.at(0)->get_num_samples();

      if (!ovr_samples && (samples < this->std_slice))
        this->std_slice = samples;
      else if (ovr_samples < this->std_slice)
        this->std_slice = ovr_samples;

      // n over m this->tsdiffs.resize(allfiles.size());
      this->data.resize(allfiles.size());
      this->fetchcounts.resize(allfiles.size(), 0);
      this->buffer_status.resize(allfiles.size(), 0);

      for (size_t m = 0; m < allfiles.size(); ++m)
        this->in_mutex.emplace_back(new std::mutex());
      auto dat = data.begin();
      for (auto &atsf : this->mxcd->in_atsfiles) {
        this->ui->tsdiff_listWidget->addItem(atsf->fileName());
        // atsf->slot_ts_reader_mode(atsfileout::atsfileout_scaled_timeseries, atsfileout::atsfileout_void);
        // connect(atsf.get(), &atsfile::signal_tsdata, this, &MainWindow::slot_tsdata_subtraction, Qt::DirectConnection);
        atsf->set_window_length(this->std_slice, 0.0, 0.0, true, 0, 0, atsfileout::atsfileout_scaled_timeseries);
        this->tsbuffers.emplace_back(atsf->get_create_buffer(2, atsfileout::atsfileout_scaled_timeseries));
        dat->resize(this->std_slice);
        ++dat;
        // need the last read
        // atsf->set_window_length(this->std_slice, 0.0, 0.0, true);
      }

      size_t i = 0, j = 0, k = 0;
      for (auto &atsfi : this->mxcd->in_atsfiles) {
        for (j = 0; j < this->mxcd->in_atsfiles.size(); ++j) {
          if (j > i) {
            this->mxcd->out_atsfiles.emplace_back(std::make_shared<atsfile>(*atsfi));
            this->mxcd->out_atsfiles.back()->set_run(atsfi->ivalue("run_number") + 100 + k);
            this->tsdiffbuffers.emplace_back(std::make_shared<threadbuffer<double>>(2, this->std_slice, atsfileout::atsfileout_scaled_timeseries, k++));
            this->tsdiffs.emplace_back(std::vector<double>(this->std_slice));
            this->mxcd->out_atsfiles.back()->set_buffer(this->tsdiffbuffers.back());
          }
        }

        i++;
      }
      for (auto &atsfi : this->mxcd->out_atsfiles) {
        if (!atsfi->channel_type().compare("ex", Qt::CaseInsensitive)) {
          atsfi->set_e_pos(-500.0, 500.0, 0.0, 0.0, 0.0, 0.0);
        }
        if (!atsfi->channel_type().compare("ey", Qt::CaseInsensitive)) {
          atsfi->set_e_pos(0.0, 0.0, -500.0, 500.0, 0.0, 0.0);
        }
      }

      connect(this->mxcd->out_atsfiles.back().get(), &atsheader::tx_adu_msg, this, &MainWindow::rx_adu_msg);

    } else {
      this->clear();
    }

    this->ui->tsdiff_listWidget->addItem("--------------------");

    //        for (const auto &ats : this->in_atsfiles) {
    //            //mxcds this->out_atsfiles.emplace_back(std::make_shared<atsfile>(*ats));
    //        }
  }
  // ----------------------------------------------------------------------------------------------------------------
  if (currentTabName == "tab_json") {
    QList<QFileInfo> allfiles;
    for (int i = 0; i < urls.size(); ++i) {
      QString strfile = urls.at(i).toLocalFile();
      if (strfile.contains(".json", Qt::CaseInsensitive))
        allfiles.append(QFileInfo(strfile));
    }
    if (allfiles.size()) {
      for (auto &atsf : this->atsfiles)
        atsf.reset();
      this->atsfiles.clear();
      if (this->edt != nullptr)
        this->edt.reset();
      if (this->phj_system_ts != nullptr)
        this->phj_system_ts.reset();
      this->phoenix_ts.setFile("");
      this->phoenix_cal_coil.setFile("");
      this->phoenix_cal_system.setFile("");
      this->chan_str_mtx.clear();
      this->chan_str_phoenix.clear();
      for (auto &vbr : this->rb_sensor_names)
        vbr.clear();

      this->ui->groupBox_0_sensor->hide();
      this->ui->groupBox_1_sensor->hide();
      this->ui->groupBox_2_sensor->hide();

    } else
      return;

    for (auto &qfi : allfiles) {

      if (qfi.fileName().endsWith("ts.json")) {

        this->phoenix_ts = qfi;
        this->phj_system_ts = std::make_unique<Phoenix_json>();
        this->edt = std::make_unique<eQDateTime>();

        int nerrors;
        bool ok = false;

        ok = phj_system_ts->ts_header_reader(qfi, nerrors);
        this->ui->json_listWidget->addItem("---------------TIMESERIES--------------------------");
        auto vals = phj_system_ts->phoenix_tags.constBegin();
        while (vals != phj_system_ts->phoenix_tags.constEnd()) {
          this->ui->json_listWidget->addItem(vals.key() + ": " + vals.value().toString());
          ++vals;
        }
        if (phj_system_ts->phoenix_tags.contains("start_date_time")) {
          this->edt->set_date_time(phj_system_ts->phoenix_tags.value("start_date_time").toLongLong());
          this->ui->json_listWidget->addItem("Start Time ISO " + this->edt->ISO_date_time());
        } else {
          this->ui->json_listWidget->addItem("File has no start time!! !!");
        }
        this->ui->json_listWidget->addItem("detected ERRORS: " + QString::number(nerrors));
        this->ui->json_listWidget->repaint();
        this->repaint();

        if (ok) {

          geocoordinates coord;
          int msec_lat, msec_lon;
          long double m_elevation;

          // map 150 to 128 for test
          if ((phj_system_ts->phoenix_tags.value("sample_freq").toDouble() > 149) && (phj_system_ts->phoenix_tags.value("sample_freq").toDouble() < 151)) {
            phj_system_ts->phoenix_tags.insert("sample_freq", 128);
          }

          if (phj_system_ts->phoenix_tags.contains("latitude") && phj_system_ts->phoenix_tags.contains("longitude")) {
            coord.set_lat_lon_rad(phj_system_ts->phoenix_tags.value("latitude").toDouble() * M_PI / 180., phj_system_ts->phoenix_tags.value("longitude").toDouble() * M_PI / 180.);
            coord.get_lat_lon_msec(msec_lat, msec_lon, m_elevation);
          }

          int i = 0;
          if (phj_system_ts->phoenix_tags.contains("Ex_dipole_length_m")) {
            if (phj_system_ts->phoenix_tags.value("Ex_dipole_length_m").toDouble() > 0.1) {
              this->atsfiles.emplace_back(std::make_shared<atsfile>(QFileInfo(qfi.baseName()), 1, 80, 1000, phj_system_ts->phoenix_tags.value("sample_freq").toDouble(), 1, i, "Ex", phj_system_ts->phoenix_tags.value("board").toString()));

              chan_str_phoenix.insert(i, "E1");
              chan_str_mtx.insert(i++, "Ex");
              this->atsfiles.back()->dip_to_pos(phj_system_ts->phoenix_tags.value("Ex_dipole_length_m").toDouble(), 0.);
              if (phj_system_ts->phoenix_tags.contains("system_serial_number"))
                this->atsfiles.back()->set_key_value("system_serial_number", phj_system_ts->phoenix_tags.value("system_serial_number").toUInt());
            }
          }
          if (phj_system_ts->phoenix_tags.contains("Ey_dipole_length_m")) {
            if (phj_system_ts->phoenix_tags.value("Ey_dipole_length_m").toDouble() > 0.1) {
              this->atsfiles.emplace_back(std::make_shared<atsfile>(QFileInfo(qfi.baseName()), 1, 80, 1000, phj_system_ts->phoenix_tags.value("sample_freq").toDouble(), 1, i, "Ey", phj_system_ts->phoenix_tags.value("board").toString()));

              chan_str_phoenix.insert(i, "E2");
              chan_str_mtx.insert(i++, "Ey");
              this->atsfiles.back()->dip_to_pos(phj_system_ts->phoenix_tags.value("Ey_dipole_length_m").toDouble(), 90.);
              if (phj_system_ts->phoenix_tags.contains("system_serial_number"))
                this->atsfiles.back()->set_key_value("system_serial_number", phj_system_ts->phoenix_tags.value("system_serial_number").toUInt());
              this->atsfiles.back()->set_key_value("sensor_type", "EFP-06");
            }
          }
          if (phj_system_ts->phoenix_tags.contains("Hx_serial")) {
            this->atsfiles.emplace_back(std::make_shared<atsfile>(QFileInfo(qfi.baseName()), 1, 80, 1000, phj_system_ts->phoenix_tags.value("sample_freq").toDouble(), 1, i, "Hx", phj_system_ts->phoenix_tags.value("board").toString()));
            chan_str_phoenix.insert(i, "H1");
            chan_str_mtx.insert(i++, "Hx");
            this->atsfiles.back()->set_key_value("sensor_sernum", phj_system_ts->phoenix_tags.value("Hx_serial"));
            if (phj_system_ts->phoenix_tags.contains("system_serial_number"))
              this->atsfiles.back()->set_key_value("system_serial_number", phj_system_ts->phoenix_tags.value("system_serial_number").toUInt());
            this->atsfiles.back()->set_key_value("sensor_type", "coil");
          }
          if (phj_system_ts->phoenix_tags.contains("Hy_serial")) {
            this->atsfiles.emplace_back(std::make_shared<atsfile>(QFileInfo(qfi.baseName()), 1, 80, 1000, phj_system_ts->phoenix_tags.value("sample_freq").toDouble(), 1, i, "Hy", phj_system_ts->phoenix_tags.value("board").toString()));
            chan_str_phoenix.insert(i, "H2");
            chan_str_mtx.insert(i++, "Hy");
            this->atsfiles.back()->set_key_value("sensor_sernum", phj_system_ts->phoenix_tags.value("Hy_serial"));
            if (phj_system_ts->phoenix_tags.contains("system_serial_number"))
              this->atsfiles.back()->set_key_value("system_serial_number", phj_system_ts->phoenix_tags.value("system_serial_number").toUInt());
            this->atsfiles.back()->set_key_value("sensor_type", "coil");
          }
          if (phj_system_ts->phoenix_tags.contains("Hz_serial")) {
            this->atsfiles.emplace_back(std::make_shared<atsfile>(QFileInfo(qfi.baseName()), 1, 80, 1000, phj_system_ts->phoenix_tags.value("sample_freq").toDouble(), 1, i, "Hz", phj_system_ts->phoenix_tags.value("board").toString()));
            chan_str_phoenix.insert(i, "H3");
            chan_str_mtx.insert(i++, "Hz");
            this->atsfiles.back()->set_key_value("sensor_sernum", phj_system_ts->phoenix_tags.value("Hz_serial"));
            if (phj_system_ts->phoenix_tags.contains("system_serial_number"))
              this->atsfiles.back()->set_key_value("system_serial_number", phj_system_ts->phoenix_tags.value("system_serial_number").toUInt());
            this->atsfiles.back()->set_key_value("sensor_type", "coil");
          }

          connect(this->phj_system_ts.get(), &Phoenix_json::signal_lines_processed, this, &MainWindow::slot_lines_processed, Qt::DirectConnection);
          this->adu = std::make_unique<adu_json>();
          this->adu->init_channels(this->atsfiles.size());
          i = 0;
          for (auto &atsf : atsfiles) {
            atsf->set_key_value("latitude", msec_lat);
            atsf->set_key_value("longitude", msec_lon);
            atsf->set_key_value("start_date_time", phj_system_ts->phoenix_tags.value("start_date_time").toUInt());
            atsf->set_run(1);
            atsf->prc_com_to_classmembers();
            this->adu->set_channel_value(i, "channel_type", atsf->channel_type());
            this->adu->set_channel_value(i, "sample_freq", atsf->get_sample_freq());
            this->adu->set_channel_value(i, "sensor_sernum", atsf->ivalue("sensor_sernum"));

            atsf->set_basedir_fake_deep(qfi.absolutePath());

            ++i;
          }
          this->edt->set_sample_freq(this->atsfiles.at(0)->get_sample_freq());
          this->edt->set_num_samples(this->atsfiles.at(0)->get_num_samples());
          this->adu->set_system_value("system_serial_number", phj_system_ts->phoenix_tags.value("system_serial_number").toUInt());
          this->adu->set_system_value("Latitude", msec_lat);
          this->adu->set_system_value("Longitude", msec_lon);
          this->adu->set_system_value("start_time", this->edt->toString("hh:mm:ss"));
          this->adu->set_system_value("start_date", this->edt->toString("hh:mm:ss"));
          this->adu->set_system_value("stop_time", this->atsfiles.at(0)->svalue("stop_time"));
          this->adu->set_system_value("stop_date", this->atsfiles.at(0)->svalue("stop_date"));
        }
      } // ok ts

      if (qfi.fileName().endsWith("scal.json")) {
        this->phj_coils.emplace_back(std::make_shared<Phoenix_json>());
        auto phj_coil = this->phj_coils.back();
        bool ok = false;

        ok = phj_coil->from_JSON_scal(&qfi);
        this->ui->json_listWidget->addItem("-----------------CALIBRATION COIL--------------------");
        auto vals = phj_coil->phoenix_tags.constBegin();
        while (vals != phj_coil->phoenix_tags.constEnd()) {
          this->ui->json_listWidget->addItem(vals.key() + ": " + vals.value().toString());
          ++vals;
        }

        this->ui->json_listWidget->repaint();
        this->repaint();

        if (!ok)
          this->phj_coils.pop_back();

        if (this->phj_coils.size() == 1) {
          this->v_rb_sensor_serials[0]->setText("serial: " + phj_coil->phoenix_tags.value("sensor_serial").toString());
          connect(this->rbl_0, &radiobutton_list::btn_name_checked, phj_coil.get(), &Phoenix_json::slot_sensortype_changed);
          this->ui->groupBox_0_sensor->show();
        }
        if (this->phj_coils.size() == 2) {
          this->v_rb_sensor_serials[1]->setText("serial: " + phj_coil->phoenix_tags.value("sensor_serial").toString());
          connect(this->rbl_1, &radiobutton_list::btn_name_checked, phj_coil.get(), &Phoenix_json::slot_sensortype_changed);
          this->ui->groupBox_1_sensor->show();
        }
        if (this->phj_coils.size() == 3) {
          this->v_rb_sensor_serials[2]->setText("serial: " + phj_coil->phoenix_tags.value("sensor_serial").toString());
          connect(this->rbl_2, &radiobutton_list::btn_name_checked, phj_coil.get(), &Phoenix_json::slot_sensortype_changed);
          this->ui->groupBox_2_sensor->show();
        }
        this->repaint();

      } // scal

      if (qfi.fileName().endsWith("rxcal.json")) {
        this->phj_system = std::make_unique<Phoenix_json>();
        bool ok = false;

        ok = this->phj_system->from_JSON_rxcal(&qfi);
        this->ui->json_listWidget->addItem("-----------------CALIBRATION SYSTEM--------------------");
        auto vals = phj_system->phoenix_tags.constBegin();
        while (vals != phj_system->phoenix_tags.constEnd()) {
          this->ui->json_listWidget->addItem(vals.key() + ": " + vals.value().toString());
          ++vals;
        }

        this->ui->json_listWidget->repaint();
        this->repaint();

        if (!ok)
          this->phj_system.reset();

      } // rxcal
    } // allfiles

    //        // system time series & coils are there
    //        if (phj_coils.size() && phj_system_ts != nullptr) {
    //            for (auto &phj_coil : this->phj_coils) {
    //                connect(phj_coil.get(), &Phoenix_json::signal_sensor_changed, phj_system_ts.get(), &adu_json::slot_sensortype_of_serial_changed );

    //            }
    //        }

  } // tab_json

  if (currentTabName == "tab_ascii2ats") {

    QList<QFileInfo> allfiles;
    for (int i = 0; i < urls.size(); ++i) {
      QString strfile = urls.at(i).toLocalFile();
      allfiles.append(QFileInfo(strfile));
    }
    for (auto &a : this->atsfiles)
      a->close();
    this->atsfiles.clear();
    QString chan_name;
    QString board = "LF";
    int chan_no = -1;

    double freq = 0;
    bool ok = false;

    freq = this->ui->lineEdit_sample_freq->text().toDouble(&ok);
    if (!ok || freq <= 0.000)
      freq = this->fcombo->get_frequency();

    if (freq > 4096)
      board = "HF";

    for (auto &qfi : allfiles) {

      chan_no = 100;
      bool has_sfreq = false;

      QString sfreq;
      QString bnam = qfi.completeBaseName();
      for (int s = 0; s < bnam.size(); ++s) {
        if (bnam.at(s).isDigit())
          sfreq.append(bnam.at(s));
        if (sfreq.size() && (bnam.at(s).toLower() == QChar('h')))
          sfreq.append(QChar('H'));
        if (sfreq.size() && (bnam.at(s).toLower() == QChar('s')))
          sfreq.append(QChar('s'));

        if ((sfreq.size() > 1) && (sfreq.endsWith(QChar('H')) || sfreq.endsWith(QChar('s')))) {
          has_sfreq = true;
          break;
        } else if (!bnam.at(s).isDigit())
          sfreq.clear();
      }

      QString tmp_name = qfi.completeBaseName();
      if (has_sfreq) {
        tmp_name = qfi.completeBaseName().remove(sfreq, Qt::CaseInsensitive);
        bool isok = false;
        QString numstring;
        if (sfreq.endsWith(QChar('H'))) {
          sfreq = sfreq.remove(QChar('H'));
          double dfreq = sfreq.toDouble(&isok);
          if (isok)
            freq = dfreq;
          double success = this->fcombo->set_frequency(freq);
          if (success == -1)
            this->ui->lineEdit_sample_freq->setText(numstring.setNum(freq));
          else
            this->ui->lineEdit_sample_freq->setText("0");
        }
        if (sfreq.endsWith(QChar('s'))) {
          sfreq = sfreq.remove(QChar('s'));
          double dfreq = sfreq.toDouble(&isok);
          if (isok)
            freq = 1.0 / dfreq;
          double success = this->fcombo->set_frequency(freq);
          if (success == -1)
            this->ui->lineEdit_sample_freq->setText(numstring.setNum(freq));
          else
            this->ui->lineEdit_sample_freq->setText("0");
        }
      }

      qDebug() << " ---------- sfreq :" << sfreq;

      if ((tmp_name.contains("Bx", Qt::CaseInsensitive)) || (tmp_name.contains("Hx", Qt::CaseInsensitive))) {
        chan_name = "Hx";
        chan_no = 2;
        if (freq < 513)
          this->ui->checkBox_chopperHx->setChecked(true);
        else
          this->ui->checkBox_chopperHx->setChecked(false);

      } else if ((tmp_name.contains("By", Qt::CaseInsensitive)) || (tmp_name.contains("Hy", Qt::CaseInsensitive))) {
        chan_name = "Hy";
        chan_no = 3;
        if (freq < 513)
          this->ui->checkBox_chopperHy->setChecked(true);
        else
          this->ui->checkBox_chopperHy->setChecked(false);

      }

      else if (tmp_name.contains("Ex", Qt::CaseInsensitive)) {
        chan_name = "Ex";
        chan_no = 0;
        if (freq < 513)
          this->ui->checkBox_chopperEx->setChecked(true);
        else
          this->ui->checkBox_chopperEx->setChecked(false);

      }

      else if (tmp_name.contains("Ey", Qt::CaseInsensitive)) {
        chan_name = "Ey";
        chan_no = 1;
        if (freq < 513)
          this->ui->checkBox_chopperEy->setChecked(true);
        else
          this->ui->checkBox_chopperEy->setChecked(false);

      }

      else if ((tmp_name.contains("Bz", Qt::CaseInsensitive)) || (tmp_name.contains("Hz", Qt::CaseInsensitive))) {
        chan_name = "Hz";
        chan_no = 4;
        if (freq < 513)
          this->ui->checkBox_chopperHz->setChecked(true);
        else
          this->ui->checkBox_chopperHz->setChecked(false);
      }

      if (chan_no < 100) {

        QDir dh(QDir::home());
        QFileInfo qfiukn(dh.absolutePath() + "/test.ats");
        auto xats = std::make_shared<atsfile>(qfiukn, 1, 80, 999, freq, 1, chan_no, chan_name, board);
        xats->set_start_date_time(this->ui->dateTimeEdit_starts->dateTime());
        QFileInfo qfiats(xats->get_new_filename());

        this->atsfiles.emplace_back(std::make_shared<atsfile>(qfiats, 1, 80, 999, freq, 1, chan_no, chan_name, board));
      }

      if (this->atsfiles.size() == allfiles.size()) {
        this->inats2ascii_files = allfiles;
      }
    }

  } // tab_ascii2ats

  if (currentTabName == "tab_tscat") {

    if (urls.isEmpty())
      return;

    QList<QDir> qdirs;

    for (int i = 0; i < urls.size(); ++i) {
      QFileInfo qfi(urls.at(i).toLocalFile());
      if (qfi.isDir()) {
        qdirs.append(QDir(qfi.absoluteFilePath()));
      }
    }

    qDebug() << qdirs;

    if (qdirs.isEmpty())
      return;

    size_t ii = 0;
    for (const auto &dir : qdirs) {
      QFileInfoList qfilist = dir.entryInfoList();
      qDebug() << qfilist;

      for (const auto &qfi : qfilist) {
        if (qfi.fileName().endsWith("ats", Qt::CaseInsensitive) && qfi.fileName().contains("_TEx", Qt::CaseInsensitive)) {
          if (qfi.fileName().contains(this->ats_file_search_str))
            this->atsex.emplace_back(std::make_shared<atsfile>(qfi, ii++, short(0), this));
        }
        if (qfi.fileName().endsWith("ats", Qt::CaseInsensitive) && qfi.fileName().contains("_TEy", Qt::CaseInsensitive)) {
          if (qfi.fileName().contains(this->ats_file_search_str))
            this->atsey.emplace_back(std::make_shared<atsfile>(qfi, ii++, short(0), this));
        }
        if (qfi.fileName().endsWith("ats", Qt::CaseInsensitive) && qfi.fileName().contains("_THx", Qt::CaseInsensitive)) {
          if (qfi.fileName().contains(this->ats_file_search_str))
            this->atshx.emplace_back(std::make_shared<atsfile>(qfi, ii++, short(0), this));
        }
        if (qfi.fileName().endsWith("ats", Qt::CaseInsensitive) && qfi.fileName().contains("_THy", Qt::CaseInsensitive)) {
          if (qfi.fileName().contains(this->ats_file_search_str))
            this->atshy.emplace_back(std::make_shared<atsfile>(qfi, ii++, short(0), this));
        }
        if (qfi.fileName().endsWith("ats", Qt::CaseInsensitive) && qfi.fileName().contains("_THz", Qt::CaseInsensitive)) {
          if (qfi.fileName().contains(this->ats_file_search_str))
            this->atshz.emplace_back(std::make_shared<atsfile>(qfi, ii++, short(0), this));
        }
      }
    }

    this->scan_tscat_headers();
    this->sort_tscat_by_start_time();
    // get the fist xml file from ats
    std::shared_ptr<atsheader> atsh;
    if (atsex.size())
      atsh = std::make_shared<atsheader>(this->atsex.begin()->get()->get_qfile_info());
    else if (atsey.size())
      atsh = std::make_shared<atsheader>(this->atsey.begin()->get()->get_qfile_info());
    else if (atshx.size())
      atsh = std::make_shared<atsheader>(this->atshx.begin()->get()->get_qfile_info());
    else if (atshy.size())
      atsh = std::make_shared<atsheader>(this->atshy.begin()->get()->get_qfile_info());
    else if (atshz.size())
      atsh = std::make_shared<atsheader>(this->atshz.begin()->get()->get_qfile_info());

    if (atsh) {
      atsh->scan_header_close();
      this->xml_filename_cat = atsh->svalue("xml_header");
    }

    // check for overlap ... maybe sort out?
    this->display_start_stop_tscat();
    this->check_for_overlap_tscat();

    this->regroup_tscat_atsfiles();

  } // tab_tscat
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  event->acceptProposedAction();
}

void MainWindow::slot_lines_processed(QString nlines_proc) {
  this->ui->statusBar->showMessage("lines processed: " + nlines_proc);
  this->repaint();
}

void MainWindow::clear(const bool clear_mxcds) {
  this->ui->ats2xml_listWidget->clear();
  if (clear_mxcds) {
    for (auto &mxcd : this->mxcds)
      if (mxcd != nullptr)
        mxcd.reset();
    this->mxcds.clear();
  }
  if (this->mxcd != nullptr)
    this->mxcd.reset();
  if (this->adujob != nullptr)
    this->adujob.reset();

  if (this->qfiHWConfig != nullptr) {
    delete this->qfiHWConfig;
    this->qfiHWConfig = nullptr;
  }
  if (this->qrlHWConfig != nullptr) {
    delete this->qrlHWConfig;
    this->qrlHWConfig = nullptr;
  }
  if (this->qbaHWConfig != nullptr) {
    delete this->qbaHWConfig;
    this->qbaHWConfig = nullptr;
  }

  if (this->qfiHwDatabase != nullptr) {
    delete this->qfiHwDatabase;
    this->qfiHwDatabase = nullptr;
  }

  if (this->qrlHwDatabase != nullptr) {
    delete this->qrlHwDatabase;
    this->qrlHwDatabase = nullptr;
  }
  if (this->qbaHwDatabase != nullptr) {
    delete this->qbaHwDatabase;
    this->qbaHwDatabase = nullptr;
  }

  if (this->qfiChannelConfig != nullptr) {
    delete this->qfiChannelConfig;
    this->qfiChannelConfig = nullptr;
  }
  if (this->qrlChannelConfig != nullptr) {
    delete this->qrlChannelConfig;
    this->qrlChannelConfig = nullptr;
  }
  if (this->qbaChannelConfig != nullptr) {
    delete this->qbaChannelConfig;
    this->qbaChannelConfig = nullptr;
  }

  this->tsdiffs.clear();
  for (auto &thrd : this->readerthreads)
    thrd.join();
  for (auto &thrd : this->writerthreads)
    thrd.join();
  for (auto &thrd : this->fetcherthreads)
    thrd.join();

  this->readerthreads.clear();
  this->writerthreads.clear();
  this->fetcherthreads.clear();
  this->can_run = 0;
  this->buffer_status.clear();
  this->fetchcounts.clear();
  this->tsdiffs.clear();
  this->data.clear();

  for (auto &mtx : this->in_mutex)
    mtx->unlock();
  for (auto &mtx : this->in_mutex)
    delete mtx;
  ;
  this->in_mutex.clear();
}

void MainWindow::prepare() {
  this->xml_cnf_basedir = procmt_homepath("5chanEHnoise") + "/";
  this->HWConfig = this->xml_cnf_basedir + "HwConfig.xml";
  this->HwDatabase = this->xml_cnf_basedir + "HwDatabase.xml";
  this->HwStatus = this->xml_cnf_basedir + "HwStatus.xml";
  this->ChannelConfig = this->xml_cnf_basedir + "ChannelConfig.xml";

  this->qfiHWConfig = new QFileInfo(HWConfig);
  this->qfiHwDatabase = new QFileInfo(HwDatabase);
  this->qfiChannelConfig = new QFileInfo(ChannelConfig);
  this->qfiHwStatus = new QFileInfo(HwStatus);
}

void MainWindow::close_tscat_channels(bool iscancelled) {
  for (auto atsf : this->atsex) {
    atsf->close();
    atsf.reset();
  }
  this->atsex.clear();

  for (auto atsf : this->atsey) {
    atsf->close();
    atsf.reset();
  }
  this->atsey.clear();

  for (auto &atsf : this->atshx) {
    atsf->close();
    atsf.reset();
  }
  this->atshx.clear();

  for (auto &atsf : this->atshy) {
    atsf->close();
    atsf.reset();
  }
  this->atshy.clear();

  for (auto atsf : this->atshz) {
    atsf->close();
    atsf.reset();
  }
  this->atshz.clear();

  if (!iscancelled) {
    if (this->atsex_cat != nullptr)
      this->atsex_cat->close(true);
    if (this->atsey_cat != nullptr)
      this->atsey_cat->close(true);
    if (this->atshx_cat != nullptr)
      this->atshx_cat->close(true);
    if (this->atshy_cat != nullptr)
      this->atshy_cat->close(true);
    if (this->atshz_cat != nullptr)
      this->atshz_cat->close(true);
  }

  if (this->atsex_cat != nullptr)
    this->atsex_cat.reset();
  if (this->atsey_cat != nullptr)
    this->atsey_cat.reset();
  if (this->atshx_cat != nullptr)
    this->atshx_cat.reset();
  if (this->atshy_cat != nullptr)
    this->atshy_cat.reset();
  if (this->atshz_cat != nullptr)
    this->atshz_cat.reset();

  this->ui->listWidget_tscat->clear();

  this->tscat_continue = true;
}

void MainWindow::fetch_tsdiff_reader(const size_t slot) {
  do {
    this->in_mutex[slot]->lock();
    this->fetchcounts[slot]++;
#ifdef QT_DEBUG
    if (!data.at(slot).size()) {

      qDebug() << "------------------PRE----------------------------------------------------------";
    }
#endif
    tsbuffers[slot]->fetch(this->data[slot], this->buffer_status[slot]);

#ifdef QT_DEBUG
    std::cerr << "fetch_tsdiff_reader[" << slot << "] getting " << data.at(slot).size() << " " << std::endl;
    if (!data.at(slot).size()) {
      qDebug() << "------------------POST----------------------------------------------------------";
    }
#endif
    this->tsdata_subtraction(slot);

  } while (this->buffer_status[slot] == threadbuffer_status::running);

  std::cerr << "fetch_tsdiff_reader[" << slot << "] finished" << std::endl;
}

void MainWindow::sort_tscat_by_start_time() {
  std::sort(this->atsex.begin(), this->atsex.end(), compstartime_ats_lhs_lt_sp);
  std::sort(this->atsey.begin(), this->atsey.end(), compstartime_ats_lhs_lt_sp);
  std::sort(this->atshx.begin(), this->atshx.end(), compstartime_ats_lhs_lt_sp);
  std::sort(this->atshy.begin(), this->atshy.end(), compstartime_ats_lhs_lt_sp);
  std::sort(this->atshz.begin(), this->atshz.end(), compstartime_ats_lhs_lt_sp);
}

void MainWindow::scan_tscat_headers() {
  for (auto &ats : this->atsex)
    ats->scan_header_close();
  for (auto &ats : this->atsey)
    ats->scan_header_close();
  for (auto &ats : this->atshx)
    ats->scan_header_close();
  for (auto &ats : this->atshy)
    ats->scan_header_close();
  for (auto &ats : this->atshz)
    ats->scan_header_close();
}

void MainWindow::display_start_stop_tscat() {
  QList<eQDateTime> starts;
  QList<eQDateTime> stops;

  this->ui->listWidget_tscat->clear();

  if (this->atsex.size()) {
    for (auto const &ats : this->atsex) {
      starts.append(ats->get_start_datetime());
      stops.append(ats->get_stop_datetime());
    }
  } else if (this->atsey.size()) {
    for (auto const &ats : this->atsey) {
      starts.append(ats->get_start_datetime());
      stops.append(ats->get_stop_datetime());
    }
  } else if (this->atshx.size()) {
    for (auto const &ats : this->atshx) {
      starts.append(ats->get_start_datetime());
      stops.append(ats->get_stop_datetime());
    }
  } else if (this->atshy.size()) {
    for (auto const &ats : this->atshy) {
      starts.append(ats->get_start_datetime());
      stops.append(ats->get_stop_datetime());
    }
  } else if (this->atshz.size()) {
    for (auto const &ats : this->atshz) {
      starts.append(ats->get_start_datetime());
      stops.append(ats->get_stop_datetime());
    }
  }

  if (starts.size()) {
    if (starts.size() == stops.size()) {
      for (int i = 0; i < starts.size(); ++i) {
        QString display;

        display = starts.at(i).ISO_date_time() + "  <-->  " + stops.at(i).ISO_date_time();
        qDebug() << display;
        this->ui->listWidget_tscat->addItem(display);
      }
    }
  } else
    this->tscat_continue = false;

  this->ui->listWidget_tscat->show();
}

void MainWindow::check_for_overlap_tscat() {
  if (this->atsex.size()) {
    for (int i = 0; i < this->atsex.size() - 1; ++i) {
      if (delta_stop_start_sp(atsex.at(i), atsex.at(i + 1)) < 0)
        this->tscat_continue = false;
      else
        this->tscat_continue = true;
    }
  }
  if (this->atsey.size()) {
    for (size_t i = 0; i < this->atsey.size() - 1; ++i) {
      if (delta_stop_start_sp(atsey.at(i), atsey.at(i + 1)) < 0)
        this->tscat_continue = false;
      else
        this->tscat_continue = true;
    }
  }
  if (this->atshx.size()) {
    for (size_t i = 0; i < this->atshx.size() - 1; ++i) {
      if (delta_stop_start_sp(atshx.at(i), atshx.at(i + 1)) < 0)
        this->tscat_continue = false;
      else
        this->tscat_continue = true;
    }
  }
  if (this->atshy.size()) {
    for (size_t i = 0; i < this->atshy.size() - 1; ++i) {
      if (delta_stop_start_sp(atshy.at(i), atshy.at(i + 1)) < 0)
        this->tscat_continue = false;
      else
        this->tscat_continue = true;
    }
  }
  if (this->atshz.size()) {
    for (size_t i = 0; i < this->atshz.size() - 1; ++i) {
      if (delta_stop_start_sp(atshz.at(i), atshz.at(i + 1)) < 0)
        this->tscat_continue = false;
      else
        this->tscat_continue = true;
    }
  }

  if (!this->tscat_continue) {

    QMessageBox msgBox;
    msgBox.setText("start <-> stop time mismatch");
    msgBox.exec();
    this->close_tscat_channels(true);
  }
}

void MainWindow::regroup_tscat_atsfiles() {
  if (!this->tscat_continue)
    return;

  if (atsex.size())
    atsex_cat = std::make_shared<atsfile>(*atsex.at(0));
  if (atsey.size())
    atsey_cat = std::make_shared<atsfile>(*atsey.at(0));
  if (atshx.size())
    atshx_cat = std::make_shared<atsfile>(*atshx.at(0));
  if (atshy.size())
    atshy_cat = std::make_shared<atsfile>(*atshy.at(0));
  if (atshz.size())
    atshz_cat = std::make_shared<atsfile>(*atshz.at(0));

  if (atsex_cat != nullptr) {
    int rmax = 900, i = 0;
    do {
      QFileInfo qfir(atsex_cat->absoluteFilePath());
      if (qfir.exists())
        atsex_cat->set_run(-1);
      else
        break;
    } while (i < rmax);
  }

  if (atsey_cat != nullptr) {
    int rmax = 900, i = 0;
    do {
      QFileInfo qfir(atsey_cat->absoluteFilePath());
      if (qfir.exists())
        atsey_cat->set_run(-1);
      else
        break;
    } while (i < rmax);
  }
  if (atshx_cat != nullptr) {
    int rmax = 900, i = 0;
    do {
      QFileInfo qfir(atshx_cat->absoluteFilePath());
      if (qfir.exists())
        atshx_cat->set_run(-1);
      else
        break;
    } while (i < rmax);
  }
  if (atshy_cat != nullptr) {
    int rmax = 900, i = 0;
    do {
      QFileInfo qfir(atshy_cat->absoluteFilePath());
      if (qfir.exists())
        atshy_cat->set_run(-1);
      else
        break;
    } while (i < rmax);
  }
  if (atshz_cat != nullptr) {
    int rmax = 900, i = 0;
    do {
      QFileInfo qfir(atshz_cat->absoluteFilePath());
      if (qfir.exists())
        atshz_cat->set_run(-1);
      else
        break;
    } while (i < rmax);
  }

  // check for run mismatch

  if (atsex_cat->ivalue("run_number") != atsey_cat->ivalue("run_number"))
    this->tscat_continue = false;
  if (atsex_cat->ivalue("run_number") != atshx_cat->ivalue("run_number"))
    this->tscat_continue = false;
  if (atsex_cat->ivalue("run_number") != atshy_cat->ivalue("run_number"))
    this->tscat_continue = false;
  if (atsex_cat->ivalue("run_number") != atshz_cat->ivalue("run_number"))
    this->tscat_continue = false;

  if (!this->tscat_continue)
    this->close_tscat_channels(true);
}

void MainWindow::setup_group_boxes() {

  rb_sensor_names << "MTC-50H" << "AMTC-30" << "MTC-80H";
  for (size_t i = 0; i < max_h_chan; ++i) {
    this->v_rb_sensor_names.emplace_back(std::vector<QRadioButton *>());
  }

  this->rbl_0 = new radiobutton_list(this);
  this->rbl_1 = new radiobutton_list(this);
  this->rbl_2 = new radiobutton_list(this);
  int i = 0;
  for (auto &v : v_rb_sensor_names) {
    for (int j = 0; j < this->rb_sensor_names.size(); ++j) {
      v.emplace_back(new QRadioButton(this->rb_sensor_names.at(j)));
      if (i == 0) {
        this->ui->groupBox_0_sensor->layout()->addWidget(v.back());
        this->rbl_0->append(v.back());
      } else if (i == 1) {
        this->ui->groupBox_1_sensor->layout()->addWidget(v.back());
        this->rbl_1->append(v.back());

      } else if (i == 2) {
        this->ui->groupBox_2_sensor->layout()->addWidget(v.back());
        this->rbl_2->append(v.back());
      }
    }

    ++i;
  }

  for (size_t i = 0; i < max_h_chan; ++i) {
    v_rb_sensor_serials.emplace_back(new QLabel("serial: "));
    if (i == 0) {
      this->ui->groupBox_0_sensor->layout()->addWidget(v_rb_sensor_serials.back());
    } else if (i == 1) {
      this->ui->groupBox_1_sensor->layout()->addWidget(v_rb_sensor_serials.back());

    } else if (i == 2) {
      this->ui->groupBox_2_sensor->layout()->addWidget(v_rb_sensor_serials.back());
    }
  }
}

void MainWindow::tsdata_subtraction(const size_t &slot) {
  std::lock_guard<std::mutex> lock(this->mtx);
  qDebug() << "slot: " << slot << "size: " << data[slot].size();
  if (!data[slot].size()) {
    qDebug() << "lost ";
  }

  this->can_run++;
  if (can_run == this->in_mutex.size()) {
    qDebug() << "runs";
    for (auto &v : this->fetchcounts)
      std::cerr << v << " ";
    size_t i = 0, j = 0, k = 0;
    for (const auto &atsfi : this->data) {
      for (const auto &atsfj : this->data) {
        if (j > i) {
          this->tsdiffs[k].resize(atsfi.size());
          std::transform(atsfi.begin(), atsfi.end(), atsfj.begin(), this->tsdiffs[k].begin(), std::minus<double>());
          auto bg = atsfi.cbegin();
          if (this->ui->diff_5percent->isChecked()) {
            for (auto &d : tsdiffs[k]) {
              if ((*bg != 0.0) && (d != 0)) {
                if ((d / *bg) < 0.05)
                  d = 0.0;
              }
              ++bg;
            }
          }
          this->tsdiffbuffers[k]->deposit(this->tsdiffs[k], this->buffer_status[0]);
          k++;
        }
        j++;
      }
      j = 0;
      i++;
    }
#ifdef QT_DEBUG
    for (const auto dat : this->data) {
      if (!dat.size()) {
        qDebug() << " hang------------------------------------------------------";
      }
    }
#endif
    // std::cerr << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    qDebug() << "runs finished \n\n\n\n";
    this->can_run = 0;
    for (auto &mtx : this->in_mutex)
      mtx->unlock();

    if ((this->buffer_status[0] == threadbuffer_status::finished) || (this->buffer_status[0] == threadbuffer_status::terminated)) {

      for (auto &v : this->readerthreads) {
        v.join();
      }

      for (auto &v : this->writerthreads) {
        v.join();
      }

      this->mxcd->close_infiles();
      this->mxcd->close_outfiles();

      this->readerthreads.clear();
      this->writerthreads.clear();
      this->mxcd.reset();
      this->can_run = 0;
      this->buffer_status.clear();
      this->fetchcounts.clear();
      this->tsdiffs.clear();
      this->data.clear();
      for (auto &buf : this->tsbuffers)
        buf.reset();
      for (auto &buf : this->tsdiffbuffers)
        buf.reset();
      this->tsbuffers.clear();
      this->tsdiffbuffers.clear();
    }
  }
}

void MainWindow::rx_adu_msg(const int &system_serial_number, const int &channel, const int &slot, const QString &message) {
  if (message.contains("atsfile::write finsihed")) {
    this->ui->pushButton_tsdiff->setEnabled(true);
  }
}

void MainWindow::slot_scan_all_measdocs() {
  if (!this->qfi_base.absoluteFilePath().size())
    return;
  if (!this->qfi_source.absoluteFilePath().size())
    return;

  int all = 0;
  for (auto &btn : this->chbtns) {
    if (btn->isChecked()) {
      ++all;
    }
  }
  if (!all)
    return;

  for (auto &mxcd : this->mxcds)
    if (mxcd != nullptr)
      mxcd.reset();
  this->mxcds.clear();
  this->ui->ats2xml_listWidget->clear();

  QList<QList<QFileInfo>> all_atsfiles;

  if (this->ui->checkBox_raw_import->isChecked()) {
    auto all_atsfiles_in = ats_in_site(this->qfi_base.absoluteFilePath(), this->qfi_source.baseName());
    for (const auto &glist : all_atsfiles_in) {
      std::unique_ptr<group_atsfiles> groupi = std::make_unique<group_atsfiles>(glist);
      auto sublist = groupi->same_recordings();
      for (const auto &subl : sublist) {
        all_atsfiles.append(subl);
      }
    }

    if (!all_atsfiles.size()) {
      return;
    }
    for (auto &allfiles : all_atsfiles) {
      QList<QFileInfo> files;
      this->mxcds.emplace_back(std::make_unique<mc_data>());
      for (auto &fnam : allfiles) {
        auto map = atsfilename_values(fnam.completeBaseName());
        for (auto &btn : this->chbtns) {
          if (btn->isChecked()) {
            int prp = btn->property(prop_chan).toInt();
            if (prp == map.value("channel").toInt())
              files.append(fnam);
          }
        }
      }
      if (files.size()) {
        this->mxcds.back()->open_files(files, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
      } else {
        this->mxcds.pop_back();
      }
    }
    for (auto &mxcd : this->mxcds) {
      for (auto &atsf : mxcd->in_atsfiles) {
        this->ui->ats2xml_listWidget->addItem(atsf->fileName());
      }
    }

  } else {
    this->allmeasdocs_source = xmls_in_site(this->qfi_base.absoluteFilePath(), this->qfi_source.baseName());
    QFileInfo qfi;
    for (auto &str : this->allmeasdocs_source) {
      qfi.setFile(str);
      if (qfi.exists()) {
        this->mxcds.emplace_back(std::make_unique<mc_data>());
        std::unique_ptr<measdocxml> measdoc = std::make_unique<measdocxml>(&qfi);
        QStringList allfiles;
        QList<QFileInfo> files;
        QList<int> channels;
        allfiles = measdoc->get_atsfiles(channels);
        for (auto &fnam : allfiles) {
          auto map = atsfilename_values(fnam);
          for (auto &btn : this->chbtns) {
            if (btn->isChecked()) {
              int prp = btn->property(prop_chan).toInt();
              if (prp == map.value("channel").toInt())
                files.append(QFileInfo(qfi.absolutePath() + "/" + fnam));
            }
          }
        }

        if (files.size()) {
          this->mxcds.back()->open_files(files, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
        } else {
          this->mxcds.pop_back();
        }
      }
    }
    for (auto &mxcd : this->mxcds) {
      for (auto &atsf : mxcd->in_atsfiles) {
        this->ui->ats2xml_listWidget->addItem(atsf->fileName());
      }
    }
  }
}

void MainWindow::set_all_channels_checked() {
  for (auto &btn : this->chbtns) {
    btn->setChecked(true);
  }
  this->slot_scan_all_measdocs();
}

void MainWindow::on_pushButton_create_xml_clicked() {
  if (!this->mxcds.size())
    return;
  if (!this->qfi_target.exists())
    return;

  for (auto &mxcd : this->mxcds) {
    if (!mxcd->in_atsfiles.size())
      return;
    this->prepare();

    // create the target meas subdir
    QDir dir;
    QList<QFileInfo> qfis;
    eQDateTime starts = mxcd->in_atsfiles.at(0)->get_start_datetime();
    QFileInfo exdir(this->qfi_target.absoluteFilePath() + "/" + starts.measdir_time());
    if (!exdir.exists()) {
      if (!dir.mkdir(this->qfi_target.absoluteFilePath() + "/" + starts.measdir_time())) {
        this->ui->ats2xml_listWidget->addItem("can not create meas dir");
        return;
      }
      dir.setCurrent(dir.absolutePath() + "/" + starts.measdir_time());
    }

    this->adujob = std::make_unique<adulib>(1, this);

    int max_chan = 10;
    std::vector<int> used_channels;
    // load an empty 10 channel adu with any sample frequency
    this->adujob->fetch_hwcfg_virtual(max_chan, mxcd->in_atsfiles.at(0)->get_sample_freq());
    this->adujob->fetch_hwdb(qfiHwDatabase);
    // this->adujob->build_channels();
    this->adujob->build_hardware();
    this->adujob->set_start_time(starts);
    this->adujob->on_set_duration(qint64((double(mxcd->in_atsfiles.at(0)->get_num_samples())) / mxcd->in_atsfiles.at(0)->get_sample_freq()), false);

    QFileInfo measdocxml_file;

    for (auto &ats : mxcd->in_atsfiles) {
      ats->set_ats_option("other_calibration_db_absolute_filepath", this->basedir.absolutePath() + "/cal/calibration.sql3");
      ats->read_header();
      if (ats->value("channel_number").toInt() < max_chan) {
        qfis.append(QFileInfo(ats->absoluteFilePath()));
        QMap<QString, QVariant> sec = ats->get_data();
        auto sec_iter = sec.cbegin();
        while (sec_iter != sec.cend()) {
          this->adujob->set(sec_iter.key(), sec_iter.value(), ats->value("channel_number").toInt(), false);
          sec_iter++;
        }
        used_channels.push_back(ats->value("channel_number").toInt());
        this->adujob->set("Latitude", ats->value("latitude"), ats->value("channel_number").toInt(), false);
        this->adujob->set("Longitude", ats->value("longitude"), ats->value("channel_number").toInt(), false);
        this->adujob->set("Height", ats->value("elevation"), ats->value("channel_number").toInt(), false);
        this->adujob->set("chopper_status", ats->value("chopper_status"), ats->value("channel_number").toInt(), false);
        this->adujob->set("ats_data_file", ats->get_qfile_info().fileName(), ats->value("channel_number").toInt(), false);
        this->adujob->set("ts_lsb", ats->value("lsb"), ats->value("channel_number").toInt(), false);
        // if (ats->value("xml_header").toString().size()) measdocxml_file.setFile(ats->value("xml_header").toString().simplified());
        ats->get_new_filename(false);
      }
    }
    measdocxml_file.setFile(mxcd->in_atsfiles.at(0)->subname_xml());
    measdocxml_file.setFile(mxcd->in_atsfiles.at(0)->absolutePath() + "/" + measdocxml_file.fileName());
    if (measdocxml_file.exists()) {
      for (auto &ich : used_channels) {
        for (auto &ats : mxcd->in_atsfiles) {
          if (ats->value("channel_number").toInt() == ich) {
            QString cal(ats->get_xmlcal());
            if (cal != "empty")
              this->adujob->set("xmlcal_sensor", QVariant(cal), ich, false);
          }
        }
      }
      if (this->measdoc != nullptr)
        this->measdoc.reset();
      this->measdoc = std::make_shared<measdocxml>(&measdocxml_file);
      connect(this->measdoc.get(), &measdocxml::signal_gps_status_qmap_created, this, &MainWindow::slot_qmap_created);
      connect(this->measdoc.get(), &measdocxml::signal_txm_cycle_qmap_created, this, &MainWindow::slot_qmap_created);
      connect(this->measdoc.get(), &measdocxml::signal_txm_sequence_qmap_created, this, &MainWindow::slot_qmap_created);
      connect(this->measdoc.get(), &measdocxml::signal_txm_waveform_qmap_created, this, &MainWindow::slot_qmap_created);
      connect(this->measdoc.get(), &measdocxml::signal_txm_epos_qmap_created, this, &MainWindow::slot_qmap_created);

      // muss verbuden werden und auf true
      // this->measdoc->set_verbose(true);
      this->measdoc->get_txm(true);
      this->measdoc->get_hw_status(true);

    }

    // THIS is a useless fragment because the ats file handles the calibration self responsible
    else {
      qDebug() << "try database";
      for (auto &ich : used_channels) {
        for (auto &ats : mxcd->in_atsfiles) {
          if (ats->value("channel_number").toInt() == ich) {
            QString cal(ats->get_xmlcal());
            if (cal != "empty")
              this->adujob->set("xmlcal_sensor", QVariant(cal), ich, false);
          }
        }
      }
    }

    for (int i = 0; i < max_chan; ++i) {
      if (std::find(used_channels.begin(), used_channels.end(), i) == used_channels.end()) {
        qDebug() << "deactivate" << i;
        this->adujob->set("active", QVariant(false), i, false);
      }
    }

    //    for (auto &ats : mxcd->in_atsfiles ) {
    //        connect(ats.get(), &atsheader::filename_changed, this, &MainWindow::xml_changed);
    //    }
    //    for (auto &ats : mxcd->in_atsfiles ) {
    //        ats->notify_xml();
    //    }

    // write
    // this->adujob->get_start_time().toString("yyyy-MM-dd_hh-mm-ss") + "_" + job.get_sample_file_string() + ".xml"
    qDebug() << exdir.absoluteFilePath() + "/" + this->xml_filename;
    this->adujob->on_set_filename(exdir.absoluteFilePath() + "/" + mxcd->in_atsfiles.at(0)->subname_xml());
    mxcd->close_infiles();

    // CALIBRATION
    //  database
    //  set adjob calibration

    this->adujob->on_write_job(true);

    for (auto &qfi : qfis) {
      QFile file(qfi.absoluteFilePath());
      if (!file.rename(exdir.absoluteFilePath() + "/" + qfi.fileName())) {
        qDebug() << qfi.absoluteFilePath() << "move failed" << exdir.absoluteFilePath() + "/" + qfi.fileName();
      }
    }

    // move files an REWRITE HEADER

    this->clear(false);
  }
}

void MainWindow::xml_changed(const QMap<QString, QVariant> &atswriter_section, const int &channel_id, const QString &measdir_wanted, const QString &measdoc_wanted) {
  this->xml_filename = measdoc_wanted;
}

void MainWindow::on_pushButton_load_sensordb_clicked() {
  if (this->calib != nullptr)
    this->calib.reset();
  auto dialog = new QFileDialog(this);
  dialog->setViewMode(QFileDialog::Detail);
  dialog->setFileMode(QFileDialog::ExistingFile);
  dialog->setWindowTitle("Select SQL Sensor DB");

  dialog->setModal(false);
  dialog->show();

  QStringList fileNames;
  if (dialog->exec())
    fileNames = dialog->selectedFiles();
  if (fileNames.size()) {
    this->sensors_sql3.setFile(fileNames.at(0));
    this->info_sql3.setFile(procmt_homepath("info.sql3"));
    this->calib = std::make_unique<calibration>(info_sql3);
    this->master_cal_db.setFile(procmt_homepath("master_calibration.sql3"));
  }

  if (this->calib != nullptr) {
    this->calib->open_create_db(this->sensors_sql3.fileName(), this->sensors_sql3.absoluteDir());
    this->calib->open_master_cal(this->master_cal_db, "tstools_mastercal_query");

    // this->calib->select_coil("MFS-07e", 464);
  }
}

void MainWindow::slot_qmap_created(const QMap<QString, QVariant> &data_map) {
  if (this->adujob != nullptr) {
    auto miter = data_map.constBegin();
    while (miter != data_map.constEnd()) {
      this->adujob->set(miter.key(), miter.value(), 0);
      ++miter;
    }
  }
}

void MainWindow::on_dirs_base_lineEdit_valid_dir(const bool &ok) {
  if (ok) {
    if (this->qfi_base.absoluteFilePath() != this->ui->dirs_base_lineEdit->text()) {
      this->qfi_base.setFile(this->ui->dirs_base_lineEdit->absoluteFilePath());
      this->basedir.setPath(this->ui->dirs_base_lineEdit->absoluteFilePath());
      this->ui->dirs_target_lineEdit->setDisabled(false);
      this->ui->dirs_source_lineEdit->setDisabled(false);

      // we changed
      this->ui->dirs_target_lineEdit->clear();
      this->ui->dirs_source_lineEdit->clear();

      this->setWindowTitle(qfi_base.baseName());

      this->ui->dirs_target_lineEdit->set_toplevel_dir(this->qfi_base.absoluteFilePath() + "/ts");
      this->ui->dirs_source_lineEdit->set_toplevel_dir(this->qfi_base.absoluteFilePath() + "/ts");
    }
    if (!ok) {
      this->ui->dirs_target_lineEdit->setDisabled(true);
      this->ui->dirs_source_lineEdit->setDisabled(true);

      this->setWindowTitle("NO SURVEY");
    }
  }
}

void MainWindow::on_dirs_source_lineEdit_valid_dir(const bool &ok) {
  if (ok) {
    this->qfi_source.setFile(this->ui->dirs_source_lineEdit->absoluteFilePath());
    this->slot_scan_all_measdocs();
  }
}

void MainWindow::on_dirs_target_lineEdit_valid_dir(const bool &ok) {
  if (ok) {
    this->qfi_target.setFile(this->ui->dirs_target_lineEdit->absoluteFilePath());
    this->ui->pushButton_create_xml->setEnabled(true);
  }
}

void MainWindow::on_dirs_base_select_pushButton_clicked() {
  QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
  auto dialog = new QFileDialog(this, "Select target dir", homeLocation);

  // auto dialog = new QFileDialog(this, homeLocation);
  dialog->setViewMode(QFileDialog::Detail);
  dialog->setFileMode(QFileDialog::Directory);
  dialog->setOption(QFileDialog::ShowDirsOnly, true);

  // dialog->setFileMode(QFileDialog::ExistingFiles);

  dialog->setWindowTitle("Select base dir");

  dialog->setModal(false);
  dialog->show();

  QStringList fileNames;
  if (dialog->exec())
    fileNames = dialog->selectedFiles();
  if (fileNames.size()) {
    QDir bd(fileNames.at(0));
    this->ui->dirs_base_lineEdit->setText(bd.absolutePath());
    this->qfi_base.setFile(bd.absolutePath());
    this->basedir.setPath(bd.absolutePath());
    qDebug() << this->ui->dirs_base_lineEdit->text() << " set!";
    if (this->qfi_base.exists()) {
      this->ui->dirs_source_lineEdit->setEnabled(true);
    }
  }
}

void MainWindow::on_dirs_target_select_pushButton_clicked() {

  if (!this->qfi_base.absoluteFilePath().size())
    return;
  auto dialog = new QFileDialog(this, "Select target dir", this->qfi_base.absoluteFilePath());
  dialog->setViewMode(QFileDialog::Detail);
  dialog->setFileMode(QFileDialog::Directory);
  dialog->setOption(QFileDialog::DontResolveSymlinks, true);
  dialog->setOption(QFileDialog::ShowDirsOnly, true);

  dialog->setModal(false);
  dialog->show();
  // work around - otherwise directories are show twice
  dialog->setDirectory(this->qfi_base.absoluteFilePath() + "/ts");

  QStringList fileNames;
  if (dialog->exec())
    fileNames = dialog->selectedFiles();
  if (fileNames.size()) {
    QDir bd(fileNames.at(0));
    this->ui->dirs_target_lineEdit->setText(bd.absolutePath());
    qDebug() << this->ui->dirs_target_lineEdit->text() << " set!";
    this->qfi_target.setFile(bd.absolutePath());
    this->ui->pushButton_create_xml->setEnabled(true);
  }
}

void MainWindow::on_dirs_source_select_pushButton_clicked() {
  if (!this->qfi_base.absoluteFilePath().size())
    return;
  auto dialog = new QFileDialog(this, "Select source dir");
  dialog->setViewMode(QFileDialog::Detail);
  dialog->setFileMode(QFileDialog::ExistingFiles);
  dialog->setOption(QFileDialog::ShowDirsOnly, true);
  dialog->setDirectory(this->qfi_base.absoluteFilePath());
  dialog->setDirectory(this->qfi_base.absoluteFilePath() + "/ts");

  dialog->setModal(false);
  dialog->show();

  QStringList fileNames;
  if (dialog->exec())
    fileNames = dialog->selectedFiles();
  if (fileNames.size()) {
    QDir bd(fileNames.at(0));
    this->ui->dirs_source_lineEdit->setText(bd.absolutePath());
    qDebug() << this->ui->dirs_source_lineEdit->text() << " set!";
    this->qfi_source.setFile(bd.absolutePath());
    this->slot_scan_all_measdocs();
  }
}

void MainWindow::on_actionCreate_Survey_triggered() {
  QDir directory(QDir::home());
  QString path = QFileDialog::getExistingDirectory(this, tr("Directory"), directory.path());
  if (path.isNull() == false) {
    directory.setPath(path);
  }
  if (directory.isEmpty()) {
    for (auto &str : pmt::survey_dirs) {
      directory.mkdir(str);
    }
    this->ui->dirs_base_lineEdit->setText(directory.absolutePath());
    this->qfi_base.setFile(directory.absolutePath());
    this->basedir.setPath(directory.absolutePath());
    this->ui->dirs_source_lineEdit->setEnabled(true);
  }
}

void MainWindow::on_pushButton_tsdiff_clicked() {
  size_t i = 0;
  this->ui->pushButton_tsdiff->setDisabled(true);
  for (auto &atsf : this->mxcd->in_atsfiles) {
    // atsf->read_all_tsplotter();
    this->readerthreads.emplace_back(std::thread(&atsfile::read_all, atsf));
    this->fetcherthreads.emplace_back(std::thread(&MainWindow::fetch_tsdiff_reader, this, i++));

    // std::launch::async(&atsfile::read_tsplotter, atsf);
    // std::async(std::launch::async(&atsfile::read_tsplotter, atsf);
  }
  for (auto &atsf : this->mxcd->out_atsfiles) {
    this->writerthreads.emplace_back(std::thread(&atsfile::write, atsf));
  }

  for (auto &v : this->fetcherthreads) {
    v.join();
  }
}

void MainWindow::on_pushButton_run_ascii2ats_clicked() {
  if (!this->atsfiles.size())
    return;
  if (this->atsfiles.size() != this->inats2ascii_files.size())
    return;

  if (this->qfi_base.fileName().size()) {
  }

  geocoordinates geocord;
  int lat, lon, elev;
  long double elevation;

  geocord.set_lat_lon(this->ui->spinBox_LatDeg->value(), this->ui->spinBox_LatMin->value(),
                      this->ui->doubleSpinBox_LatSec->value(), this->ui->comboBox_NS_Indicator->currentText(),
                      this->ui->spinBox_LonDeg->value(), this->ui->spinBox_LonMin->value(),
                      this->ui->doubleSpinBox_LonSec->value(), this->ui->comboBox_EW_Indicator->currentText(),
                      this->ui->doubleSpinBox_Elevation->value());

  geocord.get_lat_lon_msec(lat, lon, elevation);
  elev = int32_t(elevation / 100.);

  for (auto &ats : this->atsfiles) {
    ats->set_start_date_time(this->ui->dateTimeEdit_starts->dateTime());
    ats->insert("latitude", lat);
    ats->insert("longitude", lon);
    ats->insert("elevation", elev);
    ats->insert("system_type", "ADU08");
    ats->set_survey_basedir(this->basedir);

    ats->set_sample_freq(this->fcombo->get_frequency(), false, false);
    QString chcksf = this->ui->lineEdit_sample_freq->text();
    if (chcksf.size()) {
      if (chcksf != "0") {
        bool bad = false;
        double ff = chcksf.toDouble(&bad);
      }
    }

    if (ats->fileName().contains("TEx")) {
      ats->dip_to_pos(this->ui->doubleSpinBox_DipEx->value());
      ats->insert_int("sensor_sernum", this->ui->spinBox_ExSer->value());
      ats->insert("sensor_type", this->allsensors.at(0)->get_atsheader_sensor_name());
      if (this->ui->checkBox_chopperEx->isChecked())
        ats->insert("chopper_status", quint8(1));
      else
        ats->insert("chopper_status", quint8(0));
    }
    if (ats->fileName().contains("TEy")) {
      ats->dip_to_pos(this->ui->doubleSpinBox_DipEy->value());
      ats->insert_int("sensor_sernum", this->ui->spinBox_EySer->value());
      ats->insert("sensor_type", this->allsensors.at(1)->get_atsheader_sensor_name());
      if (this->ui->checkBox_chopperEy->isChecked())
        ats->insert("chopper_status", quint8(1));
      else
        ats->insert("chopper_status", quint8(0));
    }
    if (ats->fileName().contains("THx")) {
      ats->insert_int("sensor_sernum", this->ui->spinBox_HxSer->value());
      ats->insert("sensor_type", this->allsensors.at(2)->get_atsheader_sensor_name());
      if (this->ui->checkBox_chopperHx->isChecked())
        ats->insert("chopper_status", quint8(1));
      else
        ats->insert("chopper_status", quint8(0));
    }
    if (ats->fileName().contains("THy")) {
      ats->insert_int("sensor_sernum", this->ui->spinBox_HySer->value());
      ats->insert("sensor_type", this->allsensors.at(3)->get_atsheader_sensor_name());
      if (this->ui->checkBox_chopperHy->isChecked())
        ats->insert("chopper_status", quint8(1));
      else
        ats->insert("chopper_status", quint8(0));
    }
    if (ats->fileName().contains("THz")) {
      ats->insert_int("sensor_sernum", this->ui->spinBox_HzSer->value());
      ats->insert("sensor_type", this->allsensors.at(4)->get_atsheader_sensor_name());
      if (this->ui->checkBox_chopperHz->isChecked())
        ats->insert("chopper_status", quint8(1));
      else
        ats->insert("chopper_status", quint8(0));
    }

    ats->init_calibration(2);

    ats->prc_com_to_classmembers();
  }

  for (auto &ats : this->atsfiles) {

    if (this->basedir.exists()) {

      QDir sdir(this->basedir.absolutePath() + "/ts/" + this->ui->dirs_target_lineEdit->baseName());
      qDebug() << "where" << sdir.absolutePath();
      ats->set_basedir_fake_deep(sdir);
      if (!sdir.exists()) {
        sdir.mkdir(this->basedir.absolutePath() + "/ts/" + this->ui->dirs_target_lineEdit->baseName());
      }
      // ats->setFile(sdir.absolutePath() + "/" + ats->completeBaseName());
      qDebug() << "filename" << ats->absoluteFilePath();

      qDebug() << "creating: " << ats->get_new_filename();
    }
  }
  if (!this->qfi_base.absoluteFilePath().size())
    return;
  this->qfi_source.setFile(this->basedir.absolutePath() + "/ts/" + this->ui->dirs_target_lineEdit->baseName());
  if (!this->qfi_source.absoluteFilePath().size())
    return;

  // set values

  // check dir!!!!

  int ntxt = 0;
  bool is_Volt = false;
  bool is_64bit = false;
  if (this->ui->radioButton_V->isDown())
    is_Volt = true;
  if (this->ui->checkBox_force_64bit->isChecked())
    is_64bit = true;

  // mk threads  check VOLT radio

  this->readerthreads.clear();

  for (auto &ats : this->atsfiles) {
    this->readerthreads.emplace_back(std::thread(&atsfile::read_ascii_file, ats, this->inats2ascii_files.at(ntxt++), is_Volt, is_64bit));
    // ats->read_ascii_file(this->inats2ascii_files[ntxt++], is_Volt, is_64bit);
  }

  for (auto &v : this->readerthreads) {
    v.join();
  }
  this->readerthreads.clear();
  this->inats2ascii_files.clear();
  QList<QFileInfo> allfiles;
  for (auto ats : this->atsfiles)
    allfiles.append(QFileInfo(ats->absoluteFilePath()));
  for (auto ats : this->atsfiles)
    ats.reset();
  this->mxcds.emplace_back(std::make_unique<mc_data>());
  this->mxcds.back()->open_files(allfiles, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);

  for (int i = 0; i < 5; ++i)
    this->chbtns[i]->setChecked(true);
  for (auto &atsf : this->mxcds.back()->in_atsfiles) {
    this->ui->ats2xml_listWidget->addItem(atsf->fileName());
  }
  this->ui->pushButton_create_xml->setDisabled(false);
  this->ui->tabWidget->setCurrentWidget(this->ui->tab_ats2xml);
}

void MainWindow::on_pushButton_cancel_ascii2ats_clicked() {
}

void MainWindow::on_comboBox_sample_freq_currentIndexChanged(int index) {
  double freq = this->fcombo->current_index_freq(index);

  this->ui->lineEdit_sample_freq->setText("0");

  if (freq < 513)
    this->ui->checkBox_chopperHx->setChecked(true);
  else
    this->ui->checkBox_chopperHx->setChecked(false);

  if (freq < 513)
    this->ui->checkBox_chopperHy->setChecked(true);
  else
    this->ui->checkBox_chopperHy->setChecked(false);

  if (freq < 513)
    this->ui->checkBox_chopperEx->setChecked(true);
  else
    this->ui->checkBox_chopperEx->setChecked(false);

  if (freq < 513)
    this->ui->checkBox_chopperEy->setChecked(true);
  else
    this->ui->checkBox_chopperEy->setChecked(false);

  if (freq < 513)
    this->ui->checkBox_chopperHz->setChecked(true);
  else
    this->ui->checkBox_chopperHz->setChecked(false);
}

void MainWindow::on_fcombo_tscat_template_currentIndexChanged(int index) {
  this->tscat_freq = this->fcombo_tscat->current_index_freq(index);
  this->ats_file_search_str = this->fcombo_tscat->current_index_name(index).simplified();
  this->ats_file_search_str.remove(" ");
  if (this->ats_file_search_str.endsWith("z", Qt::CaseInsensitive))
    this->ats_file_search_str.chop(1);
  this->ats_file_search_str.prepend("_");

  qDebug() << this->ats_file_search_str;
}

void MainWindow::on_run_tscat_button_clicked() {
  QList<QFileInfo> allfiles;

  if (atsex.size() > 1 && this->atsex_cat != nullptr) {
    this->atsex_cat->set_num_samples(0);
    this->atsex_cat->write_header(true);
  }
  for (auto &ats : this->atsex) {
    qDebug() << "w";
    this->atsex_cat->concat(ats);
    allfiles.append(this->atsex_cat->get_qfile_info());
  }
  if (atsey.size() > 1 && this->atsey_cat != nullptr) {
    this->atsey_cat->set_num_samples(0);
    this->atsey_cat->write_header(true);
  }
  for (auto &ats : this->atsey) {
    qDebug() << "w";
    this->atsey_cat->concat(ats);
    allfiles.append(this->atsey_cat->get_qfile_info());
  }
  if (atshx.size() > 1 && this->atshx_cat != nullptr) {
    this->atshx_cat->set_num_samples(0);
    this->atshx_cat->write_header(true);
  }
  for (auto &ats : this->atshx) {
    qDebug() << "w";
    this->atshx_cat->concat(ats);
    allfiles.append(this->atshx_cat->get_qfile_info());
  }
  if (atshy.size() > 1 && this->atshy_cat != nullptr) {
    this->atshy_cat->set_num_samples(0);
    this->atshy_cat->write_header(true);
  }
  for (auto &ats : this->atshy) {
    qDebug() << "w";
    this->atshy_cat->concat(ats);
    allfiles.append(this->atshy_cat->get_qfile_info());
  }
  if (atshz.size() > 1 && this->atshz_cat != nullptr) {
    this->atshz_cat->set_num_samples(0);
    this->atshz_cat->write_header(true);
  }
  for (auto &ats : this->atshz) {
    qDebug() << "w";
    this->atshz_cat->concat(ats);
    allfiles.append(this->atshz_cat->get_qfile_info());
  }

  this->close_tscat_channels(false);
  this->mxcds.clear();
  this->mxcds.emplace_back(std::make_unique<mc_data>());
  if (allfiles.size()) {
    this->mxcds.back()->open_files(allfiles, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
  }

  if (this->qfi_target_cat.exists()) {
    this->qfi_target = this->qfi_target_cat;
    this->on_pushButton_create_xml_clicked();
  } else {

    QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    homeLocation += "/tscat_result";
    QDir dir(homeLocation);
    if (!dir.exists())
      dir.mkpath(homeLocation);
    if (dir.exists()) {
      this->qfi_target.setFile(homeLocation);
      this->on_pushButton_create_xml_clicked();
    }
  }
}

void MainWindow::on_cancel_tscat_button_clicked() {
  this->close_tscat_channels(true);
}

void MainWindow::on_pushButton_create_survey_clicked() {
  QDir directory(QDir::home());
  QString path = QFileDialog::getExistingDirectory(this, tr("Directory"), directory.path());
  if (path.isNull() == false) {
    directory.setPath(path);
  }
  if (directory.isEmpty()) {
    for (auto &str : pmt::survey_dirs) {
      directory.mkdir(str);
    }
    this->ui->dirs_base_lineEdit->setText(directory.absolutePath());
  }
}

void MainWindow::on_pushButton_convert_json_clicked() {
  if (!this->atsfiles.size() && (this->adu != nullptr)) {
    qDebug() << "go phnx";
  }

  if (this->phoenix_ts.exists()) {

    //                         this->atsfiles.back()->set_key_value("sensor_type", "coil");

    if (this->phj_coils.size() && this->atsfiles.size()) {
      for (auto &atsf : this->atsfiles) {
        for (auto &phj_coil : this->phj_coils) {
          if (phj_coil->calfiles.size()) {
            if (atsf->ivalue("sensor_sernum") == phj_coil->calfiles.at(0)->sernum) {
              QString stype(phj_coil->calfiles.at(0)->sensortype);
              // mtx and other names may be too long; remove "-" and use tha alias table from info.sql3
              stype.remove("-");
              atsf->set_key_value("sensor_type", stype);
              atsf->prc_com_to_classmembers();
            }
          }
        }
      }
    }

    this->phj_system_ts->ts_block_reader(this->phoenix_ts, this->atsfiles, chan_str_mtx, chan_str_phoenix);

    qDebug() << "tsdata finished";
    uint i = 0;
    for (auto &atsf : atsfiles) {
      auto map(atsf->get_atswriter_section());
      auto iter = map.constBegin();
      while (iter != map.constEnd()) {
        this->adu->set_channel_value(i, iter.key(), iter.value());
        ++iter;
      }
      ++i;
    }
    this->adu->to_XML_file(QFileInfo(this->atsfiles.at(0)->absolutePath() + "/" + this->atsfiles.at(0)->get_measdoc_name()));

    this->ui->json_listWidget->addItem("-----------------TIMESERIES RESULT--------------------");
    auto vals = phj_system_ts->phoenix_results.constBegin();
    while (vals != phj_system_ts->phoenix_results.constEnd()) {
      this->ui->json_listWidget->addItem(vals.key() + ": " + vals.value().toString());
      ++vals;
    }

    this->ui->json_listWidget->repaint();
    this->ui->json_listWidget->scrollToBottom();
    // this->ui->statusBar->showMessage("lines processed:" + QString::number());
    this->repaint();

    this->adu.reset();
    this->phoenix_ts.setFile("");
    for (auto &atsf : this->atsfiles)
      atsf.reset();
    this->atsfiles.clear();
    if (this->edt != nullptr)
      this->edt.reset();
    if (this->phj_system_ts != nullptr)
      this->phj_system_ts.reset();
    this->phoenix_ts.setFile("");
    this->chan_str_mtx.clear();
    this->chan_str_phoenix.clear();
  }

  if (this->phj_coils.size()) {
    for (auto &phj : phj_coils) {
      phj->save_calfiles();
    }

    this->ui->json_listWidget->addItem("-----------------CALIBRATION SENSOR RESULT--------------------");
    for (auto &phj : phj_coils) {
      auto vals = phj->phoenix_results.constBegin();
      while (vals != phj->phoenix_results.constEnd()) {
        this->ui->json_listWidget->addItem(vals.key() + ": " + vals.value().toString());
        ++vals;
      }
    }
    this->ui->json_listWidget->repaint();
    this->repaint();

    for (auto &phj_coil : phj_coils) {
      if (this->phj_coils.size() == 1) {
        disconnect(this->rbl_0, &radiobutton_list::btn_name_checked, phj_coil.get(), &Phoenix_json::slot_sensortype_changed);
      }
      if (this->phj_coils.size() == 2) {
        connect(this->rbl_1, &radiobutton_list::btn_name_checked, phj_coil.get(), &Phoenix_json::slot_sensortype_changed);
      }
      if (this->phj_coils.size() == 3) {
        connect(this->rbl_2, &radiobutton_list::btn_name_checked, phj_coil.get(), &Phoenix_json::slot_sensortype_changed);
      }
      phj_coil.reset();
    }
    phj_coils.clear();
  }

  if (this->phj_system != nullptr) {

    this->phj_system->save_calfiles();

    this->ui->json_listWidget->addItem("-----------------CALIBRATION SYSTEM RESULT--------------------");

    auto vals = phj_system->phoenix_results.constBegin();
    while (vals != phj_system->phoenix_results.constEnd()) {
      this->ui->json_listWidget->addItem(vals.key() + ": " + vals.value().toString());
      ++vals;
    }
    this->ui->json_listWidget->repaint();
    this->repaint();
    this->phj_system.reset();
  }

  this->phoenix_cal_coil.setFile("");
  this->phoenix_cal_system.setFile("");
}

void MainWindow::on_target_cat_dir_pushButton_clicked() {
  QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
  auto dialog = new QFileDialog(this, "Select target dir", homeLocation);
  dialog->setViewMode(QFileDialog::Detail);
  dialog->setFileMode(QFileDialog::Directory);
  dialog->setOption(QFileDialog::DontResolveSymlinks, true);
  dialog->setOption(QFileDialog::ShowDirsOnly, true);

  dialog->setModal(false);
  dialog->show();
  QStringList fileNames;
  if (dialog->exec())
    fileNames = dialog->selectedFiles();
  if (fileNames.size()) {
    QDir bd(fileNames.at(0));
    this->ui->target_cat_dir_lineEdit->setText(bd.absolutePath());
    this->qfi_target_cat.setFile(bd.absolutePath());
    qDebug() << this->ui->target_cat_dir_lineEdit->text() << " set!";
  }
}
