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

#ifndef EDI_FILE_H
#define EDI_FILE_H

#include "buckets.h"
#include "eqdatetime.h"
#include "geocoordinates.h"
#include "msg_logger.h"
#include "prc_com.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <map>
#include <memory>
#include <procmt_alldefines.h>

class edi_file : public QObject, public QFileInfo, prc_com {
  Q_OBJECT
public:
  explicit edi_file(QObject *parent = Q_NULLPTR);

  edi_file(const edi_file &edi);

  edi_file &operator=(edi_file const &rhs);

  bool test_full_tensor();  //!< has full xx...yy tensor data
  bool test_tipper() const; //!< has tipper data

  friend edi_file merge_edi(const edi_file &lhs, const edi_file &rhs);
  friend std::shared_ptr<edi_file> merge_edi_sp(const std::shared_ptr<edi_file> &lhs, const std::shared_ptr<edi_file> &rhs);

  ~edi_file();

  prc_com edi_HEAD;
  prc_com edi_INFO;
  prc_com edi_DEFINEMEAS;

  void set_QFileInfo(const QFileInfo &qfi);
  void set_msg_logger(std::shared_ptr<msg_logger> msg);

  bool toASCII_file(const QFileInfo &qfi) const;

  mt_data_res<std::complex<double>> z;  //!< that is the data
  std::vector<prc_com> in_ats_prc_coms; //!< meta-data, keep a copy of the input data; generated also while reading EDI

public slots:

  void set_edi_data_z(const mt_data_res<std::complex<double>> &zin, const std::vector<prc_com> &prcs);

  bool write();

  bool read();

signals:

  void signal_actual_edifile_name(const QString edifile_name);
  void signal_general_msg(const QString &sender, const QString &component, const QString &message);

private:
  bool write_head();

  bool write_info();

  bool write_definemeas();

  bool write_e_h_meas();

  bool write_mtsect();

  bool write_spectrasect();

  bool read_head();

  bool read_info();

  bool read_definemeas();

  bool read_e_h_meas();

  bool read_mtsect();

  bool read_spectrasect();

  bool seek_key(const QString key, const bool b_continue = false, const bool silent = false);

  size_t read_number_block(std::vector<double> &v);

  qint64 read_a_coh_section();

  QFile file;
  QTextStream qts;

  QString line; //!< scan and read text
  QString qts_buf;

  // tipper_tensor_data t;
  // spc_tensor_data spc;

  bool open_write();
  bool is_open = false;

  QMap<QString, QString> e_h_meas;

  // like enum
  QStringList znames_real;
  QStringList znames_imag;
  QStringList znames_var;

  //    QStringList tnames_real;
  //    QStringList tnames_imag;
  //    QStringList tnames_var;

  QStringList e_h_measids;

  std::shared_ptr<msg_logger> msg;

  /*!
   * @brief calculate the angle between two points to E direction
   * @param pos_x1
   * @param pos_x2
   * @param pos_y1
   * @param pos_y2
   * @return ZROT or TROT angle or  DBL_MAX; for Z it will be 0; for H I take ZROT
   */
  double angle(const double &pos_x1, const double &pos_x2, const double &pos_y1, const double &pos_y2) const;
  double zrot = 0.0;
  double trot = 0.0;
};
edi_file merge_edi(const edi_file &lhs, const edi_file &rhs);

std::shared_ptr<edi_file> merge_edi_sp(const std::shared_ptr<edi_file> &lhs, const std::shared_ptr<edi_file> &rhs);

#endif // EDI_FILE_H
