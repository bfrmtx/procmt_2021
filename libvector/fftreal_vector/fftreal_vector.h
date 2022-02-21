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

#ifndef FFTREAL_VECTOR_H
#define FFTREAL_VECTOR_H

#include <vector>
#include <complex>

#include <cmath>
#include "iterator_templates.h"
#include <memory>
#include "FFTReal.h"
#include <iostream>
#include <QObject>

class calibration;


enum fftreal_ctrl {
    nothing = 0,
    scale = 1,
    detrend = 2,
    hanning = 3,
    detrend_hanning = 4,
    scale_detrend_hanning = 5,
    calibrate = 6,
    inverse = 7,
    inverse_after_cabibration = 8,
    is_dc_ftt = 9
};


/*!
   \brief The fftreal_vector class is an adoption of the FFREAL for a vector it does  <br>
   x real, f "complex" in the sense that f[1] is real and f[wl/2 +1] is the imaginary part; f[0] is DC <br>
   POSITIVE forward FFT do_fft() : f(k) = sum (p = 0, N-1, x(p) * exp (+j*2*pi*k*p/N)) <br>
   NEGATIVE inverse <br> x(k) = sum (p = 0, N-1, f(p) * exp (-j*2*pi*k*p/N)) <br>

 */
class fftreal_vector : public QObject
{
    Q_OBJECT

public:

    fftreal_vector();

    ~fftreal_vector();
    /*!
     * \brief set_buffer is the main INITIALISATION; it also RESETS the pointers of ampl and cal vectors!
     * \param inbuf
     * \param outbuf complex number vector; will be resized
     * \param fcut_upper 0.0 = cut nothing, 0.05 cut the upper 5%
     * \param fcut_lower 0.0 = cut nothing, 0.1 cuts lower 10%; USE =.0 AND ISDCFFT = TRUE for fft -> inverse fft
     * \param freq   when f is set the DC part is skipped automatically
     * \param bwincal respect the length of the FFT - so 1024 and 4096 should give the same result
     * \param isdcfft keep the DC component in output - if yo create a frequency vector and use log display this will result into error log(0) -> error!
     * \param nspc when calculate the ftt -> calib -> inverse fft complex spectra ("outbuf") my not be needed; like you do for full inverse of timeseries;
     * \param cut_upper cut upper frequencies from output
     * \param cut_lower cut lower frequencies from output <br>
     * the crucial step is to force a dc ftt if needed - and that IS THE CASE if ou want to calculated the inverse
     *
     * Hence that always 0... < wl (ts) -> 0...< wl/2 (spc) will be processed; start & stop will be a subset defined by cut upper & lower
     */
    bool set_buffer(const std::vector<double> &in_out_buf, std::vector<std::complex<double>> &outbuf, const double freq = 0, const bool bwincal = true,
                    const bool isdcfft = false, const bool nspc = false, const bool inverse_from_forward = false,
                    const double fcut_upper = 0, const double fcut_lower = 0);


    void set_amplitude_spectra(std::vector<double> &fft_amplitude);

    /*!
     * \brief set_calibration
     * \param mulcal
     * \return
     */
    bool set_calibration(std::vector<std::complex<double>> &mulcal);


    bool set_mtx_calibration(std::vector<std::complex<double>> &divcal);

    /*!
       \brief unset_calibration - turns off te calibration flag
     */
    void unset_calibration();

    void set_amplitude_spectra_stacked(std::vector<double> &fft_amplitude_stacked);

    /*!
       \brief fwd_process does the fft.
       \param inbuf
       \param outbuf
       \param ctrl
     */
    void process(std::vector<double> &inbuf, std::vector<std::complex<double>> &outbuf, const int ctrl = fftreal_ctrl::hanning);

//    /*!
//     * \brief set_inverse_buffer_after_forward will calculate the inverse immedeately while calling fwd_process; settings for calibration have to be set in advance.
//     *  hence that a inverse need a RECTANGULAR window!
//     * \param outbuf
//     * \return
//     */
//    bool set_inverse_buffer_after_forward(std::vector<double> &outbuf);

//    bool unset_inverse_buffer_after_forward();

//    /*!
//       \brief set_inverse_buffer makes a inverse FFT after having done a calibration - in order to get calibrated time series
//       \param inbuf
//       \param outbuf
//       \return
//     */
//    bool set_inverse_buffer(std::vector<std::complex<double>> &in_inv_buf, std::vector<double> &outbuf);



    /*!
       \brief get_frequencies
       \return list of frequencies for each point. Hence: using full FFT means that f[0] = 0 = DC part
     */
    std::vector<double> get_frequencies() const;

//    void gen_pure_theo_cal(std::vector<double> psrc, const double &f_sample, const size_t &start_idx, const size_t &stop_idx,
//                           const double &dc_ampl, const double &dc_phase_deg, const double &mul_by, const bool mul_by_f, const bool invert) const;

    void set_external_calibration(const double &dc_ampl, const double &dc_phase_deg, const double &mul_by, const bool mul_by_f, const bool invert);

//    /*!
//     * \brief gen_frequencies generate a frequency vector
//     */
//    std::vector<double> gen_frequencies();

//    /*!
//       \brief get_frequencies_for_inverse
//       \return list of frequencies for each point, including ALL frequencies, and f[0] = 0 = DC part
//     */
//    std::vector<double> get_frequencies_for_inverse(const bool skip_dc) const;

    /*!
       \brief next_power_of_two
       \param n
       \return 16 if n = 15 or 32 if in = 17 and so on
     */
    size_t next_power_of_two(const size_t n) const;

    void get_settings(double &freq, bool &bwincal, bool &isdcfft, bool &nspc, bool &inverse_from_forward, double &fcut_upper, double &fcut_lower) const;


    double get_nyquist_freq() const;

    void set_multiply(const double &factor);

signals:

    void signal_gen_pure_theo_cal(std::vector<double> &psrc, const double &f_sample, const size_t &start_idx, const size_t &stop_idx,
                                            const double &dc_ampl, const double &dc_phase_deg, const double &mul_by, const bool &mul_by_f, const bool &invert) const;


private:

    // forward declaration


    ffft::FFTReal <double> *fft_object = nullptr;           //!< create a fftreal

    size_t wl;                                              //!< input window length

    std::vector<double> psrc;                               //!< output is a half sided: pseudo real - complex; either make complex out of this or amplitudes
    std::vector<double> *fft_amplitude_stacked = nullptr;   //!< calculate the stacked amplitude spectra ( double )
    std::vector<double> *fft_amplitude         = nullptr;   //!< calculate the amplitude spectra ( double )
    std::vector<std::complex<double>> *mulcal  = nullptr;   //!< calibrate spectra spectra ( double ); we multiply in order to avoid division by ZERO here; so if you want to divide make a 1/trf !
    std::vector<std::complex<double>> *divcal  = nullptr;
    bool has_calibration = false;                           //!< want to calibrate
    bool nspc = false;                                      //!< no spectral window: don generate complex spectra - can be used for inverse fft to timeseries where you don't use the spectra itself
    bool inverse_from_forward = false;                      //!< if you want calibrated time series in nT we immedeately calculate the inverse

    bool isdcfft = false;
    double wincal = 0.0;
    double fwl = 0.0;                                       //!< double window length
    double f_sample = 0.0;                                  //!< set the sampling frequency - a frequency vector can be generated
    size_t n = 0;                                           //!< size of the complex spectra output (stop_idx - start_idx)
    std::vector<double> *freqs = nullptr;                   //!< a frequency vector can be generated

    double multiply_by;                                     //!< can be used by fluxgate - if mtx the scale might be set to 1./flugate parm
    double fcut_upper = 0.0;                                //!< for MT the complete spectra may not be needed; use 0.05 to cut some upper
    double fcut_lower = 0.0;                                //!< for MT the complete spectra may not be needed; use 0.1 to cut 10% lower

    size_t start_idx = 0;                                   //!< start index in case we use not the full spectra
    size_t stop_idx = 0;                                    //!< stop index in case we use not the full spectra

    size_t counter = 0;                                     //!< count the ffts (needed for thread control)

    bool has_calibration_external = false;                  //!< control external calibration settings
    double dc_ampl = 0.0;                                   //!< control external calibration settings
    double dc_phase_deg = 0.0;                              //!< control external calibration settings
    double mul_by = 0.0;                                    //!< control external calibration settings
    bool mul_by_f = true;                                   //!< control external calibration settings
    bool invert;                                            //!< control external calibration settings

};

#endif // FFTREAL_VECTOR_H
