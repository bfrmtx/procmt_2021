/***************************************************************************
 *   Copyright (C) 2003 by Bernhard Friedrichs                             *
 *   bfriedr@isi2.metronix.de                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef COHERENCY_H
#define COHERENCY_H

#include <complex>
#include "allinclude.h"
#include <valarray>
#include "fftReal/FFTReal.h"
#include "my_valarray.h"
#include "string_utils.h"
#include <cmath>
#include "icalibration.h"
#include "spectra.h"

using namespace std;
/**
@author Bernhard Friedrichs
*/
class coherency {

public:
    valarray<complex<double> > tempv; //spcx, spcy;        // spectra complete complex
    valarray<double> freq, sqrt_coh, sqrt_coh_med, sqrt_coh_med_range, sqrt_coh_b;
    valarray<double> squad, scosp, smean_quad, smean_cosp,
    smed_quad, smed_cosp, smed_quad_range, smed_cosp_range;
    valarray<complex<double> >   cross, smean_cross, smed_cross, smed_cross_range;
    coherency();
    ~coherency();

    int init(v_spectra &v);
    int quad_co(int stack_it,  int imedian, double range, const v_spectra &x, const v_spectra &y);
    int coh(int stack_it,  int imedian, double range, const v_spectra &x, const v_spectra &y);
    // ul upper limit, ll lower limit
    int write(const char* type, int time_col, const unsigned int& ul, const unsigned int& ll, string& name);

    string my_name, inputfilename;      // who I am and what was my input


private:
    unsigned int pts;         // length of the FFT; is halft of window length of the fft algorithm
    unsigned int stacks;      // total amount of spectra, each of length pts
    unsigned int count;       // counter for 0 .. stacks
    unsigned int wl;          // window length of fft algoritm = 2* pts of resulting spectra
    unsigned int no_dc;      // avoid output of DC component into files
    // because often log-log display is used and DC has f = 0!
    double sample_freq;       // sampling frequency of the signal
    double ssc;              // spectral correction factor
    int is_init, is_coh, is_coh_med, is_coh_med_range,
    is_quad_co, is_quad_co_mean, is_quad_co_median, is_quad_co_median_range,
    is_cross, is_cross_mean, is_cross_median, is_cross_median_range;
    double wincal;           // cal function for the window
    
    unsigned int size;        // size of total spectra
};


#endif
