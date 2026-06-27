/***************************************************************************
                          spectra.cpp  -  description
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

#include "spectra.h"
#include <cstddef>


/***************************************************************************
 *                                                                         *
 *   init without parameters so that this can be init in a loop in main{}  *
 *                                                                         *
 ***************************************************************************/


v_spectra::v_spectra() {
    is_init = 0;
    cal_sensor_exist = 0;
    cal_system_exist = 0;
    cal_other_exist = 0;
    this->calib_sensor = NULL;
    this->calib_system = NULL;
    this->calib_other = NULL;
    is_spec =  is_pwr =  is_ampl =  is_mean_phase =  is_phase =
            is_mean_pwr =  is_mean_ampl =  is_mean_c =
            is_median_pwr =  is_median_ampl =  is_median_c =
            has_trf = 0;
    is_rect = is_hann = 0;
    is_max_pwr = is_min_pwr = is_max_ampl = is_min_ampl = 0;
    is_median_pwr_range = is_median_ampl_range = is_conj = 0;
    is_median_r_c = is_mean_c_conj =  is_median_c_conj = 0;
    f_begin = f_end = 0;
}


/***************************************************************************
 *                                                                         *
 *  destructor                                                             *
 *                                                                         *
 ***************************************************************************/


v_spectra::~v_spectra() {}


/***************************************************************************
 *                                                                         *
 * now call an extra init routine                                          *
 * and here we use an extra de-selection vector 1 = deselected sample      *
 ***************************************************************************/

// i_size is the size of the incomming data vector
int v_spectra::init(const string s_inputfilename, unsigned int i_wl, double dsample_freq, size_t i_size,
                    unsigned int i_no_dc,  vector<bool>& select, size_t overlay, const int invfft_zeropadding) {


    if ( i_size < i_wl) {
        cerr << "v_spectra::init -> not enough data; window: " << i_wl << " data: " << i_size << endl;
        exit(0);
    }

    this->invfft_zeropadding = invfft_zeropadding;

    if (!this->invfft_zeropadding) {
        wl = i_wl;                                    // set the window length in time domain
        pts = wl/2;                               // that is the length of the resulting FFT
    }
    else {                                        // FULL
        wl = this->next_power_of_two(i_wl);       // set the window length in time domain
        pts = wl/2;                               // that is the length of the resulting FFT
    }
    // this->isfftreal = ((wl & -wl) == wl);

    ovr = overlay;

    if (!this->invfft_zeropadding) {
        if (select.size() && (select.size() != i_size)) {
            cerr << "v_spectra::init -> hoops? your selection vector exists but has a"
                 << " different size as the the data vector; exit" << endl;
            exit(0);
        }

        if ((i_wl % 2)) {
            cerr << "v_spectra::init -> please use even numbers like 64, .. 1024 and so on" << endl;
            exit(0);
        }
    }
    else {
        ovr = 0;
    }





    if (ovr > wl/2) {
        cerr << "v_spectra::init -> overlay shouldn't be greater than fft window/2, ovr, wl " << ovr << ",  " << wl << endl;
    }

    if (ovr >= wl) {
        cerr << "v_spectra::init -> overlay can not be greater equal than the fft window, ovr, wl "  << ovr << ",  " << wl << endl;
        exit(0);
    }

    inputfilename = s_inputfilename;              // copy the name of the input data for ident.

    // hence that we need one point more
    // to store the DC component
    count = 0;
    no_dc = i_no_dc;                              // should be 1 in most cases to suppress
    // output of f[0] = 0 in log-log

    // if a de-selection vector was provided (1 = deselected)
    // we search for windows of wl of ZEROS inside this vector
    // we will call this in the main spectra routine
    // that is non optimum - but ok and fast enough

    if (select.size()) {                 // check whether a real selection was given
        vector<bool>::iterator i = select.begin();
        size_t j = 0;
        stacks = 0;
        do {
            i = search_n(select.begin() + j, select.end(), wl, 0);
            stacks++;
            j = (i - select.begin()) + wl;
        } while (j <= select.size());
        stacks--;
    } else
        stacks = i_size/wl;                      // possible stacks

    inget_size = i_size;                          // copy size of input vector
    sample_freq = dsample_freq;

    if (!this->invfft_zeropadding) {
        size = stacks * pts;                          // all fft values - not stacked
        spc.resize(size);                              // full size vector for raw complex spectra
    }
    else {
        stacks = 1;
        size = stacks * pts;
        spc.resize(size);
    }
    freq.resize(pts, sample_freq/wl);             // generate a frequency vector
    for (unsigned int i = 0; i < pts; i++)
        freq[i] *= i;

    cerr << "v_spectra::init -> " << stacks << " stacks initialisied, total size: " << size << endl;


    //  wincal = sqrt(1./double(sample_freq * pts));
    wincal = sqrt(1./double(sample_freq * (pts)));

    cerr << "wincal" << wincal << "  " << sample_freq << "  " << pts << endl;

    //  wincal = sqrt(double(2./(sample_freq * wl))); was orig. used for GMT fft
    count = 0;


    return is_init = 1;
}



/***************************************************************************
 *                                                                         *
 *   calculate the basis complex spectra of the raw time series            *
 *   but now together with a selection vector got from the time series     *
 ***************************************************************************/



int v_spectra::calc_sp(int hanning, int detrend, const valarray<double>& input, vector<bool>& select, string extra_scale, int inverse_trf, int dump_trf) {

    if (!is_init) {
        cerr << "v_spectra::calc_sp -> spectra not initialized!" << endl;
        cerr << "use v_spectra::init " << endl;
        exit(0);
    }

    if (!select.size() && (2 * size < input.size() - wl) ) {
        cerr << "v_spectra::calc_sp -> window length can not be grater then data size" << endl;
        exit(0);
    }

    if (hanning) {
        is_hann = 1;
        cerr << "v_spectra::calc_sp -> Hanning window and detrending used fftreal" << endl;
    } else {
        is_rect = 1;
        cerr << "v_spectra::calc_sp -> rectangular window, no detrending fftreal" << endl;
    }


    unsigned int i, j;
    valarray<double> time_slice;
    valarray<double> pseudo_complex;


    if (this->fft_object != nullptr) delete this->fft_object;
    this->fft_object = new ffft::FFTReal<double>(wl);
    if (!this->invfft_zeropadding) {
        time_slice.resize(wl);
        pseudo_complex.resize(wl);


    }

    size_t first = 0, last = 0, first_set = 0;
    double tmp_freq = 0.;


    if (!this->invfft_zeropadding) {
        // using atsview we should always have a selection vector here
        if (select.size()) {      // detailed check was done by init
            vector<bool>::iterator ii = select.begin();
            count = 0;
            i = 0;
            do {
                ii = search_n(select.begin() + i, select.end(), wl, 0);
                i = (ii - select.begin());


                if ( i <= input.size() - wl) {
                    if (!first_set) {
                        first = i;
                        cerr << "first window starts at: " << first << endl;
                        first_set = 1;
                    }
                    last = i;
                    //for (size_t zz  = 0; zz < input.size(); zz++) cerr << input[zz] << "  ";
                    time_slice = input[slice(i, wl, 1)];


                    // copy(&input[i], &input[i+wl], &time_slice[0]);
                    if (is_hann)
                        bfrtsmp::detrend_and_hanning(&time_slice[0], &time_slice[wl]);
                    //        cerr << time_slice.sum()/time_slice.size() << endl;
                    //        if (is_hann) my_valarray::detrend_and_hanning<double>(time_slice);
                    // this Hanning scales the time series internally back to 1 !!!

                    if (!is_hann && detrend)
                        my_valarray::detrend(time_slice);
                    // this will be needed for backwards calculations if transfer function is enabled
                    // for sensors with no trf for DC component - e.g. coils

                    fft_object->do_fft(&pseudo_complex[0], &time_slice[0]);

                    for (size_t sl = 0; sl < wl/2; ++sl) {
                        spc[count * pts +sl] = complex<double> (pseudo_complex[sl], -1. * pseudo_complex[wl/2 + sl]);
                    }


                    cerr <<".";


                    count++;
                }
                i += wl;
            } while (i <= select.size());
        }

        cerr << endl << "last window starts at " << last << endl;
        if (count != stacks) {
            cerr << "v_spectra::calc_sp -> runtime mismatch " << endl;
            exit(0);
        }
    }
    else {
        //my_valarray::detrend(input);
        // should be detrended by input
        valarray<double> complete(wl);
        pseudo_complex.resize(wl);

        // zero padding
        complete *= 0.0;
        for (i = 0; i < input.size(); ++i) {
            complete[i] = input[i];
        }
        fft_object->do_fft(&pseudo_complex[0], &complete[0]);

        for (size_t sl = 0; sl < wl/2; ++sl) {
            spc[sl] = complex<double> (pseudo_complex[sl], -1. * pseudo_complex[wl/2 + sl]);
        }
        ++count;
    }

    // cerr << "spc: " << freq[0] << "  " << spc[0] << ", " << freq[1] << "  " << spc[1] << " " << spc[spc.size()-1] << "  done" << endl;
    // the standard spectra
    if (!inverse_trf) {
        if (cal_sensor_exist) {
            cerr << endl << "v_spectra::calc_sp -> apply transfer function sensor" << endl;
            for ( i = 0; i < stacks; i++)
                spc[slice(i*pts, pts, 1)] /= calib_sensor->trfkt();
            if (dump_trf)
                calib_sensor->dump("sensorcal_dump.dat");
        }
        //cerr << "spc: " << freq[0] << "  " << spc[0] << ", " << freq[1] << "  " << spc[1] << " " << spc[spc.size()-1] << "  done" << endl;
        if (cal_system_exist) {
            cerr << endl << "v_spectra::calc_sp -> apply transfer function system" << endl;
            for ( i = 0; i < stacks; i++)
                spc[slice(i*pts, pts, 1)] /= calib_system->trfkt();
            if (dump_trf)
                calib_system->dump("systemcal_dump.dat");
        }
        //cerr << "spc: " << freq[0] << "  " << spc[0] << ", " << freq[1] << "  " << spc[1] << " " << spc[spc.size()-1] << "  done" << endl;
        if (cal_other_exist) {
            cerr << endl << "v_spectra::calc_sp -> apply transfer function other" << endl;
            for ( i = 0; i < stacks; i++)
                spc[slice(i*pts, pts, 1)] /= calib_other->trfkt();
            if (dump_trf)
                calib_other->dump("othercal_dump.dat");
        }
    }
    // only needed for special calculations the inverse transfer function
    else {
        if (cal_sensor_exist) {
            cerr << endl << "v_spectra::calc_sp -> apply INVERSE transfer function sensor" << endl;
            for ( i = 0; i < stacks; i++)
                spc[slice(i*pts, pts, 1)] *= calib_sensor->trfkt();
            if (dump_trf)
                calib_sensor->dump("sensorcal_dump.dat");
        }
        //cerr << "spc: " << freq[0] << "  " << spc[0] << ", " << freq[1] << "  " << spc[1] << " " << spc[spc.size()-1] << "  done" << endl;
        if (cal_system_exist) {
            cerr << endl << "v_spectra::calc_sp -> apply INVERSE transfer function system" << endl;
            for ( i = 0; i < stacks; i++)
                spc[slice(i*pts, pts, 1)] *= calib_system->trfkt();
            if (dump_trf)
                calib_system->dump("systemcal_dump.dat");
        }
        //cerr << "spc: " << freq[0] << "  " << spc[0] << ", " << freq[1] << "  " << spc[1] << " " << spc[spc.size()-1] << "  done" << endl;
        if (cal_other_exist) {
            cerr << endl << "v_spectra::calc_sp -> apply INVERSE transfer function other" << endl;
            for ( i = 0; i < stacks; i++)
                spc[slice(i*pts, pts, 1)] *= calib_other->trfkt();
            if (dump_trf)
                calib_other->dump("othercal_dump.dat");
        }

    }
    // protect this block for div by zero
    if (!freq[0]) {
        tmp_freq = freq[0];
        freq[0] = 1.;
    }
    if (extra_scale == "f") {
        cerr << endl << "v_spectra::calc_sp -> scale (div) by f " << endl;
        for ( i = 0; i < stacks; i++)
            for ( j = 1; j < pts; j++) spc[j + i * pts] /= freq[j];

    }

    if (extra_scale == "sqr(f)") {
        cerr << endl << "v_spectra::calc_sp -> scale (div) by sqr(f) " << endl;
        for ( i = 0; i < stacks; i++)
            for ( j = 1; j < pts; j++) spc[j + i * pts] /= (freq[j] * freq[j]);
    }

    if (extra_scale == "sqrt(f)") {
        cerr << endl << "v_spectra::calc_sp -> scale (div) by sqrt(f) " << endl;
        for ( i = 0; i < stacks; i++)
            for ( j = 1; j < pts; j++) spc[j + i * pts] /= sqrt(freq[j]);
    }

    if (extra_scale == "1/f") {
        cerr << endl << "v_spectra::calc_sp -> scale (div) by 1/f " << endl;
        for ( i = 0; i < stacks; i++)
            for ( j = 1; j < pts; j++) spc[j + i * pts] *= freq[j];

    }

    if (extra_scale == "1/sqr(f)") {
        cerr << endl << "v_spectra::calc_sp -> scale (div) by 1/sqr(f) " << endl;
        for ( i = 0; i < stacks; i++)
            for ( j = 1; j < pts; j++) spc[j + i * pts] *= (freq[j] * freq[j]);
    }

    if (extra_scale == "1/sqrt(f)") {
        cerr << endl << "v_spectra::calc_sp -> scale (div) by 1/sqrt(f) " << endl;
        for ( i = 0; i < stacks; i++)
            for ( j = 1; j < pts; j++) spc[j + i * pts] *= sqrt(freq[j]);
    }
    freq[0] = tmp_freq;

    cerr << endl;

    // apply the correction of the used window function
    spc *= wincal;

    is_spec = 1;

    valarray<complex<double> > xtt(freq.size());
    //    xtt = calib_sensor->trfkt();

    cerr << "sizes: " << spc.size() << "  " << xtt.size() << endl;
    //    cerr <<  "                         "  << xtt[0] << "   " << xtt[1] << endl;
    cerr << "spc ctrl: " << freq[0] << "  " << spc[0] << ", " << freq[1] << "  " << spc[1] << " " << spc[spc.size()-1] << "  done" << endl;

    if (this->fft_object != nullptr) {
        delete this->fft_object;
        this->fft_object = nullptr;
    }
    return 1;
}





/***************************************************************************
 *                                                                         *
 *   calculate  FFT backwards                                              *
 *   the output vector is not part of the spectra object!!!!               *
 ***************************************************************************/


int v_spectra::calc_ts(valarray<double>& output, int scale_f) {

    valarray<double> tmp_ts;
    valarray<complex<double> > spc_slice;
    valarray<complex<double> > ts_spc_slice;
    size_t xsize = this->inget_size;


    if (!is_init) {
        cerr << "v_spectra::calc_ts -> spectra not initialized!" << endl;
        cerr << "calculation of time series from EXISTING spectra" << endl;
        cerr << "use init " << endl;
        exit(0);
    }

    if (ovr) {
        cerr << "v_spectra::calc_ts -> you can not calculate the inverse from overlapping fft's" << endl;
        exit(0);
    }

    // wl -FFT-> pts; pts = 1/2 wl  incl DC0

    if (this->invfft_zeropadding) {
        if ( !((this->inget_size % 2) == 0) ) {
            --xsize;
        }
        output.resize(xsize);
        tmp_ts.resize(wl);
    }
    else {
        spc_slice.resize(pts);
        ts_spc_slice.resize(wl);
        if (output.size() != wl * stacks) {
            output.resize(wl * stacks);
            cerr << "v_spectra::calc_ts -> resizing time series vector " << output.size() << endl;

        }
    }

    unsigned int i, j;

    spc[0] = complex<double>(0,0);

    //   for (i = 0; i < output.size(); i++)
    //       output[i] = 1;



    valarray<double> pseudo_complex(wl);
    if (this->fft_object != nullptr) delete this->fft_object;
    this->fft_object = new ffft::FFTReal<double>(wl);


    if (scale_f == 1) {                         // the format should be that way that we correct by f now
        cerr << "v_spectra::calc_ts -> FFT (back): correcting by frequency" << endl;
        for ( i = 0; i < stacks; i++) {
            for ( j = 1; j < pts; j++)
                spc[j + i * pts] /= freq[j];
        }

    }

    if (scale_f == 2) {                         // the format should be that way that we correct by f square now
        cerr << "v_spectra::calc_ts -> FFT (back): correcting by square(frequency)" << endl;
        for ( i = 0; i < stacks; i++) {
            for ( j = 1; j < pts; j++)
                spc[j + i * pts] /= (freq[j] * freq[j]);
        }

    }


    //out = new double(10000);
    cerr << "v_spectra::calc_ts -> FFT (back): " << endl;


    if (!this->invfft_zeropadding) {
        for ( i = 0; i < stacks; i++) {

            spc_slice = spc[slice(i * pts, pts, 1)];
            //    for (j = 0; j < pts; j++) spc_slice[j] = complex<double>(34.5,2);

            //    cerr << " here ya: " << spc_slice[0] << "  " << spc_slice[1] << "  " << spc_slice[2] << endl;
            spc_slice /= wincal;



            for (size_t sl = 0; sl < pts; ++sl) {
                pseudo_complex[sl] = real(spc_slice[sl]);
                pseudo_complex[wl/2 + sl] = -1 * imag(spc_slice[sl]);
            }

            fft_object->do_ifft (&pseudo_complex[0], &output[i * wl]);



            //    cerr << input[2] << " " << out[2] << "  <a ";



            //    cerr << input[2] << " " << out[2] << "  <i " << endl;
            cerr << ".";


            // hence that the input array will be destroyed be this algorithm!!
            // now let the output directly point into the data area of the valarray
            // and let the in2 array move along the (to be destroyed) input

        }
    }
    else {

        spc /= wincal;
        for (size_t sl = 0; sl < pts; ++sl) {
            pseudo_complex[sl] = real(spc[sl]);
            pseudo_complex[wl/2 + sl] = -1 * imag(spc[sl]);
        }

        fft_object->do_ifft (&pseudo_complex[0], &tmp_ts[0]);
        for (size_t sl = 0; sl < xsize; ++sl) {
            output[sl] = tmp_ts[sl];
        }
    }
    cerr << endl;


    /* alternatively
      for ( i = 0; i < stacks; i++) {

        spc[slice(i * pts, pts, 1)] *= cwincal;  // this operation not implemented in gcc 3.3
    //    spc[i*pts] *= sqrt(2.0);     // reset the DC PART
    //    spc[(i*pts)-1] *= sqrt(2.0); // reset the Nyquist part
        out = &output[i * wl];         // move through output valarray
        input = &spc[i*pts];
        pinv = fftw_plan_dft_c2r_1d(wl, reinterpret_cast<fftw_complex*>(input), out, FFTW_ESTIMATE);
        fftw_execute(pinv);

      }
      is_spec = 0;  // we have manipulated the original
    */



    //if (!this->invfft_zeropadding) {
        output /= double(wl);              // scale backwards
    //}
    //else output /= xsize;
//        if (this->invfft_zeropadding) {
//            output *= 1000.;
//        }
    cerr << "back spc: " << freq[1] << "  " << spc[1] << " " << spc[spc.size()-1]  << endl;
    cerr << "ts: min/max " << output.min() * wl << "  " << output.max() << endl;
    cerr << "  done" << endl << endl;


    /*


      valarray<double> xoutput(wl*stacks);
    //  valarray<complex<double> > xspc(pts*stacks);
      for (i = 0; i < xoutput.size(); i++) xoutput[i] = 1;

      
      valarray<complex<double> > xspc_slice;
      xspc_slice.resize(pts);                   // variable size

      double *xout;
      complex<double> *xinput;
      cerr << "xxv_spectra::calc_ts -> FFT (back): " << endl;
      for ( i = 0; i < stacks; i++) {
        xspc_slice = xspc[slice(i * pts, pts, 1)];
        xout = &xoutput[i * wl];         // move through xoutput valarray
        xinput = &xspc_slice[0];
        cerr << xinput[2] << " " << xout[2] << "  <a ";
        //p = fftw_plan_dft_c2r_1d(wl, reinterpret_cast<fftw_complex*>(xinput), xout, FFTW_PRESERVE_INPUT);
        p = fftw_plan_dft_c2r_1d(wl, xxin, xout, FFTW_PRESERVE_INPUT);
        fftw_execute(p);
        cerr << xinput[2] << " " << xout[2] << "  <i " << endl;
        cerr << "!!!";

      }
      cerr << "ts: max " << xoutput.min() * wl << "  " << xoutput.max() << endl;
      cerr << endl;
      

      */

    if (this->fft_object != nullptr) {
        delete this->fft_object;
        this->fft_object = nullptr;
    }
    return 1;
}





















































/***************************************************************************
 *                                                                         *
 *  calculate all spectra - takes a huge amount of memory                  *
 *                                                                         *
 ***************************************************************************/



int v_spectra::single(const char *type) {

    unsigned int i;

    if (!is_spec) {
        cerr << "v_spectra::single -> basis complex spectra not calculated!" << endl;
        cerr << "use calc_sp" << endl;
        exit(0);
    }

    if (!strcmp(type, "power")) {
        spwr.resize(size);
        for (i = 0; i < size; i++)
            spwr[i] = pow(abs(spc[i]), 2.);

        is_pwr = 1;
    }

    if (!strcmp(type, "ampl")) {
        sampl.resize(size);
        for (i = 0; i < size; i++)
            sampl[i] += abs(spc[i]);
        is_ampl = 1;

    }

    if (!strcmp(type, "conj")) {
        spc_conj.resize(size);
        spc_conj = spc.apply(conj);
        is_conj = 1;
    }

    if (!strcmp(type, "phase")) {
        sphase.resize(size);
        for (i = 0; i < size; i++)
            sphase[i] += arg(spc[i]);
        is_phase = 1;
    }

    return 1;
}



/***************************************************************************
 *                                                                         *
 * calculate stacked spectra                                               *
 *                                                                         *
 ***************************************************************************/




int v_spectra::mean(const char *type) {

    if (!is_spec) {
        cerr << "v_spectra::mean -> basis complex spectra not calculated!" << endl;
        cerr << "use calc_sp" << endl;
        exit(0);
    }


    unsigned int i, j;

    if (!strcmp(type, "power")) {
        smean_pwr.resize(pts);
        for (i = 0; i < stacks; i++) {
            for (j = 0; j < pts; j++)
                smean_pwr[j] += pow(abs(spc[j + i * pts]), 2.);
            //smean_pwr[j] += (abs(spc[j + i * pts]));

        }
        // die summe der quadrate geteilt durch stacks oder ... die summe durch die stacks, dann quadriert
        smean_pwr /= stacks;
        //smean_pwr *= smean_pwr;
        is_mean_pwr = 1;
    }

    if (!strcmp(type, "ampl")) {
        smean_ampl.resize(pts);
        for (i = 0; i < stacks; i++) {
            for (j = 0; j < pts; j++)
                smean_ampl[j] += abs(spc[j + i * pts]);
        }
        smean_ampl /= stacks;
        is_mean_ampl = 1;
    }

    if (!strcmp(type, "complex")) {
        smean_c.resize(pts);
        smean_c_conj.resize(pts);
        for (i = 0; i < stacks; i++)
            smean_c += spc[slice(i *  pts, pts, 1)];

        smean_c /= stacks;

        for (i = 0; i < smean_c.size(); i++)
            smean_c_conj[i] = conj(smean_c[i]);
        is_mean_c = 1;
        is_mean_c_conj = 1;

    }

    if (!strcmp(type, "phase")) {

        smean_phase.resize(pts);
        for (i = 0; i < stacks; i++) {
            for (j = 0; j < pts; j++)
                smean_phase[j] += arg(spc[j + i * pts]);
        }
        smean_phase /= stacks;
        is_mean_phase = 1;
    }

    // try parzening
    if (!strcmp(type, "ampl_prz")) {
        smean_ampl_prz.resize(prz_pts);
        valarray<complex<double> > tmp(stacks * prz_pts);
        for (i = 0; i < stacks; i++) {
            // should create an array of stack parzening complex raw spectra
            bfrtsmp::parzening(&spc[i*pts], &spc[i*pts + pts],
                    &spc[i*pts], &spc[i*pts + pts],
                    &tmp[i*prz_pts], &tmp[i*prz_pts+prz_pts],
                    &freq[0], &freq[f_begin], &freq[f_end],
                    &flist[0], &flist[flist.size()],
                    parzen);
        }

        for (i = 0; i < stacks; i++) {
            for (j = 0; j < prz_pts; j++)
                smean_ampl_prz[j] += sqrt(abs(tmp[j + i * prz_pts]));
        }

        smean_ampl_prz /= stacks;
    }



    return 1;
}

/***************************************************************************
 *                                                                         *
 * calculate median spectra                                               *
 *                                                                         *
 ***************************************************************************/




double v_spectra::sstat(const char *type, const char *stat, double median_rg) {

    if (!is_spec) {
        cerr << "v_spectra::sstat -> basis complex spectra not calculated!" << endl;
        cerr << "use calc_sp" << endl;
        exit(0);
    }


    unsigned int j;
    // this vector will contain ONE frequency - but for all stacks
    // depending on the stat type we will return min, max, median
    valarray<double> one_f(stacks);

    if (!strcmp(type, "power")) {
        if (!is_pwr) {
            cerr << "v_spectra::sstat -> calculating single power spectra ";
            single("power");
            cerr << "  ...done" << endl;
        }
        if (!strcmp(stat, "median")) {

            smedian_pwr.resize(pts);
            for (j = 0; j < pts; j++) {
                one_f = spwr[slice(j, stacks, pts)];
                smedian_pwr[j] = my_valarray::median(one_f);
            }
            is_median_pwr = 1;

            if (1000 * median_rg) {
                smedian_pwr_range.resize(pts);
                for (j = 0; j < pts; j++) {
                    one_f = spwr[slice(j, stacks, pts)];
                    smedian_pwr_range[j] = my_valarray::median_range(one_f, median_rg);
                }
                is_median_pwr_range = 1;
            }
        }
        if (!strcmp(stat, "max")) {
            smax_pwr.resize(pts);
            for (j = 0; j < pts; j++) {
                one_f = spwr[slice(j, stacks, pts)];
                smax_pwr[j] = one_f.max();
            }
            is_max_pwr = 1;
        }
        if (!strcmp(stat, "min")) {
            smin_pwr.resize(pts);
            for (j = 0; j < pts; j++) {
                one_f = spwr[slice(j, stacks, pts)];
                smin_pwr[j] = one_f.min();
            }
            is_min_pwr = 1;
        }



        // wait for new g++
        //smean_pwr += pow(abs(spc[slice( i * pts, pts, 1)]), 2.);


    }

    if (!strcmp(type, "ampl")) {
        if (!is_ampl) {
            cerr << "v_spectra::sstat -> calculating single amplitude spectra ";
            single("ampl");
            cerr << "  ...done" << endl;
        }
        if (!strcmp(stat, "median")) {

            smedian_ampl.resize(pts);
            for (j = 0; j < pts; j++) {
                one_f = sampl[slice(j, stacks, pts)];
                smedian_ampl[j] = my_valarray::median(one_f);
            }
            is_median_ampl = 1;

            if (1000 * median_rg) {
                smedian_ampl_range.resize(pts);
                for (j = 0; j < pts; j++) {
                    one_f = sampl[slice(j, stacks, pts)];
                    smedian_ampl_range[j] = my_valarray::median_range(one_f, median_rg);
                }
                is_median_ampl_range = 1;
            }

        }
        if (!strcmp(stat, "max")) {
            smax_ampl.resize(pts);
            for (j = 0; j < pts; j++) {
                one_f = sampl[slice(j, stacks, pts)];
                smax_ampl[j] = one_f.max();
            }
            is_max_ampl = 1;
        }
        if (!strcmp(stat, "min")) {
            smin_ampl.resize(pts);
            for (j = 0; j < pts; j++) {
                one_f = sampl[slice(j, stacks, pts)];
                smin_ampl[j] = one_f.min();
            }
            is_min_ampl = 1;
        }

    }


    return 1;
}



/***************************************************************************
 *                                                                         *
 *   write spectra to disk                                                 *
 *   ll lower limit; ul upper limit                                        *
 *                                                                         *
 ***************************************************************************/

int v_spectra::write(const char* type, int stacked, int time_col, const unsigned int& ul, const unsigned int& ll,
                     double& add
                     , double& mul, string& name, valarray<double> &flist) {

    ofstream ofs;
    unsigned int i;
    if (!is_spec) {
        cerr << "v_spectra::write -> no spectra have been calculated yet" << endl;
        cerr << "use init and calc_sp" << endl;
        exit(0);
    }

    ofs.open(name.c_str());
    if (!ofs) {
        cerr << "v_spectra::write -> " << type << " can not open " << name << " as output" << endl;
        exit(0);
    }

    // power ********************************************************************
    if (!strcmp(type, "power median range")) {
        if (!is_median_pwr_range) {
            cerr << "v_spectra::write -> no median power range spectra have been calculated yet" << endl;
            cerr << "use sstat(\"power, median\")" <<endl;
            exit(0);
        }

        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smedian_pwr_range[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smedian_pwr_range[i] * mul + add
                    << endl;

    }

    if (!strcmp(type, "power median")) {
        if (!is_median_pwr) {
            cerr << "v_spectra::write -> no median power spectra have been calculated yet" << endl;
            cerr << "use sstat(\"power, median\")" << endl;
            exit(0);
        }

        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smedian_pwr[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smedian_pwr[i] * mul + add
                    << endl;

    } else if (!strcmp(type, "power max")) {
        if (!is_max_pwr) {
            cerr << "v_spectra::write -> no power max spectra have been calculated yet" << endl;
            cerr << "use sstat(\"power, max\")" << endl;
            exit(0);
        }

        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smax_pwr[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smax_pwr[i] * mul + add
                    << endl;
    } else if (!strcmp(type, "power min")) {
        if (!is_min_pwr) {
            cerr << "v_spectra::write -> no power min spectra have been calculated yet" << endl;
            cerr << "use sstat(\"power, min\")" << endl;
            exit(0);
        }
        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smin_pwr[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smin_pwr[i] * mul + add
                    << endl;
    } else if (!strcmp(type, "power") && !stacked) {
        if (!is_pwr) {
            cerr << "v_spectra::write -> no power spectra have been calculated yet" << endl;
            cerr << "use single(\"power\")" << endl;
            exit(0);
        }
        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << spwr[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << spwr[i] * mul + add
                    << endl;
    } else if (!strcmp(type, "power") && stacked) {
        if (!is_mean_pwr) {
            cerr << "v_spectra::write -> no stacked power spectra have been calculated yet" << endl;
            cerr << "use mean(\"power\")"  << endl;
            exit(0);
        }
        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smean_pwr[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smean_pwr[i] * mul + add
                    << endl;
    }

    //********************************************************************

    // amplitude ********************************************************************

    else if (!strcmp(type, "ampl median range")) {
        if (!is_median_ampl_range) {
            cerr << "v_spectra::write -> no median ampl range spectra have been calculated yet" << endl;
            cerr << "use sstat(\"ampl, median, range\")" << endl;
            exit(0);
        }
        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smedian_ampl_range[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smedian_ampl_range[i] * mul + add
                    << endl;

    } else if (!strcmp(type, "ampl median")) {
        if (!is_median_ampl) {
            cerr << "v_spectra::write -> no median ampl spectra have been calculated yet" << endl;
            cerr << "use sstat(\"ampl, median\")" << endl;
            exit(0);
        }
        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smedian_ampl[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smedian_ampl[i] * mul + add
                    << endl;

    } else if (!strcmp(type, "ampl max")) {
        if (!is_max_ampl) {
            cerr << "v_spectra::write -> no ampl max spectra have been calculated yet" << endl;
            cerr << "use sstat(\"ampl, max\")" << endl;
            exit(0);
        }

        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smax_ampl[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smax_ampl[i] * mul + add
                    << endl;
    } else if (!strcmp(type, "ampl min")) {
        if (!is_min_ampl) {
            cerr << "v_spectra::write -> no ampl min spectra have been calculated yet" << endl;
            cerr << "use sstat(\"ampl, min\")" << endl;
            exit(0);
        }
        if (time_col)
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << smin_ampl[i] * mul + add
                     << endl;
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smin_ampl[i] * mul + add
                    << endl;
    }


    else if (!strcmp(type, "ampl")  && !stacked) {
        if (!is_ampl) {
            cerr << "v_spectra::write -> no amplitude spectra have been calculated yet" << endl;
            cerr << "use single(\"ampl\")" << endl;
            exit(0);
        }

        if (time_col) {

            if (flist.size() > 2) {

                for (i = 1 + ll; i < pts - ul; i++)
                    if (find(&flist[0], &flist[flist.size()-1], freq[i]) != &flist[flist.size()])

                        ofs <<  freq[i] << "\x9" << sampl[i] * mul + add
                             << endl;
            }
            else for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << sampl[i] * mul + add
                     << endl;
        }
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << sampl[i] * mul + add
                    << endl;
    } else if (!strcmp(type, "ampl") && stacked) {
        if (!is_mean_ampl) {
            cerr << "v_spectra::write -> no stacked amplitude spectra have been calculated yet" << endl;
            cerr << "use mean(\"ampl\")" << endl;
            exit(0);
        }

        if (time_col) {

            if (flist.size() > 2) {



                for (i = 1 + ll; i < pts - ul; i++)
                    if (  find(&flist[0], &flist[flist.size()-1], freq[i]) != &flist[flist.size()-1]) {
                        //cerr << freq[i] << "< >";
                        //cerr << *find(&flist[0], &flist[flist.size()-1], freq[i]);
                        //cerr << endl;
                        ofs <<  freq[i] << "\x9" << smean_ampl[i] * mul + add
                             << endl;
                    }
            } else
                for (i = 1 + ll; i < pts - ul; i++)
                    ofs <<  freq[i] << "\x9" << smean_ampl[i] * mul + add
                         << endl;
        }
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smean_ampl[i] * mul + add
                    << endl;
    } else if (!strcmp(type, "ampl parzen") && stacked) {
        if (!is_mean_ampl) {
            cerr << "v_spectra::write -> no stacked amplitude (parzen) spectra have been calculated yet" << endl;
            cerr << "use mean(\"ampl\")" << endl;
            exit(0);
        }
        if (time_col)
            for (i = 0; i < prz_pts; i++)
                ofs <<  flist[i] << "\x9" << smean_ampl_prz[i] * mul + add
                     << endl;
        else
            for (i = 0; i < prz_pts; i++)
                ofs << smean_ampl_prz[i] * mul + add
                    << endl;
    }




    //***********************************phase********************

    else if (!strcmp(type, "phase") && stacked) {
        if (!is_mean_phase) {
            cerr << "v_spectra::write -> no stacked phase spectra have been calculated yet" << endl;
            cerr << "use mean(\"phase\")" << endl;
            exit(0);
        }

        if (time_col) {

            if (flist.size() > 2) {



                for (i = 1 + ll; i < pts - ul; i++)
                    if (  find(&flist[0], &flist[flist.size()-1], freq[i]) != &flist[flist.size()-1]) {
                        //cerr << freq[i] << "< >";
                        //cerr << *find(&flist[0], &flist[flist.size()-1], freq[i]);
                        //cerr << endl;
                        ofs <<  freq[i] << "\x9" << smean_phase[i] * mul + add
                             << endl;
                    }
            } else
                for (i = 1 + ll; i < pts - ul; i++)
                    ofs <<  freq[i] << "\x9" << smean_phase[i] * mul + add
                         << endl;
        }
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smean_phase[i] * mul + add
                    << endl;
    }


    //***********************************cal********************

    else if (!strcmp(type, "cal") && stacked) {
        if (!is_mean_phase || !is_mean_ampl) {
            cerr << "v_spectra::write -> no stacked phase // amplitude spectra have been calculated yet" << endl;
            cerr << "use mean(\"phase\")" << endl;
            exit(0);
        }

        if (time_col) {

            if (flist.size() > 2) {



                for (i = 1 + ll; i < pts - ul; i++)
                    if (  find(&flist[0], &flist[flist.size()-1], freq[i]) != &flist[flist.size()-1]) {
                        //cerr << freq[i] << "< >";
                        //cerr << *find(&flist[0], &flist[flist.size()-1], freq[i]);
                        //cerr << endl;
                        ofs <<  freq[i] << "   " << smean_ampl[i] << "   " << smean_phase[i] * mul + add
                             << endl;
                    }
            } else
                for (i = 1 + ll; i < pts - ul; i++)
                    ofs <<  freq[i] << "\x9" << smean_ampl[i] << "\x9" << smean_phase[i] * mul + add
                         << endl;
        }
        else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs << smean_ampl[i] << "\x9" << smean_phase[i] * mul + add
                    << endl;
    }





    //********************************************************************

    else if (!strcmp(type, "complex") && !stacked) {
        // check for is_spec was done already

        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << real(spc[i]) * mul + add
                     << "\x9" << imag(spc[i]) * mul + add
                     << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  real(spc[i]) * mul + add
                     << "\x9" << imag(spc[i]) * mul + add
                     << endl;
    } else if (!strcmp(type, "complex") && stacked) {
        if (!is_mean_c) {
            cerr << "v_spectra::write -> no stacked complex spectra have been calculated yet" << endl;
            cerr << "use mean(\"complex\")" << endl;
            exit(0);
        }

        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << real(smean_c[i]) * mul + add
                     << "\x9" << imag(smean_c[i]) * mul + add
                     << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  real(smean_c[i]) * mul + add
                     << "\x9" << imag(smean_c[i]) * mul + add
                     << endl;
    } else if (!strcmp(type, "complex2ap") && !stacked) {
        // check for is_spec was done already
        if (mul != 1. || add
                != 0.0) cerr << "complex2ap options mu and add are ignored "  << endl;
        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << abs(spc[i]) << "\x9" << arg(spc[i]) << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  abs(spc[i]) << "\x9" << arg(spc[i])  << endl;
    } else if (!strcmp(type, "complex2ap") && stacked) {
        if (!is_mean_c) {
            cerr << "v_spectra::write -> no stacked complex spectra have been calculated yet" << endl;
            cerr << "use mean(\"complex\")" << endl;
            exit(0);
        }
        if (mul != 1. || add
                != 0.0) cerr << "complex2ap options mu and add are ignored "  << endl;
        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << abs(smean_c[i]) << "\x9" << arg(smean_c[i])  << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  abs(smean_c[i])  << "\x9" << arg(smean_c[i]) << endl;
    }

    else if (!strcmp(type, "complex2apdeg") && !stacked) {
        // check for is_spec was done already

        if (mul != 1. || add
                != 0.0) cerr << "complex2ap options mu and add are ignored "  << endl;
        mul = 180. / M_PI;
        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << abs(spc[i]) << "\x9" << arg(spc[i]) * mul << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  abs(spc[i]) << "\x9" << arg(spc[i]) * mul << endl;
    } else if (!strcmp(type, "complex2apdeg") && stacked) {
        if (!is_mean_c) {
            cerr << "v_spectra::write -> no stacked complex spectra have been calculated yet" << endl;
            cerr << "use mean(\"complex\")" << endl;
            exit(0);
        }
        if (mul != 1. || add
                != 0.0) cerr << "complex2ap options mu and add are ignored "  << endl;
        mul = 180. / M_PI;
        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << abs(smean_c[i]) << "\x9" << arg(smean_c[i]) * mul << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  abs(smean_c[i]) << "\x9" << arg(smean_c[i]) * mul << endl;
    } else if (!strcmp(type, "trfmtx") && !stacked) {
        // check for is_spec was done already
        if (mul != 1. || add
                != 0.0) cerr << "trfmtx options mu and add are ignored "  << endl;
        mul = 180. / M_PI;
        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << abs(spc[i]) << "\x9" << arg(spc[i]) * mul << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  abs(spc[i]) << "\x9" << arg(spc[i]) * mul << endl;
    } else if (!strcmp(type, "trfmtx") && stacked) {
        if (!is_mean_c) {
            cerr << "v_spectra::write -> no stacked complex spectra have been calculated yet" << endl;
            cerr << "use mean(\"complex\")" << endl;
            exit(0);
        }
        if (mul != 1. || add
                != 0.0) cerr << "trfmtx options mu and add are ignored "  << endl;
        mul = 180. / M_PI;
        if (time_col) {
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  freq[i] << "\x9" << (abs(smean_c[i]) / 1000.) / freq[i] << "\x9" << arg(smean_c[i]) * mul << endl;
        } else
            for (i = 1 + ll; i < pts - ul; i++)
                ofs <<  abs(smean_c[i]) << "\x9" << arg(smean_c[i]) * mul << endl;
    }

    ofs.close();

    return 1;
}





/***************************************************************************
 *                                                                         *
 *   there is an existing spectra with a frequency list                    *
 *   at this point we want to select a subset of frequencies (newf)        *
 *   and SHRINK the existing spectra to this subset                        *
 *                                                                         *
 *   newf must contain a subset of freq!!!                                 *
 ***************************************************************************/

int v_spectra::sel_freq(const valarray<double>& newf) {

    size_t i, k, j = 0;
    valarray<complex<double> > tspc(newf.size());
    valarray<double> dspc(newf.size());

    size = newf.size() * stacks;
    size_t ptstmp = newf.size();
    valarray<complex<double> > ftspc(size);
    valarray<double> fdspc(size);

    // test

    for (i = 0; i < pts; i++) {
        if (freq[i] == newf[j])
            j++;
    }

    j= 0;

    if (is_spec) {
        for (k = 0; k < stacks; k++) {
            for (i = 0; i < pts; i++) {
                if (freq[i] == newf[j]) {
                    ftspc[j + k * ptstmp] = spc[i+ k * pts];
                    j++;
                }

            }
            j = 0;
            i = 0;

        }
        spc.resize(size);
        spc = ftspc;
        j= 0;
    }

    if (is_conj) {
        for (k = 0; k < stacks; k++) {
            for (i = 0; i < pts; i++) {
                if (freq[i] == newf[j]) {
                    ftspc[j + k * ptstmp] = spc_conj[i+ k * pts];
                    j++;
                }

            }

        }
        spc_conj.resize(size);
        spc_conj = ftspc;
        j= 0;
    }


    if (is_pwr) {
        for (k = 0; k < stacks; k++) {
            for (i = 0; i < pts; i++) {
                if (freq[i] == newf[j]) {
                    fdspc[j + k * ptstmp] = spwr[i+ k * pts];
                    j++;
                }

            }
        }
        spwr.resize(size);
        spwr = fdspc;
        j= 0;
    }

    if (is_ampl) {
        for (k = 0; k < stacks; k++) {
            for (i = 0; i < pts; i++) {
                if (freq[i] == newf[j]) {
                    fdspc[j + k * ptstmp] = sampl[i+ k * pts];
                    j++;
                }

            }
        }
        sampl.resize(size);
        sampl = fdspc;
        j= 0;
    }

    if (is_phase) {
        for (k = 0; k < stacks; k++) {
            for (i = 0; i < pts; i++) {
                if (freq[i] == newf[j]) {
                    fdspc[j + k * ptstmp] = sphase[i+ k * pts];
                    j++;
                }

            }
        }
        sphase.resize(size);
        sphase = fdspc;
        j= 0;
    }




    if (is_mean_pwr) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smean_pwr[i];
        }
        smean_pwr.resize(newf.size());
        smean_pwr = dspc;
        j = 0;
    }

    if (is_mean_ampl) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smean_ampl[i];
        }
        smean_ampl.resize(newf.size());
        smean_ampl = dspc;
        j = 0;
    }

    if (is_mean_phase) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smean_phase[i];
        }
        smean_phase.resize(newf.size());
        smean_phase = dspc;
        j = 0;
    }

    if (is_median_pwr) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smedian_pwr[i];
        }
        smedian_pwr.resize(newf.size());
        smedian_pwr = dspc;
        j = 0;
    }

    if (is_median_ampl) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smedian_ampl[i];
        }
        smedian_ampl.resize(newf.size());
        smedian_ampl = dspc;
        j = 0;
    }

    if (is_median_pwr_range) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smedian_pwr_range[i];
        }
        smedian_pwr_range.resize(newf.size());
        smedian_pwr_range = dspc;
        j = 0;
    }

    if (is_median_ampl_range) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smedian_ampl_range[i];
        }
        smedian_ampl_range.resize(newf.size());
        smedian_ampl_range = dspc;
        j = 0;
    }

    if (is_max_pwr) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smax_pwr[i];
        }
        smax_pwr.resize(newf.size());
        smax_pwr = dspc;
        j = 0;
    }

    if (is_min_pwr) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smin_pwr[i];
        }
        smin_pwr.resize(newf.size());
        smin_pwr = dspc;
        j = 0;
    }

    if (is_max_ampl) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smax_ampl[i];
        }
        smax_ampl.resize(newf.size());
        smax_ampl = dspc;
        j = 0;
    }

    if (is_min_ampl) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                dspc[j++] = smin_ampl[i];
        }
        smin_ampl.resize(newf.size());
        smin_ampl = dspc;
        j = 0;
    }

    if (is_mean_c) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                tspc[j++] = smean_c[i];
        }
        smean_c.resize(newf.size());
        smean_c = tspc;
        j = 0;
    }

    if (is_median_c) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                tspc[j++] = smedian_c[i];
        }
        smedian_c.resize(newf.size());
        smedian_c = tspc;
        j = 0;
    }

    if (is_median_r_c) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                tspc[j++] = smedian_r_c[i];
        }
        smedian_r_c.resize(newf.size());
        smedian_r_c = tspc;
        j = 0;
    }

    if (is_mean_c_conj) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                tspc[j++] = smean_c_conj[i];
        }
        smean_c_conj.resize(newf.size());
        smean_c_conj = tspc;
        j = 0;
    }

    if (is_median_c_conj) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                tspc[j++] = smedian_c_conj[i];
        }
        smedian_c_conj.resize(newf.size());
        smedian_c_conj = tspc;
        j = 0;
    }

    if (has_trf) {
        for (i = 0; i < pts; i++) {
            if (freq[i] == newf[j])
                tspc[j++] = trf[i];
        }
        trf.resize(newf.size());
        trf = tspc;
        j = 0;
    }


    pts =  newf.size();
    freq.resize(newf.size());
    freq = newf;


    cerr << "v_spectra::sel_freq -> " << freq.size() << " frequencies selected " << freq[0] << " - " << freq[pts-1] << endl;

    return freq.size();
}




size_t v_spectra::read_calib_sensor(string readthis, string chopper, const double& samplefreq, const double& gain, const double& factor, const double& offset, string adc, string tags) {
    size_t n, i;

    if (!is_init) {
        cerr << "v_spectra::read_calib_sensor YOU mus first init spectra!!!" << endl;
        exit(0);
    }

    if (!calib_sensor) {

        cerr << "v_spectra::read_calib_sensor no calibration function / class loaded; return" << endl;
        cal_sensor_exist = 0;
        return 0;
    }
    valarray<double> f(wl/2 + 1);
    for ( i = 0; i < f.size(); i++)
        f[i] = i * sample_freq / wl;

    if (readthis.size()) {
        calib_sensor->set_filename(readthis);
        n = calib_sensor->read(f, "auto", sample_freq);
        if (n) {

            //  cerr << "v_spectra::read_calib_sensor->read " << n << " frequencies from " << readthis <<  endl;
            // if we are successful we interpolate for later usage with FFT
            n = calib_sensor->interpolate_extend_theoretical(f, chopper, sample_freq, gain, factor, offset, adc, tags);
#ifdef TRFCONTROL

            valarray<double> a(n), p(n);
            my_valarray::cplx2ap(calib_sensor->trfkt(), a, p, 1);
            my_valarray::vwrite(calib_sensor->freq(), a, p, "calisen.dat", 1, 0);
#endif

        }
        if (n)
            cal_sensor_exist = 1;
    } else {
        cerr << "v_spectra::read_calib_sensor->generate theoretical function for sensor" << endl;
        n = calib_sensor->theoretical(f, "", sample_freq, gain, 0, 0, "", "");
        if (n)
            cal_sensor_exist = 1;
    }


    return cal_sensor_exist;

}

size_t v_spectra::theoretical(string readthis, string chopper, const double& samplefreq, const double& gain, const double& factor, const double& offset, string adc, string tags) {

    size_t n, i;

    if (!is_init) {
        cerr << "v_spectra::read_calib_sensor YOU mus first init spectra!!!" << endl;
        exit(0);
    }

    if (!calib_sensor) {

        cerr << "v_spectra::read_calib_sensor no calibration function / class loaded; return" << endl;
        cal_sensor_exist = 0;
        return 0;
    }
    valarray<double> f(wl/2 + 1);
    for ( i = 0; i < f.size(); i++)
        f[i] = i * sample_freq / wl;


    cerr << "v_spectra::theoretical->generate theoretical function for sensor" << endl;
    n = calib_sensor->theoretical(f, "", sample_freq, gain, 0, 0, "", "");
    if (n)
        cal_sensor_exist = 1;
    else
        cerr << "v_spectra::theoretical->could not generate theoretical transfer functions" << endl;


    return cal_sensor_exist;

}




size_t v_spectra::read_calib_system(string readthis, string chopper, const double& samplefreq, const double& gain, const double& factor, const double& offset, string adc, string tags) {

    size_t n, i;


    if (!is_init) {
        cerr << "v_spectra::read_calib_system YOU must first init spectra!!!" << endl;
        exit(0);
    }
    if (!calib_system) {
        cerr << "v_spectra::read_calib_system no calibration function / class loaded; return" << endl;
        cal_system_exist = 0;
        return 0;
    } else cerr << "v_spectra::calib_system->" << calib_system->my_name() << endl;
    // generate my own frequency list
    valarray<double> f(wl/2 + 1);
    for ( i = 0; i < f.size(); i++)
        f[i] = i * sample_freq / wl;

    if (readthis.size()) {
        calib_system->set_filename(readthis);
        n = calib_system->read(f, "auto", sample_freq);
        if (n) {

            //cerr << "v_spectra::read_calib_system->read " << n << " frequencies from " << readthis <<  endl;
            // if we are successful we interpolate for later usage with FFT

            n = calib_system->interpolate(f);

#ifdef TRFCONTROL

            valarray<double> a(n), p(n);
            my_valarray::cplx2ap(calib_system->trfkt(), a, p, 1);
            my_valarray::vwrite(calib_system->freq(), a, p, "calisys.dat", 1, 0);
#endif

        }

        cal_system_exist = 1;
    } else {
        //cerr << "v_spectra::read_calib_system->read theoretical function for system" << endl;
        n = calib_system->theoretical(f, "", sample_freq, gain, 0, 0, "", "");
        if (n) cal_system_exist = 1;
        else {
            cerr << "v_spectra::theoretical-> no calibration function / class loaded(needed); return" << endl;
            cal_system_exist = 0;
        }
    }
    return cal_system_exist;

}



/*!
    \fn v_spectra::set_flist_parzen(valarray<double>& inflist, const double parzen)
 */

size_t v_spectra::set_flist_parzen(valarray<double>& inflist, const double parzen) {

    if (inflist[1] > inflist[2])
        reverse(&inflist[0], &inflist[inflist.size()]);

    this->parzen = parzen;
    // estimate start stop and size
    prz_pts = my_valarray::parzen_length(freq, inflist, parzen, prz_start, prz_stop);

    // all what we need
    flist.resize(prz_pts);
    copy(&inflist[prz_start], &inflist[prz_stop], &flist[0]);

    my_valarray::possible_parzening(freq, parzen, f_begin, f_end);
    cerr << "v_spectra::set_flist_parzen-> got a frequency list " << flist[0] << "  " << flist[flist.size()-1] << "  " << flist.size() << "pts" << endl;

    return flist.size();
}

void v_spectra::fwd(const valarray<double> &in, valarray<complex<double> > &out, const bool show)
{
    size_t i,k;
    size_t m = in.size();
    double arg;
    double cosarg, sinarg;
    valarray<complex<double> > in2(m);
    out.resize(m/2);

    for (i = 0; i < m; i++) {

        in2[i] = complex<double> (0.0,0.0);
        arg = -2.0 * M_PI * (double)i / (double)m;
        for (k = 0;k < m; k++) {
            cosarg = cos(k * arg);
            sinarg = sin(k * arg);
            in2[i] += complex<double> (in[k] * cosarg, in[k] * sinarg);

        }
    }

    /* Copy the data back FORWARD DFT */
    for (i = 0; i < m/2; i++) {
        out[i] = in2[i];
    }
    this->nth_element = in2[m/2];

}

void v_spectra::inverse(const valarray<complex<double> > &in, valarray<double> &out, const bool show)
{
    size_t i,k;
    size_t m = in.size() * 2;
    size_t n = m/2;
    double arg;
    complex<double> argc;
    valarray<complex<double> > in2(m);
    valarray<complex<double> > inorig(m);
    out.resize(m);

    // create full spectrum from half spectrum
    for (i = 0; i < n; i++) {
        inorig[i] = in[i];
    }
    inorig[n] = this->nth_element;
    for (i = 1; i < n; i++) {
        inorig[n + i] = conj(in[n-i]);
    }


    for (i = 0; i < m; i++) {

        in2[i] = complex<double> (0.0,0.0);
        arg = 2.0 * M_PI * (double)i / (double)m;
        for (k = 0;k < m; k++) {
            argc = complex<double> (cos(k * arg), sin(k * arg));
            in2[i] +=inorig[k] * argc;

        }
    }

    for (i = 0; i < m; i++) {
        out[i] = real(in2[i]);
    }

}


// Cooley–Tukey FFT (in-place)
void v_spectra::fft(std::valarray<complex<double> >& x)
{
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    std::valarray<complex<double> > even = x[std::slice(0, N/2, 2)];
    std::valarray<complex<double> >  odd = x[std::slice(1, N/2, 2)];

    // conquer
    v_spectra::fft(even);
    v_spectra::fft(odd);

    // combine
    for (size_t k = 0; k < N/2; ++k)
    {
        std::complex<double> t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}


// inverse fft (in-place)
void v_spectra::ifft(std::valarray<complex<double> >& x, bool scale)
{
    // conjugate the complex numbers
    x = x.apply(std::conj);

    // forward fft
    v_spectra::fft( x );

    // conjugate the complex numbers again
    x = x.apply(std::conj);

    // scale the numbers
    if (scale) x /= x.size();
}

bool v_spectra::is_pow2(const size_t wl)
{


    assert (wl > 0);
    return  ((wl & -wl) == wl);


}

size_t v_spectra::next_power_of_two(const size_t n)
{
    size_t target, m;
    if (n > (SIZE_MAX - 1) / 2)
        throw "Vector too large";
    target = n;
    for (m = 1; m < target; m *= 2) {
        if (SIZE_MAX / 2 < m)
            throw "Vector too large";
    }

    return m;

}
