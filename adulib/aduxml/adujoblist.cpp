#include "adujoblist.h"

#include "adujoblist.h"

adujoblist::adujoblist(qint64 grid_time, QObject *parent) :
    QAbstractTableModel(parent) {
  this->qfi.setFile("empty");
  this->top_jobdir.setPath("/tmp/adujobs/");

  this->grid_time = grid_time;

  this->stops_at_cols.insert(0, "Sample Freq");
  this->stops_at_cols.insert(1, "StartDateTime");
  this->stops_at_cols.insert(2, "Duration");
  this->stops_at_cols.insert(3, "valid");

  this->TargetDirectories << QString("default") << QString("/mtdata/usb/data1") << QString("/mtdata/usb/data2");
  this->TimeFormats << QString("adjusted") << QString("absolute") << QString("relative");
  this->StorageModes << QString("USB_TO_DEFAULT") << QString("USB_TO_USB") << QString("USB_TO_JOB");
  this->AdaptConfigs << QString("FALSE") << QString("TRUE");
  this->AdaptSensorSpacings << QString("FALSE") << QString("TRUE");
  this->AdaptSensorTypes << QString("E_SERIES") << QString("NONE");
  this->AdaptChannelConfigs << QString("TRUE") << QString("FALSE");
  this->CleanJobTables << QString("TRUE") << QString("FALSE");

  // create empty dialog
  // this->stops_at =  std::unique_ptr<stoptime_dialog>(new stoptime_dialog(0, QDateTime(), 0, 0, 0));
  // this->stops_at = std::make_unique<stoptime_dialog>(index.row(),this->jobs.at(index.row()).get_start_time(), this->jobs.at(index.row()).get_duration(), this->jobs.at(index.row()).get_sample_freq());
  this->stops_at = std::make_unique<stoptime_dialog>(0, QDateTime(), 0, 0.0);
  this->stops_at->setWindowTitle("set Stop Time / Duration");

  QObject::connect(this->stops_at.get(), &stoptime_dialog::stop_time_changed, this, &adujoblist::on_duration_changed);
}

int adujoblist::rowCount(const QModelIndex & /*parent*/) const {
  // max jobs
  return 12;
}

int adujoblist::columnCount(const QModelIndex & /*parent*/) const {
  //  sample_freq , start time, start date, duration hh::mm::ss
  return stops_at_cols.size();
}

QVariant adujoblist::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {

    // qDebug() << " ---->"  channels[index.column()].get_data_from_index(index.row());
    if (index.row() < this->jobs.size()) {
      if (index.column() == 0)
        return this->jobs[index.row()].get("sample_freq", 0);
      if (index.column() == 1)
        return this->jobs[index.row()].get_start_time().toString("HH:mm:ss   yyyy-MM-dd");
      if (index.column() == 2) {
        QTime st(0, 0, 0, 0);
        st = st.addSecs(this->jobs[index.row()].get_duration());
        return st.toString("HH:mm:ss");
      }
      if (index.column() == 3) {
        return "ok";
      }
    }
  }
  return QVariant();
}

QVariant adujoblist::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();
  if (orientation == Qt::Horizontal) {
    return this->stops_at_cols.value(section);
  }
  return QVariant();
}

Qt::ItemFlags adujoblist::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool adujoblist::setData(const QModelIndex &index, const QVariant &value, int role) {

  if (!index.isValid() || role != Qt::EditRole)
    return false;

  qDebug() << "changed joblist data" << index.row() << index.column() << value.toString();
  if (index.column() == 0) {
    this->jobs[index.row()].set("sample_freq", value, -1, this->jobs.size() - 1);
  }
  return true;
}

void adujoblist::on_clicked(const QModelIndex &index) {
  qDebug() << "clicked" << index.row() << index.column();

  if (index.row() < jobs.size()) {
    if (index.column() == this->stops_at_cols.key("Duration")) {
      this->stops_at->set_starts(index.row(), this->jobs.at(index.row()).get_start_time(), this->jobs.at(index.row()).get_duration(),
                                 this->jobs.at(index.row()).get_sample_freq());
      this->stops_at->show();
    }
  }
}

void adujoblist::on_regrid_clicked() {
  for (auto &job : this->jobs) {
    job.slot_on_keep_grid(false);
  }
  // void adujoblist::adjust_start_times(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq)

  this->adjust_start_times(0, this->jobs.at(0).get_start_time(), this->jobs.at(0).get_duration(), this->jobs.at(0).get_sample_freq());

  for (auto &job : this->jobs) {
    job.slot_on_keep_grid(true);
  }
}

adujoblist::~adujoblist() {

  // this->stops_at->close();
  // this->stops_at.reset();
  this->jobs.clear();
  qDebug() << "adujoblist destroyed";
}

void adujoblist::add_job(const qint64 duration, const double freq, const QString filter_type_main, const int input, const QString special_list,
                         const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {

  if (jobs.size()) {

    if ((jobs.size() == 1) && (duration < 0) && ((qfi != nullptr) || (qurl != nullptr) || (qba != nullptr)) && this->want_set_first) {
      jobs.last().fetch_recording(qfi, qurl, qba);
      jobs.last().fetch_atswriter_part(qfi, qurl, qba);
      jobs.last().fetch_global_config(qfi, qurl, qba);
      this->want_set_first = false;
    } else {
      jobs.append(jobs.at(0));
      if ((duration < 0) && ((qfi != nullptr) || (qurl != nullptr) || (qba != nullptr))) {
        jobs.last().fetch_recording(qfi, qurl, qba);
        jobs.last().fetch_global_config(qfi, qurl, qba);
      }

      // this should be the default when adding to a list which already has a first job
      else
        jobs.last().on_set_duration(duration, false);
      jobs.last().set_index(this->jobs.size() - 1);
    }
  } else {
    jobs.append(adulib(this->grid_time, this));
    jobs.last().on_set_duration(duration, false);
    jobs.last().set_index(0);
  }

  if (jobs.size() > 1) {
    // signal not connect yet
    this->slot_update_grid();
    jobs.last().set_min_start_time(jobs.last().get_index(), jobs.at(jobs.size() - 2).get_stop_time(), jobs.at(jobs.size() - 2).get_min_delta_start(), this->grid_space.last());
  }

  QObject::connect(&this->jobs.last(), &adulib::channel_map_changed, this, &adujoblist::on_channel_map_changed);
  QObject::connect(&this->jobs.last(), &adulib::min_delta_start_changed, this, &adujoblist::adjust_start_times);
  QObject::connect(&this->jobs.last(), &adulib::duration_changed, this, &adujoblist::adjust_start_times);
  QObject::connect(&this->jobs.last(), &adulib::start_time_changed, this, &adujoblist::adjust_start_times);
  QObject::connect(&this->jobs.last(), &adulib::sample_freq_changed, this, &adujoblist::adjust_start_times);
  QObject::connect(&this->jobs.last(), &adulib::timing_change_attempt, this, &adujoblist::slot_update_grid);

  QObject::connect(&this->jobs.last(), &adulib::QTableViewChanged, this, &adujoblist::on_set);

  // the first job must load all XML configs; AFTER that I can set the sampling freq, not before
  // emits
  if (freq > 0 && jobs.size() > 1) {
    jobs.last().set("sample_freq", freq, -1, true);
    jobs.last().set("filter_type_main", filter_type_main, -1, true);
    jobs.last().set("input", input, -1, true);
  }

  if (special_list == "JLL") {
    for (auto &jb : jobs)
      jb.set_special_list(special_list);
    jobs.first().on_set_duration(jobs.first().seconds_to_midnight(-((9 * 3600) + 120)));
  }

  if (special_list == "InSituCal") {
    for (auto &jb : jobs)
      jb.set_special_list(special_list);
  }

  if (special_list == "CEA") {
    for (auto &jb : jobs)
      jb.set_special_list(special_list);
  }

  if (special_list == "CEA-daily") {
    for (auto &jb : jobs)
      jb.set_special_list(special_list);
    jobs.first().on_set_duration(jobs.first().seconds_to_midnight((-120)));
  }

  qDebug() << "adujoblist::add_job: " << jobs.last().get_start_time().toString("yyyy-MM-dd HH:mm:ss") << " <-> " << jobs.last().get_stop_time().toString("yyyy-MM-dd HH:mm:ss");
}

void adujoblist::init_from_files(const QFileInfo *qfiHwConfig, const QUrl *qurlHwConfig, const QByteArray *qbaHwConfig, const QFileInfo *qfiHwDatabase, const QUrl *qurlHwDatabase, const QByteArray *qbaHwDatabase) {
  // we need always a minimum
  this->jobs.clear();
  this->add_job(1, -1);

  // we get this soon
  QObject::connect(&this->jobs[0], &adulib::selectable_frequencies_changed, this, &adujoblist::on_selectable_frequencies_changed);

  this->jobs[0].fetch_hwcfg(qfiHwConfig, qurlHwConfig, qbaHwConfig);
  this->jobs[0].fetch_hwdb(qfiHwDatabase, qurlHwDatabase, qbaHwDatabase);
  this->jobs[0].build_channels();
  this->jobs[0].build_hardware();

  if (qfiHwConfig != nullptr) {
    QFile cptxt(qfiHwConfig->absoluteFilePath());
    cptxt.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream HWConfig_stream(&cptxt);
    this->HWConfig_stream_string = HWConfig_stream.readAll();
    cptxt.close();
  } else if (qurlHwConfig != nullptr) {
    QFile cptxt(qurlHwConfig->toLocalFile());
    cptxt.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream HWConfig_stream(&cptxt);
    this->HWConfig_stream_string = HWConfig_stream.readAll();
    cptxt.close();
  } else if (qbaHwConfig != nullptr) {

    this->HWConfig_stream_string.fromUtf8(*qbaHwConfig);
  }

  if (qfiHwDatabase != nullptr) {
    QFile cptxt(qfiHwDatabase->absoluteFilePath());
    cptxt.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream HwDatabase_stream(&cptxt);
    this->HwDatabase_stream_string = HwDatabase_stream.readAll();
    cptxt.close();
  } else if (qurlHwDatabase != nullptr) {
    QFile cptxt(qurlHwDatabase->toLocalFile());
    cptxt.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream HwDatabase_stream(&cptxt);
    this->HwDatabase_stream_string = HwDatabase_stream.readAll();
    cptxt.close();
  } else if (qbaHwDatabase != nullptr) {

    this->HwDatabase_stream_string.fromUtf8(*qbaHwDatabase);
  }

  this->want_set_first = true;
  emit this->job_zero_created();
}

void adujoblist::init_from_adulib(adulib adu) {
  jobs.append(adu);
  jobs.last().set_index(0);

  QObject::connect(&this->jobs.last(), &adulib::channel_map_changed, this, &adujoblist::on_channel_map_changed);
  QObject::connect(&this->jobs.last(), &adulib::min_delta_start_changed, this, &adujoblist::adjust_start_times);
  QObject::connect(&this->jobs.last(), &adulib::duration_changed, this, &adujoblist::adjust_start_times);
  QObject::connect(&this->jobs.last(), &adulib::start_time_changed, this, &adujoblist::adjust_start_times);
  QObject::connect(&this->jobs.last(), &adulib::sample_freq_changed, this, &adujoblist::adjust_start_times);

  QObject::connect(&this->jobs.last(), &adulib::QTableViewChanged, this, &adujoblist::on_set);
}
/*!
 * \brief adujoblist::update_from_jobdir
 * \param dirPath
 */
int adujoblist::update_from_jobdir(const QString &dirPath, const bool is_from_adu) {
  if (!this->jobs.size()) {
    qDebug() << "adujoblist::update_from_jobdir -> joblist is not initialized";
    return 0;
  }
  while (this->jobs.size() > 1) {
    this->jobs.removeLast();
  }
  this->jobs[0].submitted_to_adu_jobtable(false);
  this->want_set_first = true;
  QUrl *eu = nullptr;
  QByteArray *eb = nullptr;
  int i = 0;
  QDir dir(dirPath);
  for (QFileInfo fileInfo : dir.entryInfoList(QStringList() << "*.xml", QDir::Files, QDir::Name)) {
    if (fileInfo.exists()) {
      qDebug() << fileInfo.fileName();
      this->add_job(qint64(-1), -1, "", 0, "", &fileInfo, eu, eb);
      this->jobs.last().submitted_to_adu_jobtable(is_from_adu);
      ++i;
    }
  }

  qDebug() << "adujoblist::update_from_jobdir, total" << this->jobs.size();

  emit jobs_updated();

  return this->jobs.size();
}

adulib &adujoblist::operator[](int i) {
  return this->jobs[i];
}

adulib &adujoblist::job(const int &i) {
  return this->jobs[i];
}

int adujoblist::size() const {
  return this->jobs.size();
}

void adujoblist::clear() {
  while (this->jobs.size() > 1) {
    this->jobs.removeLast();
  }
  this->jobs[0].submitted_to_adu_jobtable(false);
}

void adujoblist::set_start_time(const QDateTime &dt, const uint jobno) {

  if (((this->jobs[0].get_special_list() == "JLL") || (this->jobs[0].get_special_list() == "CEA-daily")) && this->jobs.size() == 2) {
    this->jobs[0].set_start_time(dt, true);
    // split at JST midnight - CEA uses UTC which is always the best choice
    if (this->jobs[0].get_special_list() == "JLL")
      this->jobs[0].on_set_duration(jobs[0].seconds_to_midnight(-((9 * 3600) + 120)));
    else
      this->jobs[0].on_set_duration(jobs[0].seconds_to_midnight((-120)));
    QDateTime xdt(jobs[0].get_stop_time());
    xdt.setTimeZone(QTimeZone::utc());
    xdt = xdt.addSecs(120);
    this->jobs[1].set_start_time(xdt, true);
  } else if (jobno < (uint)this->jobs.size()) {

    if (this->grid_time > 1) {
      QDateTime newstart(dt);
      newstart.setSecsSinceEpoch((dt.toSecsSinceEpoch() / grid_time) * grid_time); // will be equal or less; integer division rounds always down
      if (newstart < dt)
        newstart = newstart.addSecs(grid_time);
      this->jobs[jobno].set_start_time(newstart, true);
    } else
      this->jobs[jobno].set_start_time(dt, true);
  }
}

void adujoblist::set_start_time_now(const int secs_from_now, const bool emits) {
  this->jobs[0].set_start_time_now(secs_from_now, emits);
}

void adujoblist::setGridTime(const qint64 grid_time) {
  for (auto &job : this->jobs) {
    job.setGridTime(grid_time);
  }
}

void adujoblist::slot_utcoffset_for_midnight_usage(const qint64 &secs_from_utc) {
  for (auto &job : this->jobs) {
    job.utcoffset_for_midnight_usage = secs_from_utc;
  }
}

void adujoblist::adjust_start_times(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq) {
  int i = 0;

  // must block signals here

  for (auto &job : this->jobs) {
    job.blockSignals(true);
  }
  // we have subjobs here! first job until midnight
  if (this->jobs.at(0).get_special_list().contains("JLL") || this->jobs.at(0).get_special_list().contains("CEA-daily")) {
    this->set_start_time(start_time, 0);
  }

  else
    for (auto &job : this->jobs) {
      // block other event - especially start time changed
      // job.blockSignals(true);

      if (i) {

        job.set_min_start_time(index, this->jobs.at(i - 1).get_stop_time(), this->jobs.at(i - 1).get_min_delta_start(), this->grid_space.at(i - 1));
      }
      qDebug() << "adjusting start times, new start time" << i << job.get_start_time().toString("yyyy-MM-dd HH:mm:ss") << " < -- > " << job.get_stop_time().toString("yyyy-MM-dd HH:mm:ss");

      // job.blockSignals(false);
      ++i;
    }

  for (auto &job : this->jobs) {
    job.blockSignals(false);
  }
}

bool adujoblist::on_set(const QString &what, const QVariant &value, const int &ichan, const int &ijob, const bool emits) {
  if (this->jobs.at(0).want_change_for_all_jobs(what)) {
    for (auto &job : this->jobs) {
      job.set(what, value, ichan, false);
    }
  } else if (ijob < this->jobs.size() && ijob >= 0) {
    return this->jobs[ijob].set(what, value, ichan, emits);
  }

  return false;
}

QVariant adujoblist::on_get(const QString &what, const int &ichan, const int &ijob) {
  if (this->jobs.size()) {
    return jobs.at(ijob).get(what, ichan);
  }
  return QVariant();
}

void adujoblist::on_duration_changed(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq) {
  this->jobs[index].on_set_duration(duration);
  // this->adjust_start_times(index, start_time, duration, sample_freq);
}

int adujoblist::on_save(const QFileInfo *qfiHwConfig, const QUrl *qurlHwConfig, const QByteArray *qbaHwConfig) {
  int i = 0;
  this->top_jobdir.mkpath(this->top_jobdir.absolutePath());
  this->top_jobdir.mkdir(this->top_jobdir.absolutePath() + "/jobs");
  this->top_jobdir.mkdir(this->top_jobdir.absolutePath() + "/ADUConf");

  // using the app we may want to update the gains in case
  if (qbaHwConfig != nullptr) {
    for (auto &job : this->jobs) {
      job.fetch_autogains_part(qfiHwConfig, qurlHwConfig, qbaHwConfig);
    }
  }

  QString path = this->top_jobdir.absolutePath() + "/jobs";
  QDir dir(path);
  dir.setNameFilters(QStringList() << "*.xml");
  dir.setFilter(QDir::Files);
  foreach (QString dirFile, dir.entryList()) {
    dir.remove(dirFile);
  }

  for (auto &job : this->jobs) {
    job.on_set_filename(QDir::cleanPath(this->top_jobdir.absolutePath() + "/jobs") + "/" + job.get_start_time().toString("yyyy-MM-dd_hh-mm-ss") + "_" + job.get_sample_file_string() + ".xml");
    job.on_write_job();
    ++i;
  }

  if (this->HWConfig_stream_string.size()) {
    QFile cptxt(this->top_jobdir.absolutePath() + "/ADUConf/HwConfig.xml");
    if (cptxt.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream HWConfig_stream(&cptxt);
      HWConfig_stream << this->HWConfig_stream_string;
      cptxt.close();
    } else
      qDebug() << "adujoblist::on_save -> HWConfig not written!";
  }

  if (this->HwDatabase_stream_string.size()) {
    QFile cptxt(this->top_jobdir.absolutePath() + "/ADUConf/HwDatabase.xml");
    if (cptxt.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream HwDatabase_stream(&cptxt);
      HwDatabase_stream << this->HwDatabase_stream_string;
      cptxt.close();
    } else
      qDebug() << "adujoblist::on_save -> HWDatabase not written!";
  }

  tinyxmlwriter tixw(true);

  tixw.push("ADUConf");
  QStringList comments;

  comments << "set this node to the path inside the ADU-07 system, the USB device shall be mounted to.";
  comments << "making it empty does not mount the device.";
  comments << "to use an external USB device set this node to";
  comments << "/mtdata/usb/data1 or /mtdata/usb/data2";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("TargetDirectory", this->TargetDirectory);

  comments << "set this node to \"relative\" to start all jobs of the job-list at the next possible time.";
  comments << "setting the nodes value to \"absolute\" will not update start-/stop times.";
  comments << "setting the nodes value to \"adjusted\" will shift the complete job-list to the new";
  comments << "start-time defined in \"start_time\" and \"start_date\" nodes";
  comments << "setting the nodes value to \"grid\" you may enter a time grid the joblist shall be started";
  comments << "on (e.g. set start_time to 00:15:00 means start joblist at next full 15 minutes - Example:";
  comments << "time now: 08:37:42 / start_time: 00:15:00 / resulting joblist start-time: 08:45:00";
  comments << "time now: 09:02:13 / start_time: 00:15:00 / resulting joblist start-time: 09:15:00";
  comments << "... ).";
  comments << "do not enter any value for the \"TimeOffset\" node";
  comments << "all time values are UTC";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("TimeFormat", this->TimeFormat);
  tixw.element("TimeOffset", this->TimeOffset);
  tixw.time_hh_mm_ss("start_time", this->jobs.at(0).get_start_time());
  tixw.date_yyyy_mm_dd("start_date", this->jobs.at(0).get_start_time());

  comments << "this node configures the data storage mode for the jobs of the job-list. use the following";
  comments << "values:";
  comments << "USB_TO_USB: store all data recorded by the job-list on this USB device";
  comments << "USB_TO_DEFAULT: store all data to the ADU-07 internal CF-card \"/mtdata/data\"";
  comments << "USB_TO_JOB: use the path of the \"target_directory\" node from the XML job file";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("StorageMode", this->StorageMode);

  comments << "set this nodes value to \"TRUE\" to replace the configuration settings for \"sensor type\",";
  comments << "\"sensor name\", \"sensor serial\" and \"sensor position\" with either the configuration of";
  comments << "the last started job from the Webinterface or if available inside the system, of the";
  comments << "automatically detected sensors. leaving it at \"FALSE\" will not update the config settings";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("AdaptConfig", this->AdaptConfig);

  comments << "set this value to TRUE to use the values currently stored inside the ADU for the";
  comments << "sensor spacing for the joblist. If beeing set to FALSE the sensor spacing values of";
  comments << "the joblist will be used.";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("AdaptSensorSpacing", this->AdaptSensorSpacing);

  comments << "Set this value to E_SERIES to overwrite the sensor config (type and serial) for all";
  comments << "intelligent sensors (MFS-07e, MFS-06e, ...) with the information that has been detected";
  comments << "during boot. This should be the default.";
  comments << "Set this value to ALL to replace all the sensor config inside the joblist with the";
  comments << "currently used values of the ADU (Webinterface).";
  comments << "Set this value to NONE to use the sensor config of the joblist and ignore all";
  comments << "currently used ADU settings.";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("AdaptSensorType", this->AdaptSensorType);

  comments << "set this nodes value to \"TRUE\" to replace the channel configuration settings for gains";
  comments << "and DC offset correction with the settings that have been detected, using the \"AutoGain AutoOffset\" ";
  comments << "functionality (either via Webinterface or special joblist jobs.)";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("AdaptChannelConfig", this->AdaptChannelConfig);

  comments << "set this nodes value to \"TRUE\" to clean up the \"jobs\" table before transferring the job-list";
  comments << "to the ADU-07 database";
  tixw.comment_multi_line(comments);
  comments.clear();
  tixw.element("CleanJobTable", this->CleanJobTable);

  tixw.push("JobList");

  i = 0;
  for (auto &job : this->jobs) {
    QString attr = "id=\"" + QString::number(i) + "\" locked = \"0\"";
    tixw.push("Job", attr);
    tixw.element("TargetDirectory", QString("/jobs") + QString("/") + job.get_start_time().toString("yyyy-MM-dd_hh-mm-ss") + "_" + job.get_sample_file_string() + QString(".xml"));
    tixw.pop();
    ++i;
  }

  tixw.pop("JobList");
  tixw.pop("ADU07Conf");

  QFile xmljobsConfig((this->top_jobdir.absolutePath() + "/ADUConf/ADUConf.xml"));

  if (!xmljobsConfig.open(QIODevice::WriteOnly | QIODevice::Text)) {
    std::cerr << "adujoblist::on_save -> can not open " << qfi.absoluteFilePath().toStdString() << std::endl;
    return 0;
  } else {
    QTextStream out(&xmljobsConfig);
    out << tixw.getXmlStr();
    xmljobsConfig.close();
  }

  return i;
}

void adujoblist::on_swap_up(const int &list_index) {
  if (list_index < this->jobs.size() && jobs.size() && list_index > 0) {
    QDateTime qtd_early = jobs.at(list_index - 1).get_start_time();
    QDateTime qtd_late = jobs.at(list_index - 1).get_start_time();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    this->jobs.swapItemsAt(list_index, list_index - 1);
#else
    this->jobs.swap(list_index, list_index - 1);
#endif
    if (list_index - 1 == 0) {
      emit job_zero_changed();
    }
  }
}

void adujoblist::on_swap_down(const int &list_index) {
  if (list_index < this->jobs.size() - 2 && this->jobs.size()) {
    QDateTime qtd_early = this->jobs.at(list_index).get_start_time();
    QDateTime qtd_late = this->jobs.at(list_index + 1).get_start_time();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    this->jobs.swapItemsAt(list_index, list_index - 1);
#else
    this->jobs.swap(list_index, list_index - 1);
#endif
    if (list_index == 0) {
      emit job_zero_changed();
    }
  }
}

void adujoblist::on_set_TargetDirectory(const QString &TargetDirectory) {
  if (this->TargetDirectories.contains(TargetDirectory, Qt::CaseSensitive)) {
    this->TargetDirectory = TargetDirectory;
  }
  if (!this->TargetDirectory.compare("default"))
    this->TargetDirectory = "";
}

void adujoblist::on_set_TimeFormat(const QString &TimeFormat) {
  if (this->TimeFormats.contains(TimeFormat, Qt::CaseSensitive)) {
    this->TimeFormat = TimeFormat;
  }
}

void adujoblist::on_set_StorageMode(const QString &StorageMode) {
  if (this->StorageModes.contains(StorageMode, Qt::CaseSensitive)) {
    this->StorageMode = StorageMode;
  }
}

void adujoblist::on_set_AdaptConfig(const QString &AdaptConfig) {
  if (this->AdaptConfigs.contains(AdaptConfig, Qt::CaseSensitive)) {
    this->AdaptConfig = AdaptConfig;
  }
}

void adujoblist::on_set_AdaptSensorSpacing(const QString &AdaptSensorSpacing) {
  if (this->AdaptSensorSpacings.contains(AdaptSensorSpacing, Qt::CaseSensitive)) {
    this->AdaptSensorSpacing = AdaptSensorSpacing;
  }
}

void adujoblist::on_set_AdaptSensorType(const QString &AdaptSensorType) {
  if (this->AdaptSensorTypes.contains(AdaptSensorType, Qt::CaseSensitive)) {
    this->AdaptSensorType = AdaptSensorType;
  }
}

void adujoblist::on_set_AdaptChannelConfig(const QString &AdaptChannelConfig) {
  if (this->AdaptChannelConfigs.contains(AdaptChannelConfig, Qt::CaseSensitive)) {
    this->AdaptChannelConfig = AdaptChannelConfig;
  }
}

void adujoblist::on_set_CleanJobTable(const QString &CleanJobTable) {
  if (this->CleanJobTables.contains(CleanJobTable, Qt::CaseSensitive)) {
    this->CleanJobTable = CleanJobTable;
  }
}

void adujoblist::on_button_sample_freq_value_accepted(const double f) {

  qDebug() << "gui set sample frequency" << f;

  emit this->sample_freq_changed(f);
}

void adujoblist::on_removeAt(const QModelIndex &index) {
  //    if (jobs.size() > 1) { // the inner function is mutexed and does the same check
  this->removeAt(index.row());
  //    }
}

void adujoblist::removeAt(int index) {
  // mutex if delete is active
  // the lock gets released automatically when the function exits
  QMutexLocker lock(&mutex);

  if (jobs.size() > 1) {
    this->jobs.removeAt(index);
    if (index == 0) {
      emit job_zero_changed();
    }
    // void adujoblist::adjust_start_times(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq)
    QDateTime start_time;
    qint64 duration = 0;
    double sample_freq = 0;
    this->adjust_start_times(0, start_time, duration, sample_freq);
  }
}

void adujoblist::slot_update_grid() {
  int i = 0;
  this->grid_space.clear();
  for (i = 0; i < jobs.size(); ++i) {
    if (i < this->jobs.size() - 1) {
      this->grid_space.append(jobs[i] - jobs.at(i + 1));
    }
  }
}

void adujoblist::set_button_sample_freq_value(const double f) {
  qDebug() << "update gui get and set sample frequency from lib" << f;
}

void adujoblist::on_sensor_names_changed(const QStringList &sensor_names) {
  qDebug() << "-----> adujoblist::on_sensor_names_changed GUI getting sensors" << sensor_names;
}

void adujoblist::on_selectable_frequencies_changed(const std::vector<double> &selectable_frequencies) {

  this->selectable_frequencies = selectable_frequencies;
#ifndef QT_NO_DEBUG_OUTPUT
  QDebug dbg(QtDebugMsg);
  dbg << "joblist getting all possible frequencies ";
  for (auto &v : this->selectable_frequencies)
    dbg << v << " ";
  qDebug() << Qt::endl;
#endif

  // need transort to GUI or other
  emit this->selectable_frequencies_changed(this->selectable_frequencies);
}

void adujoblist::on_channel_map_changed(const QMap<QString, int> &channel_map) {
  this->channel_map = channel_map;
  qDebug() << "-----> adujoblist::on_channel_map_changed:";
  qDebug() << this->channel_map;

  for (auto &job : jobs) {
    QMapIterator<QString, int> mapdat(this->channel_map);
    while (mapdat.hasNext()) {
      mapdat.next();

      job.set("channel_type", mapdat.key(), mapdat.value(), false);
    }
  }
}

bool adujoblist::set_save_absoloute_basedir_url(const QString &basedir, const QUrl &sysurl) {
  if (basedir.size()) {
    this->top_jobdir.setPath(basedir);
    if (!this->top_jobdir.exists()) {
      return this->top_jobdir.mkdir(this->top_jobdir.absolutePath());
    }
  } else if (sysurl.isLocalFile()) {
    this->top_jobdir.setPath(QDir::cleanPath(sysurl.toLocalFile()));
    if (!this->top_jobdir.exists()) {
      return this->top_jobdir.mkdir(top_jobdir.absolutePath());
    }
  }

  // ... url

  return false;
}
