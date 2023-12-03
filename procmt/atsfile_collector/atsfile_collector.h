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

#ifndef ATSFILE_COLLECTOR_H
#define ATSFILE_COLLECTOR_H

#include "atsfile_collector_global.h"
#include "ats_ascii_file.h"
#include "atsfile_collector_global.h"
#include "atsfile.h"
#include "atsfile_collector_global.h"
#include "atsheader.h"
#include "atsfile_collector_global.h"
#include "buckets.h"
#include "atsfile_collector_global.h"
#include "iterator_templates.h"
#include "atsfile_collector_global.h"
#include "prc_com.h"
#include "atsfile_collector_global.h"
#include "procmt_alldefines.h"
#include "atsfile_collector_global.h"
#include "threadbuffer.h"

#include "atsfile_collector_global.h"
#include "math_vector.h"
#include "atsfile_collector_global.h"
#include "vector_utils.h"

#include "atsfile_collector_global.h"
#include <QDataStream>
#include "atsfile_collector_global.h"
#include <QDate>
#include "atsfile_collector_global.h"
#include <QFile>
#include "atsfile_collector_global.h"
#include <QFileInfo>
#include "atsfile_collector_global.h"
#include <QObject>
#include "atsfile_collector_global.h"
#include <complex>
#include "atsfile_collector_global.h"
#include <memory>
#include "atsfile_collector_global.h"
#include <queue>
#include "atsfile_collector_global.h"
#include <thread>
#include "atsfile_collector_global.h"
#include <vector>

class atsfile_collector : public QObject, public prc_com {
  Q_OBJECT

public:
  atsfile_collector(size_t chan_type = SIZE_MAX, QObject *parent = Q_NULLPTR);
  atsfile_collector(std::shared_ptr<atsfile> atsf, QObject *parent = Q_NULLPTR);

  ~atsfile_collector();
  void reset_buffer();
  void set_spc_buffer(std::shared_ptr<threadbuffer<std::complex<double>>> &buffer);
  void set_ts_buffer(std::shared_ptr<threadbuffer<double>> &buffer);

  /*!
   * \brief set_frequencies_and_target do this in case of no parzening
   * \param f fft frequencies
   * \param ft target frequencies (which can also be same as fft frequencies)
   * \param take_all
   */
  void set_frequencies_and_target(const std::vector<double> &fft, const std::vector<double> &ft_target, const bool &take_all);

  void set_frequencies_and_target_indices(const std::vector<double> &fft, const std::vector<double> &ft_target, const std::vector<size_t> &selindices, const std::vector<size_t> &selindices_sizes);

  /*!
   * \brief set_double_buffer prepare the threadbuffer for ts data
   * \param buffer
   */
  void set_double_buffer(std::shared_ptr<threadbuffer<double>> &buffer);

  /*!
   * \brief set_complex_buffer prepare the thread buffer fpr e.g. spectra
   * \param buffer
   */
  void set_complex_buffer(std::shared_ptr<threadbuffer<std::complex<double>>> &buffer);

  void set_all_spectra_collector(std::shared_ptr<all_spectra_collector<std::complex<double>>> &asp_coll);

  void set_parzen_vectors(std::shared_ptr<std::vector<std::vector<double>>> parzen_vectors_sptr, const std::vector<double> &f, const std::vector<double> &ft);

  /*!
   * \brief operator < we may need to stort the spectra by channle type in order to have always Ex*Hy and not Hy*Ex
   * \param rhs
   * \return
   */
  bool operator<(const atsfile_collector &rhs) {
    return (this->spc.chan_type < rhs.spc.chan_type);
  }

  /*!
   * \brief collect_all spectra - do nothing else
   */
  void collect_all_spc();

  void clear();

  channel_single_spc_collector_buff<std::complex<double>> spc; //! collects the calibrated or raw spectra

  size_t ch_type() const {
    return this->chan_type;
  }

  bool is_E() const {
    return pmt::is_E<size_t>(this->chan_type);
  }

public slots:

  /*!
   * \brief slot_get_all_ts for a SINGLE READ of all data in one go
   * \param all_ts_scaled get the data from thread or class
   */
  void slot_get_all_ts(std::vector<double> &all_ts_scaled, const bool &write_immedeately);

  void slot_write_ascii_ts(const QString file_extension);
  void slot_repaint();
  void slot_guess_100(int value_0_100);

signals:

  void signal_ascii_write_finished();
  void signal_guess_100(int value_0_100);
  void signal_repaint();
  void signal_flush_collector();

private:
  // the following vectors are tsmp style: all data at once
  std::vector<double> all_ts_scaled;         //! complete timeseries at once, E is scaled to mV/km
  std::vector<std::complex<double>> all_spc; //! complete single huge spectra from single fft
  std::vector<double> calibrated_ts;         //! inverted H or scaled E ts at the end

  std::shared_ptr<threadbuffer<std::complex<double>>> spcbuffer; //! threadbuffer for (sliced) spectra
  std::shared_ptr<threadbuffer<double>> tsbuffer;                //! threadbuffer for (sliced) timeseries (e.g. de-spiking)
  int slot = -1;
  int buffer_type = 0;   //!< which output: timeseries, spectra ...
  int buffer_status = 0; //!< e.g. from feeder thread

  std::vector<double> ts_data;                //!< input slice ts scaled or raw H
  std::vector<std::complex<double>> spc_data; //!< input slice spc raw or calibrated

  std::shared_ptr<all_spectra_collector<std::complex<double>>> asp_coll; //!< move the data out of the loop run1, run2 ... complete site

  std::shared_ptr<std::vector<std::vector<double>>> parzen_vectors_sptr = nullptr; //!< already generated parzen vectors from mc_data

  std::vector<double> freqs;

  std::vector<double> target_freqs;
  std::vector<size_t> indices;          //!< prviously selected indices
  std::vector<size_t> selindices_sizes; //!< how many lines belong to the indices, take five

  size_t ats_write_counts = 0;
  size_t ats_read_counts = 0;

  size_t flush_count = 10;

  /*!
   * \brief enum_chan_type corresponds to the index of pmt::channel_types; here we can map hx (2) to rhx (7) in order to store / find this data in vectors
   */
  size_t chan_type = SIZE_MAX;

  QFileInfo qfi;

  bool select_from_target_freqs = false; //!< discrete frequencies like CSAMT or without parzening
  std::vector<std::complex<double>> lc_cplx_buf;

  std::unordered_map<double, std::vector<std::complex<double>>> line_data; //!< double contains the frequency, the vector the according data
};

static bool comp_atsfile_collector_by_channel(const std::shared_ptr<atsfile_collector> &lhs, const std::shared_ptr<atsfile_collector> &rhs) {
  return (lhs->ch_type() < rhs->ch_type());
}

#endif // ATSFILE_COLLECTOR_H
