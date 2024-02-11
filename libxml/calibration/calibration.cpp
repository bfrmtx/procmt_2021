#include "calibration.h"

calibration::calibration(const QFileInfo &dbname_info, const int channo, const int slot,
                         const bool open_db, const QString &connection_name, QObject *parent, std::shared_ptr<msg_logger> msg) :
    db_sensor_query(dbname_info, channo, slot, open_db, connection_name, parent, msg)

{

  // the constructor of db_sensor_query connects tp parent

  QString message;

  if (!this->info_db.open()) {
    message = "calibration::calibration -> can not open info database " + dbname_info.absoluteFilePath();
    emit this->tx_cal_message(this->channel, this->slot, message);
    return;
  }

  this->sensor_types_tables.insert("MFS-05", "`cal mfs-05`");

  this->sensor_types_tables.insert("MFS-06e", "`cal mfs-06e`");
  this->sensor_types_tables.insert("MFS-06", "`cal mfs-06`");

  this->sensor_types_tables.insert("MFS-07e", "`cal mfs-07e`");
  this->sensor_types_tables.insert("MFS-07", "`cal mfs-07`");
  this->sensor_types_tables.insert("MFS-09u", "`cal mfs-09u`");
  this->sensor_types_tables.insert("MFS-10e", "`cal mfs-10e`");
  this->sensor_types_tables.insert("MFS-11e", "`cal mfs-11e`");
  this->sensor_types_tables.insert("MFS-12e", "`cal mfs-12e`");

  this->sensor_types_tables.insert("SHFT-02e", "`cal shft-02e`");
  this->sensor_types_tables.insert("SHFT-02", "`cal shft-02`");
  this->sensor_types_tables.insert("SHFT-03e", "`cal shft-03e`");

  this->sensor_types_tables.insert("FGS-03", "`cal fgs-03`");
  this->sensor_types_tables.insert("FGS-03e", "`cal fgs-03e`");
  this->sensor_types_tables.insert("FGS-02", "`cal fgs-02`");
  this->sensor_types_tables.insert("FGS-04e", "`cal fgs-04e`");
  this->sensor_types_tables.insert("FGS-05e", "`cal fgs-05e`");

  this->csv_header << "serial"
                   << "chopper"
                   << "f"
                   << "a"
                   << "p"
                   << "stddev a"
                   << "stddev p";

  this->clear();
  this->init_calinfo();
}

calibration::~calibration() {
  if (this->master_calibration_db.isOpen())
    this->master_calibration_db.close();
  // destroy the reference
  this->master_calibration_db = QSqlDatabase();
  QSqlDatabase::removeDatabase(this->master_calibration_db_connection_name);

  if (this->sensor_db.isOpen())
    this->sensor_db.close();
  // destroy the reference
  this->sensor_db = QSqlDatabase();
  QSqlDatabase::removeDatabase(this->sensor_db_connection_name);

  qDebug() << "calibration destroyed";
}

void calibration::clone_caldata(std::shared_ptr<calibration> rhs) {
  this->sernum = rhs->sernum;
  this->sensortype = rhs->sensortype;
  this->freqs = rhs->freqs;
  this->f_on = rhs->f_on;
  this->phase_grad_on = rhs->phase_grad_on;
  this->ampl_on = rhs->ampl_on;
  this->f_off = rhs->f_off;
  this->phase_grad_off = rhs->phase_grad_off;
  this->ampl_off = rhs->ampl_off;
  this->f_on_master = rhs->f_on_master;
  this->phase_grad_on_master = rhs->phase_grad_on_master;
  this->ampl_on_master = rhs->ampl_on_master;
  this->f_off_master = rhs->f_off_master;
  this->phase_grad_off_master = rhs->phase_grad_off_master;
  this->ampl_off_master = rhs->ampl_off_master;
  this->f_on_ipl = rhs->f_on_ipl;
  this->phase_grad_on_ipl = rhs->phase_grad_on_ipl;
  this->ampl_on_ipl = rhs->ampl_on_ipl;
  this->f_off_ipl = rhs->f_off_ipl;
  this->phase_grad_off_ipl = rhs->phase_grad_off_ipl;
  this->ampl_off_ipl = rhs->ampl_off_ipl;
  this->cap = rhs->cap;
  this->calinfo = rhs->calinfo;
}

size_t calibration::read_std_txt_file(const int &channel, const QFileInfo &qfi) {

  if (!qfi.exists())
    return 0;

  if (channel > -1)
    this->channel = channel;

  QTextStream qts;
  QString message;
  QFile calfile;
  QString line;
  double df, da, dp;

  bool bon = false;
  bool boff = false;
  bool isheader = true;
  bool ok;

  this->clear();

  calfile.setFileName(qfi.absoluteFilePath());
  qts.setDevice(&calfile);
  if (calfile.open(QIODevice::ReadOnly)) {

    if (!this->parse_filename(qfi)) {
      this->ukn_sensor = true;
      if (this->parse_filename_other_cal(qfi)) {
        isheader = false;
      }
    }
    do {
      line = qts.readLine().trimmed();
      line = line.simplified();
      line = line.toLower();
      if (line.contains("break")) {
        message = "file " + qfi.absoluteFilePath() + " contains 'break', return ";
        emit this->tx_cal_message(this->channel, this->slot, message);
        return 0;
      }
      if (isheader) {
        if (line.indexOf("magnetometer:") != -1) {

          line = line.remove("magnetometer:");
          line = line.remove("date:");
          line = line.remove("time:");

          for (int k = 0; k < this->sensor_aliases.size(); ++k) {
            line = line.remove(this->sensor_aliases.at(k));
          }
          for (int k = 0; k < this->sensor_aliases.size(); ++k) {
            line = line.remove(this->sensor_aliases.at(k) + "#");
          }

          line = line.remove("#");

          line = line.trimmed();
          line = line.simplified();
          QStringList strvals = line.split(" ");

          if (strvals.size() == 3) {
            bool isok(false);
            this->sernum = strvals.at(0).toInt(&isok);

            if (!sernum & !isok) {
              message = "serial number could not be parsed" + qfi.baseName();
              emit this->tx_cal_message(this->channel, this->slot, message);
              return 0;
            } else if ((this->sernum < 1) || this->sernum > 1000000) {
              message = "serial number MAYBE ZERO or invalid, may run ito trouble" + qfi.baseName();
              emit this->tx_cal_message(this->channel, this->slot, message);
            }

            this->caltime = QDateTime::fromString(strvals.at(1) + " " + strvals.at(2), "yy/MM/dd HH:mm:ss");
            QStringList cent = strvals.at(1).split("/");
            if (cent.size() > 3) {
              if (!cent.at(2).startsWith("9"))
                this->caltime = this->caltime.addYears(100);
            } else {
              this->caltime = QDateTime::fromString(strvals.at(1).trimmed() + " " + strvals.at(2).trimmed(), "yyyy-MM-dd hh:mm:ss");
            }
            if (!this->caltime.isValid()) {
              // last shot for SHFT format
              this->caltime = QDateTime::fromString(strvals.at(1) + " " + strvals.at(2), "yy/MM/dd HH:mm:ss");
              this->caltime = this->caltime.addYears(100);
              if (!this->caltime.isValid())
                this->caltime = QDateTime::currentDateTime();
            }
            qDebug() << "stringlist" << strvals.at(0) << strvals.at(1) + " " + strvals.at(2);

            qDebug() << " getting " << strvals.at(0) << "calibrated on" << this->caltime.toString("yyyy-MM-dd HH:mm:ss");

            if (!this->caltime.isValid()) {
              qDebug() << "cal time" << this->caltime;
              message = "calibration time invalid, return" + qfi.baseName();
              emit this->tx_cal_message(this->channel, this->slot, message);
              // return 0;
            }
          } else {

            message = "failed to get coil id sernum calibration time " + qfi.baseName();
            qDebug() << message;
            emit this->tx_cal_message(this->channel, this->slot, message);
            return 0;
          }

          isheader = false;
        }
      } else {
        if (line.contains("chopper on")) {
          bon = true;
          boff = false;
          this->chopper = cal::chopper_on;
        }
        if (line.contains("chopper off")) {
          bon = false;
          boff = true;
          this->chopper = cal::chopper_off;
        }
        if (line.size() > 2 && !line.contains("freq") && !line.contains("hz")) {
          QStringList vals = line.split(" ");
          if (vals.size() == 3 && (bon || boff)) {
            df = vals.at(0).toDouble(&ok);
            if (!ok) {
              message = "can not convert frequency " + vals.at(0) + QString(" ") + qfi.absoluteFilePath();
              qDebug() << message;
              emit this->tx_cal_message(this->channel, this->slot, message);
              return 0;
            }
            da = vals.at(1).toDouble(&ok);
            if (!ok) {
              message = "can not convert amplitude " + vals.at(0) + QString(" ") + qfi.absoluteFilePath();
              qDebug() << message;
              emit this->tx_cal_message(this->channel, this->slot, message);
              return 0;
            }
            dp = vals.at(2).toDouble(&ok);
            if (!ok) {
              message = "can not convert phase " + vals.at(0) + QString(" ") + qfi.absoluteFilePath();
              qDebug() << message;
              emit this->tx_cal_message(this->channel, this->slot, message);
              return 0;
            }
            if (bon) {
              if (df > 1E-32) {
                this->f_on.emplace_back(df);
                this->ampl_on.emplace_back(da);
                this->phase_grad_on.emplace_back(dp);
              }
            }
            if (boff) {
              if (df > 1E-32) {
                this->f_off.emplace_back(df);
                this->ampl_off.emplace_back(da);
                this->phase_grad_off.emplace_back(dp);
              }
            }
          }
        }
      }

    } while (!qts.atEnd());

    calfile.close();
    this->input_filename = qfi;

    message = "read file " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
              QString::number(this->f_on.size()) + QString(" ,  chopper off: ") + QString::number(this->f_off.size());
    // message = "read file " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +  QString::number(non) + QString(" ,  chopper off: ") + QString::number(noff) ;
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);

    if (this->sensortype.contains("MFS") && !this->f_on.size()) {
      message = "******** sensor contains no chooper on section! CHECK " + qfi.baseName();
      qDebug() << message;
      emit this->tx_cal_message(this->channel, this->slot, message);
    }
    if (this->sensortype.contains("MFS") && !this->f_off.size()) {
      message = "******** sensor contains no chooper off section! CHECK " + qfi.baseName();
      qDebug() << message;
      emit this->tx_cal_message(this->channel, this->slot, message);
    }

    this->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_on);
    this->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_off);

    return this->f_on.size() + this->f_off.size();

  }

  else
    return 0;
}

bool calibration::operator==(const calibration &rhs) {
  bool ret_ser = (this->sernum == rhs.sernum);                // true in case same
  bool ret_type = (this->sensortype.compare(rhs.sensortype)); // 0 = ok and "false"

  if (ret_ser && !ret_type)
    return true;

  return false;
}

void calibration::open_master_cal(const QFileInfo &master_calibration_db_name, const QString &connection_name_master) {

  QString message;

  if (!master_calibration_db_name.exists()) {
    message = "calibration::open_master_cal -> can not find database " + master_calibration_db_name.absoluteFilePath();
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);

    return;
  }

  QString temp_str;

  if (channel >= 0) {
    temp_str = "_" + QString::number(channel);
  } else
    temp_str = "_void";

  this->master_calibration_db_connection_name = connection_name_master + temp_str;
  this->db_type = "QSQLITE";
  this->master_calibration_db = QSqlDatabase::addDatabase(this->db_type, this->master_calibration_db_connection_name);
  this->master_calibration_db.setConnectOptions("QSQLITE_OPEN_READONLY");

  this->master_calibration_db.setDatabaseName(master_calibration_db_name.absoluteFilePath());
  if (!this->master_calibration_db.open()) {
    message = "calibration::open_master_cal -> can not open " + master_calibration_db_name.absoluteFilePath();
  }
}

void calibration::read_measdoc(const int &channel, const cal::cal &chopper, const QFileInfo &qfi) {

  if (!qfi.exists())
    return;

  this->clear();
  this->channel = channel;
  this->chopper = chopper;
  if (this->measdoc != nullptr) {
    this->measdoc->close();
    this->measdoc.reset();
  }
  this->measdoc = std::make_unique<measdocxml>(&qfi, nullptr, nullptr);
  this->measdoc->get_calentries(channel, this->calinfo, this->f_on, this->ampl_on, this->phase_grad_on,
                                this->f_off, this->ampl_off, this->phase_grad_off);
  emit this->cal_open_success(channel, this->f_on.size(), this->f_off.size());
  emit this->sensor_serial(channel, this->calinfo.value("ci").toString(), this->calinfo.value("ci_serial_number").toInt(), 0);

  QString message;

  message = "query DB " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
            QString::number(this->f_on.size()) + QString(" ,  chopper off: ") + QString::number(this->f_off.size());
  emit this->tx_cal_message(this->channel, this->slot, message);

  if (chopper == cal::chopper_on) {
    emit this->cal_data(this->channel, this->chopper, this->f_on, this->ampl_on, this->phase_grad_on);
  } else if (chopper == cal::chopper_off) {
    emit this->cal_data(this->channel, this->chopper, this->f_off, this->ampl_off, this->phase_grad_off);
  } else {
    emit this->cal_data(this->channel, 1, this->f_on, this->ampl_on, this->phase_grad_on);
    emit this->cal_data(this->channel, 0, this->f_off, this->ampl_off, this->phase_grad_off);
  }

  if (this->f_off.size() || this->f_on.size()) {
    this->sensortype = this->calinfo.value("ci").toString();
    this->sensortype = this->set_atsheader_sensor(this->sensortype);
    this->sernum = this->calinfo.value("ci_serial_number").toInt();
    if (this->calinfo.value("ci_date").toDate().isValid() && this->calinfo.value("ci_time").toTime().isValid()) {
      this->caltime = QDateTime(this->calinfo.value("ci_date").toDate(), this->calinfo.value("ci_time").toTime());
    }
  }

  this->measdoc->close();

  if (this->f_on.size())
    this->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_on);
  if (this->f_off.size())
    this->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_off);
}

size_t calibration::interploate(const std::vector<double> &newfreqs, const int channel, const cal::cal chopper, const bool overlap, const bool extend_theo, const bool limit_upper_to_cal) {

  if (this->is_linear_cal)
    return 0;
  if (!newfreqs.size())
    return 0;

  if (channel != -1)
    this->channel = channel;

  this->f.clear();
  std::vector<double> accept_new_freqs;
  std::vector<double> fmerge, amerge, pmerge;
  std::vector<double> tfmerge, tamerge, tpmerge;

  if (chopper == cal::chopper_on) {

    this->chopper = chopper;

    // the master cal should be in the range of the standard calibration
    if (!this->f_on_master.size())
      this->get_master_cal();

    if ((this->f_on.size()) && (this->f_on.size() == this->ampl_on.size()) && (f_on.size() == this->phase_grad_on.size())) {

      // check master
      if ((this->f_on_master.size()) && (this->f_on_master.size() == this->ampl_on_master.size()) && (f_on_master.size() == this->phase_grad_on_master.size())) {

        this->mathv.merge_sorted_low_to_high_outside_join(this->f_on, this->f_on_master, fmerge,
                                                          this->ampl_on, this->ampl_on_master, amerge,
                                                          this->phase_grad_on, this->phase_grad_on_master, pmerge, overlap);

      } else {
        fmerge = this->f_on;
        amerge = this->ampl_on;
        pmerge = this->phase_grad_on;
      }

      // now clibration and master are merged
      // do the same for the theoretical if requested
      // for the higher f this is academic
      // for the lower f this is what you need

      if (extend_theo) {
        if (extend_theo && (limit_upper_to_cal == false))
          this->gen_theo_cal(newfreqs);
        else if (extend_theo && (limit_upper_to_cal == true))
          this->gen_theo_cal(newfreqs, 0.05, 0.5, true);
        if (this->f_on_theo.size()) {
          this->mathv.merge_sorted_low_to_high_outside_join(fmerge, this->f_on_theo, tfmerge,
                                                            amerge, this->ampl_on_theo, tamerge,
                                                            pmerge, this->phase_grad_on_theo, tpmerge, overlap);

          std::swap(fmerge, tfmerge);
          std::swap(amerge, tamerge);
          std::swap(pmerge, tpmerge);
        }
      }

      auto min_max = std::minmax_element(fmerge.begin(), fmerge.end());

      // we can ONLY interpolate inside the cal function
      // at lower we mostly can extend in the master cal easily
      // at higher the values have to be inside the calibration... that's the point
      for (auto &f : newfreqs) {
        if ((f >= *min_max.first) && (f <= *min_max.second))
          accept_new_freqs.emplace_back(f);
      }

      std::sort(accept_new_freqs.begin(), accept_new_freqs.end());

      this->mathv.akima_vector_double(fmerge, amerge, accept_new_freqs, this->ampl_on_ipl);
      this->mathv.akima_vector_double(fmerge, pmerge, accept_new_freqs, this->phase_grad_on_ipl);

      this->f = accept_new_freqs;

      if (!this->f.size()) {
        qDebug() << "calibration::interploate chopper on FAILED ";
      }

      this->f_on_ipl = this->f;
      this->cap.clear();
      this->cap.resize(f.size());

      for (size_t i = 0; i < f.size(); ++i) {
        this->cap[i] = std::polar(this->ampl_on_ipl.at(i), this->phase_grad_on_ipl.at(i) * M_PI / 180.);
      }

      // for FFT for example
      emit this->complex_cal_data(this->channel, this->chopper, this->f, this->cap);

      // for plotting for example
      emit this->cal_data(this->channel, chopper, this->f_on_ipl, this->ampl_on_ipl, this->phase_grad_on_ipl);
    }
  }

  if (chopper == cal::chopper_off) {

    this->chopper = chopper;

    // at the high freuquencies the coils are individual - no use to create a master cal with higher f
    // or in other words - you ,ast have a cal function
    if (!this->f_off_master.size())
      this->get_master_cal();

    if ((this->f_off.size()) && (this->f_off.size() == this->ampl_off.size()) && (f_off.size() == this->phase_grad_off.size())) {

      // check master
      if ((this->f_off_master.size()) && (this->f_off_master.size() == this->ampl_off_master.size()) && (f_off_master.size() == this->phase_grad_off_master.size())) {

        this->mathv.merge_sorted_low_to_high_outside_join(this->f_off, this->f_off_master, fmerge,
                                                          this->ampl_off, this->ampl_off_master, amerge,
                                                          this->phase_grad_off, this->phase_grad_off_master, pmerge);

      } else {
        fmerge = this->f_off;
        amerge = this->ampl_off;
        pmerge = this->phase_grad_off;
      }

      if (extend_theo) {

        // this is for high f of academic nature - not for MT processing
        if (extend_theo && (limit_upper_to_cal == false))
          this->gen_theo_cal(newfreqs);
        else if (extend_theo && (limit_upper_to_cal == true))
          this->gen_theo_cal(newfreqs, 0.05, 0.5, true);

        if (this->f_off_theo.size()) {
          this->mathv.merge_sorted_low_to_high_outside_join(fmerge, this->f_off_theo, tfmerge,
                                                            amerge, this->ampl_off_theo, tamerge,
                                                            pmerge, this->phase_grad_off_theo, tpmerge, overlap);

          std::swap(fmerge, tfmerge);
          std::swap(amerge, tamerge);
          std::swap(pmerge, tpmerge);
        }
      }

      auto min_max = std::minmax_element(fmerge.begin(), fmerge.end());

      for (auto &f : newfreqs) {
        if ((f >= *min_max.first) && (f <= *min_max.second))
          accept_new_freqs.emplace_back(f);
      }

      std::sort(accept_new_freqs.begin(), accept_new_freqs.end());

      this->mathv.akima_vector_double(fmerge, amerge, accept_new_freqs, this->ampl_off_ipl);
      this->mathv.akima_vector_double(fmerge, pmerge, accept_new_freqs, this->phase_grad_off_ipl);

      this->f = accept_new_freqs;

      if (!this->f.size()) {
        qDebug() << "calibration::interploate chopper off FAILED ";
      }
      this->f_off_ipl = this->f;
      this->cap.clear();
      this->cap.resize(f.size());

      for (size_t i = 0; i < f.size(); ++i) {
        this->cap[i] = std::polar(this->ampl_off_ipl.at(i), this->phase_grad_off_ipl.at(i) * M_PI / 180.);
      }

      // for FFT for example
      emit this->complex_cal_data(this->channel, this->chopper, this->f, this->cap);

      // for plotting for example
      emit this->cal_data(this->channel, chopper, this->f_off_ipl, this->ampl_off_ipl, this->phase_grad_off_ipl);
    }
  }

  return f.size();
}

QString calibration::fetch_single_item_from_sensortypes_table(const QString Name, const QString col_name) {
  if (!this->info_db.isOpen()) {
    QString message;
    message = "calibration::fetch_single_item_from_info_table -> database not open! ";
    emit this->tx_cal_message(this->channel, this->slot, message);
    return QString();
  }

  QString query_str;
  QString result;

  QSqlQuery query(this->info_db);
  query_str = "SELECT `Name`, `" + col_name + "` from sensortypes WHERE `Name` = '" + Name + "' limit 1";
  if (query.exec(query_str)) {
    query.first();
    if (query.isValid()) {
      result = query.value(0).toString();
    }
  }

  if (result.size()) {
    emit this->item_fetched_from_sensortypes_table(Name, col_name, result);
  }
  return result;
}

void calibration::eliminate_except_complex() {

  this->f_on.clear();
  this->phase_grad_on.clear();
  this->ampl_on.clear();
  this->f_off.clear();
  this->phase_grad_off.clear();
  this->ampl_off.clear();
  this->phase_grad_on_stddev.clear();
  this->ampl_on_stddev.clear();
  this->phase_grad_off_stddev.clear();
  this->ampl_off_stddev.clear();

  this->phase_grad_on_master_stddev.clear();
  this->ampl_on_master_stddev.clear();
  this->phase_grad_off_master_stddev.clear();
  this->ampl_off_master_stddev.clear();

  this->f_on_theo.clear();
  this->phase_grad_on_theo.clear();
  this->ampl_on_theo.clear();
  this->f_off_theo.clear();
  this->phase_grad_off_theo.clear();
  this->ampl_off_theo.clear();
  this->phase_grad_on_theo_stddev.clear();
  this->ampl_on_theo_stddev.clear();
  this->phase_grad_off_theo_stddev.clear();
  this->ampl_off_theo_stddev.clear();

  this->f_on_master.clear();
  this->phase_grad_on_master.clear();
  this->ampl_on_master.clear();
  this->f_off_master.clear();
  this->phase_grad_off_master.clear();
  this->ampl_off_master.clear();

  this->f_on_ipl.clear();
  this->phase_grad_on_ipl.clear();
  this->ampl_on_ipl.clear();
  this->f_off_ipl.clear();
  this->phase_grad_off_ipl.clear();
  this->ampl_off_ipl.clear();
}

void calibration::prepend_dc_part_f0_to_complex_final(const double real_trf_at_0, const double imag_trf_at_0) {
  if (this->cap.size() && this->f.size() == this->cap.size()) {
    this->f.insert(f.begin(), 0.0);
    this->cap.insert(this->cap.begin(), std::complex<double>(real_trf_at_0, imag_trf_at_0));
  }
}

bool calibration::open_create_db(const QString &db_name, const QDir &directory, const QString &connection_name) {

  if (this->sensor_db.isOpen())
    this->sensor_db.close();
  QString message;

  if (!db_name.size()) {
    message = "calibration::create_db -> db name empty";
    emit this->tx_cal_message(this->channel, this->slot, message);
    return false;
  }
  if (!directory.exists()) {
    message = "calibration::open_create_db -> directory does not exist";
    emit this->tx_cal_message(this->channel, this->slot, message);
    return false;
  }
  this->sensor_db_connection_name = connection_name;
  this->sensor_db = QSqlDatabase::addDatabase("QSQLITE", this->sensor_db_connection_name);
  // this->sensor_db->setConnectOptions("QSQLITE_OPEN_READONLY");
  this->sensor_db.setDatabaseName(directory.absolutePath() + "/" + db_name);
  qDebug() << "calibration::open_create_db -> setting DB name" << directory.absolutePath() + "/" + db_name;

  if (!this->sensor_db.open()) {
    message = "calibration::open_create_db -> can not create database / open" + db_name;
    emit this->tx_cal_message(this->channel, this->slot, message);
    return false;
  } else
    return true;
}

bool calibration::prepare_cal_db() {

  if (!this->sensor_db.isOpen())
    return false;
  QString message;
  QString query_str;
  QString sub_query = " (`serial` INTEGER,  `chopper` INTEGER, `f` DOUBLE, `a` DOUBLE, `p` DOUBLE )";

  QSqlQuery query(this->sensor_db);

  QMap<QString, QString>::const_iterator i = this->sensor_types_tables.constBegin();
  while (i != this->sensor_types_tables.constEnd()) {

    query_str = "CREATE TABLE IF NOT EXISTS   " + i.value() + sub_query;

    if (!query.exec(query_str)) {
      message = "calibration::prepare_cal_db -> can not create table " + i.value();
      emit this->tx_cal_message(this->channel, this->slot, message);
    }

    ++i;
  }

  sub_query = "` (`serial` INTEGER, `datetime` TEXT)";

  i = this->sensor_types_tables.constBegin();
  while (i != this->sensor_types_tables.constEnd()) {

    query_str = "CREATE TABLE IF NOT EXISTS  `ser " + i.key().toLower() + sub_query;

    if (!query.exec(query_str)) {
      message = "calibration::prepare_cal_db -> can not create serials table " + i.key();
      emit this->tx_cal_message(this->channel, this->slot, message);
    }

    ++i;
  }

  return true;
}

bool calibration::prepare_info_table() {

  QString message;

  if (!this->sensor_db.isOpen()) {
    message = "calibration::prepare_info_table -> sensor db not open";
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
    return false;
  }
  if (!this->info_db.isOpen()) {
    message = "calibration::prepare_info_table -> info db not open";
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
    return false;
  }
  this->sqlbt = std::make_unique<sql_base_tools>();

  message.clear();
  this->sqlbt->copy_complete_table(this->info_db, "sensortypes", this->sensor_db, "sensortypes", message);

  emit this->tx_cal_message(this->channel, this->slot, message);
  this->sqlbt.reset();
  return true;
}

int calibration::insert_into_cal_db() {

  if (!this->sensor_db.isOpen())
    return 0;
  QString message;

  if (!this->sensor_types_tables.contains(this->sensortype)) {
    message = "calibration::insert_into_cal_db sensor not found";
    emit this->tx_cal_message(this->channel, this->slot, message);
    return 0;
  }
  QSqlQuery query(this->sensor_db);

  int iex = this->coil_exist(true);
  if ((iex == 0) || iex == 1) {
    if (iex == 1) {
      message = "calibration::insert_into_cal_db coil already exists serial " + QString::number(this->sernum);
      emit this->tx_cal_message(this->channel, this->slot, message);
      return 0;

    } else {
      // a message was already emitted
      return 0;
    }
  }

  QString prep = "INSERT INTO ";
  QString query_str;
  QString prep_append = " (`serial`, `chopper`, `f`, `a`, `p`) ";

  query_str = prep + this->sensor_types_tables.value(this->sensortype) + prep_append;

  QString values(" VALUES");

  if ((this->f_on.size() == this->ampl_on.size()) && (this->f_on.size() == this->phase_grad_on.size()) && f_on.size()) {
    for (size_t i = 0; i < this->f_on.size(); ++i) {
      values.append("( ");
      values.append(QString::number(this->sernum) + ",");
      values.append(QString::number(1) + ",'");
      values.append(QString::number(this->f_on.at(i), 'E', 15) + "','");
      values.append(QString::number(this->ampl_on.at(i), 'E', 15) + "','");
      values.append(QString::number(this->phase_grad_on.at(i), 'E', 15));
      values.append("') ");
      if (i < this->f_on.size() - 1)
        values.append(", ");
    }

    if (!query.exec(query_str + values)) {
      message = "calibration::insert_into_cal_db -> can not insert caldata, chopper on";
      qDebug() << query_str + values;
      qDebug() << message;
      emit this->tx_cal_message(this->channel, this->slot, message);
      return 0;
    }
  }
  values = " VALUES";
  if ((this->f_off.size() == this->ampl_off.size()) && (this->f_off.size() == this->phase_grad_off.size()) && f_off.size()) {
    for (size_t i = 0; i < this->f_off.size(); ++i) {
      values.append("( ");
      values.append(QString::number(this->sernum) + ",");
      values.append(QString::number(0) + ",'");
      values.append(QString::number(this->f_off.at(i), 'E', 15) + "','");
      values.append(QString::number(this->ampl_off.at(i), 'E', 15) + "','");
      values.append(QString::number(this->phase_grad_off.at(i), 'E', 15));
      values.append("') ");
      if (i < this->f_off.size() - 1)
        values.append(", ");
    }

    if (!query.exec(query_str + values)) {
      message = "calibration::insert_into_cal_db -> can not insert caldata, chopper off";
      qDebug() << query_str + values;
      qDebug() << message;

      return 0;
    }
  }

  message = "insert SQL " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") + QString::number(f_on.size()) + QString(" ,  chopper off:") + QString::number(f_off.size());
  emit this->tx_cal_message(this->channel, this->slot, message);

  return f_on.size() + f_off.size();
}

int calibration::coil_exist(const bool insert) {

  if (!this->sensor_db.isOpen())
    return 0;
  QString message;
  QSqlQuery query(this->sensor_db);
  QString ins = "ser " + this->sensortype.toLower();

  if (query.exec("SELECT `serial`, `datetime` from `" + ins + "` WHERE `serial`='" + QString::number(this->sernum) + "'")) {
    ;
    query.first();
    if (query.isValid()) {
      this->sernum = query.value(0).toInt();
      qDebug() << query.value(1).toString();
      this->caltime = QDateTime::fromString(query.value(1).toString(), "yyyy-MM-ddTHH:mm:ss");
      return 1;

    }
    // query invalid?
    else if (insert) {
      if (!query.exec("INSERT INTO `" + ins + "` (`serial`, `datetime`) VALUES ('" + QString::number(this->sernum) + "','" + this->caltime.toString("yyyy-MM-ddTHH:mm:ss") + "')")) {
        message = "calibration::coil_exist(insert) -> could not insert serial " + QString::number(this->sernum);
        emit this->tx_cal_message(this->channel, this->slot, message);
        // could not be inserted and does not exist
        return 0;
      }
      // coil inserted and exists now
      else
        return 2;
    }
  }
  return 0;
}

void calibration::create_type_and_date(const QString &sensortype, const int &sernum, const QDateTime &caltime) {
  this->sensortype = sensortype;
  this->sernum = sernum;
  this->caltime = caltime;
}

void calibration::set_f_a_p_fromVector_chopper_on(const std::vector<double> &f_on, const std::vector<double> &ampl_on, const std::vector<double> &phase_grad_on) {
  if ((f_on.size() == ampl_on.size()) && (f_on.size() == phase_grad_on.size()) && (f_on.size() > 3)) {
    this->f_on = f_on;
    this->ampl_on = ampl_on;
    this->phase_grad_on = phase_grad_on;
  }
}

void calibration::set_f_a_p_fromVector_chopper_off(const std::vector<double> &f_off, const std::vector<double> &ampl_off, const std::vector<double> &phase_grad_off) {
  if ((f_off.size() == ampl_off.size()) && (f_off.size() == phase_grad_off.size()) && (f_off.size() > 3)) {
    this->f_off = f_off;
    this->ampl_off = ampl_off;
    this->phase_grad_off = phase_grad_off;
  }
}
/*
int calibration::get_cal(const QString &outsensortype, const int &outsernum, const int chopper,
                         std::vector<double> &f, std::vector<double> &a, std::vector<double> &p)
{




QString sens = outsensortype.toLower();
QString message;

for (int i = 0; i < this->sensor_aliases.size(); ++i) {
    if (!sens.compare(this->sensor_aliases.at(i))){
        this->sensortype = this->sensor_aliases_true_names.at(i);
    }
}
this->sernum = outsernum;
this->chopper = chopper;

if (this->coil_exist(false)) {

    if (this->get_data()) {
        if (chopper) {
            f = this->f_on;
            a = this->ampl_on;
            p = this->phase_grad_on;
        }
        else {
            f = this->f_off;
            a = this->ampl_off;
            p = this->phase_grad_off;

        }
    }
}
else {
    message = "calibration::get_cal -> could not find " + outsensortype + " #" + QString::number(outsernum);
    emit this->cal_message(channel, message);
    return 0;

}

return 0;

}
*/
int calibration::get_cal() {
  cal::cal stt = this->chopper;
  QString message;
  if (this->coil_exist(false)) {

    this->chopper = cal::chopper_off;
    this->get_data();
    this->chopper = cal::chopper_on;
    this->get_data();
    this->chopper = stt;
    message = "calibration::get_cal -> load from SQL " + this->sensortype + " #" + QString::number(this->sernum);
    emit this->tx_cal_message(this->channel, this->slot, message);
  } else {
    message = "calibration::get_cal -> could not find " + this->sensortype + " #" + QString::number(this->sernum);
    emit this->tx_cal_message(this->channel, this->slot, message);
    return 0;
  }

  return 1;
}

void calibration::zerof(std::vector<double> &ampl, std::vector<double> &phase, const double &ampl_0, const double &phase_0) {
  ampl.emplace_back(ampl_0);
  phase.emplace_back(phase_0);
}

bool calibration::select_coil(const QString &sensortype, const int &sernum, bool b_get_cal) {
  QString sens = sensortype.toLower();
  QString message;

  for (int i = 0; i < this->sensor_aliases.size(); ++i) {
    if (!sens.compare(this->sensor_aliases.at(i))) {
      this->sensortype = this->sensor_aliases_true_names.at(i);
    }
  }
  this->sernum = sernum;

  // if coil not found in db
  if (!this->coil_exist(false))
    return false;
  if (b_get_cal) {
    if (this->get_cal())
      return true;

    else
      return false;
  }

  return true;
}

QString calibration::search_coil_dir(const QDir where, const QString &sensortype, const int &sernum,
                                     const int ichan, bool b_get_cal) {
  if (!sensortype.size())
    return QString("empty");
  if (sernum == INT_MAX)
    return QString("empty");

  QStringList entries;
  entries << "*.txt"
          << "*.TXT"
          << "*.cal"
          << "*.CAL";
  QFileInfoList calstxt = where.entryInfoList(entries, QDir::Files);

  for (auto &cmp : calstxt) {
    if (cmp.baseName().startsWith(sensortype, Qt::CaseInsensitive)) {
      int n = sensortype.size();
      QChar es('e');
      bool is_eseries = sensortype.endsWith("e", Qt::CaseInsensitive);
      if ((cmp.baseName().size() > n) && (is_eseries == false) && (cmp.baseName().at(n).toLower() == es)) {
        return QString("empty");
      }
      // if (cmp.baseName().contains("e", Qt::CaseInsensitive) && !sensortype.endsWith("e", Qt::CaseInsensitive)) {
      QString snum = QString::number(sernum);
      if (cmp.baseName().endsWith(snum, Qt::CaseInsensitive)) {
        qDebug() << "calibration::search_coil_dir::found calibration file -> " << cmp.fileName();
        QFileInfo calfi(where.absolutePath() + "/" + cmp.fileName());
        size_t n = 0;
        if (b_get_cal) {
          n = this->read_std_txt_file(ichan, calfi);
          if (!n)
            return QString("empty");
        }
        return calfi.absoluteFilePath();
      }
      //}
    }
  }

  return QString("empty");
}
size_t calibration::get_master_cal(const bool no_cal_found) {

  QString message;

  if (!this->master_calibration_db.isOpen()) {

    message = "calibration::get_master_cal failed -> master cal not open";
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
    return 0;
  }

  // check again
  if (!this->ukn_sensor)
    this->sensortype = this->set_atsheader_sensor(this->sensortype);
  else
    return 0;

  QSqlQuery query(this->master_calibration_db);
  QString query_str;

  this->f_on_master.clear();
  this->ampl_on_master.clear();
  this->phase_grad_on_master.clear();
  this->phase_grad_on_master_stddev.clear();
  this->ampl_on_master_stddev.clear();

  this->f_off_master.clear();
  this->ampl_off_master.clear();
  this->phase_grad_off_master.clear();
  this->phase_grad_off_master_stddev.clear();
  this->ampl_off_master_stddev.clear();

  bool isok;

  for (int choppers = 0; choppers < 2; ++choppers) {

    query_str = "SELECT `f`, `a`, `p`, `da`, `dp` FROM `" + this->sensortype + "` WHERE `chopper` = '" + QString::number(choppers) + "'";

    if (query.exec(query_str)) {

      while (query.next()) {
        if (choppers == cal::chopper_on) {
          this->f_on_master.emplace_back(query.value(0).toDouble(&isok));
          this->ampl_on_master.emplace_back(query.value(1).toDouble(&isok));
          this->phase_grad_on_master.emplace_back(query.value(2).toDouble(&isok));
          this->ampl_on_master_stddev.emplace_back(query.value(3).toDouble(&isok));
          this->phase_grad_on_master_stddev.emplace_back(query.value(4).toDouble(&isok));
        } else {
          this->f_off_master.emplace_back(query.value(0).toDouble(&isok));
          this->ampl_off_master.emplace_back(query.value(1).toDouble(&isok));
          this->phase_grad_off_master.emplace_back(query.value(2).toDouble(&isok));
          this->ampl_off_master_stddev.emplace_back(query.value(3).toDouble(&isok));
          this->phase_grad_off_master_stddev.emplace_back(query.value(4).toDouble(&isok));
        }

        if (!isok) {
          message = "calibration::get_master_cal CAN NOT CONVERT TO DOUBLE -> " + this->sensortype + " " + QString::number(this->sernum) + " " + QString::number(this->chopper);
          emit this->tx_cal_message(this->channel, this->slot, message);
        }
      }

    } else {
      message = "calibration::get_master_cal failed or empty (ok for fluxgate) -> " + this->sensortype;
      emit this->tx_cal_message(this->channel, this->slot, message);
      return 0;
    }

    query.clear();
  }

  this->sort_lower_to_upper_check_duplicates(cal::master_cal, cal::chopper_on);
  this->sort_lower_to_upper_check_duplicates(cal::master_cal, cal::chopper_off);

  // in case we do NOT use for interpolation
  // but substitute because we did not find a calibration and use the master cal instead
  if (no_cal_found) {
    message = "calibration::get_master_cal -> " + this->sensortype + " where is your calibration!?";
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
  }

  return f_off_master.size() + f_on_master.size();
}

void calibration::set_chopper(const cal::cal chop) {
  this->chopper = chop;
}

cal::cal calibration::get_chopper() const {
  return this->chopper;
}

void calibration::master_cal_to_caldata() {
  if (this->is_linear_cal)
    return;
  if (f_on_master.size()) {
    this->f_on.resize(f_on_master.size());
    this->ampl_on.resize(f_on_master.size());
    this->phase_grad_on.resize(f_on_master.size());

    for (size_t i = 0; i < f_on_master.size(); ++i) {
      this->f_on[i] = f_on_master[i];
      this->ampl_on[i] = this->ampl_on_master[i];
      this->phase_grad_on[i] = this->phase_grad_on_master[i];
    }
  }

  if (f_off_master.size()) {
    this->f_off.resize(f_off_master.size());
    this->ampl_off.resize(f_off_master.size());
    this->phase_grad_off.resize(f_off_master.size());

    for (size_t i = 0; i < f_off_master.size(); ++i) {
      this->f_off[i] = f_off_master[i];
      this->ampl_off[i] = this->ampl_off_master[i];
      this->phase_grad_off[i] = this->phase_grad_off_master[i];
    }
  }
}

QMultiMap<QString, int> calibration::all_sensors_in_db() {

  QMultiMap<QString, int> alldb;

  QStringList alltables, allserials;

  if (!this->sensor_db.isOpen())
    return alldb;

  QString message;
  QString query_str = "SELECT name FROM sqlite_master WHERE type='table' AND name != 'sensortypes' AND name like '%ser%'";
  QSqlQuery query(this->sensor_db);

  if (query.exec(query_str)) {
    while (query.next()) {
      alltables.append(query.value(0).toString());
    }

  }

  else {
    message = "calibration::all_sensors_in_db -> could not get sensors and serials";
    emit this->tx_cal_message(this->channel, this->slot, message);
    return alldb;
  }

  if (!alltables.size()) {
    message = "calibration::all_sensors_in_db -> could not get sensors and serials";
    emit this->tx_cal_message(this->channel, this->slot, message);
    return alldb;
  }

  for (auto &tab : alltables) {
    query.clear();
    query_str = "SELECT serial FROM `" + tab + "` ORDER BY serial ASC;";
    QString sname = tab.remove("ser ");
    if (query.exec(query_str)) {
      while (query.next()) {
        alldb.insert(sname, query.value(0).toInt());
      }
    }
  }

  if (!alldb.size()) {
    message = "calibration::all_sensors_in_db -> could not get sensors and serials";
    emit this->tx_cal_message(this->channel, this->slot, message);
    return alldb;
  }

  return alldb;
}

size_t calibration::write_std_txt_file(const QFileInfo &qfi) {

  if (this->f_on.size() || this->f_off.size()) {

    QFile calfile(qfi.absoluteFilePath());
    QTextStream qts;

    if (calfile.open(QIODevice::WriteOnly)) {
      qts.setDevice(&calfile);
      qts << "Calibration measurement with Solartron" << Qt::endl;
      qts << "Metronix GmbH, Kocherstr. 3, 38120 Braunschweig" << Qt::endl
          << Qt::endl;
      qts << " Magnetometer: " << this->sensortype << " #" << QString::number(this->sernum) << "   Date: " << this->caltime.toString("dd/MM/yy") << "  Time:" << this->caltime.toString("HH:mm:ss") << Qt::endl
          << Qt::endl;
      if (this->f_on.size()) {
        qts << Qt::endl;
        qts << "FREQUENCY    MAGNITUDE    PHASE" << Qt::endl;
        qts << "Hz           V/(nT*Hz)    deg" << Qt::endl;
        qts << "Chopper On" << Qt::endl;

        for (size_t i = 0; i < f_on.size(); ++i) {
          qts << QString::number(f_on.at(i), 'E') << "  " << QString::number(ampl_on.at(i), 'E') << "  ";
          if (this->phase_grad_on.at(i) > 0.0)
            qts << " ";
          qts << QString::number(phase_grad_on.at(i), 'E') << Qt::endl;
        }
      }
      if (this->f_off.size()) {
        qts << Qt::endl;
        qts << "FREQUENCY    MAGNITUDE    PHASE" << Qt::endl;
        qts << "Hz           V/(nT*Hz)    deg" << Qt::endl;
        qts << "Chopper Off" << Qt::endl;
        for (size_t i = 0; i < f_off.size(); ++i) {
          qts << QString::number(f_off.at(i), 'E') << "  " << QString::number(ampl_off.at(i), 'E') << "  ";
          if (phase_grad_off.at(i) > 0.0)
            qts << " ";
          qts << QString::number(phase_grad_off.at(i), 'E') << Qt::endl;
        }
      }
      qts << Qt::endl;
      calfile.close();

    } else {
      QString message;
      message = "calibration::write_std_txt_file -> could not open/write " + qfi.absoluteFilePath();
      emit this->tx_cal_message(this->channel, this->slot, message);
      return 0;
    }

  } else {
    QString message;
    message = "calibration::write_std_txt_file ->no on or off data " + qfi.absoluteFilePath();
    emit this->tx_cal_message(this->channel, this->slot, message);
    return 0;
  }

  return this->f_on.size() + f_off.size();
}

size_t calibration::write_csv_table(const QFileInfo &qfi) const {
  if (!(this->f_on.size() + f_off.size()))
    return 0;

  QFile file(qfi.absoluteFilePath());
  if (file.open(QIODevice::WriteOnly)) {
    QTextStream qts;
    qts.setDevice(&file);
    qts.setRealNumberNotation(QTextStream::ScientificNotation);
    qts.setRealNumberPrecision(6);

    qts << this->csv_header.join(",") << Qt::endl;
    size_t i;
    for (i = 0; i < this->f_on.size(); ++i) {
      qts << this->sernum << ", 1, " << this->f_on.at(i) << ", " << this->ampl_on.at(i) << ", " << this->phase_grad_on.at(i) << ", ";
      if ((this->f_on.size() == this->ampl_on_stddev.size()) && (this->f_on.size() == this->phase_grad_on_stddev.size())) {
        qts << this->ampl_on_stddev.at(i) << ", " << this->phase_grad_on_stddev.at(i) << Qt::endl;
      } else {
        qts << "0, 0" << Qt::endl;
      }
    }

    for (i = 0; i < this->f_off.size(); ++i) {
      qts << this->sernum << ", 0, " << this->f_off.at(i) << ", " << this->ampl_off.at(i) << ", " << this->phase_grad_off.at(i) << ", ";
      if ((this->f_off.size() == this->ampl_off_stddev.size()) && (this->f_off.size() == this->phase_grad_off_stddev.size())) {
        qts << this->ampl_off_stddev.at(i) << ", " << this->phase_grad_off_stddev.at(i) << Qt::endl;
      } else {
        qts << "0, 0" << Qt::endl;
      }
    }
    qts << Qt::endl;
  }

  file.close();

  return this->f_on.size() + f_off.size();
}

QString calibration::create_xlmstr() const {
  tinyxmlwriter tixw(false);
  tixw.push("calibration");

  tixw.element("cal_version", "1.0");
  tixw.element("creator", "lib calibration");
  tixw.element("user", "metronix");

  tixw.push("calibrated_item");
  if (this->sensortype.startsWith("fg", Qt::CaseInsensitive))
    tixw.element("ci", "identifier=\"fluxgate\"", this->sensortype);
  if (this->sensortype.contains("squid", Qt::CaseInsensitive))
    tixw.element("ci", "identifier=\"squid\"", this->sensortype);
  if (this->sensortype.contains("efp", Qt::CaseInsensitive))
    tixw.element("ci", "identifier=\"electrode\"", this->sensortype);
  else
    tixw.element("ci", "identifier=\"coil\"", this->sensortype);
  tixw.element("ci_serial_number", QString::number(this->sernum));
  tixw.emptyelement("ci_revision");
  tixw.element("ci_date", this->caltime.toString("yyyy-MM-dd"));
  tixw.element("ci_time", this->caltime.toString("HH:mm:ss"));
  tixw.emptyelement("ci_calibration_valid_until");
  tixw.emptyelement("ci_next_calibration");
  tixw.emptyelement("ci_tag");
  tixw.emptyelement("ci_owner");
  tixw.emptyelement("ci_owners_address");
  tixw.element("ci_manufacturer", "metronix");
  tixw.element("ci_manufacturers_address", "Kocher Str. 3, 38120 Braunschweig, Germany");
  tixw.pop("calibrated_item");

  tixw.push("calibration_equipment");
  tixw.emptyelement("ce");
  tixw.emptyelement("ce_serial_number");
  tixw.emptyelement("ce_revision");
  tixw.emptyelement("ce_date");
  tixw.emptyelement("ce_time");
  tixw.emptyelement("ce_calibration_valid_until");
  tixw.emptyelement("ce_next_calibration");
  tixw.emptyelement("ce_tag");
  tixw.emptyelement("ce_operator");
  tixw.element("ce_location", "Magnetsrode");
  tixw.element("ce_contact_address", "Kocher Str. 3, 38120 Braunschweig, Germany");
  tixw.emptyelement("ce_comments");
  tixw.pop("calibration_equipment");

  tixw.push("calibration_protocol");
  tixw.push("mtx");
  tixw.emptyelement("mtx_serial_numer_engraved");
  tixw.emptyelement("mtx_preamplifier_serialnumber");
  tixw.emptyelement("mtx_ch1_div_ch2_at_1000gain_0dot025_hz");
  tixw.emptyelement("mtx_phase_deg_at_1000gain_0dot025_hz");
  tixw.emptyelement("mtx_ch1_div_ch2_at_1000gain_0dot025_hz_calibrated");
  tixw.emptyelement("mtx_phase_deg_at_1000gain_0dot025_hz_calibrated");
  tixw.emptyelement("rec_freq_resp_at_0dot_0025_to_100_hz_at_gain_1000");
  tixw.emptyelement("rec_freq_resp_at_0dot_0025_to_100_hz_at_gain_100");
  tixw.emptyelement("rec_freq_resp_at_0dot_0025_to_100_hz_at_gain_10");
  tixw.emptyelement("rec_freq_resp_at_0dot_0025_to_100_hz_at_gain_1");
  tixw.emptyelement("gain_factor_preamplifier");
  tixw.emptyelement("actual_sensitivity_ch2_by_ch1");
  tixw.emptyelement("actual_sensitivity_theta");
  tixw.emptyelement("calibrated_sensitivity_ch2_by_ch1");
  tixw.emptyelement("calibrated_sensitivity_theta");
  tixw.emptyelement("calibrated_cal_path_ch2_by_ch1");
  tixw.emptyelement("calibrated_cal_path_theta");
  tixw.emptyelement("calibrated_chopper_on");
  tixw.emptyelement("calibrated_chopper_off");
  tixw.emptyelement("calibrated_chopper_ukn");
  tixw.pop("mtx");
  tixw.pop("calibration_protocol");
  tixw.comment("deg is degrees from 0-360");
  QString caldata_qualifier;
  caldata_qualifier = "chopper=\"on\" gain_3=\"0\" gain_2=\"0\" gain_1=\"0\" impedance=\"(0,0)\"";
  for (size_t i = 0; i < this->f_on.size(); ++i) {
    tixw.push("caldata", caldata_qualifier);
    tixw.sci_element("c0", "unit=\"V\"", 0.0, 6);
    tixw.sci_element("c1", "unit=\"Hz\"", this->f_on.at(i), 6);
    tixw.sci_element("c2", "unit=\"V/(nT*Hz)\"", this->ampl_on.at(i), 6);
    tixw.sci_element("c3", "unit=\"deg\"", this->phase_grad_on.at(i), 6);
    tixw.pop("caldata");
  }
  caldata_qualifier = "chopper=\"off\" gain_3=\"0\" gain_2=\"0\" gain_1=\"0\" impedance=\"(0,0)\"";
  for (size_t i = 0; i < this->f_off.size(); ++i) {
    tixw.push("caldata", caldata_qualifier);
    tixw.sci_element("c0", "unit=\"V\"", 0.0, 6);
    tixw.sci_element("c1", "unit=\"Hz\"", this->f_off.at(i), 6);
    tixw.sci_element("c2", "unit=\"V/(nT*Hz)\"", this->ampl_off.at(i), 6);
    tixw.sci_element("c3", "unit=\"deg\"", this->phase_grad_off.at(i), 6);
    tixw.pop("caldata");
  }
  // <caldata chopper="on" gain_2="0" gain_3="0" gain_1="0" impedance="(0,0)">
  //    <c0 unit="V">0.00000000e+00</c0>
  //    <c1 unit="Hz">1.00000000e+04</c1>
  //    <c2 unit="V/(nT*Hz)">5.15550000e-05</c2>
  //    <c3 unit="deg">-5.78930000e+01</c3>
  // <caldata chopper="off" gain_2="0" gain_3="0" gain_1="0" impedance="(0,0)">

  tixw.pop("calibration");

  return tixw.getXmlStr();
}

size_t calibration::write_xml(const QFileInfo &qfi) const {

  if (f_on.size() || f_off.size()) {
    QFile xmlcal(qfi.absoluteFilePath());

    if (!xmlcal.open(QIODevice::WriteOnly | QIODevice::Text)) {
      std::cerr << "calibration -> can not open " << qfi.absoluteFilePath().toStdString() << std::endl;
      return 0;
    }

    QTextStream out(&xmlcal);
    out << this->create_xlmstr();
    xmlcal.close();
  }

  return f_on.size() + f_off.size();
}

size_t calibration::write_json(const QFileInfo &qfi) {
  QFile jsonFile(qfi.absoluteFilePath());
  if (jsonFile.open(QFile::WriteOnly)) {
    jsonFile.write(this->toJsonByteArray());
    jsonFile.close();

    return 0;
  }
  return f_on.size() + f_off.size();
}

void calibration::update_calinfo() {
  if (this->sensortype.startsWith("fg", Qt::CaseInsensitive))
    this->calinfo.insert("ci_sensortype", QString("fluxgate"));
  if (this->sensortype.contains("squid", Qt::CaseInsensitive))
    this->calinfo.insert("ci_sensortype", QString("squid"));
  if (this->sensortype.contains("efp", Qt::CaseInsensitive))
    this->calinfo.insert("ci_sensortype", QString("electrode"));
  else
    this->calinfo.insert("ci_sensortype", "coil");
  this->calinfo.insert("ci", this->sensortype);

  this->calinfo.insert("units_f", "Hz");
  this->calinfo.insert("units_amplitude", "V/(nT*Hz)");
  this->calinfo.insert("units_phase", "degrees");

  this->calinfo.insert("ci_serial_number", this->sernum);
  this->calinfo.insert("ci_date", this->caltime.date().toString("yyyy-MM-dd"));
}

QByteArray calibration::toJsonByteArray(QJsonDocument::JsonFormat format, cal::cal chopper) {

  this->update_calinfo();

  if (this->root_obj != nullptr)
    this->root_obj.reset();
  this->root_obj = std::make_unique<QJsonObject>();

  auto item = std::make_unique<QJsonObject>();

  if (((chopper == cal::cal::chopper_undef) | (chopper == cal::cal::chopper_on)) && this->f_on.size()) {
    auto chon = std::make_unique<QJsonObject>();
    QJsonArray ar_fon;

    for (auto &f : this->f_on) {
      ar_fon << f;
    }
    QJsonArray ar_aon;
    for (auto &a : this->ampl_on) {
      ar_aon << a;
    }
    QJsonArray ar_pon;
    for (auto &p : this->phase_grad_on) {
      ar_pon << p;
    }
    chon->insert("frequency", ar_fon);
    chon->insert("amplitude", ar_aon);
    chon->insert("phase", ar_pon);

    item->insert("chopper on", *chon);
  }

  if (((chopper == cal::cal::chopper_undef) | (chopper == cal::cal::chopper_off)) && this->f_off.size()) {

    auto choff = std::make_unique<QJsonObject>();

    QJsonArray ar_foff;
    for (auto &f : this->f_off) {
      ar_foff << f;
    }
    QJsonArray ar_aoff;
    for (auto &a : this->ampl_off) {
      ar_aoff << a;
    }
    QJsonArray ar_poff;
    for (auto &p : this->phase_grad_off) {
      ar_poff << p;
    }

    choff->insert("frequency", ar_foff);
    choff->insert("amplitude", ar_aoff);
    choff->insert("phase", ar_poff);

    item->insert("chopper off", *choff);
  }

  item->insert("values", QJsonObject::fromVariantMap(this->calinfo));

  this->root_obj->insert("calibration", *item);

  if (this->json_doc != nullptr)
    this->json_doc.reset();
  this->json_doc = std::make_unique<QJsonDocument>(*root_obj);

  return this->json_doc->toJson(format);
}

size_t calibration::fromQBytearray(const QByteArray &json_bytes) {
  auto json_doc = QJsonDocument::fromJson(json_bytes);

  if (json_doc.isNull()) {
    qDebug() << Q_FUNC_INFO << "Failed to create JSON doc.";
    return false;
  }
  if (!json_doc.isObject()) {
    qDebug() << Q_FUNC_INFO << "JSON is not an object.";
    return false;
  }

  auto jobjects = json_doc.object();

  auto jcal = jobjects["calibration"].toObject();
  const QJsonObject &jcalon = jcal["chopper on"].toObject();
  const QJsonObject &jcaloff = jcal["chopper off"].toObject();
  const QJsonObject &jvals = jcal["values"].toObject();

  this->calinfo = jvals.toVariantMap();

  if (!jcalon.isEmpty()) {
  }
  if (!jcaloff.isEmpty()) {
  }
  if (!this->f_on.size() && !this->f_off.size()) {
    const QJsonObject &ja = jcal["amplitude"].toObject();
    const QJsonObject &jf = jcal["frequency"].toObject();
    const QJsonObject &jp = jcal["phase"].toObject();
  }

  iterate_json(jobjects);

  return this->f_on.size() + this->f_off.size();
}

size_t calibration::from_JSON(const int channel, const QFileInfo *qfi, const QByteArray *qba) {
  if (qfi != nullptr) {
    if (!qfi->exists())
      return false;
    QFile file_obj(qfi->absoluteFilePath());
    if (!file_obj.open(QIODevice::ReadOnly)) {
      qDebug() << Q_FUNC_INFO << "Failed to open " << qfi->absoluteFilePath();
      return 0;
    }

    QByteArray json_bytes(file_obj.readAll());
    file_obj.close();

    if (this->fromQBytearray(json_bytes)) {
      qDebug() << Q_FUNC_INFO << "CAL built from file";
      this->channel = channel;
      return this->f_on.size() + this->f_off.size();
    }

    return 0;
  }

  if (qba != nullptr) {

    return 0;
  }

  return 0;
}

void calibration::iterate_json(const QJsonObject &qjd) {
  QJsonObject::const_iterator first = qjd.constBegin();
  QJsonObject::const_iterator last = qjd.constEnd();

  while (first != last) {
    if (!first.value().isObject()) {
      qDebug() << first.key();

      if (first.key() == "chopper on") {
        this->read_json_chopper_on = true;
      } else if (first.key() == "chopper off") {
        this->read_json_chopper_on = false;
      } else if (first.key() == "values") {
        //     item->insert("values", QJsonObject::fromVariantMap(this->calinfo));
        this->calinfo = qjd.toVariantMap();
      }
    }
    if (first.value().isObject()) {
      qDebug() << "object: " << first.value().toObject();
      this->iterate_json(first.value().toObject());
    }

    ++first;
  }
}
void calibration::init_calinfo() {

  QString message;

  if (!this->info_db.isOpen()) {
    message = "calibration::init_calinfo failed -> info db not open";
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
    return;
  }

  QSqlQuery query(this->info_db);

  QString query_str("SELECT calibration_header.key, calibration_header.value FROM calibration_header");

  if (query.exec(query_str)) {
    this->calinfo.clear();
    while (query.next()) {
      this->calinfo.insert(query.value(0).toString(), query.value(1));
    }
  }

  if (!this->calinfo.size()) {
    message = "calibration::init_calinfo failed -> NO data loaded from master cal";
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
    return;
  }

  return;
}

double calibration::get_has_chopper_f() {

  if (this->sensor_db.isOpen())
    return -1;
  QString message;
  QString query_str = "SELECT `chopperon_lessequal` FROM  `sensortypes` WHERE `Name` = " +
                      this->sensortype;
  QSqlQuery query(this->sensor_db);

  bool isok = false;

  if (query.exec(query_str)) {
    query.first();
    if (query.isValid()) {
      this->chopper_f = query.value(0).toDouble(&isok);
    } else
      this->chopper_f = 0;

    if (!isok) {
      message = "calibration::get_has_chopper_f CAN NOT CONVERT TO DOUBLE -> " + this->sensortype + " " + QString::number(this->sernum) + " " + QString::number(this->chopper);
      emit this->tx_cal_message(this->channel, this->slot, message);
    }

    return this->chopper_f;
  }

  return -1;
}

QMap<QString, QString> calibration::get_sensor_types_tables() const {
  return this->sensor_types_tables;
}

int calibration::sensors_summary() {

  if (!this->sensor_db.isOpen())
    return 0;

  QSqlQuery query(this->sensor_db);
  QString message, query_str;
  QString sub_query = "`";

  int isize;
  int isum = 0;

  QMap<QString, QString>::const_iterator i = this->sensor_types_tables.constBegin();

  while (i != this->sensor_types_tables.constEnd()) {

    query_str = "SELECT `serial` FROM `ser " + i.key().toLower() + sub_query;

    if (!query.exec(query_str)) {
      message = "calibration:sensors_summary WRONG " + i.key();
      emit this->tx_cal_message(this->channel, this->slot, message);
    } else {
      isize = 0;
      while (query.next()) {
        ++isize;
      }
      message = i.key() + " " + QString::number(isize) + " coils found";
      emit this->tx_cal_message(this->channel, this->slot, message);
      isum += isize;
      if (isize > 0) {
        if (!this->selectable_sensors.contains(i.key()))
          this->selectable_sensors.append(i.key());
      }
      isize = 0;
    }

    // qDebug() << message;
    ++i;
  }
  return isum;
}

int calibration::f_index(const std::vector<double> &vf, const double freq) const {
  size_t i = 0;
  for (auto &f : vf) {
    if (f == freq)
      return i;
    ++i;
  }

  return -1;
}

size_t calibration::gen_theo_cal(const std::vector<double> &fin_f, const double dad, const double dpg, const bool limit_fcal_high) {

  if (this->is_linear_cal)
    return 0;
  std::complex<double> p1, p2, p3, p4, p5, p6, p7, im(0.0, 1.0), trf;
  std::vector<double> fin;
  fin.reserve(fin_f.size());
  QString message;

  // fluxgate is a theoretical as far we know
  if (limit_fcal_high && !this->sensortype.contains("FGS")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin_f) {
        if (f <= this->f_on.back())
          fin.emplace_back(f);
      }
    }
    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin_f) {
        if (f <= this->f_off.back())
          fin.emplace_back(f);
      }
    }

  }
  //!< @todo where to document the 100Hz limit?
  // above 100Hz we start to see amplitude and phase shifts
  else if (this->sensortype.contains("FGS")) {
    for (auto &f : fin_f) {
      if (f <= 100.0)
        fin.emplace_back(f);
    }
  }

  else
    fin = fin_f;

  if (this->chopper == cal::chopper_on) {
    this->f_on_theo.clear();
    this->ampl_on_theo.clear();
    this->phase_grad_on_theo.clear();
    this->ampl_on_theo_stddev.clear();
    this->phase_grad_on_theo_stddev.clear();
    this->f_on_theo.reserve(fin.size());
    this->ampl_on_theo.reserve(fin.size());
    this->phase_grad_on_theo.reserve(fin.size());
    if (dad > 0.0001)
      this->ampl_on_theo_stddev.reserve(fin.size());
    else
      this->ampl_on_theo_stddev.clear();
    if (dpg > 0.0001)
      this->phase_grad_on_theo_stddev.reserve(fin.size());
    else
      this->phase_grad_on_theo_stddev.clear();
  }
  if (this->chopper == cal::chopper_off) {
    this->f_off_theo.clear();
    this->ampl_off_theo.clear();
    this->phase_grad_off_theo.clear();
    this->ampl_off_theo_stddev.clear();
    this->phase_grad_off_theo_stddev.clear();
    this->f_off_theo.reserve(fin.size());
    this->ampl_off_theo.reserve(fin.size());
    this->phase_grad_off_theo.reserve(fin.size());
    if (dad > 0.0001)
      this->ampl_off_theo_stddev.reserve(fin.size());
    else
      this->ampl_off_theo_stddev.clear();
    if (dpg > 0.0001)
      this->phase_grad_off_theo_stddev.reserve(fin.size());
    else
      this->phase_grad_off_theo_stddev.clear();
  }

  // for old 05 we must make a new one

  if ((this->sensortype == "MFS-06e") || (this->sensortype == "MFS-10e") || (this->sensortype == "MFS-06") || (this->sensortype == "MFS-05")) {

    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);

        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 90);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 90.0);

        } else {
          gen_trf_mfs06e_chopper_on_template(f, p1, p2, p4, im, trf);
          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_on_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }

      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 90.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 90.0);

        } else {
          gen_trf_mfs06e_chopper_off_template(f, p1, p2, p3, p4, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }

      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_off_theo.size();
    }
  }
  if (this->sensortype == "MFS-09u") {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);

        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 90);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 90.0);

        } else {
          gen_trf_mfs09u_chopper_on_template(f, p1, p2, im, trf);
          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_on_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }

      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 90.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 90.0);

        } else {
          gen_trf_mfs09u_chopper_on_template(f, p1, p2, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }

      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_off_theo.size();
    }
  }
  if (this->sensortype == "MFS-07e") {

    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 90.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 90.0);

        } else {

          gen_trf_mfs07e_chopper_on_template(f, p1, p2, p4, im, trf);
          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_on_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 0.0, 1.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 0.0, 1.0);

        } else {

          gen_trf_mfs07e_chopper_off_template(f, p1, p2, p3, p4, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }

      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_off_theo.size();
    }
  }

  if (this->sensortype == "MFS-07") {

    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 90.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 90.0);

        } else {

          gen_trf_mfs07_chopper_on_template(f, p1, p2, p4, im, trf);

          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_on_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);

        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 0.0, 1.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 0.0, 1.0);

        } else {

          gen_trf_mfs07_chopper_off_template(f, p1, p2, p3, p4, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }

      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_off_theo.size();
    }
  }

  if ((this->sensortype == "SHFT-02") || (this->sensortype == "SHFT-02e")) {

    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        // theo does not work below 100 - master cal has some more
        if (f > 99)
          this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 90.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 90.0);

        } else {

          gen_trf_shft02e_template(f, p1, im, trf);

          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_on_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        // theo does not work below 100 - master cal has some more
        if (f > 99)
          this->f_off_theo.emplace_back(f);

        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 0.0, 1.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 0.0, 1.0);

        } else {

          gen_trf_shft02e_template(f, p1, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }

      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_off_theo.size();
    }
  }

  // geomag-01 raklin
  if (this->sensortype == "FGS-02") {

    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);

        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_fgs02_template(trf);
          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_on_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_fgs02_template(trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);

      return this->f_off_theo.size();
    }
  }

  // bartington mag-03, low noise, 100 000 nT
  if ((this->sensortype == "FGS-03") || (this->sensortype == "FGS-03e") || (this->sensortype == "FGS-04") || (this->sensortype == "FGS-04e")) {

    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_fgs100nt_template(trf);
          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_fgs100nt_template(trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  // bartington mag-03, low noise, 70 000 nT mybe never sold
  if ((this->sensortype == "FGS-05") || (this->sensortype == "FGS-05e")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_fgs70nt_template(trf);
          this->ampl_on_theo.emplace_back(abs(trf) / f);
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_fgs70nt_template(trf);
          this->ampl_off_theo.emplace_back(abs(trf) / f);
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  // -------------------------------------- BOARDS  ---------------------------------------------------------------------
  if ((this->sensortype == "ADB08e-HF")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_adb_08e_hf(f, p1, p2, p3, p4, this->adu08e_hp, im, trf);
          this->ampl_on_theo.emplace_back(abs(trf));
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_adb_08e_hf(f, p1, p2, p3, p4, this->adu08e_hp, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf));
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  if ((this->sensortype == "ADB08e-LF")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_adb_08e_lf(f, p1, p2, p3, p4, this->sensor_resistance, this->input_div, this->adu08e_rf_1_2, this->adu08e_lp, im, trf);

          this->ampl_on_theo.emplace_back(abs(trf));
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_adb_08e_lf(f, p1, p2, p3, p4, this->sensor_resistance, this->input_div, this->adu08e_rf_1_2, this->adu08e_lp, im, trf);

          this->ampl_off_theo.emplace_back(abs(trf));
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  if ((this->sensortype == "ADB07e-HF")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_adb_07e_hf(f, p1, p2, p3, this->adu07e_hp, this->gain_1, this->gain_2, im, trf);
          this->ampl_on_theo.emplace_back(abs(trf));
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_adb_07e_hf(f, p1, p2, p3, this->adu07e_hp, this->gain_1, this->gain_2, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf));
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  if ((this->sensortype == "ADB07e-LF")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_adb_07e_lf(f, p1, p2, p3, p4, this->sensor_resistance, this->input_div, this->adu07e_rf_1_2_3_4, this->adu07e_lp, im, trf);

          this->ampl_on_theo.emplace_back(abs(trf));
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_adb_07e_lf(f, p1, p2, p3, p4, this->sensor_resistance, this->input_div, this->adu07e_rf_1_2_3_4, this->adu07e_lp, im, trf);

          this->ampl_off_theo.emplace_back(abs(trf));
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  if ((this->sensortype == "ADB07e-MF")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_adb_07e_mf(f, this->sample_freq, p1, p2, p3, p4, this->sensor_resistance, this->input_div, this->adu07e_rf_1_2_3_4, this->adu07e_lp, this->adu07e_hp, im, trf);

          this->ampl_on_theo.emplace_back(abs(trf));
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_adb_07e_mf(f, this->sample_freq, p1, p2, p3, p4, this->sensor_resistance, this->input_div, this->adu07e_rf_1_2_3_4, this->adu07e_lp, this->adu07e_hp, im, trf);

          this->ampl_off_theo.emplace_back(abs(trf));
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  return 0;
}

size_t calibration::gen_adb_pre_cal(const std::vector<double> &fin_f, const double dad, const double dpg, const bool limit_fcal_high) {

  if (this->is_linear_cal)
    return 0;
  std::complex<double> p1, p2, p3, p4, p5, p6, p7, im(0.0, 1.0), trf;
  std::vector<double> fin;
  fin.reserve(fin_f.size());
  QString message;

  // fluxgate is a theoretical as far we know
  if (limit_fcal_high && !this->sensortype.contains("FGS")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin_f) {
        if (f <= this->f_on.back())
          fin.emplace_back(f);
      }
    }
    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin_f) {
        if (f <= this->f_off.back())
          fin.emplace_back(f);
      }
    }

  }
  //!< @todo where to document the 100Hz limit?
  // above 100Hz we start to see amplitude and phase shifts
  else if (this->sensortype.contains("FGS")) {
    for (auto &f : fin_f) {
      if (f <= 100.0)
        fin.emplace_back(f);
    }
  }

  else
    fin = fin_f;

  if (this->chopper == cal::chopper_on) {
    this->f_on_theo.clear();
    this->ampl_on_theo.clear();
    this->phase_grad_on_theo.clear();
    this->ampl_on_theo_stddev.clear();
    this->phase_grad_on_theo_stddev.clear();
    this->f_on_theo.reserve(fin.size());
    this->ampl_on_theo.reserve(fin.size());
    this->phase_grad_on_theo.reserve(fin.size());
    if (dad > 0.0001)
      this->ampl_on_theo_stddev.reserve(fin.size());
    else
      this->ampl_on_theo_stddev.clear();
    if (dpg > 0.0001)
      this->phase_grad_on_theo_stddev.reserve(fin.size());
    else
      this->phase_grad_on_theo_stddev.clear();
  }
  if (this->chopper == cal::chopper_off) {
    this->f_off_theo.clear();
    this->ampl_off_theo.clear();
    this->phase_grad_off_theo.clear();
    this->ampl_off_theo_stddev.clear();
    this->phase_grad_off_theo_stddev.clear();
    this->f_off_theo.reserve(fin.size());
    this->ampl_off_theo.reserve(fin.size());
    this->phase_grad_off_theo.reserve(fin.size());
    if (dad > 0.0001)
      this->ampl_off_theo_stddev.reserve(fin.size());
    else
      this->ampl_off_theo_stddev.clear();
    if (dpg > 0.0001)
      this->phase_grad_off_theo_stddev.reserve(fin.size());
    else
      this->phase_grad_off_theo_stddev.clear();
  }
  // -------------------------------------- BOARDS  ---------------------------------------------------------------------
  if ((this->sensortype == "ADB08e-HF")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_adb_08e_hf_xcalib(f, p1, p2, p3, p4, this->adu08e_hp, this->gain_1, im, trf);
          this->ampl_on_theo.emplace_back(abs(trf));
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_adb_08e_hf_xcalib(f, p1, p2, p3, p4, this->adu08e_hp, this->gain_1, im, trf);
          this->ampl_off_theo.emplace_back(abs(trf));
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  if ((this->sensortype == "ADB08e-LF")) {
    if (this->chopper == cal::chopper_on) {
      for (auto &f : fin) {
        this->f_on_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_on_theo, this->phase_grad_on_theo, 1.0, 0.0);
          this->zerof(this->ampl_on_theo_stddev, this->phase_grad_on_theo_stddev, 1.0, 0.0);

        } else {
          gen_trf_adb_08e_lf_xcalib(f, p1, p2, p3, p4, this->gain_1, this->gain_2, this->sensor_resistance, this->input_div, this->adu08e_rf_1_2, this->adu08e_lp, im, trf);

          this->ampl_on_theo.emplace_back(abs(trf));
          this->phase_grad_on_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_on_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_on_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper on: ") +
                QString::number(this->f_on_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_on_theo.size();
    }

    if (this->chopper == cal::chopper_off) {
      for (auto &f : fin) {
        this->f_off_theo.emplace_back(f);
        if (f == 0.0) {
          this->zerof(this->ampl_off_theo, this->phase_grad_off_theo, 1.0, 0.0);
          this->zerof(this->ampl_off_theo_stddev, this->phase_grad_off_theo_stddev, 1.0, 0.0);

        }

        else {
          gen_trf_adb_08e_lf_xcalib(f, p1, p2, p3, p4, this->gain_1, this->gain_2, this->sensor_resistance, this->input_div, this->adu08e_rf_1_2, this->adu08e_lp, im, trf);

          this->ampl_off_theo.emplace_back(abs(trf));
          this->phase_grad_off_theo.emplace_back(std::arg(trf) * 180. / M_PI);

          if (dad > 0.0001)
            this->ampl_off_theo_stddev.emplace_back(this->ampl_off_theo.back() * dad);
          if (dpg > 0.0001)
            this->phase_grad_off_theo_stddev.emplace_back(dpg);
        }
      }
      message = "gen theo cal " + this->sensortype + " #" + QString::number(this->sernum) + " -> " + QString("chopper off: ") +
                QString::number(this->f_off_theo.size());
      emit this->tx_cal_message(this->channel, this->slot, message);
      return this->f_off_theo.size();
    }
  }

  return 0;
}

size_t calibration::theo_cal_to_active() {
  if ((this->chopper == cal::chopper_on) && (this->ampl_on_theo.size() == this->f_on_theo.size())) {
    this->f.clear();
    this->cap.resize(this->f_on_theo.size());
    this->f = this->f_on_theo;
    for (size_t i = 0; i < f.size(); ++i) {
      this->cap[i] = std::polar(this->ampl_on_theo.at(i), this->phase_grad_on_theo.at(i) * M_PI / 180.);
    }
  } else if ((this->chopper == cal::chopper_off) && (this->ampl_off_theo.size() == this->f_off_theo.size())) {
    this->f.clear();
    this->cap.resize(this->f_off_theo.size());
    this->f = this->f_off_theo;
    for (size_t i = 0; i < f.size(); ++i) {
      this->cap[i] = std::polar(this->ampl_off_theo.at(i), this->phase_grad_off_theo.at(i) * M_PI / 180.);
    }
  }

  return f.size();
}

void calibration::slot_gen_pure_theo_cal(std::vector<double> &psrc, const double &f_sample, const size_t &start_idx, const size_t &stop_idx,
                                         const double &dc_ampl, const double &dc_phase_deg, const double &mul_by, const bool &mul_by_f, const bool &invert) const {

  if (stop_idx <= start_idx)
    return;
  if ((stop_idx - start_idx) != psrc.size() / 2)
    return;
  if (f_sample == 0.0)
    return;

  std::complex<double> p1, p2, p3, p4, im(0.0, 1.0), trf;
  size_t wlh = psrc.size() / 2;
  double fwl = double(psrc.size());

  QString message;

  size_t starts = start_idx;
  double f;

  // dc part
  if (starts == 0) {
    std::complex<double> zval = std::polar(dc_ampl, dc_phase_deg * M_PI / 180.);
    // we don't want to to anything (mule, scale) in order to set a definit value
    psrc[0] = zval.real();
    psrc[wlh] = -1.0 * zval.imag(); // ref. to fftreal
    ++starts;
  }

  if ((this->sensortype == "MFS-06e") || (this->sensortype == "MFS-10e") || (this->sensortype == "MFS-06")) {

    if (this->chopper == cal::chopper_on) {
      for (size_t i = starts; i < stop_idx; ++i) {

        f = (double(i) * (f_sample / fwl));

        gen_trf_mfs06e_chopper_on_template(f, p1, p2, p4, im, trf);
        if (mul_by_f) {
          trf *= (f * mul_by);
        } else {
          trf *= mul_by;
        }
        if (invert) {
          trf = 1.0 / trf;
        }
        auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
        ctmp *= trf;
        psrc[i] = ctmp.real();
        psrc[wlh + i] = -1.0 * ctmp.imag();
      }
    }

    if (this->chopper == cal::chopper_off) {
      for (size_t i = starts; i < stop_idx; ++i) {

        f = (double(i) * (f_sample / fwl));
        gen_trf_mfs06e_chopper_off_template(f, p1, p2, p3, p4, im, trf);

        if (mul_by_f) {
          trf *= f * mul_by;
        } else {
          trf *= mul_by;
        }
        if (invert) {
          trf = 1.0 / trf;
        }
        auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
        ctmp *= trf;
        psrc[i] = ctmp.real();
        psrc[wlh + i] = -1.0 * ctmp.imag();
      }
    }
  }

  if (this->sensortype == "MFS-07e") {

    if (this->chopper == cal::chopper_on) {
      for (size_t i = starts; i < stop_idx; ++i) {

        f = (double(i) * (f_sample / fwl));

        gen_trf_mfs07e_chopper_on_template(f, p1, p2, p4, im, trf);
        if (mul_by_f) {
          trf *= f * mul_by;
        } else {
          trf *= mul_by;
        }
        if (invert) {
          trf = 1.0 / trf;
        }
        auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
        ctmp *= trf;
        psrc[i] = ctmp.real();
        psrc[wlh + i] = -1.0 * ctmp.imag();
      }
    }

    if (this->chopper == cal::chopper_off) {
      for (size_t i = starts; i < stop_idx; ++i) {

        f = (double(i) * (f_sample / fwl));
        gen_trf_mfs07e_chopper_off_template(f, p1, p2, p3, p4, im, trf);

        if (mul_by_f) {
          trf *= f * mul_by;
        } else {
          trf *= mul_by;
        }
        if (invert) {
          trf = 1.0 / trf;
        }
        auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
        ctmp *= trf;
        psrc[i] = ctmp.real();
        psrc[wlh + i] = -1.0 * ctmp.imag();
      }
    }
  }
  if (this->sensortype == "MFS-07") {

    if (this->chopper == cal::chopper_on) {
      for (size_t i = starts; i < stop_idx; ++i) {

        f = (double(i) * (f_sample / fwl));

        gen_trf_mfs07_chopper_on_template(f, p1, p2, p4, im, trf);
        if (mul_by_f) {
          trf *= f * mul_by;
        } else {
          trf *= mul_by;
        }
        if (invert) {
          trf = 1.0 / trf;
        }
        auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
        ctmp *= trf;
        psrc[i] = ctmp.real();
        psrc[wlh + i] = -1.0 * ctmp.imag();
      }
    }

    if (this->chopper == cal::chopper_off) {
      for (size_t i = starts; i < stop_idx; ++i) {

        f = (double(i) * (f_sample / fwl));
        gen_trf_mfs07_chopper_off_template(f, p1, p2, p3, p4, im, trf);

        if (mul_by_f) {
          trf *= f * mul_by;
        } else {
          trf *= mul_by;
        }
        if (invert) {
          trf = 1.0 / trf;
        }
        auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
        ctmp *= trf;
        psrc[i] = ctmp.real();
        psrc[wlh + i] = -1.0 * ctmp.imag();
      }
    }
  }

  // geomag-01 raklin
  if (this->sensortype == "FGS-02") {

    for (size_t i = starts; i < stop_idx; ++i) {

      f = (double(i) * (f_sample / fwl));

      gen_trf_fgs02_template(trf);
      if (mul_by_f) {
        trf *= f * mul_by;
      } else {
        trf *= mul_by;
      }
      if (invert) {
        trf = 1.0 / trf;
      }
      auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
      ctmp *= trf;
      psrc[i] = ctmp.real();
      psrc[wlh + i] = -1.0 * ctmp.imag();
    }
  }

  // bartington mag-03, low noise, 100 000 nT
  if ((this->sensortype == "FGS-03") || (this->sensortype == "FGS-03e") || (this->sensortype == "FGS-04") || (this->sensortype == "FGS-04e")) {

    for (size_t i = starts; i < stop_idx; ++i) {

      f = (double(i) * (f_sample / fwl));

      gen_trf_fgs100nt_template(trf);
      if (mul_by_f) {
        trf *= f * mul_by;
      } else {
        trf *= mul_by;
      }
      if (invert) {
        trf = 1.0 / trf;
      }
      auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
      ctmp *= trf;
      psrc[i] = ctmp.real();
      psrc[wlh + i] = -1.0 * ctmp.imag();
    }
  }

  // bartington mag-03, low noise, 70 000 nT
  if ((this->sensortype == "FGS-05") || (this->sensortype == "FGS-05e")) {

    for (size_t i = starts; i < stop_idx; ++i) {

      f = (double(i) * (f_sample / fwl));

      gen_trf_fgs70nt_template(trf);
      if (mul_by_f) {
        trf *= f * mul_by;
      } else {
        trf *= mul_by;
      }
      if (invert) {
        trf = 1.0 / trf;
      }
      auto ctmp = std::complex<double>(psrc[i], -1. * psrc[wlh + i]);
      ctmp *= trf;
      psrc[i] = ctmp.real();
      psrc[wlh + i] = -1.0 * ctmp.imag();
    }
  }
}

void calibration::slot_set_chopper(const int c_0_off_1_on) {
  if (c_0_off_1_on == 1)
    this->chopper = cal::chopper_on;
  else
    this->chopper = cal::chopper_off;
}

void calibration::map_to_zero(const double pzx) {

  for (auto &p : phase_grad_off) {
    if (std::abs(p) > pzx)
      p = 0;
  }
  for (auto &p : phase_grad_on) {
    if (std::abs(p) > pzx)
      p = 0;
  }
}

void calibration::div_gain_1() {
  for (auto &a : this->ampl_off) {
    a /= this->gain_1;
  }

  for (auto &a : this->ampl_on) {
    a /= this->gain_1;
  }

  for (auto &a : this->ampl_off_theo) {
    a /= this->gain_1;
  }

  for (auto &a : this->ampl_on_theo) {
    a /= this->gain_1;
  }
}

void calibration::div_gain_2() {
  for (auto &a : this->ampl_off) {
    a /= this->gain_2;
  }
  for (auto &a : this->ampl_on) {
    a /= this->gain_2;
  }

  for (auto &a : this->ampl_off_theo) {
    a /= this->gain_2;
  }
  for (auto &a : this->ampl_on_theo) {
    a /= this->gain_2;
  }
}

void calibration::mult_input_div() {
  if (this->input_div == cal::div_1)
    return;
  double mulme = 1.0;
  if (this->input_div == cal::div_8)
    mulme = 8.0;

  for (auto &a : this->ampl_off) {
    a /= mulme;
  }
  for (auto &a : this->ampl_on) {
    a /= mulme;
  }

  for (auto &a : this->ampl_off_theo) {
    a /= mulme;
  }
  for (auto &a : this->ampl_on_theo) {
    a /= mulme;
  }
}

void calibration::remove_fixed_gain() {
  for (auto &a : this->ampl_off) {
    a /= 2.0;
  }
  for (auto &a : this->ampl_on) {
    a /= 2.0;
  }
}

void calibration::remove_div_8() {
  if (this->input_div == cal::div_1)
    return;

  double d = double(this->input_div);

  for (auto &a : this->ampl_off) {
    a *= d;
  }
  for (auto &a : this->ampl_on) {
    a *= d;
  }
}

std::vector<double> calibration::gen_cal_freqs(const double start, const double stop, const int steps_per_decade) {
  // dist would be log_stop - log_start
  // we calculate per decade

  QString message;
  std::vector<double> cf;

  cf = this->mathv.gen_equidistant_logvector(start, stop, steps_per_decade);

  if (cf.size() == 0) {
    message = "calibration::gen_cal_freqs: could not generate frequency vector!";
    message.append("start: " + QString::number(start) + ", stop: " + QString::number(stop));
    if (stop < start)
      message.append(" wrong order, stop < start !");
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
  }

  // we do not need to sort again here

  return cf;
}

QStringList calibration::get_selectable_sensors() const {

  return this->selectable_sensors;
}

bool calibration::parse_filename(const QFileInfo &qfi) {

  QString filename = qfi.baseName();
  filename = filename.toLower();

  for (int i = 0; i < this->sensor_aliases.size(); ++i) {
    if (filename.startsWith(this->sensor_aliases.at(i))) {
      this->sensortype = this->sensor_aliases_true_names.at(i);
      return true;
    }
  }
  QString message = "can not parse sensor name from filename " + filename;
  emit this->tx_cal_message(this->channel, this->slot, message);
  return false;
}

bool calibration::parse_filename_other_cal(const QFileInfo &qfi) {

  this->sensortype = qfi.baseName();
  this->ukn_sensor = true;

  QStringList tags(qfi.baseName().split("_"));

  for (auto const &tag : tags) {
    if (tag.contains("Gain1", Qt::CaseInsensitive)) {
      QStringList values(tag.split("-"));
      bool ok = false;
      if (values.size() == 2) {
        int d = values.at(1).toInt(&ok);
        if (ok)
          this->gain_1 = double(d);
      }
    }
    if (tag.contains("Gain2", Qt::CaseInsensitive)) {
      QStringList values(tag.split("-"));
      bool ok = false;
      if (values.size() == 2) {
        int d = values.at(1).toInt(&ok);
        if (ok)
          this->gain_2 = double(d);
      }
    }

    if (tag.contains("DIV", Qt::CaseInsensitive)) {
      QStringList values(tag.split("-"));
      bool ok = false;
      if (values.size() == 2) {
        int d = values.at(1).toInt(&ok);
        if (ok)
          this->input_div = int(d);
      }
    }
    if (tag.contains("SensorResistance", Qt::CaseInsensitive)) {
      QStringList values(tag.split("-"));
      bool ok = false;
      if (values.size() == 2) {
        int d = values.at(1).toInt(&ok);
        if (ok)
          this->sensor_resistance = double(d);
      }
    }

    // ------- get the boards -------------------
    if (tag.contains("ADB08e-HF", Qt::CaseInsensitive)) {
      this->sensortype = "ADB08e-HF";
      this->ukn_sensor = false;
    }
    if (tag.contains("ADB08e-LF", Qt::CaseInsensitive)) {
      this->sensortype = "ADB08e-LF";
      this->ukn_sensor = false;
    }
    if (tag.contains("ADB07e-LF", Qt::CaseInsensitive)) {
      this->sensortype = "ADB07e-LF";
      this->ukn_sensor = false;
    }
    if (tag.contains("ADB07e-HF", Qt::CaseInsensitive)) {
      this->sensortype = "ADB07e-HF";
      this->ukn_sensor = false;
    }

    if (tag.contains("ADB07e-MF", Qt::CaseInsensitive)) {
      this->sensortype = "ADB07e-MF";
      this->ukn_sensor = false;
    }
    // -------- get the filters -- set virtually for all systems  that may include fake filters 1 Hz HP for ADU-08e
    // -------- master is ADU-08e

    if (tag.contains("HF-HP-500Hz", Qt::CaseInsensitive)) {
      this->adu08e_hp = cal::adu08e_hp500hz_on;
      this->adu07e_hp = cal::adu07e_hp500hz_on;
    }

    if (tag.contains("HF-HP-1Hz", Qt::CaseInsensitive)) {
      this->adu08e_hp = cal::adu08e_hp1hz_on;
      this->adu07e_hp = cal::adu07e_hp1hz_on;
    }

    if (tag.contains("LF-LP-4Hz", Qt::CaseInsensitive)) {
      this->adu08e_lp = cal::adu08e_lp4hz_on;
      this->adu08e_lp = cal::adu07e_lp4hz_on;
    }

    if (tag.contains("LF-RF-1", Qt::CaseInsensitive)) {
      this->adu08e_rf_1_2 = cal::adu08e_rf_1;
      this->adu07e_rf_1_2_3_4 = cal::adu07e_rf_1;
    }

    if (tag.contains("LF-RF-2", Qt::CaseInsensitive)) {
      this->adu08e_rf_1_2 = cal::adu08e_rf_2;
      this->adu07e_rf_1_2_3_4 = cal::adu07e_rf_2;
    }

    if (tag.contains("LF-RF-3", Qt::CaseInsensitive)) {
      this->adu08e_rf_1_2 = cal::adu07e_rf_3;
      this->adu07e_rf_1_2_3_4 = cal::adu07e_rf_3;
    }

    if (tag.contains("LF-RF-4", Qt::CaseInsensitive)) {
      this->adu08e_rf_1_2 = cal::adu07e_rf_4;
      this->adu07e_rf_1_2_3_4 = cal::adu07e_rf_4;
    }
  }

  return true;
}

void calibration::set_sample_freq(const double &sample_freq) {
  this->sample_freq = sample_freq;
}

size_t calibration::get_data() {

  if (!this->sensor_db.isOpen())
    return 0;

  QSqlQuery query(this->sensor_db);
  QString message, query_str;

  query_str = "SELECT `f`, `a`,`p` FROM " + this->sensor_types_tables.value(this->sensortype) + " WHERE `chopper` = '" + QString::number(this->chopper) + "' AND serial = '" + QString::number(this->sernum) + "'";

  //    query_str = "SELECT printf (\\""%E|%E|%E\\"", `f`, `a`,`p`) FROM " + this->sensor_types_tables.value(this->sensortype)
  //            + " WHERE `chopper` = '" + QString::number(this->chopper) + "' AND serial = '" +  QString::number(this->sernum) + "'" ;

  qDebug() << query_str;
  // SELECT `f`, `a`,`p` from  `cal mfs-06` WHERE `chopper`=1;

  bool isok;
  if (query.exec(query_str)) {
    // if (query.isValid()) {
    if (this->chopper) {
      this->f_on.clear();
      this->ampl_on.clear();
      this->phase_grad_on.clear();
    } else {
      this->f_off.clear();
      this->ampl_off.clear();
      this->phase_grad_off.clear();
    }
    while (query.next()) {
      if (this->chopper) {
        this->f_on.emplace_back(query.value(0).toDouble(&isok));
        this->ampl_on.emplace_back(query.value(1).toDouble(&isok));
        this->phase_grad_on.emplace_back(query.value(2).toDouble(&isok));
      } else {
        this->f_off.emplace_back(query.value(0).toDouble(&isok));
        this->ampl_off.emplace_back(query.value(1).toDouble(&isok));
        this->phase_grad_off.emplace_back(query.value(2).toDouble(&isok));
      }

      if (!isok) {
        message = "calibration::get_data failed, CAN NOT CONVERT TO DOUBLE -> " + this->sensortype + " " + QString::number(this->sernum) + " " + QString::number(this->chopper);
        emit this->tx_cal_message(this->channel, this->slot, message);
      }
    }
    //        }
    //        else {
    //            message = "calibration::get_data failed -> " + this->sensortype + " " + QString::number(this->sernum) + " " +  QString::number(this->chopper);
    //            emit this->cal_message(channel, message);
    //            return 0;
    //        }
  } else {
    message = "calibration::get_data failed -> " + this->sensortype + " " + QString::number(this->sernum) + " " + QString::number(this->chopper);
    emit this->tx_cal_message(this->channel, this->slot, message);
    return 0;
  }

  this->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_off);
  this->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_on);

  if (chopper)
    return f_on.size();
  return f_off.size();
}

void calibration::clear() {

  this->f_on.clear();
  this->f_off.clear();
  this->freqs.clear();
  this->f.clear();
  this->cap.clear();
  this->ampl_off.clear();
  this->ampl_on.clear();
  this->phase_grad_off.clear();
  this->phase_grad_on.clear();
  this->phase_grad_on_stddev.clear();
  this->ampl_on_stddev.clear();
  this->phase_grad_off_stddev.clear();
  this->ampl_off_stddev.clear();

  this->f_on_master.clear();
  this->phase_grad_on_master.clear();
  this->ampl_on_master.clear();
  this->f_off_master.clear();
  this->phase_grad_off_master.clear();
  this->ampl_off_master.clear();
  this->f_on_ipl.clear();
  this->phase_grad_on_ipl.clear();
  this->ampl_on_ipl.clear();
  this->f_off_ipl.clear();
  this->phase_grad_off_ipl.clear();
  this->ampl_off_ipl.clear();

  this->sernum = 0;
  this->chopper = cal::chopper_undef;
  QMap<QString, QVariant>::iterator qv = calinfo.begin();
  while (qv != this->calinfo.end()) {
    qv->setValue(QVariant(""));
    ++qv;
  }

  this->caltime.setMSecsSinceEpoch(0);
  this->sensortype = "";
}

size_t calibration::sort_lower_to_upper_check_duplicates(const cal::cal &what, const cal::cal &chop) {

  QString message;

  size_t iif;

  if (what == cal::calibration) {
    if (chop == cal::chopper_on) {
      if (this->f_on.size()) {

        iif = this->f_on.size();
        if (this->f_on.size() == this->ampl_on_stddev.size() && (this->f_on.size() == this->phase_grad_on_stddev.size())) {
          mathv.sort_vectors(this->f_on, this->ampl_on, this->phase_grad_on, this->ampl_on_stddev, this->phase_grad_on_stddev);
          mathv.remove_duplicates_from_sorted(this->f_on, this->ampl_on, this->phase_grad_on, this->ampl_on_stddev, this->phase_grad_on_stddev, true);
        } else {
          mathv.sort_vectors(this->f_on, this->ampl_on, this->phase_grad_on);
          mathv.remove_duplicates_from_sorted(this->f_on, this->ampl_on, this->phase_grad_on, true);
        }

        if (this->f_on.size() != iif) {
          message = "calibration::sort_lower_to_upper_check_duplicates ->  ************* DUPLICATES had been detected and REMOVED, CHOPPER ON!";
          emit this->tx_cal_message(this->channel, this->slot, message);
        }

        return f_on.size();
      }
    }
    if (chop == cal::chopper_off) {
      if (this->f_off.size()) {

        iif = this->f_off.size();
        if (this->f_off.size() == this->ampl_off_stddev.size() && (this->f_off.size() == this->phase_grad_off_stddev.size())) {
          mathv.sort_vectors(this->f_off, this->ampl_off, this->phase_grad_off, this->ampl_off_stddev, this->phase_grad_off_stddev);
          mathv.remove_duplicates_from_sorted(this->f_off, this->ampl_off, this->phase_grad_off, this->ampl_off_stddev, this->phase_grad_off_stddev, true);
        } else {
          mathv.sort_vectors(this->f_off, this->ampl_off, this->phase_grad_off);
          mathv.remove_duplicates_from_sorted(this->f_off, this->ampl_off, this->phase_grad_off, true);
        }

        if (this->f_off.size() != iif) {
          message = "calibration::sort_lower_to_upper_check_duplicates ->  ************* DUPLICATES had been detected and REMOVED, CHOPPER OFF!";
          emit this->tx_cal_message(this->channel, this->slot, message);
        }
        return f_off.size();
      }
    }
  }
  if (what == cal::master_cal) {
    if (chop == cal::chopper_on) {
      if (this->f_on_master.size()) {

        iif = this->f_on_master.size();
        if (this->f_on_master.size() == this->ampl_on_master_stddev.size() && (this->f_on_master.size() == this->phase_grad_on_master_stddev.size())) {
          mathv.sort_vectors(this->f_on_master, this->ampl_on_master, this->phase_grad_on_master, this->ampl_on_master_stddev, this->phase_grad_on_master_stddev);
          mathv.remove_duplicates_from_sorted(this->f_on_master, this->ampl_on_master, this->phase_grad_on_master, this->ampl_on_master_stddev, this->phase_grad_on_master_stddev, true);
        } else {
          mathv.sort_vectors(this->f_on_master, this->ampl_on_master, this->phase_grad_on_master);
          mathv.remove_duplicates_from_sorted(this->f_on_master, this->ampl_on_master, this->phase_grad_on_master, true);
        }

        if (this->f_on_master.size() != iif) {
          message = "calibration::sort_lower_to_upper_check_duplicates ->  ************* DUPLICATES had been detected and REMOVED, CHOPPER ON!";
          emit this->tx_cal_message(this->channel, this->slot, message);
        }

        return f_on_master.size();
      }
    }
    if (chop == cal::chopper_off) {

      if (this->f_off_master.size()) {

        iif = this->f_off_master.size();
        mathv.sort_vectors(this->f_off_master, this->ampl_off_master, this->phase_grad_off_master);
        mathv.remove_duplicates_from_sorted(this->f_off_master, this->ampl_off_master, this->phase_grad_off_master, true);

        if (this->f_off_master.size() != iif) {
          message = "calibration::sort_lower_to_upper_check_duplicates ->  ************* DUPLICATES had been detected and REMOVED, CHOPPER OFF!";
          emit this->tx_cal_message(this->channel, this->slot, message);
        }

        return f_off_master.size();
      }
    }
  }
  //////////////// interpolated does not carry the stddev
  if (what == cal::interpol_cal) {
    if (chop == cal::chopper_on) {
      if (this->f_on_ipl.size()) {

        iif = this->f_on_ipl.size();
        mathv.sort_vectors(this->f_on_ipl, this->ampl_on_ipl, this->phase_grad_on_ipl);
        mathv.remove_duplicates_from_sorted(this->f_on_ipl, this->ampl_on_ipl, this->phase_grad_on_ipl, true);

        if (this->f_on_ipl.size() != iif) {
          message = "calibration::sort_lower_to_upper_check_duplicates ->  ************* DUPLICATES had been detected and REMOVED, CHOPPER ON!";
          emit this->tx_cal_message(this->channel, this->slot, message);
        }
      }
      return f_on_ipl.size();
    }

    if (chop == cal::chopper_off) {
      if (this->f_off_ipl.size()) {

        iif = this->f_off_ipl.size();
        mathv.sort_vectors(this->f_off_ipl, this->ampl_off_ipl, this->phase_grad_off_ipl);
        mathv.remove_duplicates_from_sorted(this->f_off_ipl, this->ampl_off_ipl, this->phase_grad_off_ipl, true);

        if (this->f_off_ipl.size() != iif) {
          message = "calibration::sort_lower_to_upper_check_duplicates ->  ************* DUPLICATES had been detected and REMOVED, CHOPPER OFF!";
          emit this->tx_cal_message(this->channel, this->slot, message);
        }
      }
      return f_off_ipl.size();
    }
  }
  return 0;
}

QString calibration::get_adb_name() const {
  QString name = this->sensortype;
  if (this->input_div == cal::div_1)
    name.append(", DIV 1, ");
  if (this->input_div == cal::div_8)
    name.append(", DIV 8, ");
  name.append("Gain1 ");
  name.append(QString::number(int(this->gain_1)));
  name.append(", ");

  if (this->dac_on)
    name.append("DAC on, ");
  // if (this->dac_on) name.append("DAC off, ");

  if (this->gain_2 > 1.1) {
    name.append("Gain2 ");
    name.append(QString::number(int(this->gain_2)));
    name.append(", ");
  }

  // setting for 07e will covered while parse_filename_other_cal
  if (this->adu08e_rf_1_2 == cal::adu08e_rf_1)
    name.append("RF-1, ");
  if (this->adu08e_rf_1_2 == cal::adu08e_rf_2)
    name.append("RF-2, ");
  if (this->adu08e_rf_1_2 == cal::adu07e_rf_3)
    name.append("RF-3, ");
  if (this->adu08e_rf_1_2 == cal::adu07e_rf_4)
    name.append("RF-4, ");
  if (this->adu08e_rf_1_2 == cal::adu08e_rf_off)
    name.append("RF-off, ");

  if (this->adu08e_lp == cal::adu08e_lp4hz_on)
    name.append("LF-LP-4Hz");
  if (this->adu08e_hp == cal::adu08e_hp500hz_on)
    name.append("HF-HP-500Hz");
  if (this->adu08e_hp == cal::adu08e_hp1hz_on)
    name.append("HF-HP-1Hz");

  return name;
}

void calibration::set_linear() {
  this->is_linear_cal = true;
}

bool calibration::fold(const std::shared_ptr<calibration> &other_cal) {
  if (other_cal == nullptr)
    return false;
  // have no cal or I am linear
  if (this->is_linear_cal) {
    this->cap = other_cal->cap;
    this->f = other_cal->f;
    return true;
  } else {
    if (this->f.size() == other_cal->f.size()) {
      if ((this->f.front() == other_cal->f.front()) && (this->f.back() == other_cal->f.back())) {
        std::transform(this->cap.begin(), this->cap.end(), other_cal->cap.begin(), this->cap.begin(), std::multiplies<std::complex<double>>());
        // test
        //                if (other_cal->adu08e_hp == cal::adu08e_hp500hz_on) {
        //                    for (auto &val :cap) {
        //                        val *= 1000;
        //                    }
        //                }
        return true;
      } else
        return false;
    } else
      return false;
  }

  return false;
}
