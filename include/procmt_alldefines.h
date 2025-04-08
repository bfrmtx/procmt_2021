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

#ifndef PROCMT_ALLDEFINES
#define PROCMT_ALLDEFINES

#include <QDir>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <climits>
#include <complex>
#include <cstdint>
#include <queue>
#include <vector>

/*!
@file
Contains all definitions for ProcMT and depending
*/

// do not try for enum threadctrl : std::int32_t  ... that does not work in the sense we want it
// don not use none or nil twice - that also does not work - names will be expanded
// treat as global variables

enum tns : std::size_t {
  xx = 0,           //!< zxx component
  xy = 1,           //!< zxy component and coherency for the tensor row xx xy
  yx = 2,           //!< zyx component and coherency for the tensor row yx yy
  yy = 3,           //!< zyy component
  tns_size = 4,     //!< Z tensor without tipper (Hz)
  tx = 4,           //!< tx component
  ty = 5,           //!< ty component
  tns_tip_size = 6, //!< Z tensor plus Tipper
  xxxy = 1,         //!< coherency for the tensor row xx xy
  yxyy = 2,         //!< coherency for the tensor row yx yy
  tns_scalar = 0    //!< presently scalar NOT supported
};

enum tns_sel : std::size_t {
  xx_yy = 1,             //!< zxx and zyy components
  xy_yx = 2,             //!< zxy and zyx components
  xx_xy_yx_yy = 3,       //!< all z components
  tx_ty = 4,             //!< tx and ty components
  xx_xy_yx_yy_tx_ty = 5, //!< complete tensor and tipper
};

const double mue0 = 4.0 * M_PI * 1.0E-7; //!< mue0 constant; placed here for simplicity

const size_t _min_stacks = 3; //!< we must have more than 3 values to make a statistic

namespace cal {
/*!
 * \brief cal various cal status; they are independent so values occur serveral times
 */
enum cal : std::int32_t {

  nothing = 0,                      //!< nothing specific; use for init; no active parts like chopper known
  chopper_off = 0,                  //!< no chopper, same like above
  chopper_on = 1,                   //!< has a chooper AND chopper is switched ON
  chopper_undef = -1,               //!< undefined chopper
  any = 0,                          //!< plot / use anything
  amplitude = 1,                    //!< plot / use the amplitude
  phase_grad = 2,                   //!< plot / use the phase in grad 0...360
  complex_cal = 3,                  //!< plot / use the the complex values
  calibration = 0,                  //!< use the calibration as deliverd
  master_cal = 1,                   //!< use the master cal - that is an average of many coils
  interpol_cal = 2,                 //!< use the interplolated: these are the frequency <-> value pairs fitting to you FFT
  theo_cal = 3,                     //!< caluclated theoretical values, eg. have frequency index for your FFT
  adu08e_rf_1 = 1,                  //!< ADU-08e RF-1 on
  adu08e_rf_2 = 2,                  //!< ADU-08e RF-2 on
  adu07e_rf_1 = 1,                  //!< ADU-07e RF-1 on
  adu07e_rf_2 = 2,                  //!< ADU-07e RF-2 on
  adu07e_rf_3 = 3,                  //!< ADU-07e RF-3 on
  adu07e_rf_4 = 4,                  //!< ADU-07e RF-4 on
  adu08e_rf_off = 0,                //!< ADU-08e RF off
  adu07e_rf_off = 0,                //!< ADU-07e RF off
  adu08e_lp4hz_on = 4,              //!< ADU-08e Low Pass 4Hz on
  adu07e_lp4hz_on = 4,              //!< ADU-07e Low Pass 4Hz on
  adu08e_lp1hz_on = 1,              //!< ADU-08e Low Pass 1Hz on
  adu08e_lp_off = 0,                //!< ADU-08e Low Pass  off
  adu07e_lp_off = 0,                //!< ADU-07e Low Pass  off
  adu08e_hp500hz_on = 500,          //!< ADU-08e High Pass 500Hz on
  adu07e_hp500hz_on = 500,          //!< ADU-08e High Pass 500Hz on
  adu08e_hp1hz_on = 1,              //!< ADU-08e High Pass 1Hz on
  adu07e_hp1hz_on = 1,              //!< ADU-07e High Pass 1Hz on
  adu08e_hp_off = 0,                //!< ADU-08e High Pass off
  adu07e_hp_off = 0,                //!< ADU-07e High Pass off
  div_1 = 1,                        //!< input divider 1 (off direct)
  div_8 = 8,                        //!< input divider 8 (on, divides by 8 for +/- 10V extended input, e.g. coils
  apply_hf_spc_cal_sample_f = 8000, //!< sample frequency above we may want to apply spectral correction of the board
  apply_lf_spc_cal_sample_f = 800   //!< sample frequency above we may want to apply spectral correction of the board

};

} // namespace cal

/*!
 * \brief threadctrl: The threadctrl enum controls the thread model
 */
enum threadctrl : std::int32_t {

  threadctrl_single = 0,               //!<  thread needs to be triggered as we have in slider bar; so start/stop on TRIGGER
  threadctrl_continuous_join = 1,      //!<  thread waits for all other same threads - like if we need all 5 spectra and have to wait all input for MT
  threadctrl_continuous_individual = 2 //!<  thread is connected to other independently - like in ascii writers, filter, cuts
};

enum atsfileout : std::int32_t {

  atsfileout_void,

  ////////////////// continious

  // double time series data
  atsfileout_unscaled_timeseries,          //!<  raw, but you can not compare channels
  atsfileout_scaled_timeseries,            //!<  that sould be the default
  atsfileout_calibrated_scaled_timeseries, //!<  that sould be the default for H when using GUI; includes E field length
  atsfileout_unscaled_fir_filtered,        //!<  low / high pass, decimation - we don't want scale here

  // complex double spectra data
  atsfileout_spectra,            //!< includes E field length!
  atsfileout_calibrated_spectra, //!< includes E field length!
  atsfileout_raw_spectra,        //!< spectra as it is .... no use

  ///////////////////// stacked comes here !!!!!

  //  double stacked
  atsfileout_stacked_spectra,            //!< e.g. from raw - but does not have a physical unit
  atsfileout_calibrated_stacked_spectra, //!< that is the calibtrated output, needed for processing
  atsfileout_stacked_amplitude_spectra,  //!< take the amplitude - that is a real number

  // double stacked time series
  atsfileout_stacked_timeseries,                   //!<  raw, but you can not compare channels
  atsfileout_stacked_scaled_timeseries,            //!<  needed - for example for pre-stacking in time domain
  atsfileout_stacked_calibrated_scaled_timeseries, //!<  AFTER FFT -> CAL -> inv FFT

  atsfileout_ac_spectra

};

/*!
 * \brief The mttype enum definies various processing styles and input behaviour of files
 */
enum mttype : std::int32_t {
  nomt = 0,         //!< stream all files to somewhere
  mt = 1,           //!< local 4 or 5 channels
  emap = 2,         //!< take first to E and second H; treated as local
  rr = 3,           //!< take first 4, 5 channels, secondly Hx and Hy
  emap_rr = 4,      //!< take emap then rr = three XML files
  rr_full = 5,      //!< not implemented
  emap_rr_full = 6, //!< not implemented
  csem = 7          //!< output ex,ey,hx,hy,hz,dip1, dip2
                    // CSAMT check; we want use this for freqlistst
};

/*!
 * \brief The proc_type enum definies various spectra type (mt, parzen, huber_lines, lines) and how we handle the spectra
 */
enum proc_type : std::int32_t {
  no_mtproc = 0,
  mt_prz = 1,
  mt_woprz = 2,
  csamt = 3,
  lines = 4,
  csem_rxtx = 5,
  csamt_h_h = 6

};

// THE WHOLE NAMESPACE PMT ALLOWS ONLY ONE BY ONE DEFINITIONS
namespace pmt {

static QStringList survey_dirs_minimum{"cal", "config", "db", "dump", "edi", "filters", "jobs",
                                       "jle", "log", "processings", "shell", "tmp", "ts"}; //!< old structure is mimimum since 2008

static QStringList survey_dirs{"cal", "config", "db", "dump", "edi", "csem", "filters", "jobs",
                               "jle", "log", "processings", "shell", "tmp", "ts"}; //!< directory structure of the survey

template <class T>
bool create_survey_subdirs(const QDir &base_dir) {

  if (!base_dir.exists()) {
    bool ret;
    ret = base_dir.mkdir(base_dir.path());
    if (!ret)
      return ret;
    for (auto &str : pmt::survey_dirs) {
      QDir sub(base_dir.path() + "/" + str);
      ret = sub.mkdir(sub.path());
      if (!ret)
        return ret;
    }
    return ret;
  }
  return false;
}

///////////////// KEEP TOGETHER  /////////////////////
// synced with chan_no, chan_type !

static QStringList csem_dipcols{"rowid", "site_name", "freq", "msec_lat", "msec_lon", "cm_elev", "x", "y", "z",
                                "Ex_real", "Ex_real_Err", "Ex_imag", "Ex_imag_Err",
                                "Ey_real", "Ey_real_Err", "Ey_imag", "Ey_imag_Err",
                                "Ez_real", "Ez_real_Err", "Ez_imag", "Ez_imag_Err",
                                "Hx_real", "Hx_real_Err", "Hx_imag", "Hx_imag_Err",
                                "Hy_real", "Hy_real_Err", "Hy_imag", "Hy_imag_Err",
                                "Hz_real", "Hz_real_Err", "Hz_imag", "Hz_imag_Err",
                                "Tx_Curr_U_real", "Tx_Curr_U_real_Err", "Tx_Curr_U_imag", "Tx_Curr_U_imag_Err",
                                "Tx_Curr_V_real", "Tx_Curr_V_real_Err", "Tx_Curr_V_imag", "Tx_Curr_V_imag_Err",
                                "Tx_Curr_W_real", "Tx_Curr_W_real_Err", "Tx_Curr_W_imag", "Tx_Curr_W_imag_Err",
                                "U_msec_lat", "U_msec_lon", "U_cm_elev",
                                "V_msec_lat", "V_msec_lon", "V_cm_elev",
                                "W_msec_lat", "W_msec_lon", "W_cm_elev"};

static QStringList channel_types{"Ex", "Ey", "Hx", "Hy", "Hz", "REx", "REy", "RHx", "RHy", "RHz"}; //!< all possible input names

// is a numbering of the above !!

/*!
 * \brief The chan_type enum is the numbering for QStringList channel_types
 */
enum chan_type : std::size_t {
  ex = 0,
  ey = 1,
  hx = 2,
  hy = 3,
  hz = 4,
  rex = 5,
  rey = 6,
  rhx = 7,
  rhy = 8,
  rhz = 9,
  chan_type_size = 10
  // use so for whatever
};

template <typename T>
bool is_E(const size_t chtype) {
  if (chtype == 0)
    return true;
  else if (chtype == 1)
    return true;
  else if (chtype == 5)
    return true;
  else if (chtype == 6)
    return true;
  return false;
}
///////////////// ////////////// /////////////////////

static QMap<QString, double> auto_bw({{"off", 0},
                                      {"1/8 (larger FFT)", 0.125},
                                      {"1/4 (larger FFT)", 0.25},
                                      {"1/2 (larger FFT)", 0.5},
                                      {"1 (normal FFT)", 1},
                                      {"2 (smaller FFT)", 2},
                                      {"4 (smaller FFT)", 4},
                                      {"8 (smaller FFT)", 8}});

static QMap<QString, double> auto_prz({{"off", 0},
                                       {"1/8 (less smoothing)", 0.125},
                                       {"1/4 (less smoothing)", 0.25},
                                       {"1/2 (less smoothing)", 0.5},
                                       {"1 (normal parzen", 1},
                                       {"2 (more smoothing)", 2},
                                       {"4 (more smoothing)", 4},
                                       {"8 (more smoothing)", 8}});

///////////////// KEEP TOGETHER  /////////////////////
// static QStringList ac_spectra_names {"ExEx", "EyEy", "HxHx", "HyHy", "HzHz", "ExEy", "ExHx", "ExHy", "ExHz", "EyHx", "EyHy", "EyHz",
//                                      "HxHy", "HxHz", "HyHz", "ExRHx", "ExRHy", "EyRHx", "EyRHy",
//                                      "HxRHx", "HxRHy", "HyRHy", "RHxRHx", "RHxRHy", "RHyRHy", "ExREx", "EyREy"}; //!< all used auto- and cross-spectra

static QStringList ac_spectra_names{"ExEx", "EyEy", "HxHx", "HyHy", "HzHz",
                                    "ExEy", "ExHx", "ExHy", "EyHx", "EyHy", "HxHy", "HxHz", "HyHz",
                                    "HxRHx", "HxRHy", "HyRHx", "HyRHy", "RHxRHx", "RHxRHy",
                                    "RHyRHy", "ExHz", "EyHz", "ExRHx", "ExRHy", "EyRHx", "EyRHy", "ExREx", "EyREy", "RHxRHz", "RHyRHz", "RHzRHz",
                                    "HxRHz", "HyRHz", "HzRHx", "HzRHy", "HzRHz"};

/*!
 * \brief The spce enum numbers the QStringList ac_spectra_names; I want to use for vector indices; its the INDEX of ac_spectra_names
 */
enum spce : std::size_t {
  exex = 0,
  eyey = 1,
  hxhx = 2,
  hyhy = 3,
  hzhz = 4,
  exey = 5,
  exhx = 6,
  exhy = 7,
  eyhx = 8,
  eyhy = 9,
  hxhy = 10,
  hxhz = 11,
  hyhz = 12,
  // start rr
  hxrhx = 13,
  hxrhy = 14,
  hyrhx = 15,

  hyrhy = 16,
  rhxrhx = 17,
  rhxrhy = 18,
  rhyrhy = 19,
  // other
  exhz = 20,
  eyhz = 21,
  exrhx = 22,
  exrhy = 23,
  eyrhx = 24,
  eyrhy = 25,
  exrex = 26,
  eyrey = 27,
  rhxrhz = 28,
  rhyrhz = 29,
  rhzrhz = 30,
  // parallel test and other
  hxrhz = 31,
  hyrhz = 32,
  hzrhx = 33,
  hzrhy = 34,
  hzrhz = 35,
  spce_size = 36,
  spce_mt_size = 31
};

/*!
 *
 */
template <class T>
size_t spc_to_auto_cross_spc(const size_t lhs, const size_t rhs) {

  // allow ex ... rhz
  if ((lhs < pmt::chan_type::chan_type_size) && (rhs < pmt::chan_type::chan_type_size)) {
    QString findme = pmt::channel_types.at(int(lhs)) + pmt::channel_types.at(int(rhs));

    int pos = pmt::ac_spectra_names.indexOf(findme);
    if (pos == -1) {
      //    qDebug() << findme << "not found!";
      return pmt::spce::spce_size;
    } else
      return T(pos);
  } else
    return pmt::spce::spce_size;
}

/*!
 * * \brief db_cols_to_auto_cross_ssp_index returns the enum spce or enum chan_type of a string like HxHy_re or Ey_re
 */
template <class T>
size_t db_cols_to_auto_cross_ssp_index(const QString str_re_im, int &is_ac_1_is_ssp_2_fail_0) {

  if ((str_re_im.endsWith("_re") || str_re_im.endsWith("_im")) && (str_re_im.size() > 4)) {
    QString str = (str_re_im.left(str_re_im.size() - 3));
    is_ac_1_is_ssp_2_fail_0 = 0;
    int pos = -1;
    pos = pmt::ac_spectra_names.indexOf(str);
    if (pos != -1) {
      is_ac_1_is_ssp_2_fail_0 = 1;
      return size_t(pos);
    }
    pos = pmt::channel_types.indexOf(str);
    if (pos != -1) {
      is_ac_1_is_ssp_2_fail_0 = 2;
      return size_t(pos);
    }
  }

  is_ac_1_is_ssp_2_fail_0 = 0;
  return SIZE_MAX;
}

///////////////// ////////////// /////////////////////

} // end namespace pmt

enum tensor : std::int32_t {
  no = 0,         //!< invalid tensor
  MT = 1,         //!< 5 channel MT tensor
  MT_no_Z = 2,    //!< 4 channel MT tensor
  MT_ExHy = 3,    //!< scalar with Ex Hy
  MT_EyHx = 4,    //!< scalar with Ey Hx
  MT_RR = 5,      //!< 5 / 10 channel remote reference
  MT_RR_no_Z = 6, //!< 4 /  8 channel remote reference
  // for the first we can use standard frequency lists

  CSAMT_tensor = 7,      //!< tensor CSAMT with two dipoles, as defined by Zonge
  CSAMT_tensor_no_Z = 8, //!< tensor CSAMT with two dipoles, as defined by Zonge, no Hz
  CSAMT_ExHy = 8,        //!< scalar CSAMT with Ex Hy
  CSAMT_EyHx = 9,        //!< scalar CSAMT with Ey Hx
  CSAMT_MT = 10,         //!< MT with no parzen - similar to CSAMT
  GDS = 11,              //!< GDS - only H
  GDSRR = 12,            //!< GDS - only H of two sites
  VLF = 13               //!< VLF  very low frequencies
};

#endif // PROCMT_ALLDEFINES
