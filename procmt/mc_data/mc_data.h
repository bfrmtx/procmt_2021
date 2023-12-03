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

#ifndef MC_DATA_H
#define MC_DATA_H

#include "mc_data_global.h"
#include <QDebug>
#include "mc_data_global.h"
#include <QFile>
#include "mc_data_global.h"
#include <QList>
#include "mc_data_global.h"
#include <QObject>
#include "mc_data_global.h"
#include <QSqlDatabase>
#include "mc_data_global.h"
#include <QSqlError>
#include "mc_data_global.h"
#include <QSqlQuery>
#include "mc_data_global.h"
#include <QTextStream>
#include "mc_data_global.h"
#include <QtSql>

#include "mc_data_global.h"
#include <list>
#include "mc_data_global.h"
#include <map>
#include "mc_data_global.h"
#include <queue>

#include "mc_data_global.h"
#include "iterator_templates.h"
#include "mc_data_global.h"
#include "procmt_alldefines.h"

#include "mc_data_global.h"
#include "acsp_prz.h"
#include "mc_data_global.h"
#include "atmfile.h"
#include "mc_data_global.h"
#include "ats_ascii_file.h"
#include "mc_data_global.h"
#include "atsfile.h"
#include "mc_data_global.h"
#include "atsfile_collector.h"
#include "mc_data_global.h"
#include "calibration.h"
#include "mc_data_global.h"
#include "firfilter.h"
#include "mc_data_global.h"
#include "measdocxml.h"
#include "mc_data_global.h"
#include "prc_com.h"
#include "mc_data_global.h"
#include "threadbuffer.h"
// #include "acsp_lines.h"

#include "mc_data_global.h"
#include "math_vector.h"
#include "mc_data_global.h"
#include "msg_logger.h"

/*!
   \brief The mc_data class i the multi channel data class; first you open all needed input files
 */
class mc_data : public QObject, public prc_com {
  Q_OBJECT

public:
  // public data sections
  std::vector<std::shared_ptr<atsfile>> in_atsfiles; //!< either direct open mode or controlled by measdocs
  std::vector<std::thread> readerthreads;
  std::vector<prc_com> in_ats_prc_coms; //!< keep a copy of the input data

  std::vector<std::shared_ptr<atsfile>> out_atsfiles; //!< filtered files in general (or cut/shift/truncate)
  std::vector<std::thread> writerthreads;             //!< filtered files in general (or cut/shift/truncate)

  std::vector<std::shared_ptr<ats_ascii_file>> out_ats_ascii_files; //!< for ASCII export
  std::vector<std::thread> asciiwriterthreads;                      //!< for ASCII export

  std::vector<std::shared_ptr<atsfile_collector>> out_atsfile_collectors; //!< for catching output from in_atsfiles
  std::vector<std::thread> spc_threads;                                   //!< for sliced and for huge 1:1 FFT for generating H as nT

  std::vector<std::shared_ptr<threadbuffer<double>>> tsbuffers;
  std::vector<std::shared_ptr<threadbuffer<std::complex<double>>>> spcbuffers; //!< collect the spectra for MT for example

  std::vector<std::shared_ptr<measdocxml>> measdocs;      //!< open measdocs should be the default
  std::vector<std::shared_ptr<calibration>> calibrations; //!< calibrations, get from measdocs or database

  std::shared_ptr<std::vector<std::vector<double>>> parzen_vectors_sptr = nullptr;

  std::shared_ptr<all_spectra_collector<std::complex<double>>> all_acspc_collector = nullptr; //!< store the MT results here; take a copy from main loop

  QBitArray atm_master; //!< for multiple selection, join

  // functions

  explicit mc_data(QObject *parent = Q_NULLPTR);

  void set_messenger(std::shared_ptr<msg_logger> msg);

  ~mc_data();

  size_t size() const {
    return this->in_atsfiles.size();
  }

  bool set_prc_com(std::shared_ptr<prc_com> mxc, const bool strip_classname = true, const bool check_options = false);

  void slide_to_same_pos();

  /*!
   * \brief insert_prc_com inserts a prc_com class and generates new kay value pairs in case - for example for a filter or the command line
   * \param filters
   * \return
   */
  void insert_prc_com(std::shared_ptr<prc_com> cmdline);

  bool prepare_firfilter();

  int select_target_frequencies(const QString &dbname = "info.sql3", const QString &table_name = "default_mt_frequencies");

  void check_survey_basedir(const QDir from_atsfile); //!< emits signal_survey_basedir

  QDir survey_base_dir; //!< top directory of our survey

  /*!
   * \brief force_second_half_remote is for parallel test with second ADU only; activate with if (this->ivalue("force_second_set_remote") == 1)
   */
  void force_second_half_remote();

  void stack_all_tsplotter_start();
  void stack_all_tsplotter_join();

public slots:

  void make_auto_cross_spectra_parzen();

  void slot_apply_sys_calib(const uint on_1_off_0);

  // void make_auto_cross_spectra_lines(std::shared_ptr<all_spectra_collector<std::complex<double> > > all_acspc_collector);

  /*!
     \brief open_files
     \param qfis
     \param slide_to_samepos_after_open
     \param continious, set false if you want interaction like in tsplotter
     \return maximum files can be read
   */
  size_t open_files(const QList<QFileInfo> qfis, const bool slide_to_samepos_after_open = true,
                    const mttype mt_type = mttype::nomt,
                    const threadctrl tcl = threadctrl::threadctrl_single);

  prc_com scan_header_close(const QList<QFileInfo> qfis);

  void slot_counts_per(const int counts);

  /*!
   * \brief slot_guess_100 some modules can only guess where we are
   * \param value_0_100
   */
  void slot_guess_100(int value_0_100);

  void slot_rx_adu_msg(const int &system_serial_number, const int &channel, const int &slot, const QString &message);

  void slot_rx_adu_msg_write_ascii(const int &system_serial_number, const int &channel, const int &slot, const QString &message);

  int slot_sync_info(const eQDateTime &timings);

  /*!
     \brief close_infiles and deletes all connections and the list
   */
  QList<QFileInfo> close_infiles();

  QList<QFileInfo> close_measdocs();

  void slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);

  /*!
     \brief close_outfiles and deletes all connections and the list
   */
  QList<QFileInfo> close_outfiles();

  void close_ascii_outfiles();

  void close_atsfile_collectors();

  void close_calibration();

  void slot_repaint();

  void slot_set_GB_mem(int gigabytes);

  void slot_get_txm_cycle_qmap_created(const QMap<QString, QVariant> &data_map);

  /*!
     \brief read_error basic error handling
     \param QDataStreamStatus
   */
  void read_error(const QDataStream::Status QDataStreamStatus);

  /*!
   * \brief set_window_length
   * \param wl
   * \param fcut_upper
   * \param fcut_lower
   * \param last_read
   * \param use_samples
   * \param overlapping that is the true overlapping in points
   * \return
   */
  bool set_window_length(const size_t &wl, const double &fcut_upper = 0.0, const double &fcut_lower = 0.0, const bool &last_read = false, const quint64 &use_samples = 0, const quint64 &overlapping = 0);

  bool try_set_window_length(const size_t &wl, double &get_maxf, double &get_minf, double &fcut_upper, double &fcut_lower, const bool limit_upper_to_cal);

  size_t get_window_length() const;

  /*!
     \brief read_infiles emit a single read slide
   */
  void read_infiles(bool emits);

  void start_in_ats_channel_threads();

  void start_out_ats_channel_threads();

  // void start_mt_threads();

  void start_collect_all_spc_threads();
  void join_collect_all_spc_threads();

  /*!
   * \brief start_single_spectra_threads
   * \param inverse_fft calculate inverse
   * \param theo_only control theo versus real calibration, ref \class atsfile
   * \param write_immedeately move and write data and/or start the threads for writing
   */
  void start_single_spectra_threads(const bool inverse_fft, const bool write_immedeately);

  void join_single_spectra_threads();

  void start_out_ats_ascii_channel_threads();

  void join_in_ats_channel_threads();
  void join_out_ats_channel_threads();

  // void join_mtthreads();
  void join_out_ats_ascii_channel_threads();

  void slot_append_parzenvector(const double &f, const std::vector<double> &przv);

  /*!
     \brief connect_in_atsfiles prepare the threads; mostly connect the read signal with a loop which will executed by the thread start
      create and open ascii dat files from in files
      set use_samples to 0 in order to READ ALL
     \param single_continuousjoin_continuousindividual 0, 1 continuously, 2 continously individual ( = direct producer & consumer threads as we have for ascii output for example )
   */
  void prepare_ascii_outfiles(const size_t &nbuffers, const size_t &wl, const quint64 &use_samples, const atsfileout &buffer_type, const QString &file_extension);

  void prepare_ats_outfiles(const size_t &nbuffers, const size_t &wl, const quint64 &use_samples, const atsfileout &buffer_type, const QString &file_extension);

  /*!
   * \brief prepare_mt_spectra prepares frequency lists and parzen vectors - hence MULTIPLE SIGNALS are emitted and CATCHED
   * \param nbuffers
   * \param wl
   * \param fcut_upper 0.1 - 10% of upper spectra will not be used
   * \param fcut_lower 0.05 - 5% of lower spectra will not be used
   * \param use_samples - absolute number of sampes to be used (only meaningful if a segment is to be processed), 0 == ignore
   * \param overlapping set 0 if you want to use overlapping_factor, else it will be taken as it is
   * \param overlapping_factor say 0.4 if you want 40% overlapping - must be used if you want to use auto bandwidth and auto parzen
   * \param parzen_radius
   * \param min_lines minimum of spectral line to join as one frequency
   * \param asp_coll all channels all spectra
   * \param buffer_type
   * \return false in case no possible setting found (now window length accoding to calibration for example)
   */
  bool prepare_mt_spectra(const size_t &nbuffers, const size_t &wl, const double &fcut_upper, const double &fcut_lower, const quint64 &use_samples, const quint64 &overlapping,
                          const double &overlapping_factor, const double &parzen_radius,
                          const size_t &min_lines,
                          const int &force_frequency_list,
                          const atsfileout &buffer_type, const bool limit_upper_to_cal);

  bool prepare_csem_spectra(const size_t &nbuffers, const size_t &wl, const double &fcut_upper, const double &fcut_lower, const atsfileout &buffer_type, const bool limit_upper_to_cal);

  void slot_set_target_frequencies(const std::vector<double> &flist);

  void slot_frequencies_fft_indices_selected(const std::vector<size_t> &selindices, const std::vector<size_t> &selindices_sizes);

signals:

  void signal_slide_to_second(quint32 ui_timestamp); //!< signals to other e.g atfiles

  void signal_slide_to_sample(const quint64 &sample_no);

  void read(bool emits); //!< signals to other e.g. atsfiles

  void signal_fwd_rx_adu_msg(const int &system_serial_number, const int &channel, const int &slot, const QString &message);

  void signal_read_ascii_finished(const QString message);
  void mc_data_closed_infiles();  //!< signals to other apps
  void mc_data_closed_outfiles(); //!< signals to other apps

  void frequencies_selected(const std::vector<double> &selfreqs); //!< same signal like in math_vector; used for mt without parzening

  void files_open(QStringList files); //!< signals to other apps

  void read_tsdata_ready(QDataStream::Status); //!< emits the collect read status

  void signal_get_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);

  void signal_counts_per(const int counts);

  void signal_text_message(const QString message);

  /*!
   * \brief signal_guess_100 forward the status of atsfiles and collectors and so on
   * \param value_0_100
   */
  void signal_guess_100(int value_0_100);
  void signal_repaint();

  void signal_survey_basedir(const QDir base); //!< while open files check if we have a survey

  void signal_got_freq_targets(const std::vector<double> &fftfreqs, const std::vector<double> &target_frequencies, const double &parzen_radius);
  void signal_got_freq_targets_lines(const std::vector<double> &fftfreqs, const std::vector<double> &target_frequencies, const size_t &min_lines, const double &bw);

  void signal_general_msg(const QString &sender, const QString &component, const QString &message);
  void signal_general_msg_nums(const QString &sender_and_msg, const QVariant num_1, const QString &num_seperator, const QVariant num_2);

private:
  std::shared_ptr<prc_com> mxc = nullptr; //!< keep a copy of the global command lines and others

  //*********************************************************************************************
  /*!
   * \brief fill_prc_com_with_base_values conatin the variables I need in my QMAP; use in main modules  this->cmdline->append_data_from_other(this->mtsite->get_data(), "mc_data_");
   */
  void fill_prc_com_with_base_values();

  //*********************************************************************************************

  int read_nothing0_close1_rewindlast2_on_read_at_eof_beof = 0;

  int accumulate_read_errors = 0;

  volatile size_t ats_in_global_read_counts = 0;   //!< let the threads know the mcp readings
  volatile size_t ats_out_global_write_counts = 0; //!< let the threads know the mcp writings / spectra done etc

  std::vector<size_t> ats_in_global_read_counts_individual; //!< connect individual
  std::vector<size_t> ats_out_global_write_counts_individual;

  size_t ats_in_channels_arrived = 0;
  size_t ats_in_channels_expected = 0;

  int threadcontrol = 0; //!< copy the thread control mode (def see procmt_alldefines.h)
  int mt_type = 0;       //!< copy the mt (or none) mode (def see procmt_alldefines.h)

  std::shared_ptr<firfilter> firfil; //!< uesed as template for fir filter creation

  // only for checks at the beginning
  QList<eQDateTime> atstimings;

  QSqlDatabase dbmcd;
  std::vector<double> target_frequencies;
  double parzen_radius = 0.0; //!< actual paren radius (e.g. from MT spectra)
  size_t wl = 0;              //!< data read size, aka window length (chunk)
  double sample_freq = 0.0;   //!< sample freq of actual files

  double overlapping_factor;
  quint64 overlapping;
  std::shared_ptr<msg_logger> msg;

  int files_not_synced = 0; //!< should be zero; counts non synced files

  void auto_bw_prz();

  size_t next_power_of_two(const size_t n);

  int has_gigybytes = 1;           //! set gigabytes of memory
  size_t mc_data_max_counts = 0;   //! buffer the predicted loops
  size_t mc_data_last_reading = 0; //! buffer last chunk size

  size_t min_lines = 0;
  std::vector<size_t> selindices;
  std::vector<size_t> selindices_sizes;

  size_t flush_count = 0; //!< count signals from the outfile collectors

  bool parzen_vector_failed = false;

protected:
  mutable std::mutex mutex_acsp;
};

#endif // MC_DATA_H
