

#include "eDateTime.h"

eDateTime::eDateTime() : QDateTime() { this->setup_existing_data(); }

void eDateTime::setup_existing_data() {

  setTimeZone(QTimeZone::utc());
  dfrac = 0.0;
  datetime = 0;
  sample = 0;
  isotime = "no";
  qstr_precision_time = "no";
  stdstr_precision_time = "no";
  sfrac = "no";

  hashd["sample_freq"] = sample_freq;
  hashd["dfrac"] = dfrac;

  hashi["datetime"] = datetime;
  hashi["sample"] = sample;

  hashs["isotime"] = isotime;
  hashs["sfrac"] = sfrac;
  hashs["qstr_precision_time"] = qstr_precision_time;
}

void eDateTime::set_UTC_TimeStamp(const qint64 &utc_gmt) {
  this->setup_existing_data();
  uint secs_from_1970 = (unsigned)utc_gmt;
  setSecsSinceEpoch(secs_from_1970);
  setTimeZone(QTimeZone::utc());
  datetime = utc_gmt;
}

void eDateTime::set_UTC_TimeStamp(const long &utc_gmt) {
  qint64 i = (qint64)utc_gmt;
  this->set_UTC_TimeStamp(i);
}

void eDateTime::set_UTC_TimeStamp(const int &utc_gmt) {
  qint64 i = (qint64)utc_gmt;
  this->set_UTC_TimeStamp(i);
}

void eDateTime::set_UTC_TimeStamp(const double &utc_gmt) {
  this->setup_existing_data();
  dfrac = modf(utc_gmt, &dtmp);
  uint secs_from_1970 = (unsigned)dtmp;
  setSecsSinceEpoch(secs_from_1970);
  setTimeZone(QTimeZone::utc());
  datetime = (qint64)secs_from_1970;
}

eDateTime::eDateTime(const double &utc_gmt) {
  this->setup_existing_data();
  set_UTC_TimeStamp(utc_gmt);
}

void eDateTime::set_ISOTime(const QString &iso8601) {
  double d;
  this->set_ISOTime(iso8601, d);
}

qint64 eDateTime::set_ISOTime(const QString &iso8601, double &dfrac) {
  datetime = LONG_MAX;
  dfrac = 0.0;
  setTimeZone(QTimeZone::utc());

  //  QDateTime mytime;

  QStringList date_time = iso8601.split("T");
  QString qsdate = date_time[0];
  QString time_frac = date_time[1];
  QStringList time_split_frac = time_frac.split(".");

  QString qstime = time_split_frac[0];
  QString sfrac = "0";

  // existing .xxxx
  if (time_split_frac.size() == 2) {

    sfrac = time_split_frac[1];
    QString qx = "0." + sfrac;
    dfrac = qx.toDouble();
    this->dfrac = dfrac;
  }

  // qDebug() << qsdate << qstime << "frac: " << sfrac;
  date_time = iso8601.split(".");

  // break half intelligence UTC needed
  mytime.setTimeZone(QTimeZone::utc());
  mytime = QDateTime::fromString(date_time[0], "yyyy-MM-ddThh:mm:ss");
  mytime.setTimeZone(QTimeZone::utc());

  datetime = mytime.toSecsSinceEpoch();

  // I would like to call constructor or = of parent QDateTime = mytime;
  setSecsSinceEpoch(datetime);
  setTimeZone(QTimeZone::utc());

  return datetime;
}

QString eDateTime::get_precision_time() {

  tmp_str = QString::number(dfrac, 'f');
  tmp_str.remove(0, 1);

  qstr_precision_time = QString::number(datetime) + tmp_str;
  stdstr_precision_time = qstr_precision_time.toStdString();

  return qstr_precision_time;
}

QString eDateTime::get_ISOTime(bool bfrac = true) {

  setSecsSinceEpoch(datetime);
  setTimeZone(QTimeZone::utc());

  isotime = toString(Qt::ISODate);
  if (bfrac) {
    sfrac = QString::number(dfrac);
    sfrac.remove(0, 1);
    qDebug() << "appending " << sfrac;
    isotime += sfrac;
  }

  return isotime;
}

eDateTime::~eDateTime() {}

void eDateTime::get_data(QHash<QString, QString> &out_hashs,
                         QHash<QString, double> &out_hashd,
                         QHash<QString, qint64> &out_hashi) const {}

void eDateTime::set_data(const QHash<QString, QString> &in_hashs,
                         const QHash<QString, double> &in_hashd,
                         const QHash<QString, qint64> &in_hashi) {}
