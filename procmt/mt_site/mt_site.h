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

#ifndef MT_SITE_H
#define MT_SITE_H

#include "mt_site_global.h"
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql>
#include <algorithm>
#include <complex>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <vector>

#include "buckets.h"
#include "iterator_templates.h"
#include "math_vector.h"
#include "mt_data.h"
#include "prc_com.h"
#include "procmt_alldefines.h"
#include "statmaps.h"

#include "iterator_complex_templates.h"

/*!
 * \brief make_d make d estimator
 */
template <typename T>
std::vector<T> make_d(const std::vector<T> &Hx, const std::vector<T> &Hy, const std::vector<T> &Exy) {

  std::vector<T> d(2);
  d[0] = std::inner_product(Hx.begin(), Hx.end(), Exy.begin(), T(0));
  d[1] = std::inner_product(Hy.begin(), Hy.end(), Exy.begin(), T(0));

  return d;
}

/*!
 * \brief make_mest make m estimator
 */
template <typename T>
std::vector<T> make_mest(const std::vector<T> &Hx, const std::vector<T> &Hy,
                         const std::vector<T> &Hxr, const std::vector<T> &Hyr,
                         const std::vector<T> d) {

  std::vector<T> G1(2);
  std::vector<T> G2(2);
  std::vector<T> mest(2);
  G1[0] = std::inner_product(Hx.begin(), Hx.end(), Hxr.begin(), T(0)); // row 1
  G1[1] = std::inner_product(Hx.begin(), Hx.end(), Hyr.begin(), T(0));
  G2[0] = std::inner_product(Hy.begin(), Hy.end(), Hxr.begin(), T(0)); // row 2
  G2[1] = std::inner_product(Hy.begin(), Hy.end(), Hyr.begin(), T(0));

  T divisor = G1[0] * G2[1] - G1[1] * G2[0];

  mest[0] = ((T(-1) * G1[1] * d[1]) + (G2[1] * d[0])) / divisor;
  mest[1] = ((G1[0] * d[1]) - (G2[0] * d[0])) / divisor;

  return mest;
}

/*!
 * \brief The mt_site class
 */
class mt_site : public QObject, public prc_com {

  Q_OBJECT
public:
  explicit mt_site(const size_t num_threads, const mttype enum_mttype, QObject *parent = Q_NULLPTR);

  // mt_site& operator = (mt_site const &rhs);

  // mt_site(const mt_site &rhs);

  ~mt_site();

  // void assign();

  // ac[0] = z[0] ~ f[0]
  std::vector<acsp> ac_spectra; //!< all auto and cross spectra and single spc for each frequency one

  // std::vector<acsp>       tmp_ac_spectra;                 //!< all auto and cross spectra
  // std::vector<f_spectra<std::complex<double>>> ssp;       //!< all input spectra ex, ey.... rhz, for each frequency ONE container

  // example z[2 == frequency index].f = 318 Hz (high[0] to low [last]; z[2 == here 318 Hz].size() = 600 stacks for 318 Hz; z[2].d[xy][599] las xy of 318 Hz of xy tensor;
  // z[2].d[xy] is a vector
  std::vector<mt_data_t<std::complex<double>>> z;  //!< all tensors all f all stacks z[yy][132.0] ...vector<complex>data z[stack, stack, n-1]
                                                   //    std::map<int, regression_data> zstat;                   //!< place the statistic results hers; z.re = x, z.im = y
  mt_data_res<std::complex<double>> zet;           //!< result tensors zet[xy][132.0](22.7, 0.3);
  mt_data_res<std::complex<double>> zet_last_proc; //!< result tensors from last processing zet[xy][132.0](22.7, 0.3);

  mt_data_res<double> rho; //!< result rhos rho[xy][132.0] - to be used for statistics
  mt_data_res<double> phi; //!< result phis phi[xy][132.0] - to be used for statistics

  QMap<QString, QVariant> tx_e_pos; //!< positions of TX electrodes U, V, W

  void open(std::shared_ptr<all_spectra_collector<std::complex<double>>> &all_channel_collector, const bool resize, const bool resize_z, const int ac1_ssp2_acssp3);

  std::vector<prc_com> in_ats_prc_coms; //!< keep a copy of the input data

  std::vector<double> get_freqs() const;
  std::vector<std::future<void>> futures;

  void join_dump_threads();

  /*!
   * \brief that CAN be the acsp size in case ALL have acsp
   * \return returns the size of valid ac spectra
   */
  size_t size() const;

  /*!
   * \brief that CAN be the acsp size in case ALL have ssp - single spectra; in sts MT ssp are NOT stored and you should get 0
   * \return returns the size of valid ac spectra
   */
  size_t ssp_size() const;

  void set_z_from_last_proc(const mt_data_res<std::complex<double>> &zet_old);

  void clear();

  void resize();

  void resize_z();

  /*!
   * \brief get_vectors_fde Frequency Data Error
   * \param what
   */
  void get_vectors_fde(const QString &what);

  /*!
   * \brief resize_auto_spc_only pre-allocate auto spectra only
   */
  void resize_auto_spc_only();
  /*!
   * \brief dump_path path where to dump dump files
   */
  QString dump_path;

  /*!
   * \brief insert_prc_com get data from command line starting with mt_site_ : strip_classname is true here
   * \param cmdline
   */
  void insert_prc_com(std::shared_ptr<prc_com> cmdline);

  void set_mt_mode(const mttype &enum_mttype, const proc_type &enum_proc_type);

  void dump_ac_spc();

  void clean_up_residuals();

signals:

  /*!
   * \brief signal_general_msg sending data to the msg logger
   * \param sender
   * \param component
   * \param message
   */
  void signal_general_msg(const QString &sender, const QString &component, const QString &message);

  /*!
   * \brief signal_general_msg_nums sending data to the msg logger
   * \param sender_and_msg
   * \param num_1
   * \param num_seperator
   * \param num_2
   */
  void signal_general_msg_nums(const QString &sender_and_msg, const QVariant num_1, const QString &num_seperator, const QVariant num_2);

  /*!
   * \brief signal_general_msg_4strs sending data to the msg logger
   * \param sender_and_msg
   * \param field2
   * \param field3
   * \param field4
   */
  void signal_general_msg_4strs(const QString &sender_and_msg, const QString &field2, const QString &field3, const QString &field4);

  /*!
   * \brief signal_mt_site_finished is emitted when the run() (thread) is finished
   */
  void signal_mt_site_finished();
  void signal_csem_site_finished();

  /*!
   * \brief signal_mt_site_EDI_finished to be catched by ediplotter display module
   * \param zet
   */
  void signal_mt_site_EDI_finished(const mt_data_res<std::complex<double>> &zet);

  void signal_d_vector_created(const QString &acsp_name, std::vector<double> &v);
  void signal_c_vector_created(const QString &mem_name, std::vector<std::complex<double>> &v);

  void signal_de_vector_created(const QString &acsp_name, std::vector<double> &v, std::vector<double> &e);
  void signal_ce_vector_created(const QString &mem_name, std::vector<std::complex<double>> &v, std::vector<double> &e);

  void signal_fd_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<double> &v);
  void signal_fc_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<std::complex<double>> &v);

  void signal_fde_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<double> &v, std::vector<double> &e);
  void signal_fce_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<std::complex<double>> &v, std::vector<double> &e);

  void signal_mt_site_updated();

  void signal_frequency_added(const double &frequency);

  void signal_csem_dip_qmap(const QMap<QString, QVariant> &data, const int dip);

  void signal_get_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);
  void signal_update_progress_bar(const int counts);

public slots:

  void run();

  void run_csem();

  /*!
   * \brief write_bin_spectra writes a spectra dump file for spectra plotter
   * \param file_written file to write, will be placed inside the dump_path; NO PATH variable insided filename
   * \return
   */
  size_t write_bin_spectra(QFileInfo &qfi);

  /*!
   * \brief read_bin_spectra read a spectral dump file; ABSOLUTE filepath to be given
   * \param qfi
   * \return
   */
  size_t read_bin_spectra(const QFileInfo &qfi);

  size_t write_ascii_single_spectra(QFileInfo &qfidirname);

  // void append_spectra_collector(std::shared_ptr<spectra_collector> &spcl, const std::vector<prc_com> &prcs);

  /*!
   * \brief slot_ssp_stack_all - stack all single spectra without MT; used for parallel tests and others
   */
  void slot_ssp_stack_all();

  void slot_ssp_stack_half_csem(const int &first_1_second_2_all_3);

  /*!
   * \brief slot_calc_auto_spc only auto spectra
   */
  void slot_calc_auto_spc();

  /*!
   * \brief slot_calc_ac_spc auto- and cross spectra
   */
  void slot_calc_ac_spc();

  void slot_dump_ampl_spc(bool error_bars = false);

  void slot_dump_ssp_coherency(bool error_bars = false);

  void calc_z();

  void calc_tipper();

  void calc_rho_phi();

  void stack_all();

  void huber(const double hlimit = 1.58, const size_t iter_limit = 20);

  void xstddev(const size_t what);

  void xstddev_auto(const size_t what);

  void ssp_xstddev();

  void ssp_normalize_by_E();

  /*!
   * \brief prepare_parallel_coherency calculate parallel cohereny like for parallel test of quality of Hx & Hxr
   */
  void prepare_parallel_coherency();

  void coherency_threshold(const double cohlower, const double cohupper, const size_t what);

  //   void calc_median_if_rho_phi();

  //    void calc_median_if_limits(const double lower_roh, const double upper_rho,
  //                     const double lower_phi, const double upper_phi);

  /*!
   * \brief quadrant_check on medium and simple structures we expect the xy is re,im positive and yx both negative; this may not be true on complex 3D structures
   * \param what
   */
  void quadrant_check(const size_t what);

  /*!
   * \brief inside_range_median_rho_phi (calc_rho_phi must have been calculated in advance!) sets to false if outside
   * \param lower_rho
   * \param upper_rho
   * \param lower_phi
   * \param upper_phi
   * \param what
   */
  void inside_range_median_rho_phi(const double lower_rho, const double upper_rho,
                                   const double lower_phi, const double upper_phi, const size_t what);

  void slot_save_data_sql(const QString &filenname);

  //  void rhoa(const double& f, std::vector<double>::const_iterator it );

  void abs_stack_tensor_spectra(const bool dump, const QString insertme);

  /*!
   * \brief slot_update_booleans: if zxy was false, set all other also to false; updates the counts
   */
  void slot_update_booleans(const size_t what, const bool emits = false);

  /*!
   * \brief slot_reset_booleans set all to true again
   */
  void slot_reset_booleans();

private:
  // enum spce
  std::map<int, std::map<double, double>> ac_spectra_stacked;
  //*********************************************************************************************
  /*!
   * \brief fill_prc_com_with_base_values conatin the variables I need in my QMAP; use in main modules  this->cmdline->append_data_from_other(this->mtsite->get_data(), "mt_site_");
   */
  void fill_prc_com_with_base_values();

  //*********************************************************************************************

  void prepare_parallel_coherency_vector(const size_t index);

  // made for parallel threads - look for corresponding without "_vector"
  void calc_z_vector(const double &f);
  void calc_z_vector_rr(const double &f);

  void calc_tipper_vector(const double &f);

  void calc_rho_phi_vector(const double &f);

  void stack_all_vector(const double &f);           //!< classic MT
  void stack_all_acspectra_vector(const double &f); //!< looking at the simple stacked spectra

  void xstddev_vector(const double &f);

  void xstddev_vector_auto(const double &f);

  void huber_vector(const double &f, const double hlimit = 1.58, const size_t iter_limit = 20);

  void calc_parallel_coherency_vector(const double &f); //!< for parallel oriented spectra only

  void coherency_vector(const double &f, const double &cohlower, const double &cohupper);

  void inside_range_median_vector_rho_phi(const double &f, const double &lower_rho, const double &upper_rho, const double &lower_phi, const double &upper_phi);

  /*!
   * \brief calc_auto_spc_vector calculates the auto spectra for one frequency - used by futures
   * \param f
   */
  void calc_auto_spc_vector(const double &f);

  /*!
   * \brief calc_ac_spc_vector  calculates the auto- and cross spectra for one frequency - used by futures
   * \param f
   */
  void calc_ac_spc_vector(const double &f);

  /*!
   * \brief median_complex calculates the median for the real part and the imaginary part
   */
  void median_complex();

  // void median_rho_phi();

  void median_if_complex_vector(const double &f);

  // void median_if_rho_phi_vector(const double& f);

  void quadrant_check_vector(const double &f);

  void ssp_stack_all_vector(const double freq);

  void ssp_stack_half_csem_vector(const double freq, const int &first_1_second_2_all_3);

  void ssp_xstddev_vector(const double &f);

  void ssp_normalize_by_E_vector(const double &f);

  void ssp_dump_stack_all_vector(const size_t &spce, bool error_bars = false);

  /*!
   * \brief ssp_dump_coherency_all_vector plain dump of Hx vs. Hy in paralle test for example
   * \param spce
   * \param error_bars
   */
  void ssp_dump_coherency_all_vector(const size_t &spce);

  void cp_ssp_vals_to_QMap(const acsp &ac, QMap<QString, QVariant> &data, const int dip);

  void cp_tx_epos_vals_to_QMap(QMap<QString, QVariant> &data);

  mttype mymttye = mttype::mt;
  proc_type myproc_type = proc_type::mt_prz;

  double round_phi;

  std::vector<double> freqs;

  mutable std::recursive_mutex z_mutex;

  std::vector<qint64> start_date_times;
  std::vector<qint64> stop_date_times;

  int memory_allocated = 0;

  std::unique_ptr<QSqlDatabase> db = nullptr;
  QString connection_name = "edistorage";
  QString db_type = "QSQLITE";
  QFileInfo dbname;

  std::vector<std::thread> asciiwriterthreads;
  std::vector<std::thread> ssp_threads;
  std::vector<std::thread> ac_threads;

  size_t num_threads = 8;

  int counts; // progress bar by count f
};

#endif // MT_SITE_H
