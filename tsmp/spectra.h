/***************************************************************************
                          spectra.h  -  description
                             -------------------
    begin                : Mon Feb 5 2001
    copyright            : (C) 2001 by B. Friedrichs
    email                : bfr@metronix.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPECTRA_H
#define SPECTRA_H

#include "allinclude.h"
#include "fftReal/FFTReal.h"
#include "mtx_iter.h"
#include "my_valarray.h"
#include "string_utils.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <valarray>

#include "allinclude.h"
#include "bfr_iter.h"
#include "icalibration.h"

using namespace std;

/**
 *@author B. Friedrichs
 */

// adu_value in mV / (trf) = nT

// single spec mV/ sqrt(Hz)
// power spec mV^2 / Hz

class v_spectra {

public:
  v_spectra &operator/=(const v_spectra &v) {

    if (pts != v.pts) {
      cerr << "v_spectra::operator /=  -> sizes of spectra are different!!! Result undefined " << endl;
    }

    // spc /= v.spc;
    for (size_t i = 0; i < spc.size(); i++)
      spc[i] /= v.spc[i];

    return *this;
  }

  v_spectra &operator*=(const v_spectra &v) {

    if (pts != v.pts) {
      cerr << "v_spectra::operator *=  -> sizes of spectra are different!!! Result undefined " << endl;
    }

    spc *= v.spc;

    return *this;
  }

  v_spectra &operator+=(const v_spectra &v) {

    if (pts != v.pts) {
      cerr << "v_spectra::operator +=  -> sizes of spectra are different!!! Result undefined " << endl;
    }

    spc += v.spc;

    return *this;
  }

  v_spectra &operator-=(const v_spectra &v) {

    if (pts != v.pts) {
      cerr << "v_spectra::operator -=  -> sizes of spectra are different!!! Result undefined " << endl;
    }

    spc -= v.spc;

    return *this;
  }

  v_spectra &operator/=(const double &v) {

    spc /= v;

    return *this;
  }

  v_spectra &operator*=(const double &v) {

    spc *= v;
    return *this;
  }

  v_spectra &operator+=(const double &v) {

    spc += v;

    return *this;
  }

  v_spectra &operator-=(const double &v) {

    spc -= v;

    return *this;
  }

  v_spectra &operator/=(const valarray<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] /= v[i];

    return *this;
  }

  v_spectra &operator*=(const valarray<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] *= v[i];

    return *this;
  }

  v_spectra &operator+=(const valarray<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] += v[i];

    return *this;
  }

  v_spectra &operator-=(const valarray<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] -= v[i];

    return *this;
  }

  v_spectra &operator+=(const complex<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] *= v;

    return *this;
  }

  v_spectra &operator-=(const complex<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] -= v;

    return *this;
  }

  v_spectra &operator/=(const complex<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] /= v;

    return *this;
  }

  v_spectra &operator*=(const complex<double> &v) {

    for (size_t i = 0; i < spc.size(); i++)
      spc[i] *= v;
    return *this;
  }

  // Cooley–Tukey FFT (in-place)
  void fft(std::valarray<complex<double>> &x);
  // inverse fft (in-place)
  void ifft(std::valarray<complex<double>> &x, bool scale = true);
  bool is_pow2(const size_t wl);
  size_t next_power_of_two(const size_t n);

  valarray<complex<double>> spc, spc_conj;                // spectra complete complex
  valarray<double> spwr, sampl, sphase;                   // complete power and amplitude
  valarray<double> smean_pwr, smean_ampl, smean_ampl_prz, // mean of each
      smedian_pwr, smedian_ampl,                          // median of each
      smedian_pwr_range, smedian_ampl_range,              // median range of each
      freq,                                               // list of frequencies
      smax_pwr, smin_pwr, smax_ampl, smin_ampl,
      smean_phase;

  valarray<complex<double>> smean_c, smedian_c, smedian_r_c, // same for complex values
      smean_c_conj, smedian_c_conj;

  valarray<complex<double>> trf; // transfer function
  string SensorType, SystemType; // what sensor and digitizer was used
  string my_name, inputfilename; // who I am and what was my input

  int calc_sp(int hanning, int detrend, const valarray<double> &input, vector<bool> &select, string extra_scale = "", int inverse_trf = 0, int dump_trf = 0);
  //  int power(int stack_it);
  //  int amplitude(int stack_it);
  int calc_ts(valarray<double> &output, int scale_f);

  int single(const char *type); // power, ampl, conj
  int mean(const char *type);   // power, ampl, complex
  double sstat(const char *type, const char *stat, double median_rg);

  int init(const string s_inputfilename, unsigned int i_wl, double isample_freq, size_t i_size,
           unsigned int i_no_dc, vector<bool> &select, size_t overlay = 0, const int invfft_zeropadding = 0);
  int write(const char *type, int stacked, int time_col, const unsigned int &ul, const unsigned int &ll,
            double &add, double &mul, string &name, valarray<double> &flist);
  int sel_freq(const valarray<double> &newf);

  size_t read_calib_sensor(string readthis, string chopper, const double &samplefreq, const double &gain,
                           const double &factor, const double &offset, string adc, string tags);

  size_t theoretical(string readthis, string chopper, const double &samplefreq, const double &gain,
                     const double &factor, const double &offset, string adc, string tags);

  // simple interface for calibartion class - but we do not ues the frequency list because it will generated here
  // inside the class; however you should prove the originbal sampling frequency befor filtering because even
  // if you have a low sample freq now the tranfer function of the high sampling freq. has to be applied in case
  size_t read_calib_system(string readthis, string chopper, const double &samplefreq, const double &gain,
                           const double &factor, const double &offset, string adc, string tags);

  valarray<complex<double>> &get_spc() { return this->spc; }
  valarray<double> &get_freq() { return this->freq; }
  size_t get_stacks() { return stacks; }
  size_t get_pts() { return pts; }
  size_t get_wl() { return wl; }
  size_t get_size() { return size; }
  size_t get_ovr() { return ovr; }
  double get_sample_freq() { return sample_freq; }

  // initialize spectra with window_length wich is the resulting fft_length/2
  v_spectra();
  ~v_spectra();

  void set_calibsensor(ICalibration *scalib) { this->calib_sensor = scalib; }
  void set_calibsystem(ICalibration *ycalib) { this->calib_system = ycalib; }
  void set_calibother(ICalibration *ocalib) { this->calib_other = ocalib; }
  size_t set_flist_parzen(valarray<double> &inflist, const double parzen);

  // control flags
  int is_init, is_spec, is_pwr, is_ampl, is_conj, is_mean_phase, is_phase,
      is_mean_pwr, is_mean_ampl, is_mean_c,
      is_median_pwr, is_median_ampl, is_median_c,
      is_median_pwr_range, is_median_ampl_range,
      is_max_pwr, is_min_pwr, is_max_ampl, is_min_ampl, is_median_r_c, is_mean_c_conj, is_median_c_conj,
      has_trf, is_rect, is_hann;

  void fwd(const valarray<double> &in, valarray<complex<double>> &out, const bool show = false);
  void inverse(const valarray<complex<double>> &in, valarray<double> &out, const bool show = false);
  complex<double> nth_element = complex<double>(0.0, 0.0);

private:
  ICalibration *calib_sensor; /*!< pointer to Interface class */
  int cal_sensor_exist;
  ICalibration *calib_system; /*!< pointer to Interface class */
  int cal_system_exist;
  ICalibration *calib_other; /*!< pointer to Interface class */
  int cal_other_exist;

  string sensor_calfile;
  string system_calfile;
  string other_calfile;

  ffft::FFTReal<double> *fft_object = nullptr;

  size_t size;        // size of total spectra  not stacked
  size_t inget_size;  // size of time series input;
  size_t pts;         // length of the FFT; is half of window length of the fft algorithm
  size_t stacks;      // total amount of spectra, each of length pts
  size_t count;       // counter for 0 .. stacks
  size_t wl;          // window length of fft algorithm = 2* pts of resulting spectra
  size_t ovr;         // overlay 0 ... < wl; shouldn't be greater than wl/2
  unsigned int no_dc; // avoid output of DC component into files
  // because often log-log display is used and DC has f = 0!
  double sample_freq; // sampling frequency of the signal

  double wincal; // cal function for the window

  size_t f_begin;         //!< freq index min of native frequencies for parzening
  size_t f_end;           //!< freq index max of native frequencies for parzening
  double parzen;          //!< parzen radius 0.1 .. 0.25
  valarray<double> flist; //!< contains a list of valid frequencies for parzening
  size_t prz_pts;         //!< amount of freqs selected by parzening
  size_t prz_start;       //!< index min of flist input can be used for parzening
  size_t prz_stop;        //!< index max of flist input can be used for parzening

  int invfft_zeropadding; //!< full ts inversion with one slice

  /***************************************************************************
   *                                                                         *
   *   definitions of the basic operators                                    *
   *                                                                         *
   *   only the the bas complex spc vector will be used                      *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/
};

#endif
