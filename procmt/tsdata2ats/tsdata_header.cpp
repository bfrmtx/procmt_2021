#include "tsdata_header.h"

tsdata_header::tsdata_header(const QFileInfo &qfi, QObject *parent) :
    QObject(parent), QFileInfo(qfi), prc_com(false) {

  this->isok = this->open();
}

tsdata_header::tsdata_header() :
    QObject(0), QFileInfo(), prc_com(false) {
}

tsdata_header::~tsdata_header() {
}

bool tsdata_header::open() {
  this->file.setFileName(this->absoluteFilePath());

  this->isok = file.open(QIODevice::ReadOnly | QIODevice::Text);

  if (!this->isok)
    return this->isok;
  this->create_global_header();

  QString line;
  QStringList key_val;
  qts.setDevice(&this->file);

  do {
    line = this->qts.readLine(1024).simplified();
    if (line.startsWith(">"))
      line.remove(0, 1);
    key_val = line.split(":");
    for (auto &str : key_val) {
      str = str.simplified();
    }

    if (this->global_header.contains(key_val.at(0)) && key_val.size() && !key_val.contains("STARTTIME") && !key_val.contains("ENDTIME")) {
      global_header.insert(key_val.at(0), get_native(key_val.at(1), this->global_header.value(key_val.at(0))));
      // this->data.insert("gain_stage1", get_native(value, data.value("gain_stage1")));

      qDebug() << "setting" << key_val.at(0) << global_header.value(key_val.at(0));

    }

    else if ((key_val.contains("STARTTIME")) || (key_val.contains("ENDTIME"))) {

      if (key_val.contains("STARTTIME")) {

        QString start_str = line.remove(key_val.at(0));
        start_str = start_str.simplified();
        if (start_str.startsWith(":"))
          start_str.remove(0, 1);

        qDebug() << "assmebling start " << start_str;
        this->start_time.setTimeZone(QTimeZone::utc());
        this->start_time = QDateTime::fromString(start_str, "yyyy-MM-dd HH:mm:ss");
        qDebug() << "got start time UTC:" << this->start_time.toString("yyyy-MM-dd HH:mm:ss");
      }

      if (key_val.contains("ENDTIME")) {

        QString start_str = line.remove(key_val.at(0));
        start_str = start_str.simplified();
        if (start_str.startsWith(":"))
          start_str.remove(0, 1);

        qDebug() << "assmebling end " << start_str;
        this->stop_time.setTimeZone(QTimeZone::utc());
        this->stop_time = QDateTime::fromString(start_str, "yyyy-MM-dd HH:mm:ss");
        qDebug() << "got start time UTC:" << this->stop_time.toString("yyyy-MM-dd HH:mm:ss");
      }
    }

  } while (!line.contains("INFO_END"));

  qDebug() << "getting channels:" << this->global_header.value("NCHAN");

  if (global_header.value("NCHAN").toInt() != 5) {
    std::cerr << "tsdata_header::create_global_header -> is only prepared to read 5 channel data" << std::endl;
    return false;
  }

  return this->isok;
}

size_t tsdata_header::read() {
  if (this->qts.status() == QTextStream::Ok) {
    return read_five_columns(&this->qts, this->x, this->y, this->a, this->b, this->c, this->bx, this->by, this->ba, this->bb, this->bc,
                             this->maxx, this->minx, this->maxy, this->miny, this->maxa, this->mina, this->maxb, this->minb, this->maxc, this->minc,
                             this->gt_not_number, this->gt_number_replace, true, this->size_hint);
  } else
    return 0;
}

bool tsdata_header::guess_values() {
  qint64 msecs = this->start_time.msecsTo(this->stop_time);
  this->nsamples = this->x.size();
  this->sample_freq = ((double)msecs / 1000.0) / (double(this->nsamples));
  qDebug() << "calculated sample frequency:" << this->sample_freq;
  qDebug() << "samples:" << this->nsamples;

  int input_seconds = msecs / 1000;

  int days = input_seconds / 60 / 60 / 24;
  int hours = (input_seconds / 60 / 60) % 24;
  int minutes = (input_seconds / 60) % 60;
  int seconds = input_seconds % 60;

  qDebug() << "duration days:" << days << ",hours: " << hours << ",minutes:" << minutes << ",seconds:" << seconds;

  qDebug() << "Chan1 " << this->maxx << this->minx << "LSB 32bit :" << ((this->maxx - this->minx)) / ((double)INT32_MAX);

  if (!this->nsamples || (this->sample_freq == 0))
    return false;
  else
    return true;
}

int tsdata_header::create_global_header() {
  this->global_header.insert("Station", QString("empty"));
  this->global_header.insert("INSTRUMENT", QString("empty"));
  this->global_header.insert("WINDOW", QString("empty"));
  this->global_header.insert("LATITUDE", (double)0.0);
  this->global_header.insert("LONGITUDE", (double)0.0);
  this->global_header.insert("ELEVATION", (double)0.0);
  // skip the redundant UTM
  this->global_header.insert("COORD_SYS", QString("empty"));
  this->global_header.insert("DECLIN", (double)0.0);
  this->global_header.insert("FORM", QString("empty"));
  this->global_header.insert("FORMAT", QString("empty"));
  this->global_header.insert("SEQ_REC", QString("empty"));
  this->global_header.insert("NCHAN", (int)0);
  this->global_header.insert("STARTTIME", QString("empty"));
  this->global_header.insert("ENDTIME", QString("empty"));
  this->global_header.insert("T_UNITS", QString("empty"));
  this->global_header.insert("DELTA_T", (double)0.0);
  this->global_header.insert("MIS_DATA", (double)0.0);

  return this->global_header.size();
}

quint64 tsdata_header::create_ats_files() {
  qDebug() << "create";
  QFileInfo qfi("/tmp/test.ats");
  atsfile *xats = new atsfile(qfi, 1, 80, 1, 5, 1, 0, "Ex", "LF");

  xats->set_sample_freq(5, false, false);
  xats->set_start_date_time(this->start_time);
  xats->set_change_measdir(false);
  QFileInfo filnam(xats->get_new_filename(false));
  qDebug() << filnam.baseName();
  xats->setFile("/tmp/" + filnam.fileName());
  filnam.setFile(xats->get_new_filename(false));
  qDebug() << filnam.absoluteFilePath();

  double lsb;
  if (std::abs(this->minx) > std::abs(this->maxx))
    lsb = std::abs(this->minx) * 6 / ((double)INT32_MAX);
  else
    lsb = std::abs(this->maxx) * 6 / ((double)INT32_MAX);
  //    return xats->set_write_double_vector(this->x, ((this->maxx - this->minx)) / ((double)INT32_MAX));

  return xats->set_write_double_vector(this->x, lsb);
}
