#include "emerald.h"

emerald::emerald(const QFileInfo &qfi, const int &skip_bytes, QObject *parent) :
    QObject(parent), QFileInfo(qfi), prc_com(false) {
  this->skip_bytes = skip_bytes;
}

size_t emerald::open(bool close_again) {
  if (!this->exists()) {
    qDebug() << "file " << this->absoluteFilePath() << "does not exist";
    return 0;
  }

  int i;

  qds.setByteOrder(QDataStream::LittleEndian);
  this->file.setFileName(this->absoluteFilePath());
  this->qds.setDevice(&this->file);
  // new - we must set 32(0) or 64 (1); emerald has 32bit float
  this->qds.setFloatingPointPrecision(QDataStream::FloatingPointPrecision(0));

  if (!this->file.isOpen()) {
    if (!file.open(QIODevice::ReadOnly)) {
      return SIZE_MAX;
    }
  }

  qint8 skc;
  QString strbuf;
  qint64 pos = file.pos();
  // read 100 bytes first
  for (int i = 0; i < 100; ++i) {
    qds >> skc;
    strbuf.append(QChar::fromLatin1(skc));
  }
  // now check possible end;
  bool nextstop = false;
  for (i = 0; i < 400; ++i) {
    pos = file.pos();
    this->qds >> skc;
    // std::cout << skc;
    //  take it if not "_"
    if (skc != 95) {
      strbuf.append(QChar::fromLatin1(skc));
    }

    if (skc == 95)
      nextstop = true;
    if ((skc != 95) && (nextstop == true)) {

      break;
    }
    //        if ((i % 32 == 0) && i ) {
    //            std::cout << std::endl;
    //        }
  }

  this->emh.buffer_to_map(strbuf);

  this->file.seek(pos);

  /*

  if (this->skip_bytes > 0) {

      qint8 skc;
      std::cout << " ------------ skip -------------" << std::endl;
      for (int i = 0; i < skip_bytes ; ++i) {
          this->qds >> skc;
          std::cout << skc;
          if ((i % 32 == 0) && i ) {
              std::cout << std::endl;
          }
      }
      std::cout << std::endl;
      std::cout << " ------------ end -------------" << std::endl;
      std::cout << std::endl;


  }

  if (this->skip_bytes == -1) {

      qint8 skc;
      std::cout << " ------------ skip -------------" << std::endl;
      for (i = 0; i < 180 ; ++i) {
          this->qds >> skc;
          std::cout << skc;
          if ((i % 32 == 0) && i ) {
              std::cout << std::endl;
          }
      }




      bool nextstop = false;
      for (i = 0; i < 100; ++i) {
          pos = file.pos();
          this->qds >> skc;
          std::cout << skc;
          if (skc == 95) nextstop = true;
          if ((skc != 95) && (nextstop == true)) {

              break;
          }
          if ((i % 32 == 0) && i ) {
              std::cout << std::endl;
          }


      }

      this->file.seek(pos);




  qDebug() << "bytes" << file.pos();
      std::cout << std::endl;
      std::cout << int(pos) << " ------------ end -------------" << std::endl;
      this->skip_bytes = int(pos);

      std::cout << std::endl;

      emit this->signal_detected_bytes(this->skip_bytes);

  }





  qint64 pos = file.pos();

  std::string gh_string(this->general_header.str());

  qds.readRawData(&gh_string[0], sizeof(gh_string));
  this->general_header.get_str_data(gh_string);


  qDebug() << "bytes:" << file.pos();
*/

  emit this->signal_detected_bytes(pos);

  qDebug() << "starts at:" << this->emerald_site.value("StartTime");
  qDebug() << "stops at:" << this->emerald_site.value("StopTime");

  this->edt_start.set_date_time_double(this->emerald_site.value("StartTime").toDouble());
  this->edt_start.set_sample_freq(this->emerald_site.value("SampleRate").toDouble());
  this->edt_stop.set_date_time_double(this->emerald_site.value("StopTime").toDouble());

  guessed_samples = edt_start.samples_to(edt_stop);
  guessed_samples += 1000; // add 4 secs to avoid re-alloc

  if (this->emerald_channels.size()) {
    this->nchan = this->emerald_channels.size();

    qDebug() << "preparing:";
    for (auto map : this->emerald_channels) {
      qDebug() << "Channel:" << map.value("Type") << "DataUnit" << map.value("DataUnit");
      qDebug() << "ModuleResponse:" << map.value("ModuleResponse");
    }
  }

  if (close_again)
    this->file.close();

  return this->emerald_channels.size();
}

size_t emerald::check_first(const size_t &nsamples) {
  std::vector<float> data(this->nchan);

  size_t lines = 0;
  size_t i;
  float d;
  qDebug() << "size of float" << sizeof(d) << "and should be 4";

  qint64 pos = file.pos();

  do {
    for (i = 0; i < data.size(); ++i) {
      this->qds >> d;

      data[i] = d;
      std::cout << data[i] << " ";
    }
    std::cout << std::endl;
    ++lines;
  } while (lines < nsamples);

  this->file.seek(pos);

  return lines;
}

size_t emerald::to_textfile(const QFileInfo &name_qfi) {
  std::vector<float> data(this->nchan);

  size_t lines = 0;
  size_t i, j;
  float d;

  this->textfile.setFileName(name_qfi.absoluteFilePath());
  this->textfile.open(QIODevice::WriteOnly | QIODevice::Text);

  if (!textfile.isOpen()) {
    this->file.close();
    return 0;
  }
  QTextStream out(&this->textfile);

  do {
    for (i = 0; i < data.size(); ++i) {
      this->qds >> d;
      data[i] = d;
    }
    ++lines;

    out << lines << " ";
    for (j = 0; j < data.size(); ++j)
      out << data[j] << " ";
    out << Qt::endl;

  } while (!qds.atEnd());
  this->file.close();
  this->textfile.close();

  return lines;
}

bool emerald::set_survey_basedir(const QDir &basedir, const QString &site_str, const bool create) {
  if (!basedir.exists() && create) {

    if (!basedir.mkpath(basedir.absolutePath())) {
      return false;
    }
  }

  if (basedir.exists()) {
    for (auto &str : pmt::survey_dirs) {
      qDebug() << "check/create" << str;
      QDir dirs(basedir.absolutePath() + "/" + str);
      if (dirs.exists())
        qDebug() << "exists";
      if (create) {
        if (!dirs.exists()) {
          if (dirs.mkpath(dirs.absolutePath()))
            qDebug() << "created";
          else
            qDebug() << "creating" << str << "failed!";
        }
        if (str == "ts") {
          this->sdir.setPath(basedir.absolutePath() + "/" + str + "/" + site_str);
          if (this->sdir.exists())
            qDebug() << "exists";
          else if (this->sdir.mkpath(sdir.absolutePath()))
            qDebug() << "created";
          else
            qDebug() << "creating" << this->sdir.absolutePath() << "failed!";
        }
      }
    }
  }

  if (basedir.exists()) {
    this->basedir = basedir;
    return true;
  }
  return false;
}

bool emerald::dip_to_pos(const double length, const double decangle) {
  if ((fabs(length) < 0.001))
    return false;
  double angle = decangle;
  if ((fabs(angle) < 0.5))
    angle = 0.0;
  this->x1 = this->x2 = this->y1 = this->y2 = this->z1 = this->z2 = 0.0;

  const double assume_zero_length = 10E-3;
  this->x1 = -cos(M_PI / 180. * angle) * length / 2.;
  this->x2 = cos(M_PI / 180. * angle) * length / 2.;
  this->y1 = -sin(M_PI / 180. * angle) * length / 2.;
  this->y2 = sin(M_PI / 180. * angle) * length / 2.;

  if (fabs(this->x1) < assume_zero_length)
    this->x1 = 0.;
  if (fabs(this->x2) < assume_zero_length)
    this->x2 = 0.;
  if (fabs(this->y1) < assume_zero_length)
    this->y1 = 0.;
  if (fabs(this->y2) < assume_zero_length)
    this->y2 = 0.;
  if (fabs(this->z1) < assume_zero_length)
    this->z1 = 0.;
  if (fabs(this->z2) < assume_zero_length)
    this->z2 = 0.;

  return true;
}

void emerald::set_collect_only(const bool coll) {
  this->collect_only = coll;
}

eQDateTime emerald::get_start() const {
  return this->edt_start;
}

eQDateTime emerald::get_stop() const {
  return this->edt_stop;
}

bool emerald::set_data_write_append(const std::vector<std::vector<double>> &xdata_in, const QMap<QString, int> &xchannel_map) {
  if (this->data_in.size() != xdata_in.size())
    return false;
  if (this->channel_map.size() != xchannel_map.size())
    return false;
  else {
    QList<QString> mykeys = this->channel_map.keys();
    QList<QString> your_keys = xchannel_map.keys();
    int j = 0;
    for (int i = 0; i < this->channel_map.size(); ++i) {
      if (mykeys.at(i).compare(your_keys.at(i)))
        ++j;
    }
    if (j)
      return false;
  }
  for (size_t i = 0; i < this->data_in.size(); ++i) {
    // this->data_in[i].insert(this->data_in[i].end(), xdata_in[i].begin(), xdata_in[i].end());
    QString myname = this->channel_map.key(i);
    int your_pos = xchannel_map.value(myname);
    this->data_in[i].insert(this->data_in[i].end(), xdata_in[your_pos].begin(), xdata_in[your_pos].end());
  }

  return true;
}

void emerald::write_ats() {
  int i = 0;
  for (auto &a : atsfiles) {
    auto result = std::minmax_element(this->data_in[i].begin(), this->data_in[i].end());
    double min = this->data_in[i][(result.first - this->data_in[i].begin())];
    double max = this->data_in[i][(result.second - this->data_in[i].begin())];
    double lsb;
    if (std::abs(min) > std::abs(max))
      lsb = std::abs(min) * 6 / ((double)INT32_MAX);
    else
      lsb = std::abs(max) * 6 / ((double)INT32_MAX);

    qDebug() << "writing" << a->get_qfile_info().absoluteFilePath();
    qDebug() << "lsb" << lsb << "min" << min << "max" << max << "samples" << a->size();
    // a->set_write_double_vector(this->data_in[i], ( ( max - min ) / ((double)INT32_MAX)));
    //  take 22 bit; this function also calls prc_com_to_classmembers
    // a->set_write_double_vector(this->data_in[i], 1/4194304.0);
    a->set_write_double_vector(this->data_in[i], lsb);

    ++i;
  }
}

void emerald::delete_data() {
  for (size_t i = 0; i < data_in.size(); ++i)
    data_in[i].clear();
  data_in.clear();
}

std::vector<std::vector<double>> emerald::get_data() const {
  return this->data_in;
}

QMap<QString, int> emerald::get_channel_map() const {
  return this->channel_map;
}

void emerald::set_guessed_samples(const quint64 guessed_samples) {
  this->guessed_samples = guessed_samples;
}

quint64 emerald::get_guessed_samples() const {
  return this->guessed_samples;
}

void emerald::set_sernum(const int sern) {
  this->sernum = abs(sern);
}

void emerald::slot_got_atswriter_channel(const QMap<QString, QVariant> &data_map, const int id) {
  this->emerald_channels.append(data_map);
}

void emerald::slot_got_emerald_emerald(const QMap<QString, QVariant> &data_map) {
  this->emerald_site = data_map;
}

void emerald::slot_convert(const quint64 new_guessed_samples) {
  if (!this->emerald_channels.size()) {

    qDebug() << "no channels loaded";
    return;
  }

  size_t i = 0;
  for (auto &chan : this->emerald_channels) {

    QFileInfo qfiukn("test.ats");

    QString chan_name = chan.value("Type").toString();
    if (chan_name.contains("Bx", Qt::CaseInsensitive)) {
      chan_name = "Hx";
      i = 2;
      this->channel_map.insert(chan_name, i);
    }
    if (chan_name.contains("BY", Qt::CaseInsensitive)) {
      chan_name = "Hy";
      i = 3;
      this->channel_map.insert(chan_name, i);
    }
    if (chan_name.contains("Bz", Qt::CaseInsensitive)) {
      chan_name = "Hz";
      i = 4;
      this->channel_map.insert(chan_name, i);
    }
    if (chan_name.contains("Ex", Qt::CaseInsensitive)) {
      chan_name = "Ex";
      i = 0;
      this->channel_map.insert(chan_name, i);
    }
    if (chan_name.contains("Ey", Qt::CaseInsensitive)) {
      chan_name = "Ey";
      i = 1;
      this->channel_map.insert(chan_name, i);
    }

    QString board("LF");
    if (this->emerald_site.value("SampleRate").toDouble() > 4096)
      board = "HF";

    edt_start.set_date_time_double(this->emerald_site.value("StartTime").toDouble());
    edt_start.set_sample_freq(this->emerald_site.value("SampleRate").toDouble());
    edt_stop.set_date_time_double(this->emerald_site.value("StopTime").toDouble());

    qDebug() << chan_name;
    auto xats = std::make_unique<atsfile>(qfiukn, 1, 80, this->sernum, this->emerald_site.value("SampleRate").toDouble(), 1, (int)i, chan_name, board);
    xats->set_start_date_time(edt_start);
    QFileInfo name(xats->get_new_filename());
    QFileInfo qfi;
    if (this->sdir.exists())
      qfi.setFile(this->sdir, name.fileName());
    else
      qfi.setFile(name.fileName());
    qDebug() << qfi.absoluteFilePath();
    this->atsfiles.emplace_back(std::make_shared<atsfile>(qfi, 1, 80, this->sernum, this->emerald_site.value("SampleRate").toDouble(), 1, (int)i, chan_name, board));
    this->atsfiles.back()->set_start_date_time(edt_start);
    this->atsfiles.back()->set_basedir_fake_deep(this->sdir);
    this->data_in.emplace_back(std::vector<double>());
    xats.reset();
  }

  i = 0;
  // atsfiles should be same order like IndexInFile
  for (auto &a : atsfiles) {
    double length = this->emerald_channels.at(i).value("DipoleLength").toDouble();
    double angle = this->emerald_channels.at(i).value("HorizontalOrientation").toDouble();
    // can not be resolved because not real XML; we try
    QString noxml = this->emerald_channels.at(i).value("ModuleResponse").toString();
    int coilserial = 0;
    if (noxml != "empty" && noxml.contains("metronix", Qt::CaseInsensitive)) {
      QStringList items = noxml.split("-");
      noxml = items.last();
      // if (noxml.contains("rsp"), Qt::CaseInsensitive) {
      items = noxml.split(".");
      noxml = items.first();
      coilserial = noxml.toInt();

      //}
      // else noxml = "empty";
    } else
      noxml = "empty";

    a->set_key_value("header_version", 80);
    a->set_key_value("header_length", 1024);
    a->set_key_value("sample_freq", this->emerald_site.value("SampleRate").toDouble());
    a->set_key_value("system_serial_number", this->sernum);
    a->set_key_value("lsb", 1. / 4194304.0);

    if (this->dip_to_pos(length, angle)) {
      a->set_e_pos(this->x1, this->x2, this->y1, this->y2, this->z1, this->z2);
      a->set_key_value("sensor_type", "EFP06");
    } else {
      // assuming H for no dipole
      a->set_key_value("sensor_type", "MFS06");
      if (coilserial)
        a->set_key_value("sensor_sernum", coilserial);
    }
    ++i;
  }

  QString channelchk;
  i = 0;
  for (auto &a : this->atsfiles) {
    channelchk.append(a->channel_type());
    channelchk.append(":");
    channelchk.append(this->emerald_channels.at(i++).value("Type").toString());
    if (i < this->atsfiles.size())
      channelchk.append(", ");
  }
  emit this->signal_start_str(channelchk);

  if (this->file.isOpen())
    this->file.close();
  // read/skip the header
  this->open();

  i = 0;
  int lines = 0;
  QString st_str("> " + this->edt_start.ISO_date_time());
  emit this->signal_start_str(st_str);

  if (new_guessed_samples)
    for (auto &v : this->data_in)
      v.reserve(new_guessed_samples);
  else
    for (auto &v : this->data_in)
      v.reserve(guessed_samples);
  float f;
  do {
    // reads the data in the order of the file
    for (i = 0; i < this->data_in.size(); ++i) {
      this->qds >> f;
      // emerald uses V instead of mV
      //  E is NOT scaled - has to be divided by dipole length
      // unit is V not mV
      this->data_in[i].emplace_back(f * 1000);
    }
    //        for (i = 0; i < this->data_in.size(); ++i) std::cerr << i <<":" <<data_in[i].back() << "     ";
    //        std::cerr << std::endl;
    lines++;
    // std::cout << std::endl;
  } while (!qds.atEnd());

  QString st_strb("  " + this->edt_stop.ISO_date_time() + " <");

  emit this->signal_start_str(st_strb);

  // xats->set_write_double_vector(this->x, ((this->maxx - this->minx)) / ((double)INT32_MAX));
  qDebug() << lines << "samples read";

  if (collect_only == false) {
    i = 0;
    for (auto &a : atsfiles) {
      auto result = std::minmax_element(this->data_in[i].begin(), this->data_in[i].end());
      double min = this->data_in[i][(result.first - this->data_in[i].begin())];
      double max = this->data_in[i][(result.second - this->data_in[i].begin())];
      double lsb;
      if (std::abs(min) > std::abs(max))
        lsb = std::abs(min) * 6 / ((double)INT32_MAX);
      else
        lsb = std::abs(max) * 6 / ((double)INT32_MAX);

      // double lsb = ( max - min ) / ((double)INT32_MAX);
      qDebug() << "writing" << a->get_qfile_info().absoluteFilePath();
      qDebug() << "lsb" << lsb << "min" << min << "max" << max << "samples" << a->size();
      // a->set_write_double_vector(this->data_in[i], ( ( max - min ) / ((double)INT32_MAX)));
      //  take 22 bit; this function also calls prc_com_to_classmembers
      // a->set_write_double_vector(this->data_in[i], 1./4194304.0);
      a->set_write_double_vector(this->data_in[i], lsb);
      // write_vector<double>("/home/bfr/" + QString::number(i) + ".dat", this->data_in[i]);
      ++i;
    }
  }
}

emerald_header::emerald_header() {

  // general header

  items << "recLen" << "type"
        << "wordLen" << "vers"
        << "proc" << "wordsPerRec"
        << "gh_totalRecs" << "firstEvent"
        << "totalEvents" << "gh_nExtItems";

  // event header
  items << "start" << "startUs"
        << "stop" << "stopUs"
        << "f0" << "f1" << "f2"
        << "recInFile" << "nextEvent"
        << "prevEvent" << "ev_totalRecs"
        << "dataStartRec" << "ev_nExtItems";
  // f0 = f sample, f1 = low pass, f1 = high pass
}

void emerald_header::buffer_to_map(const QString &buffer) {
  QString buf = buffer;
  qint8 skc = 95;
  buf.remove(QChar::fromLatin1(skc));

  QStringList data = buf.split(QChar(' '));

  int i = 0;

  if (data.size() >= items.size()) {
    for (auto &str : items) {

      if (!str.compare("f0") || !str.compare("f1") || !str.compare("f2"))
        header.insert(str, std::fabs(data.at(i++).toDouble()));

      else if (!str.compare("start") || !str.compare("stop") || !str.compare("startUs") || !str.compare("stopUs"))
        header.insert(str, ((qint64)(data.at(i++).toLongLong())));
      else if (!str.compare("recLen") || !str.compare("gh_totalRecs") || !str.compare("firstEvent") || !str.compare("totalEvents"))
        header.insert(str, ((qint64)(data.at(i++).toLongLong())));
      else if (!str.compare("nextEvent") || !str.compare("prevEvent") || !str.compare("ev_totalRecs") || !str.compare("dataStartRec"))
        header.insert(str, ((qint64)(data.at(i++).toLongLong())));

      else
        header.insert(str, (QString)data.at(i++));
    }
  }

  this->edt_start.set_date_time_int_doublefracs(header.value("start").toULongLong(), 0.0, abs(header.value("f0").toDouble()), header.value("totalEvents").toULongLong());

  this->edt_start.set_date_time_int_doublefracs(header.value("start").toULongLong(), (header.value("startUs").toDouble() / 1000000.), abs(header.value("f0").toDouble()), header.value("totalEvents").toULongLong());
  qDebug() << "header starts at:" << this->edt_start.ISO_date_time_frac(0);
  qDebug() << "what";
}

bool compstart_lhs_lt(const std::unique_ptr<emerald> &lhs, const std::unique_ptr<emerald> &rhs) {
  return (lhs->get_start() < rhs->get_start());
}
