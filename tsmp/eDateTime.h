#ifndef EDATETIME_H
#define EDATETIME_H

// #define QT_NO_DEBUG_OUTPUT

#include "allinclude.h"
#include <QDateTime>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QTimeZone>
#include <QVector>
#include <climits> // INT_MAX
#include <cmath>
#include <cstring>
#include <iostream>
#include <istream>
#include <ostream>

/**
 @author Dr. Bernhard Friedrichs <bfr@isi7>
*/

class eDateTime : public QDateTime {

private:
  qint64 datetime; /*! this is the main counter; increment */
  QString isotime;
  QString qstr_precision_time; /*! returns 1234.67 UNIX TIME plus FRACTION */
  std::string stdstr_precision_time; /*! for conversion to iostream */
  QString tmp_str;
  double dfrac;       /*! faction of time between 0 and 0.9999... */
  QString sfrac;      /*! same as above as string for I/O */
  double sample_freq; /*! can hold a sample frequency in order to increment by
                         sample */
  qint64 sample;      /*! can return a sample corresponding to date */
  QDateTime mytime;

  QVector<eDateTime> vedt;
  double dtmp;

  void setup_existing_data();

  QHash<QString, QString> hashs; /*!< contains all string data */
  QHash<QString, QString>::const_iterator iths;

  QHash<QString, double> hashd; /*!< contains all double data */
  QHash<QString, double>::const_iterator ithd;

  QHash<QString, qint64> hashi; /*!< contains all integer data */
  QHash<QString, qint64>::const_iterator ithi;

  QHash<QString, QString> hash; /*!< "export/import container" ALL as strings */

public:
  eDateTime();

  eDateTime(const qint64 &utc_gmt);
  eDateTime(const double &utc_gmt);

  void set_UTC_TimeStamp(const qint64 &utc_gmt);
  void set_UTC_TimeStamp(const double &utc_gmt);
  void set_UTC_TimeStamp(const int &utc_gmt);
  void set_UTC_TimeStamp(const long &utc_gmt);

  qint64 get_UTC_TimeStamp() const { return datetime; };

  void set_sample_freq(const double &sample_freq) {
    this->sample_freq = sample_freq;
  };
  double get_sample_freq() const { return this->sample_freq; };

  qint64 set_ISOTime(const QString &iso8601, double &dfrac);
  void set_ISOTime(const QString &iso8601);
  QString get_ISOTime(bool frac);
  QString get_precision_time();
  QString set_precision_time(const QString &qstrptime);

  ~eDateTime();

  /**
   * The output data structure follows OpenMT: Hash of strings, double and 64bit
   * int
   * @param ihashd string paramaters such as (Filter, RadioFilter)
   * @param ohashd double parameters such as (dfrac, 0.1299)
   * @param ohashi interger parameters  such as (datetime, 1234)
   */
  void get_data(QHash<QString, QString> &out_hashs,
                QHash<QString, double> &out_hashd,
                QHash<QString, qint64> &out_hashi) const;

  /**
   * * The output data structure follows OpenMT: Hash of strings, double and
   * 64bit int
   * @param ihashs string paramaters such as (Filter, RadioFilter)
   * @param ihashd double parameters such as (dfrac, 0.1299)
   * @param ihashi interger parameters  such as (datetime, 1234)
   */
  void set_data(const QHash<QString, QString> &in_hashs,
                const QHash<QString, double> &in_hashd,
                const QHash<QString, qint64> &in_hashi);

  /**
   * ++ increment pre
   * @return next element
   */
  eDateTime &operator++() {
    ++datetime;
    return *this;
  }

  /**
   * increment ++ (post)
   * @param
   * @return next element
   */
  eDateTime operator++(int) {
    eDateTime t = *this;
    ++datetime;
    return t;
  }

  /**
   * -- decrement pre
   * @return next element
   */
  eDateTime &operator--() {
    --datetime;
    return *this;
  }

  /**
   * decrement -- (post)
   * @param
   * @return next element
   */
  eDateTime operator--(int) {
    eDateTime t = *this;
    --datetime;
    return t;
  }

  eDateTime &operator=(const eDateTime &e) {

    datetime = e.datetime;
    dfrac = e.dfrac;
    sample_freq = e.sample_freq;
    vedt = e.vedt;
    // make sure that QDateTime si also updated
    setSecsSinceEpoch(datetime);
    setTimeZone(QTimeZone::utc());
    return *this;
  }

  eDateTime &operator=(const QDateTime &e) {

    datetime = e.toSecsSinceEpoch();
    setSecsSinceEpoch(datetime);
    setTimeZone(QTimeZone::utc());
    return *this;
  }

  friend bool operator<(const eDateTime &p, const eDateTime &q) {
    return (p.datetime < q.datetime);
  }

  friend bool operator>(const eDateTime &p, const eDateTime &q) {
    return (p.datetime > q.datetime);
  }

  friend bool operator==(const eDateTime &p, const eDateTime &q) {
    return (p.datetime == q.datetime);
  }

  friend bool operator!=(const eDateTime &p, const eDateTime &q) {
    return !(p.datetime == q.datetime);
  }
  friend std::ostream &operator<<(std::ostream &os, eDateTime &si) {
    si.get_precision_time();
    os << si.stdstr_precision_time;
    return os;
  }

  friend std::istream &operator>>(std::istream &is, eDateTime &si) {
    double d;
    is >> d;
    si.set_UTC_TimeStamp(d);
    return is;
  }

  friend size_t samples_to_StartTime(const eDateTime &ismytime,
                                     const eDateTime &isstart) {

    double diffsecs = (double)(isstart.datetime - ismytime.datetime);
    double diffrac = isstart.dfrac - ismytime.dfrac;
    double total = diffsecs + diffrac;

    // qDebug() << isstart.datetime << "  a " << ismytime.datetime << " " <<
    // diffrac << " "<< total << " " <<ismytime.sample_freq << " re " <<
    // (size_t)(total * ismytime.sample_freq);

    return (size_t)(total * ismytime.sample_freq);
  }
};

#endif
