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

#ifndef ATS_ASCII_FILE_H
#define ATS_ASCII_FILE_H

#include "ats_ascii_file_global.h"
#include "eqdatetime.h"
#include "procmt_alldefines.h"
#include "threadbuffer.h"
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <climits>
#include <vector>

class ats_ascii_file : public QObject, public QFileInfo {
  Q_OBJECT
public:
  ats_ascii_file(const QFileInfo &qfi, const QString file_extension, const size_t slot, QObject *parent = Q_NULLPTR);
  ats_ascii_file(const QString &qfi_str, const QString file_extension, const size_t slot, QObject *parent = Q_NULLPTR);
  ~ats_ascii_file();

  void set_buffer(std::shared_ptr<threadbuffer<double>> &buffer);
  void reset_buffer();

  void set_frequencies(const std::vector<double> &f);

  void set_eQDatetime(const eQDateTime &edt);

  /*!
   * \brief write_all_data OPEN the file moves the data into the class and writes to the disk and CLOSE
   * \param all_ts_scaled data to be moved
   * \return samples written
   */
  size_t write_all_data(const std::vector<double> &all_ts_scaled, const int signal_guess_up_to_here, const int signal_guess_is_when_finished);

  /*!
   * \brief write thread write (checks the threadbuffer )
   * \return
   */
  QTextStream::Status write();

public slots:

  bool open();
  void close();

signals:

  void signal_repaint();
  void signal_guess_100(int value_0_100);

private:
  QFile file;
  QTextStream qts;
  std::vector<double> ascii_data;
  std::vector<double> freq; //!< when writing aplitude spectra we want f
  int buffer_type = 0;      //!< which output: timeseries, spectra ...
  std::shared_ptr<threadbuffer<double>> buffer;
  int buffer_status = 0;

  size_t ats_write_counts = 0;
  size_t max_counts = SIZE_MAX;

  bool last = false; //!< last is a last buffer of different size that may be written outside the while loop - may not ne necessary
  size_t slot = 0;
  size_t add_signal_guess_100 = 0;
  std::unique_ptr<eQDateTime> edt = nullptr;
  quint64 starts_at = 0;
};

#endif // ATS_ASCII_FILE_H
