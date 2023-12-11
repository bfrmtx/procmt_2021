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

#ifndef PROCMT_LIB_H
#define PROCMT_LIB_H

#include "edi_file.h"
#include "math_vector.h"
#include "mc_data.h"
#include "msg_logger.h"
#include "mt_site.h"
#include "prc_com.h"
#include "procmt_alldefines.h"
#include "procmt_lib_global.h"
#include "qstring_utilities.h"
#include <QDir>
#include <QMultiMap>
#include <cfloat>
#include <climits>
#include <iostream>
#include <memory>

class procmt_lib : public QObject, public prc_com {

  Q_OBJECT

public:
  /*!
   * \brief procmt_lib
   * \param cmdline the commadline parameters
   * \param msg the message logger from the parent
   * \param mtsite the mtsite from the parent
   * \param parent parent object
   */
  procmt_lib(std::shared_ptr<prc_com> cmdline, std::shared_ptr<msg_logger> msg, std::shared_ptr<mt_site> mtsite, QObject *parent);

  ~procmt_lib();

signals:

  /*!
   * \brief signal_mtdata_finished is emitted when the run() has been finished
   * \param message
   */
  void signal_mtdata_finished(const QString message);

  /*!
   * \brief signal_file_progess emitted for each run (set) so if you have three recordings (32k, 4k, 512) it will emit three times
   * \param message
   */
  void signal_file_progess(const QString &message);

  void signal_update_progress_bar(const int counts);

  void signal_get_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);

public slots:

  void set_data(const QStringList &allmeasdocs_center, const QStringList &allmeasdocs_emap, const QStringList &allmeasdocs_rr,
                const mttype &my_mttype, const proc_type &myproc_type);

  void slot_update_progress_bar(const int counts);

  void slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);

  /*!
   * \brief run after all data (cmdline and set_data) are done a thread can be started herewtih
   */
  void run();

private:
  std::shared_ptr<mt_site> mtsite; //!< the final result of all processed ats data will be placed here; shared with the parent

  std::unique_ptr<mc_data> mcd; //!< for getting the command line options known from mc_data

  QList<QFileInfo> qfis_center;
  QList<QFileInfo> qfis_emap;
  QList<QFileInfo> qfis_rr;

  int iqfis = 0;
  int iqfis_emap = 0;
  int iqfis_rr = 0; //!< remote reference and transmitter

  std::shared_ptr<msg_logger> msg = nullptr;
  std::shared_ptr<prc_com> cmdline = nullptr;

  int ac1_ssp2_acssp3 = 1; //!< which spectra to open

  mttype my_mttype = mttype::mt;
  proc_type myproc_type = proc_type::mt_prz;
};

#endif // PROCMT_LIB_H
