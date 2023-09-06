#include "adulib.h"

adulib::adulib(qint64 grid_time, QObject *parent) :
    QAbstractTableModel(parent) {

  this->values_needs_individual_channel_setting << "dac_val"
                                                << "gain_stage1"
                                                << "gain_stage2"
                                                << "ts_lsb"
                                                << "active"
                                                << "xmlcal_channel"
                                                << "xmlcal_sensor";
  this->values_needs_mapping_channel_setting << "pos_x1"
                                             << "pos_x2"
                                             << "pos_y1"
                                             << "pos_y2"
                                             << "pos_z1"
                                             << "pos_z2"
                                             << "channel_type"
                                             << "ext_gain"
                                             << "sensor_type"
                                             << "ats_data_file"
                                             << "sensor_sernum"
                                             << "length"
                                             << "North"
                                             << "East"
                                             << "West"
                                             << "South";
  this->items_want_change_for_all_jobs = values_needs_mapping_channel_setting;
  this->start_time.setTimeSpec(Qt::UTC);
  this->start_time.setSecsSinceEpoch(0);
  this->duration = 1;
  this->index = 0;
  this->delta_start = 0;
  this->min_delta_start = min_delta_low;

  this->grid_time = grid_time;

  // create an empty XML for creating templates
  this->measdoc_template = std::make_unique<measdocxml>(this);

  this->atswriter_comments = this->measdoc_template->get_qmap("atswriter_comments");
  this->items_want_change_for_all_jobs.append(this->atswriter_comments.keys());

  this->atswriter_configuration = this->measdoc_template->get_qmap("atswriter_configuration");
  this->items_want_change_for_all_jobs.append(this->atswriter_configuration.keys());

  this->atswriter_output_file = this->measdoc_template->get_qmap("atswriter_output_file");
  this->items_want_change_for_all_jobs.append(this->atswriter_output_file.keys());

  this->selftest_result.insert("AdbBoards", QList<QVariant>());
  //    this->selftest_result.insert("Boards", QList<QVariant>()); // this is not actually in a single element
  this->selftest_result.insert("Limits", QList<QVariant>());

  this->tmp_recording.insert("start_time", QString("empty"));
  this->tmp_recording.insert("stop_time", QString("empty"));
  this->tmp_recording.insert("start_date", QString("empty"));
  this->tmp_recording.insert("stop_date", QString("empty"));
  this->tmp_recording.insert("stop_date", QString("empty"));
  this->tmp_recording.insert("target_directory", QString("/mtdata/data"));
  this->tmp_recording.insert("CyclicEvent", int(0));
  this->tmp_recording.insert("Cycle", int(0));
  this->tmp_recording.insert("Granularity", QString("empty"));
}

adulib::~adulib() {

  if (this->measdoc_template != nullptr)
    this->measdoc_template.reset();
  this->delete_tinyxml2();

  if (this->channel_cfg != nullptr) {
    this->channel_cfg.reset();
  }

  qDebug() << "adulib destroyed";
}

adulib::adulib(const adulib &newjob) :
    QAbstractTableModel(newjob.parent()) {
  *this = newjob;
}

adulib &adulib::operator=(const adulib &rhs) {

  if (&rhs == this)
    return *this;
  this->channels.clear();
  for (auto &chan : rhs.channels) {
    this->channels.push_back(chan);
  }

  this->start_time = rhs.start_time;
  this->duration = rhs.duration;
  this->min_delta_start = rhs.min_delta_start;
  this->delta_start = rhs.delta_start;
  this->channel_map = rhs.channel_map;
  this->sensor_names = rhs.sensor_names;
  this->values_needs_mapping_channel_setting = rhs.values_needs_mapping_channel_setting;
  this->values_needs_individual_channel_setting = rhs.values_needs_individual_channel_setting;
  this->items_want_change_for_all_jobs = rhs.items_want_change_for_all_jobs;

  this->atswriter_comments = rhs.atswriter_comments;
  this->atswriter_output_file = rhs.atswriter_output_file;
  this->atswriter_configuration = rhs.atswriter_configuration;
  this->GMS = rhs.GMS;
  this->index = rhs.index;
  this->gps_status = rhs.gps_status;
  this->system_status = rhs.system_status;
  this->recording_status = rhs.recording_status;

  this->selectable_frequencies = rhs.selectable_frequencies;
  this->resistivity_mode = rhs.resistivity_mode;

  this->special_list = rhs.special_list; // like "JLL"
  this->grid_time = rhs.grid_time;
  return *this;
}

int adulib::rowCount(const QModelIndex & /*parent*/) const {
  if (this->channels.size())
    return channels.at(0).get_data_size();

  return 0;
}

int adulib::columnCount(const QModelIndex & /*parent*/) const {
  return (int)channels.size();
  // return (int) this->size();
}

QVariant adulib::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {

    // qDebug() << " ---->"  channels[index.column()].get_data_from_index(index.row());

    return channels[index.column()].get_data_from_index(index.row());
  } else
    return QVariant();
}

QVariant adulib::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    if ((channels.at(0).get_Type("adbboard") == 10) && (channels.at(5).get_Type("adbboard") == 11)) {
      if (section < 5)
        return QString("channel %1").arg(section) + QString(" / slot %1").arg(section);
      else
        return QString("channel %1").arg(section - 5) + QString(" / slot %1").arg(section);
    } else {
      return QString("channel %1").arg(section) + QString(" / slot %1").arg(section);
    }
  }

  return channels.at(0).get_name_from_index(section);
}

Qt::ItemFlags adulib::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool adulib::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid() || role != Qt::EditRole)
    return false;

  for (auto &chan : channels) {
    if (this->set(chan.get_name_from_index(index.row()), value, index.column())) {
      emit this->QTableViewChanged(chan.get_name_from_index(index.row()), value, index.column(), -1, false);
      return true;
    }
  }
  return true;
}

// bool adulib::operator<(const adulib &rhs) const {
//   return (this->start_time < rhs.start_time);
// }

quint64 adulib::operator-(const adulib &rhs) const {

  return (this->get_stop_time().secsTo(rhs.get_start_time()));
}

// bool adulib::operator==(const adulib &rhs) {
//   return (this->start_time == rhs.start_time);
// }

// bool adulib::operator !=(const adulib &rhs)
// {
//     return (this->start_time != rhs.start_time);
// }

size_t adulib::size() const {

  if ((GMS == 7)) {

    // if channel types are LF and HF we must have ADU-07
    if ((channels.at(0).get_Type("adbboard") == 10) && (channels.at(5).get_Type("adbboard") == 11)) {
      // .. and we devide by two
      return this->channels.size() / 2;
    }
  }
  // we can have ADU-08 or a ADU-07 of all HF of all LF
  return this->channels.size();
}

size_t adulib::total_slots() const {
  return this->channels.size();
}

QVariant adulib::get(const QString &what, const int &ichan) const
//! @todo mutex if delete is active
{
  if (this->atswriter_comments.contains(what)) {
    return this->atswriter_comments.value(what);
  } else if (this->atswriter_output_file.contains(what)) {
    return this->atswriter_output_file.value(what);
  } else if (this->atswriter_configuration.contains(what)) {
    return this->atswriter_configuration.value(what);
  } else if (this->recording_status.contains(what)) {
    return this->recording_status.value(what);
  } else if (this->system_status.contains(what)) {
    return this->system_status.value(what);
  } else if (this->gps_status.contains(what)) {
    return this->gps_status.value(what);
  } else if (this->selftest_result.contains(what)) {
    return this->selftest_result.value(what);
  }

  else {

    if (!what.compare("sample_freq")) {
      // return the first active board
      for (auto &chan : channels) {
        if (chan.is_active())
          return chan.get(what).toDouble();
      }
    }

    if (ichan == -1) {
      for (auto &chan : channels) {
        if (chan.is_active())
          return chan.get(what);
      }
    }

    if (ichan < 0 || ichan >= (int)this->channels.size()) {
      qDebug() << ichan << "does not match channels! size:" << this->channels.size();
      return false;
    }
    return channels[ichan].get(what);
  }
}

bool adulib::set(const QString &what, const QVariant &value, const int &ichan, const bool emits) {
  // qDebug() << "##mo##" << what << value << ichan;
  bool ok;

  if (this->atswriter_comments.contains(what)) {
    this->atswriter_comments.insert(what, value.toString());
    if (emits) {
      emit this->QTableViewChanged(what, value, 0, -1, false);
    }
    return true;
  }
  if (this->atswriter_output_file.contains(what)) {
    this->atswriter_output_file.insert(what, value.toString());
    if (emits) {
      emit this->QTableViewChanged(what, value, 0, -1, false);
    }
    return true;
  }

  if (this->atswriter_configuration.contains(what)) {
    this->atswriter_configuration.insert(what, value.toString());
    if (emits) {
      emit this->QTableViewChanged(what, value, 0, -1, false);
    }
    return true;
  }

  if (this->gps.contains(what)) {
    this->gps.set(what, value);
  }

  if (this->txm.contains(what)) {
    this->txm.set(what, value);
  }

  // protect channel name aliasing
  if (QString(what).contains("channel_type")) {
    if (this->channel_map.contains(value.toString())) {
      // qDebug() << "channel type already exists";
      return false;
    }
  }
  // logic comes here

  if (!what.compare("read_hwcfg_db_jobfile")) {
    for (auto &chan : channels) {
      chan.set(what, value);
    }
  }

  if (!what.compare("sample_freq")) {

    QVariant tmp_sample_freq = this->get("sample_freq", 0);
    if (tmp_sample_freq != value) {
      // for all channels
      // do not emit here but check at set_min_delta_start
      for (auto &chan : channels) {
        chan.set(what, value);
      }

      double fsum = 0;
      for (auto &chan : channels) {
        fsum += chan.get(what).toDouble(&ok);
        if (!ok)
          qFatal("checking frequency in set / adulib failed!!");
      }

      // all channels have no frequency
      if (fsum < 0.0001) {
        qDebug() << "ooops adu frequencies are off, check for other";
        for (auto &chan : channels) {
          std::vector<double> ff(chan.get_avail_board_frequencies(true));
          if (ff.size()) {
            fsum = ff.at(0);
          }
        }
        for (auto &chan : channels) {
          chan.set(what, QVariant(fsum));
        }
      }
      this->set_min_delta_start(emits);

      return true;
    } else
      return true;
  }

  else if (this->GMS == 8) {
    if ((!this->values_needs_mapping_channel_setting.contains(what)) && (!this->values_needs_individual_channel_setting.contains(what))) {
      for (auto &chan : channels) {
        chan.set(what, value);
      }
      return true;
    }

    // for all channels
    if (ichan < 0) {
      for (auto &chan : channels) {
        chan.set(what, value);
      }
      return true;
    }

    if ((ichan >= 0) && (ichan < (int)channels.size())) {
      channels[ichan].set(what, value);
      if (QString(what).contains("channel_type")) {
        this->update_channel_map(emits);
      }
      return true;
    }
    return false;
  }

  //    //for all channels
  // some channels will handle errornous setting self responsible
  // this is for filters where we try to set all the same
  else if ((!this->values_needs_mapping_channel_setting.contains(what)) && (!this->values_needs_individual_channel_setting.contains(what))) {
    for (auto &chan : channels) {
      chan.set(what, value);
    }

    return true;
  }

  //! @todo check for 3 H ADU and so on
  else {

    if (ichan < 0 || ichan > (int)this->channels.size()) {
      qDebug() << ichan << "does not match channels! size:" << this->channels.size();
      return false;
    }

    if ((channels.size() > 5) && (this->values_needs_mapping_channel_setting.contains(what))) {
      if ((channels.at(0).get_Type("adbboard") == 10) && (channels.at(5).get_Type("adbboard") == 11)) {
        if (ichan > 4) {
          channels[ichan].set(what, value);
          channels[ichan - channels.size() / 2].set(what, value);
        } else {
          channels[ichan].set(what, value);
          channels[ichan + channels.size() / 2].set(what, value);
        }
      }
      if (QString(what).contains("channel_type")) {
        this->update_channel_map(emits);
      }
      return true;
    }
  }

  return true;
}

double adulib::get_dipole_length(const int &ichan) {
  double pos_x1 = this->get("pos_x1", ichan).toDouble();
  double pos_y1 = this->get("pos_y1", ichan).toDouble();
  double pos_z1 = this->get("pos_z1", ichan).toDouble();
  double pos_x2 = this->get("pos_x2", ichan).toDouble();
  double pos_y2 = this->get("pos_y2", ichan).toDouble();
  double pos_z2 = this->get("pos_z2", ichan).toDouble();

  double tx = pos_x2 - pos_x1;
  double ty = pos_y2 - pos_y1;
  double tz = pos_z2 - pos_z1;
  return sqrt(tx * tx + ty * ty + tz * tz);
}

QMap<QString, int> adulib::get_channel_map() const {
  return this->channel_map;
}

QString adulib::get_sample_file_string() const {
  double dsample = this->get_sample_freq();
  QString sample_file_string;
  int sample_freq_h, sample_freq_s;
  if (dsample >= 1.0) {
    sample_freq_h = (int)dsample;
    sample_freq_s = 0;
    sample_file_string = QString::number(sample_freq_h) + "H";

  } else {
    sample_freq_h = 0;
    sample_freq_s = (int)(1. / dsample);
    sample_file_string = QString::number(sample_freq_s) + "S";
  }

  return sample_file_string;
}

bool adulib::fetch_hwcfg(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {

  if (this->hwcfg == nullptr)
    this->hwcfg = std::make_shared<measdocxml>(this);

  bool success = false;

  if (!this->hwcfg->open(qfi, qurl, qba)) {
    qFatal("adulib::fetch_hwcfg file or url NOT FOUND or FAILED to OPEN properly");
    return false;
  }

  QString root_node_name;
  success = this->hwcfg->root_attributes(this->GMS, this->Type, this->Version, this->Name, root_node_name);

  if (!success) {

    qFatal("adulib::fetch_hwcfg file or url NOT FOUND or FAILED to OPEN properly");
  }

  // init the max channels - the will be switched off in case not existing

  if (this->GMS == 7) {

    qDebug() << "init ADU07 from HWConfig";
    // that implies the copy constructor
    for (size_t i = 0; i < 10; ++i) {
      channels.push_back(channel(i));
    }
  } else if (this->GMS == 8) {
    qDebug() << "init ADU08 from HWConfig, 10 (partly virtual) channels";
    // that implies the copy constructor
    for (size_t i = 0; i < 10; ++i) {
      channels.push_back(channel(i));
    }
  } else if (this->GMS == 9) {
    qDebug() << "init ADU09 from HWConfig, 5 (lf only) channels";
    // that implies the copy constructor
    for (size_t i = 0; i < 5; ++i) {
      channels.push_back(channel(i));
    }
  } else {
    std::cerr << "adulib::fetch_cfg no GMS version in HWconfig" << std::endl;
    return false;
  }

  // load the ALIAS tables

  QStringList hs_alias;
  QList<int> hinput_divs;
  QStringList hs = this->get_H_sensors(&hs_alias, &hinput_divs);

  QStringList es_alias;
  QList<int> einput_divs;
  QStringList es = this->get_E_sensors(&es_alias, &einput_divs);

  for (auto &ch : channels) {
    ch.init_aliases(es, es_alias, einput_divs);
    ch.init_aliases(hs, hs_alias, hinput_divs);
  }

  if (root_node_name == "measurement") {
    this->set("read_hwcfg_db_jobfile", adudefs::adulib_source::jobfile, 0, false);
  } else if (root_node_name == "HwConfig") {
    this->set("read_hwcfg_db_jobfile", adudefs::adulib_source::database, 0, false);
  } else {
    this->set("read_hwcfg_db_jobfile", adudefs::adulib_source::unknown, 0, false);
  }

  if (!this->channels.size()) {
    qDebug() << "no channels loaded";
    exit(0);
  }

  return true;
}

bool adulib::fetch_hwcfg_virtual(const size_t n_channels, const double &sample_freq, const int &Type, const QString &Version, const QString &Name) {
  this->Type = Type;
  this->Name = Name;
  this->Version = Version;
  this->GMS = 8;
  for (size_t i = 0; i < n_channels; ++i) {
    channels.push_back(channel(i));
  }

  QStringList hs_alias;
  QList<int> hinput_divs;
  QStringList hs = this->get_H_sensors(&hs_alias, &hinput_divs);

  QStringList es_alias;
  QList<int> einput_divs;
  QStringList es = this->get_E_sensors(&es_alias, &einput_divs);

  for (auto &ch : channels) {
    ch.init_aliases(es, es_alias, einput_divs);
    ch.init_aliases(hs, hs_alias, hinput_divs);
  }
  QMap<QString, QVariant> dat;
  dat.insert("GMS", int(8));
  dat.insert("RevMain", int(1));
  dat.insert("RevSub", int(0));
  dat.insert("Type", int(13));
  dat.insert("Serial", int(0));

  int i = 1;
  for (auto &ch : channels) {
    ch.set_GMS("adbboard", dat.value("GMS").toInt());
    ch.set_RevMain("adbboard", dat.value("RevMain").toInt());
    ch.set_RevSub("adbboard", dat.value("RevSub").toInt());
    ch.set_Type("adbboard", dat.value("Type").toInt());
    ch.set_Serial("adbboard", i++);
  }

  dat.insert("GMS", int(7));  // sensors belong to 7 already
  dat.insert("Type", int(0)); // I don't know at this moment

  for (auto &ch : channels) {
    ch.set_GMS("sensor", dat.value("GMS").toInt());
    ch.set_RevMain("sensor", dat.value("RevMain").toInt());
    ch.set_RevSub("sensor", dat.value("RevSub").toInt());
    ch.set_Type("sensor", dat.value("Type").toInt());
    ch.set_Serial("sensor", i++);
  }
  for (auto &ch : channels) {
    ch.append_sample_freq(sample_freq);
  }

  return true;
}

bool adulib::fetch_hwdb(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {

  if (this->channels.size() == 0) {
    qFatal("adulib::fetch_hwdb NO CHANNELS, adulib::fetch_hwcfg has no channels????");
  }
  if (this->hwdb == nullptr) {
    this->hwdb = std::make_shared<measdocxml>(this);
    connect(this->hwdb.get(), &measdocxml::signal_adbboard_data_qmap_created, this, &adulib::slot_adbboard_data_qmap_created);
  }
  if (!this->hwdb->open(qfi, qurl, qba))
    return false;

  return true;
  emit state_changed();
}

bool adulib::is_submitted_adujobtable() const {
  return this->bool_is_submitted_adujobtable;
}

void adulib::submitted_to_adu_jobtable(const bool submitted_true_false) {
  this->bool_is_submitted_adujobtable = submitted_true_false;
}

bool adulib::fetch_hwstat(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {
  if (this->hwstat == nullptr) {
    this->hwstat = std::make_shared<measdocxml>(this);
    connect(this->hwstat.get(), &measdocxml::signal_gps_status_qmap_created, this, &adulib::slot_gps_status_qmap_created);
    connect(this->hwstat.get(), &measdocxml::signal_recording_status_qmap_created, this, &adulib::slot_recording_status_qmap_created);
    connect(this->hwstat.get(), &measdocxml::signal_system_status_qmap_created, this, &adulib::slot_system_status_qmap_created);
  }

  if (!this->hwstat->open(qfi, qurl, qba))
    return false;

  this->hwstat->get_hw_status();

  //    qDebug() << "adulib::fetch_hwstat: +++++++++++++++++++++++++++++++++++";
  //    qDebug() << "GPS:";
  //    qDebug() << this->gps_status;
  //    qDebug() << "Recording:";
  //    qDebug() << this->recording_status;
  //    qDebug() << "System:";
  //    qDebug() << this->system_status;
  //    qDebug() << "adulib::fetch_hwstat finish ----------------------------";

  emit state_changed();
  return true;
}

QMap<QString, QVariant> adulib::fetch_selftest(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {
  // mbk str:
  //  The UI is currently only interested in an aggregated result of the SelfTest.
  //  The result will be viasualized as a colored button on the home screen.
  //  STR_state:  0: (Crit, Red), 1: (Warn, Yellow), 2: (OK, green)
  //  This should probably be computed from <AdbError> and <InitError> tags.
  //  Suggestions:
  //  - provide a getter for STR_state that can be called after fetch_selftest
  //  - QVariantMap and QVariantList can be encapsulated in a single QVariant
  //  - split up eyh
  QMap<QString, QVariant> selftest_result;
  int i = 0;
  QList<QMap<QString, QVariant>> channel_data;

  // start with good
  selftest_result.insert("selftest_summary", QVariant::fromValue(2));

  auto xmldoc(std::make_unique<measdocxml>(qfi, qurl, qba));
  // pre-define and set they types
  for (auto &ch : this->channels) {
    channel_data.append(ch.get_selftest_template());
  }
  for (auto &ch : this->channels) {
    xmldoc->get_channel_section("Selftest", channel_data, ch.get_slot(), true);
  }
  i = 0;
  for (auto &ch : this->channels) {
    ch.fetch_selftest_part(channel_data.at(i++));
  }

  // fetch_selftest_part(const QMap<QString, QVariant> &xmlpart);

  qDebug() << "++++++++++++ SELFTEST +++++++++++++++++++++++++++++++++";
  int severity_errors = 2;
  int resistivityerrors = 2;
  i = 0;
  // (0: crit, 1: fair, 2: good)

  // Rhoden has DC offset 20; in resistivity < 200
  for (auto &channel : this->channels) {
    if (channel.is_active()) {
      if (channel.get_selftest("Severity").toString().contains("Critical")) {
        severity_errors = 0;
      }
      if (channel.get_selftest("Severity").toString().contains("Warning")) {
        if (severity_errors != 0)
          severity_errors = 1;
      }
      // check resistivity and DC offset and anisotropy
      if (channel.get("channel_type").toString().startsWith("E")) {

        if (channel.get_selftest("Resistivity").toDouble() < 0.01) {
          resistivityerrors = 1;
        } else if (!this->resistivity_mode.compare("low") &&
                   ((channel.get_selftest("Resistivity").toDouble() > 2000) ||
                    (channel.get_selftest("DCOffset").toDouble() > 20))) {
          resistivityerrors = 1;
        } else if (!this->resistivity_mode.compare("normal") && ((channel.get_selftest("Resistivity").toDouble() > 5000) ||
                                                                 (channel.get_selftest("DCOffset").toDouble() > 50))) {
          resistivityerrors = 1;
        } else if (!this->resistivity_mode.compare("high") &&
                   ((channel.get_selftest("Resistivity").toDouble() > 10000) ||
                    (channel.get_selftest("DCOffset").toDouble() > 90))) {
          resistivityerrors = 1;
        }
      }
      if (channel.get_selftest("DCOffset").toDouble() > 120) {
        resistivityerrors = 0;
      }
    }
  }

  selftest_result.insert("selftest_summary", QVariant::fromValue(severity_errors));

  // if other checks are worse, tell it
  if (resistivityerrors < severity_errors)
    selftest_result.insert("selftest_summary", QVariant::fromValue(resistivityerrors));

  // #ifndef QT_NO_DEBUG
  //     if (QDateTime::currentDateTime().toMSecsSinceEpoch() % 10000 > 5000) {
  //         selftest_result.insert("selftest_summary", QVariant::fromValue(2));
  //         qDebug() << "OVERRIDE STR COLOR!!!!!!!!!!!!!!!!!!!!!!!!";
  //     }
  // #endif

  this->selftest_result = selftest_result;
  emit state_changed();

  return selftest_result;
}

QMap<QString, QVariant> adulib::fetch_global_config(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {

  auto xmldoc(std::make_unique<measdocxml>(qfi, qurl, qba));

  if (!xmldoc->is_ok()) {
    return QMap<QString, QVariant>();
  }
  QMap<QString, QVariant> tmpmap;
  tmpmap.insert("sample_freq", double(0.0));
  if (!xmldoc->get_section("input", "global_config", tmpmap, true)) {
    return QMap<QString, QVariant>();
  }

  if (tmpmap.contains("sample_freq"))
    this->set("sample_freq", tmpmap.value("sample_freq"), -1);

  return tmpmap;
}

QMap<QString, QVariant> adulib::fetch_recording(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {

  auto xmldoc(std::make_unique<measdocxml>(qfi, qurl, qba));

  if (!xmldoc->is_ok()) {
    return QMap<QString, QVariant>();
  }
  QMap<QString, QVariant> tmpmap(this->tmp_recording);
  if (!xmldoc->get_section("measurement", "recording", tmpmap, true)) {
    return QMap<QString, QVariant>();
  }
  this->tmp_recording = tmpmap;
  QDateTime qdtstart(QDate::fromString(this->tmp_recording.value("start_date").toString(), "yyyy-MM-dd"), QTime::fromString(this->tmp_recording.value("start_time").toString(), "hh:mm:ss"), Qt::UTC);
  QDateTime qdtstop(QDate::fromString(this->tmp_recording.value("stop_date").toString(), "yyyy-MM-dd"), QTime::fromString(this->tmp_recording.value("stop_time").toString(), "hh:mm:ss"), Qt::UTC);
  qint64 lc_duration = qdtstart.secsTo(qdtstop);

  this->set_start_time(qdtstart);
  this->on_set_duration(lc_duration);
  qDebug() << "start date" << this->tmp_recording.value("start_date");
  return this->tmp_recording;
}

int adulib::fetch_atswriter_part(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {
  int i = 0;
  // QList<QMap<QString, QVariant> > channel_data;
  std::unique_lock<std::mutex> l(this->lock);

  if (this->channel_cfg == nullptr) {
    this->channel_cfg = std::make_unique<measdocxml>(qfi, qurl, qba);
  } else {
    this->channel_cfg->close();
    this->channel_cfg->open(qfi, qurl, qba);
  }
  // pre-define and set they types
  for (auto &ch : this->channels) {
    connect(this->channel_cfg.get(), &measdocxml::signal_got_atswriter_channel, &ch, &channel::slot_fetch_atswriter);
    connect(this->channel_cfg.get(), &measdocxml::signal_got_adbboard_autogain_channel, &ch, &channel::slot_adbboard_autogains);
    connect(this->channel_cfg.get(), &measdocxml::signal_got_adbboard_autogain_channel, &ch, &channel::slot_act_gains);

    //       channel_data.append(ch.get_atswriter_template());
  }

  connect(this->channel_cfg.get(), &measdocxml::signal_atswriter_configuration_qmap_created, this, &adulib::slot_atswriter_configuration_qmap_created);
  connect(this->channel_cfg.get(), &measdocxml::signal_atswriter_comments_qmap_created, this, &adulib::slot_atswriter_comments_qmap_created);
  connect(this->channel_cfg.get(), &measdocxml::signal_atswriter_output_file_qmap_created, this, &adulib::slot_atswriter_output_file_qmap_created);

  this->channel_cfg->get_atswriter_section();

  // we should have all boards at this momenent
  this->channel_cfg->get_adbboard_autgains();

  //    for (auto &ch: this->channels) {
  //        xmldoc->get_channel_section("ATSWriter", channel_data, ch.get_slot(), true);
  //    }
  i = 0;
  //    for (auto &ch: this->channels) {
  //        ch.fetch_atswriter_part(channel_data.at(i));
  //    }

  //! @todo do I need this what about HF/LF and BB
  //    for (auto &ch: this->channels) {
  //    channel_data.append(ch.fetch_atswriter(qfi, qurl));
  //    ++i;
  //    }
  //    for (auto &tmpmap : channel_data) {
  //        if (tmpmap.contains("channel")) {
  //            qDebug() << "adulib::fetch_atswriter_part -> apply for complete adu from channel" << tmpmap.value("channel");
  //            if (tmpmap.contains("pos_x1")) this->set("pos_x1", tmpmap.value("pos_x1"), tmpmap.value("channel").toInt());
  //            if (tmpmap.contains("pos_x2")) this->set("pos_x2", tmpmap.value("pos_x2"), tmpmap.value("channel").toInt());
  //            if (tmpmap.contains("pos_y1")) this->set("pos_y1", tmpmap.value("pos_y1"), tmpmap.value("channel").toInt());
  //            if (tmpmap.contains("pos_y2")) this->set("pos_y2", tmpmap.value("pos_y2"), tmpmap.value("channel").toInt());
  //            if (tmpmap.contains("pos_z1")) this->set("pos_z1", tmpmap.value("pos_z1"), tmpmap.value("channel").toInt());
  //            if (tmpmap.contains("pos_z2")) this->set("pos_z2", tmpmap.value("pos_z2"), tmpmap.value("channel").toInt());
  //        }
  //    }

  // make it available for all boards and channels

  return i;
}

void adulib::fetch_autogains_part(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {

  std::unique_lock<std::mutex> l(this->lock);

  if (this->channel_cfg == nullptr) {
    this->channel_cfg = std::make_unique<measdocxml>(qfi, qurl, qba);
  } else {
    this->channel_cfg->close();
    this->channel_cfg->open(qfi, qurl, qba);
  }

  for (auto &ch : this->channels) {
    connect(this->channel_cfg.get(), &measdocxml::signal_got_adbboard_autogain_channel, &ch, &channel::slot_act_gains);
  }

  this->channel_cfg->get_adbboard_autgains();
}

void adulib::build_channels() {
  // that is done i in fetch_cfg
  // cfgroot = cfg->FirstChildElement();

  size_t i = 0;
  QMap<QString, QVariant> dat;
  dat.insert("GMS", int(0));
  dat.insert("RevMain", int(0));
  dat.insert("RevSub", int(0));
  dat.insert("Type", int(0));
  dat.insert("Serial", int(0));

  QList<QMap<QString, QVariant>> data;

  for (i = 0; i < this->channels.size(); ++i) {
    data.append(dat);
  }

  for (auto &ch : channels) {
    qDebug() << "hwcfg adbboard channel section" << ch.get_slot();
    this->hwcfg->get_channel_section("AdbBoards", data, ch.get_slot(), false);
  }

  i = 0;
  for (auto &ch : channels) {

    ch.set_GMS("adbboard", data.at(i).value("GMS").toInt());
    ch.set_RevMain("adbboard", data.at(i).value("RevMain").toInt());
    ch.set_RevSub("adbboard", data.at(i).value("RevSub").toInt());
    ch.set_Type("adbboard", data.at(i).value("Type").toInt());
    ch.set_Serial("adbboard", data.at(i).value("Serial").toInt());
    ++i;
  }

  data.clear();

  for (i = 0; i < this->channels.size(); ++i) {
    data.append(dat);
  }

  for (auto &ch : channels) {
    this->hwcfg->get_channel_section("Sensors", data, ch.get_slot(), false);
  }

  i = 0;
  for (auto &ch : channels) {

    ch.set_GMS("sensor", data.at(i).value("GMS").toInt());
    ch.set_RevMain("sensor", data.at(i).value("RevMain").toInt());
    ch.set_RevSub("sensor", data.at(i).value("RevSub").toInt());
    ch.set_Type("sensor", data.at(i).value("Type").toInt());
    ch.set_Serial("sensor", data.at(i).value("Serial").toInt());
    ++i;
  }

  for (auto &chan : channels) {

    chan.set("sensor_sernum", chan.get_Serial("sensor"));
  }

  qDebug() << "channel build done";
}

int adulib::slot_adbboard_data_qmap_created(const QMap<QString, QVariant> &data_map) {
  QString classname;
  QString strval;
  QStringList strvals;
  double dval;
  int type, revmain, revsub, gms;
  gms = data_map.value("GMS").toInt();
  revmain = data_map.value("RevMain").toInt();
  revsub = data_map.value("RevSub").toInt();
  type = data_map.value("Type").toInt();

  qDebug() << "Name:" << data_map.value("Name").toString();

  for (auto &chan : channels) {
    if (chan.has_board(type, revmain, revsub, gms, classname)) {
      if (!QString::compare(classname, "sensor")) { // TODO: @bfr the sensor names were missing, is this the right solution?
        chan.set_Name("sensor", data_map.value("Name").toString());
      }
      if (!QString::compare(classname, "adbboard")) {
        if (data_map.contains("SampleFreqs")) {
          strval = data_map.value("SampleFreqs").toString();
          strvals = strval.split(",");
          qDebug() << "set sample freqs";
          for (auto &val : strvals) {
            val = val.trimmed();
            dval = val.toDouble();
            if ((int)dval == 4096 && gms == 7 && type == 11) {
            } else {
              chan.append_sample_freq(dval);
            }
          }
        }
        if (data_map.contains("Filters")) {
          strval = data_map.value("Filters").toString();
          strvals = strval.split(",");
          qDebug() << "set filters";
          for (auto &val : strvals) {
            val = val.trimmed();
            chan.append_filter(val);
          }
        }
        if (data_map.contains("Filters_LF")) {
          strval = data_map.value("Filters_LF").toString();
          strvals = strval.split(",");
          qDebug() << "set filters";
          for (auto &val : strvals) {
            val = val.trimmed();
            chan.append_filter_lf(val);
          }
        }
        if (data_map.contains("Filters_HF")) {
          strval = data_map.value("Filters_HF").toString();
          strvals = strval.split(",");
          qDebug() << "set filters";
          for (auto &val : strvals) {
            val = val.trimmed();
            chan.append_filter_hf(val);
          }
        }

        if (data_map.contains("GainsStage1")) {
          strval = data_map.value("GainsStage1").toString();
          strvals = strval.split(",");
          qDebug() << "set gain stages 1";
          for (auto &val : strvals) {
            val = val.trimmed();
            dval = val.toDouble();
            chan.append_gain_stage1(dval);
          }
        }
        if (data_map.contains("GainsStage2")) {
          strval = data_map.value("GainsStage2").toString();
          strvals = strval.split(",");
          qDebug() << "set gain stages 2";
          for (auto &val : strvals) {
            val = val.trimmed();
            dval = val.toDouble();
            chan.append_gain_stage2(dval);
          }
        }
        if (data_map.contains("DAC")) {
          strval = data_map.value("DAC").toString();
          strvals = strval.split(",");
          qDebug() << "set DACs";
          for (auto &val : strvals) {
            val = val.trimmed();
            dval = val.toDouble();
            chan.append_dac(dval);
          }
        }
      }
    }
  }

  return 1;
}

void adulib::slot_atswriter_comments_qmap_created(const QMap<QString, QVariant> &data_map) {
  auto dm = data_map.constBegin();
  while (dm != data_map.constEnd()) {
    this->atswriter_comments.insert(dm.key(), dm.value());
    ++dm;
  }
}

void adulib::slot_atswriter_configuration_qmap_created(const QMap<QString, QVariant> &data_map) {
  auto dm = data_map.constBegin();
  while (dm != data_map.constEnd()) {
    this->atswriter_configuration.insert(dm.key(), dm.value());
    ++dm;
  }
}

void adulib::slot_atswriter_output_file_qmap_created(const QMap<QString, QVariant> &data_map) {
  auto dm = data_map.constBegin();
  while (dm != data_map.constEnd()) {
    this->atswriter_output_file.insert(dm.key(), dm.value());
    ++dm;
  }
}

void adulib::slot_gps_status_qmap_created(const QMap<QString, QVariant> &data_map) {

  auto dm = data_map.constBegin();
  while (dm != data_map.constEnd()) {
    this->gps_status.insert(dm.key(), dm.value());
    ++dm;
  }
  QDateTime qdtgps(QDate::fromString(this->gps_status.value("Date").toString(), "yyyy-MM-dd"), QTime::fromString(this->gps_status.value("Time").toString(), "hh:mm:ss"), Qt::UTC);
  this->gps_status.insert("GPSDateTime", QDateTime(qdtgps));
  this->coordinates.set_lat_lon_msec(this->gps_status.value("Latitude").toInt(), this->gps_status.value("Longitude").toInt(), this->gps_status.value("Height").toDouble() / 100.);
  QString qslon;
  QString qslat;
  this->coordinates.get_lat_lon_str(qslat, qslon);
  this->gps_status.insert("Longitude_str", qslon);
  this->gps_status.insert("Latitude_str", qslat);
  this->gps_status.insert("Elevation_str", QString::number(this->gps_status.value("Height").toInt() / 100) + " m");

  // detailed information is
  // ALL Satellites("NumSats", int (0));
  // ("NumSatsGPS", int (0));
  // ("NumSasGalileo", int (0));
  // ("NumSatsBeiDou", int (0));
  // ("NumSatsGlonass", int (0));

  // (0: crit, 1: fair, 2: good)
  if (this->gps_status.value("SyncState").toInt() == 4) {
    this->gps_status.insert("SyncState_str", "G4Fix (ready to record)");
    this->gps_status.insert("gps_status_btn", 2);
  } else if (this->gps_status.value("SyncState").toInt() == 3) {
    this->gps_status.insert("SyncState_str", "G3Fix (wait for G4Fix please)");
    this->gps_status.insert("gps_status_btn", 1);

  } else if (this->gps_status.value("SyncState").toInt() == 2) {
    this->gps_status.insert("SyncState_str", "G2 (wait for G4Fix please)");
    this->gps_status.insert("gps_status_btn", 0);

  } else if (this->gps_status.value("SyncState").toInt() == 1) {
    this->gps_status.insert("SyncState_str", "G1 (wait for G4Fix please)");
    this->gps_status.insert("gps_status_btn", 0);

  } else if (this->gps_status.value("SyncState").toInt() == 0) {
    this->gps_status.insert("SyncState_str", "searching Satellites, wait please");
    this->gps_status.insert("gps_status_btn", 0);

  } else {
    this->gps_status.insert("SyncState_str", "unknown status");
    this->gps_status.insert("gps_status_btn", 0);
  }

  switch (this->gps_status.value("DynamicMode").toInt()) {
  case 0:
    this->gps_status.insert("DynamicMode_str", "stationary");
    break;

  case 1:
    this->gps_status.insert("DynamicMode_str", "moving");
    break;

  default:
    this->gps_status.insert("DynamicMode_str", "unknown");
    break;
  }
}

void adulib::slot_recording_status_qmap_created(const QMap<QString, QVariant> &data_map) {
  auto dm = data_map.constBegin();
  while (dm != data_map.constEnd()) {
    this->recording_status.insert(dm.key(), dm.value());
    ++dm;
  }
}

void adulib::slot_system_status_qmap_created(const QMap<QString, QVariant> &data_map) {
  auto dm = data_map.constBegin();
  while (dm != data_map.constEnd()) {
    this->system_status.insert(dm.key(), dm.value());
    ++dm;
  }
}

void adulib::slot_on_keep_grid(const bool &is_checked) {
  if (is_checked)
    this->keep_grid = true;
  else
    this->keep_grid = false;
}

void adulib::build_hardware() {
  //!< @todo setup for 08 system

  this->hwdb->get_adbboard("Board", false); // expects slot to be called before continuing

  // this->check_gms_revmain_revsub(gms, type, revmain, revsub);
  this->map_xml_channels();

  // set EFP-06 and MFS-06 depending on the slot
  for (auto &chan : channels)
    chan.replace_UNKN_E();
  for (auto &chan : channels)
    chan.set_dac_steps();

  if (this->GMS == 7) {
    for (auto &chan : channels)
      chan.set("filter_type", "ADU07_LF_RF_4");
    for (auto &chan : channels)
      chan.set("filter_type", "ADU07_HF_HP_1HZ");
  }

  size_t i = 0;
  for (auto &chan : channels) {
    //!< @todo prepare for EMAP

    if ((chan.get_Name("sensor").contains("MFS", Qt::CaseInsensitive)) ||
        (chan.get_Name("sensor").contains("FGS", Qt::CaseInsensitive)) ||
        (chan.get_Name("sensor").contains("SHFT", Qt::CaseInsensitive)) ||
        (chan.get_Name("sensor").contains("BF", Qt::CaseInsensitive)) ||
        (chan.get_Name("sensor").contains("MTC", Qt::CaseInsensitive))) {
      if (i == 2 || i == 7)
        chan.set("channel_type", "Hx");
      if (i == 3 || i == 8)
        chan.set("channel_type", "Hy");
      if (i == 4 || i == 9)
        chan.set("channel_type", "Hz");
    }

    if ((chan.get_Name("sensor").contains("EFP", Qt::CaseInsensitive)) ||
        (chan.get_Name("sensor").contains("ROD", Qt::CaseInsensitive)) ||
        (chan.get_Name("sensor").contains("PLATE", Qt::CaseInsensitive)) ||
        (chan.get_Name("sensor").contains("BUFFER", Qt::CaseInsensitive))) {
      if (i == 0 || i == 5)
        chan.set("channel_type", "Ex");
      if (i == 1 || i == 6)
        chan.set("channel_type", "Ey");
    }
    ++i;
  }

  std::vector<double> pfreq;
  std::vector<double> tmpfreq;
  for (auto &chan : channels) {
    tmpfreq = chan.get_avail_board_frequencies(true);

    for (auto &xfreq : tmpfreq) {
      if (std::find(pfreq.begin(), pfreq.end(), xfreq) != pfreq.end()) {

      } else {
        pfreq.push_back(xfreq);
      }
    }
  }

  // qDebug() << pfreq.size();

  // this activates max 5 or 6 channels - and that does the trick !!!!!!
  // this->slot_set_sample_freq(512);
  this->set("sample_freq", QVariant(double(512)), -1);

  for (auto &chan : channels) {

    if (chan.is_active())
      sensor_names << chan.get_Name("sensor");
  }
  // NOW tell about channels
  this->update_channel_map();
  emit this->sensor_names_changed(sensor_names);
  this->selectable_frequencies.clear();
  for (auto const &d : pfreq)
    this->selectable_frequencies.append(d);
  // this->selectable_frequencies = QVector<double>::fromStdVector(pfreq);
  emit this->selectable_frequencies_changed(pfreq);

  this->delete_tinyxml2();
}

QStringList adulib::info(const QString classname) const {
  size_t i = 0;
  QStringList info;
  for (auto &chan : channels) {
    QString str;
    str.append("chan ");
    str.append(QString::number(i));
    str.append(", name ");
    str.append(chan.get_Name(classname));
    str.append(", Type ");
    str.append(QString::number(chan.get_Type(classname)));
    str.append(", RevMain ");
    str.append(QString::number(chan.get_RevMain(classname)));
    str.append(", RevSub ");
    str.append(QString::number(chan.get_RevSub(classname)));
    str.append(", GMS ");
    str.append(QString::number(chan.get_GMS(classname)));
    str.append(", Serial ");
    str.append(QString::number(chan.get_Serial(classname)));

    info.append(str);
    str.clear();
    ++i;
  }

  return info;
}

QStringList adulib::info_two() {
  size_t i = 0;
  QStringList info;
  for (auto &chan : channels) {
    QString str;
    str.append("chan ");
    str.append(QString::number(i));
    str.append(", type ");
    str.append(chan.get("channel_type").toString());
    str.append(", name ");
    str.append(chan.get_Name("sensor"));

    str.append(", freq ");
    str.append(chan.get("sample_freq").toString());

    str.append(", South ");
    str.append(chan.get("pos_x1").toString());
    str.append(", North ");
    str.append(chan.get("pos_x2").toString());
    str.append(", East ");
    str.append(chan.get("pos_y2").toString());
    str.append(", West ");
    str.append(chan.get("pos_y1").toString());
    str.append(", active ");
    str.append(chan.get("active").toString());

    info.append(str);
    str.clear();
    ++i;
  }

  return info;
}

bool adulib::is_active(const int channel_no) const {
  if ((channel_no >= 0) && channel_no < (int)channels.size()) {
    return this->channels.at(channel_no).is_active();
  }
  return false;
}

QStringList adulib::get_avail_channel_types() const {
  if (this->channels.size()) {
    return channels.at(0).get_avail_channel_types();
  }

  QStringList empty("not setup yet!");
  return empty;
}

void adulib::set_start_time(const QDateTime &start_time, const bool emits) {
  if (this->start_time != start_time) {
    emit this->timing_change_attempt();

    if (this->grid_time > 1) {
      QDateTime newstart(start_time);
      // I want to round dwon here - let us see; may be easier using the app to ch up than ch down
      newstart.setSecsSinceEpoch((start_time.toSecsSinceEpoch() / grid_time) * grid_time); // will be equal or less; integer division rounds always down

      this->start_time = newstart;

    } else {
      this->start_time = start_time;
    }

    if (emits)
      emit this->start_time_changed(this->index, this->start_time, this->duration, this->get("sample_freq", 0).toDouble());
    emit this->timing_changed();
  }
}

void adulib::set_start_time_now(const int secs_from_now, const bool emits) {
  this->local_time = QDateTime::currentDateTime();
  this->local_time = this->local_time.addSecs(qint64(abs(secs_from_now)));
  this->utc_time_base = this->local_time.toUTC();
  this->utc_time_base.to_full_minute();
  this->set_start_time(this->utc_time_base, emits);
}

QDateTime adulib::set_min_start_time(const int &index, const QDateTime &previous_stop_time, const qint64 &previous_min_delta_start, const qint64 &previous_diff_secs) {

  QDateTime old_start_time = this->start_time;
  emit this->timing_change_attempt();

  qDebug() << "check index emitter is" << index << "receiver is" << this->index;
  ;

  QDateTime stops(previous_stop_time);             // get the prev stop time and properties of previous_stop_time
  stops = stops.addSecs(previous_min_delta_start); // and add the min delta start (the previous job knows his frequency and decides between min_delta_low and min_delta_high
  int secs = stops.toString("ss").toInt();

  QDateTime newstart(stops);
  newstart.setSecsSinceEpoch((stops.toSecsSinceEpoch() / grid_time) * grid_time); // will be equal or less; integer division rounds always down
  if (newstart < stops)
    newstart = newstart.addSecs(grid_time);

  //      // limit in order to avoid endless loop in case of error
  //      for (int iss = 0; iss < 10000; ++iss) {
  //          if ( (newstart.toSecsSinceEpoch() - stops.toSecsSinceEpoch()) < previous_min_delta_start) {
  //              newstart = newstart.addSecs(grid_time);
  //          }
  //          else break;
  //      }

  // change somewhere in the list so we ignore a previous grid
  if ((index > 0) && (this->index == index)) {
    if (this->start_time < newstart) {
      this->start_time = newstart;
    }
  }

  // keep the old start time if keep grid
  else if (this->keep_grid && (this->start_time < newstart)) {
    this->start_time = newstart;
  }

  // we try to enter before previous_diff_secs is negative

  //      else if (this->keep_grid && previous_diff_secs > previous_min_delta_start) {
  //          // so we keep the grid
  //          this->start_time = previous_stop_time.addSecs(previous_diff_secs);

  //          qDebug() << " aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  //          qDebug() << start_time;

  //          // ? previous job 10s longer (so no start but duration change)
  //          // want to have full minute start time
  //          secs = this->start_time.toString("ss").toInt();
  //          if( secs && (previous_stop_time.addSecs(previous_min_delta_start) > this->start_time) ) {
  //              this->start_time = start_time.addSecs(grid_time - secs);
  //              qDebug() << " bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
  //              qDebug() << start_time;
  //          }
  //          else if (secs) {

  //              qDebug() << " ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
  //              qDebug() << start_time;
  //              this->start_time = this->start_time.addSecs(-1 * secs);
  //          }
  //      }
  // set the minimum
  else {
    this->start_time = newstart;
    qDebug() << "time time time timetime timetime timetime timetime timetime timetime timetime timetime timetime timetime time";
  }

  if (old_start_time != this->start_time)
    emit this->timing_changed();
  return this->start_time;
}

QDateTime adulib::get_start_time() const {
  return this->start_time;
}

QDateTime adulib::get_stop_time() const {
  return this->start_time.addSecs(this->duration);
}

qint64 adulib::seconds_to_midnight(const qint64 offset_seconds) const {
  QDateTime midnight;
  midnight.setTimeSpec(Qt::UTC);
  midnight = start_time;
  midnight.setTime(QTime(0, 0, 0, 0));
  midnight = midnight.addSecs(86400 + offset_seconds);
  qint64 result = start_time.secsTo(midnight);
  if (result < 0)
    result += 86400;
  return result;
}

void adulib::map_xml_channels() {
  //    int gmstype = 7;
  size_t count = 0;
  //    for ( auto &chan : channels) {
  //        count += (size_t) chan.get_GMS("adbboard");
  //    }

  //    if (count == channels.size() * gmstype) {
  if (GMS == 7) {

    // ony IN2 was connected (Flugate, SHFT)
    if (channels.size() == 10) {
      if ((channels.at(2).get_Type("sensor") == 203) && (channels.at(3).get_Type("sensor") == 203) && (channels.at(4).get_Type("sensor") == 203)) {
        if ((channels.at(7).get_Type("sensor") != 203) && (channels.at(8).get_Type("sensor") != 203) && (channels.at(9).get_Type("sensor") != 203)) {
          qDebug() << "mapping ADU07 sensors form IN2 - small conns not conencted";
          size_t dcount = 5;
          for (count = 0; count < dcount; ++count) {
            channels[count].copy_item_xmlvals("sensor", channels.at(count + dcount));
          }
          for (count = 2; count < 5; ++count) {
            channels[count].set("input", 1);
          }
          for (count = 7; count < 10; ++count) {
            channels[count].set("input", 1);
          }
        }
      }
    }

    if (channels.size() == 10) {
      qDebug() << "mapping ADU07 sensors";
      size_t dcount = 5;
      for (count = 0; count < dcount; ++count) {
        channels[count + dcount].copy_item_xmlvals("sensor", channels.at(count));
      }
    } else if (channels.size() == 5) {
      qDebug() << "ADU07 channels ok = 5";
    }

  } else {
    qDebug() << "expecting ADU 8";
  }
}

void adulib::update_channel_map(bool emits) {
  QMap<QString, int> tmp_channel_map(this->channel_map);

  int i = 0;
  this->channel_map.clear();
  for (auto &chan : channels) {
    if (chan.is_active()) {
      this->channel_map.insert(chan.get("channel_type").toString(), i);
    }
    ++i;
  }

  if (emits) {
    if (this->channel_map != tmp_channel_map)
      emit this->channel_map_changed(this->channel_map);
  }
}

bool adulib::want_change_for_all_jobs(const QString &what) const {
  return this->items_want_change_for_all_jobs.contains(what);
}

int adulib::get_index() const {
  return this->index;
}

void adulib::set_index(const int index) {
  this->index = index;
}

double adulib::get_sample_freq() const {
  return this->get("sample_freq", 0).toDouble();
}

QVector<double> adulib::get_selectable_frequencies() const {
  return this->selectable_frequencies;
}

int adulib::get_active_channels() const {
  int i = 0;
  for (auto &chan : channels) {
    if (chan.is_active()) {
      ++i;
    }
  }
  return i;
}

QMap<QString, QVariant> adulib::poll_system_status() {
  return QMap<QString, QVariant>(); // mbk fix missing return values
}

bool adulib::detect_sensors(bool emits) {

  if (emits) {
    emit this->sensors_detected(true);
    return true; // mbk fix missing return values
  }
  return false; // mbk fix missing return values
}

void adulib::channel_calibration() {
  xcal.push("calibration_channels");
  for (auto &chan : channels) {
    if (chan.is_active()) {
      xcal.push("channel", "id", chan.get_slot());
      xcal.emptyelement("calibration");
      xcal.pop();
    }
  }
  xcal.pop("calibration_channels");
}

void adulib::sensor_calibration() {
  xcal.push("calibration_sensors");
  for (auto &chan : channels) {
    if (chan.is_active()) {
      xcal.push("channel", "id", chan.get_slot());

      if (chan.get("xmlcal_sensor").toString() != "empty") {
        xcal.insert(chan.get("xmlcal_sensor").toString());
      } else
        xcal.emptyelement("calibration");
      xcal.pop();
    }
  }
  xcal.pop("calibration_sensors");
}

void adulib::delete_tinyxml2() {

  if (this->hwdb != nullptr) {
    this->hwdb->close();
    this->hwdb.reset();
  }

  if (this->hwcfg != nullptr) {
    this->hwcfg->close();
    this->hwcfg.reset();
  }
}

qint64 adulib::set_delta_start(const qint64 &delta_start) {
  if (delta_start > grid_time - 1) {
    qint64 n = delta_start / grid_time;
    this->delta_start = n * grid_time;

  } else
    this->delta_start = 0;
  return this->delta_start;
}

qint64 adulib::get_delta_start() const {
  return this->delta_start;
}

qint64 adulib::get_min_delta_start() const {
  return this->min_delta_start;
}

void adulib::set_min_delta_start(const bool emits) {
  qint64 tmp = this->min_delta_start;

  if (this->GMS == 7) {
    if (this->get("sample_freq", 0).toDouble() > 66000) {
      this->min_delta_start = min_delta_high; // defined in eqdatetime
    } else {
      this->min_delta_start = min_delta_low; // defined in eqdatetime
    }
  }
  if (this->GMS == 8) {
    if (this->get("sample_freq", 0).toDouble() > 263000) {
      this->min_delta_start = min_delta_high; // defined in eqdatetime
    } else {
      this->min_delta_start = min_delta_low; // defined in eqdatetime
    }
  }

  if (this->min_delta_start != tmp)
    emit this->timing_change_attempt();
  if (emits) {
    if (this->min_delta_start != tmp)
      emit this->min_delta_start_changed(this->index, this->start_time, this->duration, this->get("sample_freq", 0).toDouble());
  }
}

bool adulib::on_write_job(const bool set_astwriter_finhished) {
  QString xmlstring = on_build_xml(set_astwriter_finhished);

  QFile xmljob(this->qfi.absoluteFilePath());

  if (!xmljob.open(QIODevice::WriteOnly | QIODevice::Text)) {
    std::cerr << "can not open " << qfi.absoluteFilePath().toStdString() << std::endl;
    return false;
  } else {
    QTextStream out(&xmljob);
    out << xmlstring;
    xmljob.close();
    return true;
  }

  return true;
}

QString adulib::on_build_xml(const bool set_astwriter_finhished) {

  if ((this->special_list == "JLL") || (this->special_list == "CEA-daily")) {
    qDebug() << " ++++++++++++++++++++ prepare JLL jobs ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";

    tix.clear(false);
    tix.header();
    tix.push("measurement");
    tix.push("recording");
    tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
    tix.element("start_time", this->start_time.toString("HH:mm:ss"));
    tix.element("stop_date", this->start_time.addSecs(this->duration).toString("yyyy-MM-dd"));
    tix.element("stop_time", this->start_time.addSecs(this->duration).toString("HH:mm:ss"));
    tix.element("target_directory", "/mtdata/data");
    tix.element("CyclicEvent", 0);
    tix.element("Cycle", 60);
    tix.element("Granularity", "seconds");
    tix.push("input");
    tix.push("Hardware", "GMS=\"" + QString::number(this->GMS) + "\" Type=\"" + QString::number(this->Type) + "\" Name=\"" + this->Name + "\" Version=\"" + this->Version + "\"");

    tix.push("global_config");

    size_t ifc = size_t(this->get_first_active_channel());
    tix.element("meas_channels", this->get_active_channels());
    tix.element("sample_freq", this->get_sample_freq());
    tix.element("buffer", 16384);
    tix.element("start_stop_mode", this->channels[ifc].get("start_stop_mode").toInt());
    tix.element("calon", this->channels[ifc].get("calon").toInt());
    tix.element("atton", this->channels[ifc].get("atton").toInt());
    tix.element("calref", this->channels[ifc].get("calref").toInt());
    tix.element("calint", this->channels[ifc].get("calint").toInt());
    tix.element("calfreq", this->channels[ifc].get("calfreq").toDouble());
    tix.element("short_circuit", this->channels[ifc].get("short_circuit").toInt());

    // begin never used
    tix.element("decimation", 0);
    tix.element("flush_fill", 0);
    tix.element("ovl_fill", 0);
    tix.element("start_stop_fill", 0);
    // end never used

    tix.pop("global_config");

    tix.push("channel_config");
    for (auto &chan : channels) {
      if (chan.is_active()) {
        tix.push("channel", "id", chan.get_slot());
        tix.element("gain_stage1", chan.get("gain_stage1").toInt());
        tix.element("gain_stage2", chan.get("gain_stage2").toInt());
        tix.element("ext_gain", chan.get("ext_gain").toInt());
        tix.element("filter_type", chan.get("filter_type").toString());
        // JLL must switch the chopper on for this list - even though we want to take 1024Hz for 4 hrs, the rest is low freq!
        tix.element("echopper", 1);
        tix.element("hchopper", 1);
        tix.element("dac_val", chan.get("dac_val").toDouble());
        tix.element("dac_on", chan.get("dac_on").toInt());
        tix.element("input", chan.get("input").toInt());
        tix.element("input_divider", chan.get("input_divider").toInt());
        tix.pop();
      }
    }
    tix.pop("channel_config");
    tix.pop("input");
    tix.pop("Hardware");

    tix.push("output");
    tix.push("ProcessingTree", "id", 0);

    tix.push("configuration");
    tix.element("processings", "mt_auto");
    tix.pop("configuration");

    tix.push("output");
    tix.push("DigitalFilter");

    tix.push("configuration");
    tix.element("filter_type", "mtx32x");
    tix.element("filter_factor", "");
    tix.element("filter_length", "");
    tix.pop("configuration");

    tix.push("output");
    tix.push("ATSWriter");
    tix.push("configuration");
    tix.element("survey_header_name", this->get("survey_header_name", -1).toString().simplified());
    tix.element("meas_type", this->get("meas_type", -1).toString());
    tix.element("powerline_freq1", this->get("powerline_freq1", -1).toString());
    tix.element("powerline_freq2", this->get("powerline_freq2", -1).toString());

    for (auto &chan : channels) {
      if (chan.is_active()) {
        tix.push("channel", "id", chan.get_slot());
        if (set_astwriter_finhished) {
          tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
          tix.element("start_time", this->start_time.toString("HH:mm:ss"));
          tix.element("sample_freq", chan.get("sample_freq").toDouble());

        } else {
          tix.element("start_time", "");
          tix.element("start_date", "");
          tix.element("sample_freq", "");
        }
        tix.element("num_samples", chan.get("num_samples").toULongLong());
        tix.element("ats_data_file", chan.get("ats_data_file").toString());

        QString ch_type = chan.get("channel_type").toString();
        QChar qch = ch_type.at(ch_type.size() - 1);
        if (qch.isDigit()) {
          tix.element("channel_type", ch_type.left(ch_type.size() - 1));
        } else {
          tix.element("channel_type", ch_type);
        }

        tix.element("sensor_sernum", chan.get("sensor_sernum").toString());

        // the XML file needs the alias MFS06e but the app shall show MFS-06e
        tix.element("sensor_type", chan.get("sensor_type_alias").toString());
        tix.element("ts_lsb", chan.get("ts_lsb").toDouble());
        tix.element("sensor_cal_file", "SENSOR.CAL");
        tix.element("pos_x1", chan.get("pos_x1").toDouble());
        tix.element("pos_y1", chan.get("pos_y1").toDouble());
        tix.element("pos_z1", chan.get("pos_z1").toDouble());
        tix.element("pos_x2", chan.get("pos_x2").toDouble());
        tix.element("pos_y2", chan.get("pos_y2").toDouble());
        tix.element("pos_z2", chan.get("pos_z2").toDouble());

        tix.pop();
      }
    }

    tix.pop("configuration");

    tix.push("comments");
    tix.element("site_name", this->get("site_name", -1).toString());
    tix.element("site_name_rr", this->get("site_name_rr", -1).toString());
    tix.element("site_name_emap", this->get("site_name_emap", -1).toString());
    tix.element("client", this->get("client", -1).toString());
    tix.element("contractor", this->get("contractor", -1).toString());
    tix.element("area", this->get("area", -1).toString());
    tix.element("survey_id", this->get("survey_id", -1).toString());
    tix.element("operator", this->get("operator", -1).toString());
    tix.element("general_comments", this->get("general_comments", -1).toString());
    tix.pop("comments");

    tix.push("output_file");
    tix.element("line_num", this->get("line_num", -1).toString());
    tix.element("site_num", this->get("site_num", -1).toString());
    tix.element("run_num", this->get("run_num", -1).toString());
    tix.element("adu_ser_num", this->get("adu_ser_num", -1).toString());
    tix.element("ats_file_size", this->get("ats_file_size", -1).toString());
    tix.pop("output_file");

    tix.pop("ATSWriter");
    tix.pop("output");

    tix.pop("DigitalFilter");
    tix.pop("output");

    tix.pop("ProcessingTree");

    // subjob  :

    tix.push("ProcessingTree", "id", 1);

    tix.push("configuration");
    tix.element("processings", "mt_auto");
    tix.pop("configuration");

    tix.push("output");

    tix.push("SubJobManager");
    tix.push("configuration");
    tix.element("CyclicEvent", "1");

    if (this->special_list == "JLL") {

      tix.element("Cycle", "1");
      tix.element("Granularity", "days");
      tix.pop("configuration");
      tix.push("subjob");
      tix.element("start_time", "16:00:00");
      tix.element("stop_time", "20:00:00");
      tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
      tix.element("stop_date", this->start_time.toString("yyyy-MM-dd"));
      tix.pop("subjob");
    }

    else if (this->special_list == "CEA-daily") {

      tix.element("Cycle", "15"); // all n minutes
      tix.element("Granularity", "minutes");
      tix.pop("configuration");
      tix.push("subjob");

      QDateTime next_datetime, new_start_datetime;
      next_datetime.setTimeSpec(Qt::UTC);
      new_start_datetime.setTimeSpec(Qt::UTC);
      next_datetime = this->start_time;

      // set the time to the full hour; that is always possible without date switching
      next_datetime.setTime(QTime(this->start_time.time().hour(), 0, 0, 0));

      for (int ii = 1; ii < 6; ++ii) {
        new_start_datetime = next_datetime.addSecs(60 * 15 * ii);
        if (new_start_datetime > this->start_time)
          break;
      }

      tix.element("start_date", new_start_datetime.toString("yyyy-MM-dd"));
      tix.element("start_time", new_start_datetime.toString("HH:mm:ss"));
      tix.element("stop_date", new_start_datetime.addSecs(120).toString("yyyy-MM-dd"));
      tix.element("stop_time", new_start_datetime.addSecs(120).toString("HH:mm:ss"));

      tix.pop("subjob");
    }

    tix.push("output");
    tix.push("ATSWriter");
    tix.push("configuration");
    tix.element("survey_header_name", this->get("survey_header_name", -1).toString().simplified());
    tix.element("meas_type", this->get("meas_type", -1).toString());
    tix.element("powerline_freq1", this->get("powerline_freq1", -1).toString());
    tix.element("powerline_freq2", this->get("powerline_freq2", -1).toString());

    for (auto &chan : channels) {
      if (chan.is_active()) {
        tix.push("channel", "id", chan.get_slot());
        if (set_astwriter_finhished) {
          tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
          tix.element("start_time", this->start_time.toString("HH:mm:ss"));
          tix.element("sample_freq", chan.get("sample_freq").toDouble());

        } else {
          tix.element("start_time", "");
          tix.element("start_date", "");
          tix.element("sample_freq", "");
        }
        tix.element("num_samples", chan.get("num_samples").toULongLong());
        tix.element("ats_data_file", chan.get("ats_data_file").toString());

        QString ch_type = chan.get("channel_type").toString();
        QChar qch = ch_type.at(ch_type.size() - 1);
        if (qch.isDigit()) {
          tix.element("channel_type", ch_type.left(ch_type.size() - 1));
        } else {
          tix.element("channel_type", ch_type);
        }

        tix.element("sensor_sernum", chan.get("sensor_sernum").toString());

        // the XML file needs the alias MFS06e but the app shall show MFS-06e
        tix.element("sensor_type", chan.get("sensor_type_alias").toString());
        tix.element("ts_lsb", chan.get("ts_lsb").toDouble());
        tix.element("sensor_cal_file", "SENSOR.CAL");
        tix.element("pos_x1", chan.get("pos_x1").toDouble());
        tix.element("pos_y1", chan.get("pos_y1").toDouble());
        tix.element("pos_z1", chan.get("pos_z1").toDouble());
        tix.element("pos_x2", chan.get("pos_x2").toDouble());
        tix.element("pos_y2", chan.get("pos_y2").toDouble());
        tix.element("pos_z2", chan.get("pos_z2").toDouble());

        tix.pop();
      }
    }

    tix.pop("configuration");

    tix.push("comments");
    tix.element("site_name", this->get("site_name", -1).toString());
    tix.element("site_name_rr", this->get("site_name_rr", -1).toString());
    tix.element("site_name_emap", this->get("site_name_emap", -1).toString());
    tix.element("client", this->get("client", -1).toString());
    tix.element("contractor", this->get("contractor", -1).toString());
    tix.element("area", this->get("area", -1).toString());
    tix.element("survey_id", this->get("survey_id", -1).toString());
    tix.element("operator", this->get("operator", -1).toString());
    tix.element("general_comments", this->get("general_comments", -1).toString());
    tix.pop("comments");

    tix.push("output_file");
    tix.element("line_num", this->get("line_num", -1).toString());
    tix.element("site_num", this->get("site_num", -1).toString());
    tix.element("run_num", this->get("run_num", -1).toString());
    tix.element("adu_ser_num", this->get("adu_ser_num", -1).toString());
    tix.element("ats_file_size", this->get("ats_file_size", -1).toString());
    tix.pop("output_file");

    tix.pop("ATSWriter");
    tix.pop("output");

    tix.pop("SubJobManager");
    tix.pop("output");

    tix.pop("ProcessingTree");
    tix.pop("output");

    tix.pop("recording");

    this->channel_calibration();
    tix.insert(this->xcal.getXmlStr());
    xcal.clear(false);

    this->sensor_calibration();
    tix.insert(this->xcal.getXmlStr());
    xcal.clear(false);

    tix.pop("measurement");
  }

  if (this->special_list == "InSituCal") {
    qDebug() << " ++++++++++++++++++++ prepare InSitu Cal jobs ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";

    tix.clear(false);
    tix.header();
    tix.push("measurement");
    tix.push("recording");
    tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
    tix.element("start_time", this->start_time.toString("HH:mm:ss"));
    tix.element("stop_date", this->start_time.addSecs(this->duration).toString("yyyy-MM-dd"));
    tix.element("stop_time", this->start_time.addSecs(this->duration).toString("HH:mm:ss"));
    tix.element("target_directory", "/mtdata/data");
    tix.element("CyclicEvent", 0);
    tix.element("Cycle", 60);
    tix.element("Granularity", "seconds");
    tix.push("input");

    tix.push("Hardware", "GMS=\"" + QString::number(this->GMS) + "\" Type=\"" + QString::number(this->Type) + "\" Name=\"" + this->Name + "\" Version=\"" + this->Version + "\"");

    tix.push("global_config");

    size_t ifc = size_t(this->get_first_active_channel());
    tix.element("meas_channels", this->get_active_channels());
    tix.element("sample_freq", this->get_sample_freq());
    tix.element("buffer", 16384);
    tix.element("start_stop_mode", this->channels[ifc].get("start_stop_mode").toInt());

    // cal set to on
    tix.element("calon", 1);
    // always on, except MFS-07e 256 Hz  // check if this works with HF and ADU-07e ...
    if ((this->get_sample_freq() <= 257) && (channels[2].get("sensor_type_alias").toString() == "MFS07e")) {
      tix.element("atton", 0);
    } else
      tix.element("atton", 1);

    // always 0 for insitu
    tix.element("calref", 0);
    tix.element("calint", 0);

    // calfreq depends on sample freq

    int sfreq = int(this->get_sample_freq());
    double calfreq = 0;

    if (sfreq == 131072)
      calfreq = 256.0;
    if (sfreq == 32768)
      calfreq = 64.0;
    if (sfreq == 4096)
      calfreq = 8.0;
    if (sfreq == 256)
      calfreq = 0.125;

    tix.element("calfreq", calfreq);
    tix.element("short_circuit", 0);

    // begin never used
    tix.element("decimation", 0);
    tix.element("flush_fill", 0);
    tix.element("ovl_fill", 0);
    tix.element("start_stop_fill", 0);
    // end never used

    tix.pop("global_config");

    tix.push("channel_config");
    for (auto &chan : channels) {
      if (chan.is_active()) {
        tix.push("channel", "id", chan.get_slot());
        // gains always 1
        tix.element("gain_stage1", 1);
        tix.element("gain_stage2", 1);
        tix.element("ext_gain", chan.get("ext_gain").toInt());
        tix.element("filter_type", chan.get("filter_type").toString());
        tix.element("echopper", chan.get("echopper").toInt());
        tix.element("hchopper", chan.get("hchopper").toInt());
        tix.element("dac_val", chan.get("dac_val").toDouble());
        tix.element("dac_on", chan.get("dac_on").toInt());
        tix.element("input", chan.get("input").toInt());

        // on always for insitu cal, including E
        tix.element("input_divider", 1);
        tix.pop();
      }
    }
    tix.pop("channel_config");
    tix.pop("input");
    tix.pop("Hardware");

    tix.push("output");
    tix.push("ProcessingTree", "id", 0);
    tix.push("configuration");
    tix.element("processings", "insitucal");
    tix.pop("configuration");

    tix.push("output");
    tix.push("ATSWriter");
    tix.push("configuration");
    tix.element("survey_header_name", this->get("survey_header_name", -1).toString().simplified());
    tix.element("meas_type", this->get("meas_type", -1).toString());
    tix.element("powerline_freq1", this->get("powerline_freq1", -1).toString());
    tix.element("powerline_freq2", this->get("powerline_freq2", -1).toString());

    for (auto &chan : channels) {
      if (chan.is_active()) {
        tix.push("channel", "id", chan.get_slot());
        if (set_astwriter_finhished) {
          tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
          tix.element("start_time", this->start_time.toString("HH:mm:ss"));
          tix.element("sample_freq", chan.get("sample_freq").toDouble());

        } else {
          tix.element("start_time", "");
          tix.element("start_date", "");
          tix.element("sample_freq", "");
        }
        tix.element("num_samples", chan.get("num_samples").toULongLong());
        tix.element("ats_data_file", chan.get("ats_data_file").toString());

        QString ch_type = chan.get("channel_type").toString();
        QChar qch = ch_type.at(ch_type.size() - 1);
        if (qch.isDigit()) {
          tix.element("channel_type", ch_type.left(ch_type.size() - 1));
        } else {
          tix.element("channel_type", ch_type);
        }

        tix.element("sensor_sernum", chan.get("sensor_sernum").toString());

        // the XML file needs the alias MFS06e but the app shall show MFS-06e
        tix.element("sensor_type", chan.get("sensor_type_alias").toString());
        tix.element("ts_lsb", chan.get("ts_lsb").toDouble());
        tix.element("sensor_cal_file", "SENSOR.CAL");
        tix.element("pos_x1", chan.get("pos_x1").toDouble());
        tix.element("pos_y1", chan.get("pos_y1").toDouble());
        tix.element("pos_z1", chan.get("pos_z1").toDouble());
        tix.element("pos_x2", chan.get("pos_x2").toDouble());
        tix.element("pos_y2", chan.get("pos_y2").toDouble());
        tix.element("pos_z2", chan.get("pos_z2").toDouble());

        tix.pop();
      }
    }

    tix.pop("configuration");

    tix.push("comments");
    tix.element("site_name", this->get("site_name", -1).toString());
    tix.element("site_name_rr", this->get("site_name_rr", -1).toString());
    tix.element("site_name_emap", this->get("site_name_emap", -1).toString());
    tix.element("client", this->get("client", -1).toString());
    tix.element("contractor", this->get("contractor", -1).toString());
    tix.element("area", this->get("area", -1).toString());
    tix.element("survey_id", this->get("survey_id", -1).toString());
    tix.element("operator", this->get("operator", -1).toString());
    tix.element("general_comments", this->get("general_comments", -1).toString());
    tix.pop("comments");

    tix.push("output_file");
    tix.element("line_num", this->get("line_num", -1).toString());
    tix.element("site_num", this->get("site_num", -1).toString());
    tix.element("run_num", this->get("run_num", -1).toString());
    tix.element("adu_ser_num", this->get("adu_ser_num", -1).toString());
    tix.element("ats_file_size", this->get("ats_file_size", -1).toString());
    tix.pop("output_file");

    tix.pop("ATSWriter");
    tix.pop("output");
    tix.pop("ProcessingTree");

    tix.pop("output");
    tix.pop("recording");

    this->channel_calibration();
    tix.insert(this->xcal.getXmlStr());
    xcal.clear(false);

    this->sensor_calibration();
    tix.insert(this->xcal.getXmlStr());
    xcal.clear(false);

    if (this->gps.is_active()) {
      tix.push("HwStatus", "GMS=\"" + QString::number(this->GMS) + "\" Type=\"" + QString::number(this->Type) + "\" Name=\"" + this->Name + "\" Version=\"" + this->Version + "\"");
      tix.insert(this->gps.getXmlStr());
      tix.pop("HwStatus");
    }

    tix.pop("measurement");
  }

  else {

    tix.clear(false);
    tix.header();
    tix.push("measurement");
    tix.push("recording");
    tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
    tix.element("start_time", this->start_time.toString("HH:mm:ss"));
    tix.element("stop_date", this->start_time.addSecs(this->duration).toString("yyyy-MM-dd"));
    tix.element("stop_time", this->start_time.addSecs(this->duration).toString("HH:mm:ss"));
    tix.element("target_directory", "/mtdata/data");
    tix.element("CyclicEvent", 0);
    tix.element("Cycle", 60);
    tix.element("Granularity", "seconds");
    tix.push("input");
    tix.push("Hardware", "GMS=\"" + QString::number(this->GMS) + "\" Type=\"" + QString::number(this->Type) + "\" Name=\"" + this->Name + "\" Version=\"" + this->Version + "\"");
    if (this->txm.is_active()) {
      tix.insert(this->txm.getXmlStr());
    }
    tix.push("global_config");

    size_t ifc = size_t(this->get_first_active_channel());
    tix.element("meas_channels", this->get_active_channels());
    tix.element("sample_freq", this->get_sample_freq());
    tix.element("buffer", 16384);
    tix.element("start_stop_mode", this->channels[ifc].get("start_stop_mode").toInt());
    tix.element("calon", this->channels[ifc].get("calon").toInt());
    tix.element("atton", this->channels[ifc].get("atton").toInt());
    tix.element("calref", this->channels[ifc].get("calref").toInt());
    tix.element("calint", this->channels[ifc].get("calint").toInt());
    tix.element("calfreq", this->channels[ifc].get("calfreq").toDouble());
    tix.element("short_circuit", this->channels[ifc].get("short_circuit").toInt());

    // begin never used
    tix.element("decimation", 0);
    tix.element("flush_fill", 0);
    tix.element("ovl_fill", 0);
    tix.element("start_stop_fill", 0);
    // end never used

    tix.pop("global_config");

    tix.push("channel_config");
    for (auto &chan : channels) {
      if (chan.is_active()) {
        tix.push("channel", "id", chan.get_slot());
        tix.element("gain_stage1", chan.get("gain_stage1").toInt());
        tix.element("gain_stage2", chan.get("gain_stage2").toInt());
        tix.element("ext_gain", chan.get("ext_gain").toInt());
        tix.element("filter_type", chan.get("filter_type").toString());
        tix.element("echopper", chan.get("echopper").toInt());
        tix.element("hchopper", chan.get("hchopper").toInt());
        tix.element("dac_val", chan.get("dac_val").toDouble());
        tix.element("dac_on", chan.get("dac_on").toInt());
        tix.element("input", chan.get("input").toInt());
        tix.element("input_divider", chan.get("input_divider").toInt());
        tix.pop();
      }
    }
    tix.pop("channel_config");
    tix.pop("input");
    tix.pop("Hardware");

    tix.push("output");
    tix.push("ProcessingTree", "id", 0);
    tix.push("configuration");
    tix.element("processings", "mt_auto");
    tix.pop("configuration");

    tix.push("output");
    tix.push("ATSWriter");
    tix.push("configuration");
    tix.element("survey_header_name", this->get("survey_header_name", -1).toString().simplified());
    tix.element("meas_type", this->get("meas_type", -1).toString());
    tix.element("powerline_freq1", this->get("powerline_freq1", -1).toString());
    tix.element("powerline_freq2", this->get("powerline_freq2", -1).toString());

    for (auto &chan : channels) {
      if (chan.is_active()) {
        tix.push("channel", "id", chan.get_slot());
        if (set_astwriter_finhished) {
          tix.element("start_date", this->start_time.toString("yyyy-MM-dd"));
          tix.element("start_time", this->start_time.toString("HH:mm:ss"));
          tix.element("sample_freq", chan.get("sample_freq").toDouble());

        } else {
          tix.element("start_time", "");
          tix.element("start_date", "");
          tix.element("sample_freq", "");
        }
        tix.element("num_samples", chan.get("num_samples").toULongLong());
        tix.element("ats_data_file", chan.get("ats_data_file").toString());

        QString ch_type = chan.get("channel_type").toString();
        QChar qch = ch_type.at(ch_type.size() - 1);
        if (qch.isDigit()) {
          tix.element("channel_type", ch_type.left(ch_type.size() - 1));
        } else {
          tix.element("channel_type", ch_type);
        }

        tix.element("sensor_sernum", chan.get("sensor_sernum").toString());

        // the XML file needs the alias MFS06e but the app shall show MFS-06e
        tix.element("sensor_type", chan.get("sensor_type_alias").toString());
        tix.element("ts_lsb", chan.get("ts_lsb").toDouble());
        tix.element("sensor_cal_file", "SENSOR.CAL");
        tix.element("pos_x1", chan.get("pos_x1").toDouble());
        tix.element("pos_y1", chan.get("pos_y1").toDouble());
        tix.element("pos_z1", chan.get("pos_z1").toDouble());
        tix.element("pos_x2", chan.get("pos_x2").toDouble());
        tix.element("pos_y2", chan.get("pos_y2").toDouble());
        tix.element("pos_z2", chan.get("pos_z2").toDouble());

        tix.pop();
      }
    }

    tix.pop("configuration");

    tix.push("comments");
    tix.element("site_name", this->get("site_name", -1).toString());
    tix.element("site_name_rr", this->get("site_name_rr", -1).toString());
    tix.element("site_name_emap", this->get("site_name_emap", -1).toString());
    tix.element("client", this->get("client", -1).toString());
    tix.element("contractor", this->get("contractor", -1).toString());
    tix.element("area", this->get("area", -1).toString());
    tix.element("survey_id", this->get("survey_id", -1).toString());
    tix.element("operator", this->get("operator", -1).toString());
    tix.element("general_comments", this->get("general_comments", -1).toString());
    tix.pop("comments");

    tix.push("output_file");
    tix.element("line_num", this->get("line_num", -1).toString());
    tix.element("site_num", this->get("site_num", -1).toString());
    tix.element("run_num", this->get("run_num", -1).toString());
    tix.element("adu_ser_num", this->get("adu_ser_num", -1).toString());
    tix.element("ats_file_size", this->get("ats_file_size", -1).toString());
    tix.pop("output_file");

    tix.pop("ATSWriter");
    tix.pop("output");
    tix.pop("ProcessingTree");

    tix.pop("output");
    tix.pop("recording");

    this->channel_calibration();
    tix.insert(this->xcal.getXmlStr());
    xcal.clear(false);

    this->sensor_calibration();
    tix.insert(this->xcal.getXmlStr());
    xcal.clear(false);

    if (this->gps.is_active()) {
      tix.push("HwStatus", "GMS=\"" + QString::number(this->GMS) + "\" Type=\"" + QString::number(this->Type) + "\" Name=\"" + this->Name + "\" Version=\"" + this->Version + "\"");
      tix.insert(this->gps.getXmlStr());
      tix.pop("HwStatus");
    }

    tix.pop("measurement");
  }
  qDebug() << tix.getXmlStr();
  return tix.getXmlStr();
}

QString adulib::on_build_channel_config() {
  // if string not empty, XML from aduclient

  // take float here for easy read in XML
  // double will be always in SCI format
  QMap<QString, QVariant> wrs;

  QMap<QString, QVariant> comments;

  QMap<QString, QVariant> ats_writer_config_map;

  QMap<QString, QVariant> output_file;

  int i = 0;
  std::unique_lock<std::mutex> l(this->lock);
  for (auto &chan : this->channels) {
    if (chan.is_active()) {
      wrs.insert("pos_x1", float(chan.get("pos_x1").toFloat()));
      wrs.insert("pos_x2", float(chan.get("pos_x2").toFloat()));
      wrs.insert("pos_y1", float(chan.get("pos_y1").toFloat()));
      wrs.insert("pos_y2", float(chan.get("pos_y2").toFloat()));
      wrs.insert("pos_z1", float(chan.get("pos_z1").toFloat()));
      wrs.insert("pos_z2", float(chan.get("pos_z2").toFloat()));
      wrs.insert("channel_type", chan.get("channel_type"));
      wrs.insert("sensor_type", chan.get("sensor_type_alias"));
      wrs.insert("sensor_sernum", chan.get("sensor_sernum"));

      this->channel_cfg->set_channel_section("ATSWriterSettings", wrs, i, false);
    }
    ++i;
  }
  comments.insert("site_name", this->get("site_name", -1).toString());
  comments.insert("site_name_rr", this->get("site_name_rr", -1).toString());
  comments.insert("site_name_emap", this->get("site_name_emap", -1).toString());
  comments.insert("client", this->get("client", -1).toString());
  comments.insert("contractor", this->get("contractor", -1).toString());
  comments.insert("area", this->get("area", -1).toString());
  comments.insert("survey_id", this->get("survey_id", -1).toString());
  comments.insert("operator", this->get("operator", -1).toString());
  comments.insert("general_comments", this->get("general_comments", -1).toString());
  this->channel_cfg->set_simple_section("ATSWriterSettings", "comments", comments);

  output_file.insert("line_num", this->get("line_num", -1).toString());
  output_file.insert("site_num", this->get("site_num", -1).toString());
  output_file.insert("run_num", this->get("run_num", -1).toInt());
  output_file.insert("adu_ser_num", this->get("adu_ser_num", -1).toString());
  output_file.insert("ats_file_size", this->get("ats_file_size", -1).toString());

  this->channel_cfg->set_simple_section("ATSWriterSettings", "output_file", output_file);

  ats_writer_config_map.insert("meas_type", this->get("meas_type", -1).toString());
  ats_writer_config_map.insert("powerline_freq1", this->get("powerline_freq1", -1).toFloat());
  ats_writer_config_map.insert("powerline_freq2", this->get("powerline_freq2", -1).toFloat());
  ats_writer_config_map.insert("survey_header_name", this->get("survey_header_name", -1).toString());

  this->channel_cfg->set_simple_section("ATSWriterSettings", "configuration", ats_writer_config_map);

  return this->channel_cfg->getXmlStr();
}

void adulib::on_set_filename(const QString &filename) {
  this->qfi.setFile(filename);
}

void adulib::on_set_duration(const qint64 &duration, const bool emits) {
  if (duration && (this->duration != duration)) {
    emit this->timing_change_attempt();
    this->duration = duration;
    emit this->timing_changed();
    if (emits)
      emit this->duration_changed(this->index, this->start_time, this->duration, this->get("sample_freq", 0).toDouble());
  }
}

void adulib::on_set_long_duration(const int &days, const int &hh, const int &mm, const int &ss, const bool emits) {
  qint64 secs = 86400 * std::abs(days) + 3600 * std::abs(hh) + 60 * std::abs(mm) + std::abs(ss);
  if (secs && (this->duration != secs)) {
    emit this->timing_change_attempt();
    this->duration = secs;
    emit this->timing_changed();
    if (emits)
      emit this->duration_changed(this->index, this->start_time, this->duration, this->get("sample_freq", 0).toDouble());
  }
}

qint64 adulib::get_duration() const {
  return this->duration;
}

QStringList adulib::get_H_sensors(QStringList *alias_names, QList<int> *input_divider) const {
  QStringList sensorlist;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "get_H_sensors");
  db.setConnectOptions("QSQLITE_OPEN_READONLY");
  db.setDatabaseName("info.sql3");
  if (db.open()) {
    QSqlQuery query(db);
    if (alias_names == nullptr) {
      if (query.exec("select sensortypes.Name from sensortypes where  sensortypes.Unit like \"H\";")) {
        while (query.next()) {
          sensorlist << query.value(0).toString();
        }
      } else
        qDebug() << "db err: could not get H sensors";
    } else if ((alias_names != nullptr) && (input_divider != nullptr)) {
      if (query.exec("select sensortypes.Name, sensortypes.atsheadername, sensortypes.needs_input_divider from sensortypes where  sensortypes.Unit like \"H\";")) {
        while (query.next()) {
          sensorlist << query.value(0).toString();
          alias_names->append(query.value(1).toString());
          input_divider->append(query.value(2).toInt());
        }
      } else
        qDebug() << "db err: could not get H sensors";
    }
    db.close();
  } else
    qDebug() << "can not open" << db.databaseName();
  return sensorlist;
}

QStringList adulib::get_E_sensors(QStringList *alias_names, QList<int> *input_divider) const {
  QStringList sensorlist;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "get_E_sensors");
  db.setConnectOptions("QSQLITE_OPEN_READONLY");
  db.setDatabaseName("info.sql3");
  if (db.open()) {
    QSqlQuery query(db);
    if (alias_names == nullptr) {
      if (query.exec("select sensortypes.Name from sensortypes where  sensortypes.Unit like \"E\";")) {
        while (query.next()) {
          sensorlist << query.value(0).toString();
        }
      } else
        qDebug() << "db err: could not get E sensors ";
    } else if ((alias_names != nullptr) && (input_divider != nullptr)) {
      if (query.exec("select sensortypes.Name, sensortypes.atsheadername, sensortypes.needs_input_divider  from sensortypes where  sensortypes.Unit like \"E\";")) {
        while (query.next()) {
          sensorlist << query.value(0).toString();
          alias_names->append(query.value(1).toString());
          input_divider->append(query.value(2).toInt());
        }
      } else
        qDebug() << "db err: could not get E sensors ";
    }
    db.close();
  }

  else
    qDebug() << "can not open" << db.databaseName();
  return sensorlist;
}

int adulib::get_selftest_table(QList<QString> &what, QList<QString> &limits_strs, QList<QVariant> &le, QList<QVariant> &ge) const {
  QString dbname;
  if (this->GMS == 8)
    dbname = "STR08e";
  else
    dbname = "STR07e";
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbname);
  db.setConnectOptions("QSQLITE_OPEN_READONLY");
  db.setDatabaseName("info.sql3");
  if (db.open()) {
    QList<QVariant> value;
    QSqlQuery query(db);
    if (query.exec("select * from " + dbname + ";")) {
      while (query.next()) {
        what.append(query.value(0).toString());
        value.append(query.value(1));
        limits_strs.append(query.value(2).toString());
        le.append(query.value(3));
        ge.append(query.value(4));
      }
    } else
      qDebug() << "db err: ";
    db.close();
  }

  else
    qDebug() << "can not open" << db.databaseName();
  if (what.size()) {
    if ((what.size() == limits_strs.size()) && (what.size() == le.size()) && (what.size() == ge.size())) {
      return what.size();
    }
  } else {
    what.clear();
    limits_strs.clear();
    le.clear();
    ge.clear();
  }

  return 0;
}

int adulib::get_first_active_channel() const {
  int i = 0;
  for (auto &chan : channels) {
    if (chan.is_active()) {
      return i;
    }
    ++i;
  }
  return 0;
}

unsigned int adulib::get_gms(void) {
  return unsigned(this->GMS);
}

unsigned int adulib::get_type(void) {
  return unsigned(this->Type);
}

void adulib::set_special_list(const QString special_list_name) {
  this->special_list = special_list_name;
}

QString adulib::get_special_list() const {
  return this->special_list;
}

void adulib::setGridTime(const qint64 grid_time) {
  this->grid_time = grid_time;
}
