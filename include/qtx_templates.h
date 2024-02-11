/***************************************************************************
**                                                                        **
**  ProcMT and modules                                                    **
**  Copyright (C) 2017-2021 metronix GmbH                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
** Parts of the software are linked against the Open Source Version of Qt **
** Qt source code copy is available under https://www.qt.io               **
** Qt source code modifications are non-proprietary,no modifications made **
** https://www.qt.io/licensing/open-source-lgpl-obligations               **
****************************************************************************
**           Author: metronix geophysics                                  **
**  Website/Contact: www.metronix.de                                      **
**                                                                        **
**                                                                        **
****************************************************************************/


#ifndef QTX_TEMPLATES
#define QTX_TEMPLATES

#include <QVariant>
#include <QString>
#include <QMap>
#include <QFileInfo>
#include <QFileInfo>
#include <QCoreApplication>
#include <iostream>

class _freq_flip_flop
{
public:
  _freq_flip_flop() {}

  void set_f(const double &f) {
    if (f > 0.99999) {
      this->s = 0;
      this->f = uint (f);
    }
    else if ( (f > 0)  && (this->f == 0)) {
      this->s = uint (1.0 / f);
      this->f = 0;
    }
    else {
      this->reset();
    }
  }

  void set_s(const double &s) {
    if (s == 0.0) {
      this->reset();
    }
    if (s > 10E-14) this->set_f(1.0/s);
  }

  bool has_f() const {
    if (this->s || this->f) return true;
    return false;
  }

  void reset() {
    this->f = 0;
    this->s = 0;
  }

private:

  unsigned int f = 0;
  unsigned int s = 0;

};

/*!
 * \brief next_filename
 * \param qfi filename like site12.edi
 * \return site12.edi if not exists, else site12_1.edi site12_2.edi and so on
 */
inline QFileInfo next_filename(const QFileInfo &qfi) {

  if (qfi.exists()) return qfi;
  QFileInfo qfinew;
  QString path = qfi.absolutePath();
  QString name = qfi.completeBaseName();
  QString ext = qfi.suffix();
  int max = 1000, i = 1;
  do {
    if (path.size()) qfinew.setFile(path + "/" + name + "_" + QString::number(i) + "." + ext);
    else qfinew.setFile(name + "_" + QString::number(i) + "." + ext);
    if (!qfinew.exists()) i = max;
    ++i;
  } while (i < max);


  return qfinew;
}

/*!
   \brief get_native convinience function: make sure that the original type is preserved and no replaced by another - e.g. QString
   \param in
   \param native
   \return
 */
inline QVariant get_native(const QVariant &in, const QVariant &native) {

  if (in == native) return native;
  QVariant tmp = in;
  bool ok = tmp.convert(native.metaType());
  // can not convert - keep old!
  if (!ok) return native;
  // return in converted to native type
  return tmp;
}

/*!
 * \brief intfreq2string_unit smaple frequency where you assume that they are int - like all native sampling frequencies
 * \param f
 * \return
 */
inline QString intfreq2string_unit(const double &f) {
  int int_f = 0;
  QString unit("failed");
  if (f >= 1.0) {
    int_f = int(f);
    unit = " Hz ";
    return QString::number(int_f) + unit;
  }
  else if (f < 1.0) {
    int_f = int(1.0/f);
    unit = " s ";
    return QString::number(int_f) + unit;
  }

  return unit;


}

inline QString doublefreq2string_unit(const double &f) {
  double int_f = 0;

  if (f >= 1.0) {
    int_f = f;
    return QString::number(int_f, 'f', 4) + " Hz ";
  }
  else if (f < 1.0) {
    int_f = 1.0/f;
    return QString::number(int_f, 'f', 4) + " s ";
  }

  return QString("failed");


}

inline QString doublefreq2string_unit_measdoc(const double &f) {
  int int_f = 0;
  QString unit("failed");
  if (f >= 1.0) {
    int_f = int(f);
    unit = "H";
    return "_" + QString::number(int_f) + unit;
  }
  else if (f < 1.0) {
    int_f = int(1.0/f);
    unit = "S";
    return "_" + QString::number(int_f) + unit;
  }

  return unit;

}

//QMap<QString, QVariant> ats_filename_to_data(const QString &base_name) {
//  QStringList items = base_name.split(("_"));
//  QMap<QString, QVariant> map;
//  // 071_V01_C00_R000_TEx_BL_2S
//  if (items.size() < 7) return map;
//  if (base_name.contains(".xml")) return map;
//  map.insert("adu_ser", items.[0]);
//  map.insert("file_ver", items[1].remove(0,1).toInt());
//  map.insert("adu_chan", items[2].remove(0,1).toInt());
//  map.insert("adu_run", items[3].remove(0,1).toInt();
//  map.insert("adu_chantype", items[4].remove(0,1));
//  map.insert("adu_boardtype", items[5].remove(0,1));
//  map.insert("adu_fsample", items[6]);


//  return map;

//}

/*!
 * \brief copy_native try to make sure that source type is converted into target type; on pre-existing will be copied as they are<br>
 *  this can happen if the  source is coming from XML as tring
 *
 * \param target
 * \param source
 */
inline void copy_native(QMap<QString, QVariant> &target, const QMap<QString, QVariant> &source  ) {

  QMap<QString, QVariant>::const_iterator iter = source.constBegin();
  while (iter != source.constEnd()) {
    if (target.contains(iter.key())) target.insert(iter.key(), get_native(iter.value(), target.value(iter.key())));
    else target.insert(iter.key(), iter.value());
    ++iter;
  }
}

/*!
 * \brief qvariant_double_to_sci_str if in is double type we make a scientific string in order to avoid rouning errors in Qt interfaces
 * \param in
 * \param double_str
 * \return false if no bouble or can not be converted
 */
inline QString qvariant_double_to_sci_str(const QVariant &in, const int precision = 6) {

  QString double_str;
  // compare if we want to make a double E+NN string
  if (in.metaType().id() == QMetaType::Double) {
    double_str = QString::number(in.toDouble(), 'E', precision );
  }

  return double_str;
}

/*!
 * \brief fall_back_default if you want top open one of the default files of procmt whi are located in the bin dir; during debugging the path is different!
 * \param qfi either file to open (explictly given because you want it or empty
 * \param what_to_open one of the default files like info.sql3, master_calibration.sql3, procmt.sql3
 * \return
 */
inline QFileInfo fall_back_default_str (const QString &what_to_open) {
  if (!what_to_open.size()) return QFileInfo();
  QFileInfo qfinew;
  qfinew.setFile(QCoreApplication::applicationDirPath() + "/" + what_to_open);  // typical windows / linux / mac
  if (qfinew.exists()) return qfinew;
  // second try
  qfinew.setFile(QString(PMTSQLPATH) + "/bin/" + what_to_open);
  return qfinew;
}

inline QFileInfo fall_back_default (const QFileInfo &what_to_open) {

  if (what_to_open.exists()) return what_to_open;
  QFileInfo qfinew;
  qfinew.setFile(QCoreApplication::applicationDirPath() + "/" + what_to_open.fileName());  // typical windows / linux / mac
  if (qfinew.exists()) return qfinew;
  qfinew.setFile(QString(PMTSQLPATH) + "/bin/" + what_to_open.fileName());
  if (qfinew.exists()) return qfinew;
  return QFileInfo();
}


/*!
 * \brief procmt_homepath get the path either for application or debug
 * \return
 */
inline QString procmt_homepath(const QString &what_to_open) {
  if (!what_to_open.size()) return QString();
  QFileInfo qfinew;
  qfinew.setFile(QCoreApplication::applicationDirPath() + "/" + what_to_open);  // typical windows / linux / mac
  if (qfinew.exists()) return qfinew.absoluteFilePath();
  qfinew.setFile(QString(PMTSQLPATH) + "/bin/" + what_to_open);
  if (qfinew.exists()) return qfinew.absoluteFilePath();
  return QString();
}

#endif
