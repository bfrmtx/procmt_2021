/***************************************************************************
                          my_valarray.h  -  description
                             -------------------
    begin                : Thu Nov 9 2000
    copyright            : (C) 2000 by
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/
/*! \file my_valarray.h
    \brief contains a list of function templates for valarrays.
    \author B. Friedrichs
    \date Nov 9 2000
   

*/

#ifndef MY_VALARRAY_H
 #define MY_VALARRAY_H

 #include <valarray>
 #include <vector>
 #include <string>
 #include <iostream>
 #include <fstream>
 #include <complex>
 #include <typeinfo>
#include "allinclude.h"
#include "string_utils.h"

using namespace std;


// nachprfen ob die initialisierten valarrays explizit am ende
// der funktionen gelöscht werden mssen
/*!
\brief namespace my_valarray defines template functions for valarrays
*/
namespace my_valarray {

template <class T>
int detrend (valarray<T>& v) {

    size_t i;
    T tt, slope = 0, sumtx = 0;
    T t_factor = 2.0/(v.size()-1);
    T mean = v.sum()/ v.size();

    for (i = 0; i < v.size(); i++) {

        tt = i * t_factor - 1.;
        slope += tt * tt;
        sumtx += tt * v[i];
    }
    slope = sumtx / slope;

    for (i = 0; i < v.size(); i++) {
        tt = i * t_factor - 1.;
        v[i] -= (mean + tt * slope);                  // detrend
    }

    return 1;
}


/***************************************************************************
*Cross Correlation Function of valarrays x and y                          *
*                                                                         *
* output is placed into cffx                                              *
*                                                                         *
* startx and starty are the points where CCF has to start
* wl is window length of comparsion, sample_freq
  shifts is the amount of samples used for CCF analysis
* shift = 0 results in a complete comparsion of to time series
* ccfxy will be normalized with ccf[0]                                    *
***************************************************************************/
template <class T>
size_t CCF(valarray<T>& ccfxy, const valarray<T>& x, const valarray<T>& y,
           const size_t startx, const size_t starty,  const size_t wl, const double sample_freq, size_t shifts) {

    valarray<T> ccf_tmpx(wl), ccf_tmpy(wl);
    size_t i, n = x.size();

    if (!shifts)
        shifts = y.size() - wl - starty;             // take 0 shifts as full
    if ((y.size() - wl - starty) < shifts) {
        cerr << "CCF -> adjustings shifts from " <<  shifts << "to ";
        shifts = y.size() - wl - starty;
        cerr << shifts << endl;

    }
    ccfxy.resize(shifts);
    ccf_tmpx = x[slice(startx, wl, 1)];
    detrend(ccf_tmpx);
    //  cerr << ccf_tmpx.sum() << endl;
    //  ccf_tmpx *= ccf_tmpx;
    //  cerr << ccf_tmpx.sum() << endl;

    cerr << "CCF -> getting valarrays of " << x.size() << " & " << y.size() << ", offsets: " << startx << " & " << starty;
    cerr << " shifts: " << shifts << " and window: " << wl << endl;

    for (i = 0; i < shifts; i++ ) {

        ccf_tmpy = y[slice(starty + i, wl, 1)];
        detrend(ccf_tmpy);
        ccf_tmpy *= ccf_tmpx;
        ccfxy[i] = ccf_tmpy.sum();
    }

    //  cerr << endl;
    ccfxy /= abs(T(ccfxy[0]));
    T *mae, *mie;
    mae = max_element(&ccfxy[0], &ccfxy[shifts-1]);
    mie = min_element(&ccfxy[0], &ccfxy[shifts-1]);

    cerr << "CCF -> done, max: " << *mae << " at point " << mae - &ccfxy[0] << " (" << (mae - &ccfxy[0])/sample_freq << "s, ";
    cerr << ((mae - &ccfxy[0])/sample_freq)/60 << "m, " << ((mae - &ccfxy[0])/sample_freq)/3600 << "h)" << endl;


    cerr << "CCF -> done, min: " << *mie << " at point " << mie - &ccfxy[0] << " (" << (mie - &ccfxy[0])/sample_freq << "s, ";
    cerr << ((mie - &ccfxy[0])/sample_freq)/60 << "m, " << ((mie - &ccfxy[0])/sample_freq)/3600 << "h)" << endl;


    return n;
}


/***************************************************************************
 *   scans a data vector;                                                  *
 *   if a following value are differ more than                             *
 *   spike_min_height from the mean of the left shoulder,                  *
 *   this point will be marked as a spike if:                              *
 *   left and right shoulder are equal within the skewness factor          *
 *   (otherwise it might be a step and not a spike)                        *
 *    relaxation is the number of points possibly influenced by the spike  *
 ***************************************************************************/

template <class T>
size_t fnd_spk(const valarray<T>& v, vector<size_t>& res, size_t shoulder_width,
               double spike_min_height, size_t relaxation, double skewness) {

    size_t l, m, k_index;
    valarray<size_t> k(relaxation);
    T suml, sumr, mean, ratio;
    res.resize(0);
    if (skewness < 1) {
      cerr << "fnd_spk -> skewness must be > 1, set to 1.2! " << endl;
      skewness = 1.2;
    }
    if (relaxation < 3) {
      cerr << "fnd_spk -> relaxation must be > 2; set to 3!, try also 8, 12; depending on the sample rate " << endl;
      relaxation = 3;

    }

    valarray<T> sl(shoulder_width), sr(shoulder_width);

    cerr  << "fnd_spk (" << v.size() << " pts) -> shoulder_width: " <<  shoulder_width << ", spike_min_height: "  << spike_min_height
    << ", relaxation: " << relaxation <<  ", skewness: " << skewness << endl;

    // cna not start with 0 because we need left shoulder and relaxation
    for (l = shoulder_width + relaxation ; l < v.size() - shoulder_width - relaxation -2; l++) {

        // take a moving average
        sl = v[slice(l- shoulder_width - relaxation, shoulder_width, 1)];
        suml = sl.sum();
        mean = suml/shoulder_width;

        // find a difference
        if ( abs(mean - v[l+1]) > spike_min_height) {                            // position of spike?   (l+1)

            k_index = 0;
            k *= 0;        // reset k
                                                                                 // now calculate the right shoulder of the spike
            sr = v[slice(l + relaxation, shoulder_width, 1)];
            sumr = sr.sum();
            // compare left shoulder and spike within relaxation length          // redo test at same point (l+1)
            for (m = l+1; m < l + relaxation; m++) {
                if ( abs(mean - v[m]) > spike_min_height)
                    k[k_index++] = m;                                            // find all points inside relaxtion
            }
            //k_index = 0;

            // skewness parameter (close to 1 - otherwise you can find a step!)
            ratio = abs(sl.sum()/ sr.sum());
            if (ratio < 1)
                ratio = 1/ratio;

            // so - if we do NOT have a step
            if ( ratio <= skewness ) {
                for (k_index = 0; k_index < relaxation; k_index++) {
                    if (k[k_index])
                        res.push_back(k[k_index]);                               // copy tmp values from k into res
                }                                                                // the result vector
                l += relaxation +1;                                              // move the index behind the spike and its
            }                                                                    // ralaxation area.
        }
    }
    return res.size();                                                           // return number of detected spikes
}


/***************************************************************************
 *  scans a data vector for steps                                          *
 *  that is if a smooth curve jumps with step_min_height and continues.    *
 *  step width is the minimum number of points where the vector should     *
 *  not jump again.                                                        *
 *  The step width is doubled to find really out whether the data is almost*
 *  the same on the left and right side of the step (except from the       *
 *  skweness parameter                                                     *
 *  res contains the indices! where the steps where found in the time series
 ***************************************************************************/


template <class T>
size_t fnd_stp(const valarray<T>& v, vector<size_t>& res, size_t step_width, double step_min_height,
               size_t relaxation, double skewness) {

    size_t l, m, n;
    T actual_step;
    valarray<T> sl(step_width), sr(step_width), sll(step_width), srr(step_width);
    //const string name = "eval.dat";
    cerr  << "fnd_stp (" << v.size() << " pts) -> step_width: " <<  step_width << " step_min_height: "  << step_min_height
    << " relaxation: " << relaxation <<  " skewness: " << skewness << endl;
    res.resize(0);
    for (l = 2 * step_width + relaxation ; l < v.size() - 2 * step_width - relaxation - 2; l++) {
        // find a difference
        if ( abs(v[l] - v[l+1]) > step_min_height) {
            actual_step = abs(v[l] - v[l+1]);
            m = l+1;
            while ( (abs(v[m] - v[m+1]) > step_min_height) && m < v.size() - 2 * step_width - relaxation - 2 )
                m++;
            //      if ( ( m - l) > 1) {
            //        cerr << "fnd_stp -> multistep: " << l << "  " << m << " (" <<  m - l << ")" << endl;
            //      }

            // check for step width on the left
            // copy right and left
            sl = v[slice(l- step_width - relaxation, step_width, 1)];
            sll = v[slice(l- 2*step_width - relaxation, step_width, 1)];
            sr = v[slice(m + relaxation, step_width, 1)];
            srr = v[slice(m + relaxation + step_width, step_width, 1)];
            //      cerr << sl.sum() << "s " ;

            if ( (abs((sl.sum()/step_width - sr.sum()/step_width )) > (skewness * actual_step))
                    && (abs((sll.sum()/step_width - srr.sum()/step_width )) > (skewness * actual_step)) ) {
                for (n = l; n < m ; n++)
                    res.push_back(n);
                l = n;
                //        cerr << "l: " << l << "  ";
            }

        }
    }
    //  cerr << "ready" << endl;
    return res.size();

}
/***************************************************************************
 *                                                                         *
 *   detrend and hanning of e.g. time series                               *
 *   as preparation for a fft                                              *
 *   sources from GMT                                                      *
 *   will not be defined for other than floats                             *
 *                                                                         *
 ***************************************************************************/


template <class T>
double detrend_and_hanning (valarray<T>& v) {

    size_t i;
    double j;
    T tt, slope = 0, sumtx = 0;
    T h_period = M_PI/double(v.size()), h_scale = 2., hc = 0, hw = 0;

    //h_scale = sqrt(8./3.); this was originally used by GMT fft routine

    /* hence that h_scale has to be re-used after using the fft in the sense
      that the result of the ft has to multiplied with
      sqrt(h_scale / (sampling_freq * window_length))

      This routine with h_scale 2.0 actaully returns 1 as integral of the
      hanning window so scaling should not be neccessary

    */


    T t_factor = 2.0/(v.size()-1);
    T mean = v.sum()/ v.size();

    j = 0.;
    for (i = 0; i < v.size(); i++) {

        tt = j * t_factor - 1.;
        slope += tt * tt;
        sumtx += tt * v[i];
        ++j;
    }
    slope = sumtx / slope;

    for (i = 0; i < v.size(); i++) {
        tt = j * t_factor - 1.;
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        v[i] -= (mean + tt * slope);                  // detrend
        v[i] *= hw;                                   // hanning
        ++j;
    }
    return h_scale;
}

/***************************************************************************
 *                                                                         *
 *   simply detrend a vector                                               *
 *                                                                         *
 *   sources from GMT                                                      *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/



/***************************************************************************
 *                                                                         *
 *   Hanning window for e.g. time series                                   *
 *   might not work with other than floating points                        *
 *                                                                         *
 *  source from GMT                                                        *
 *                                                                         *
 ***************************************************************************/

template <class T>
int hanning (valarray<T>& v) {

    size_t i;
    T tt, slope = 0, sumtx = 0;
    T h_period = M_PI/double(v.size()), h_scale = 2., hc = 0, hw = 0;

    //h_scale = sqrt(8./3.); this was originally used by GMT fft routine

    /* hence that h_scale has to be re-used after using the fft in the sense
      that the result of the ft has to multiplied with
      sqrt(h_scale / (sampling_freq * window_length))

      This routine with h_scale 2.0 actaully returns 1 as integral of the
      hanning window so scaling should not be neccessary
    */


    for (i = 0; i < v.size(); i++) {
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        v[i] *= hw;                                   // hanning
    }
    return 1;
}



/***************************************************************************
 *                                                                         *
 *   calculate the median of a valarray                                    *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/


template <class T>
T median(const valarray<T>& v) {

    T med;
    valarray<T> w = v;
    sort(&w[0], &w[w.size()]);
    if (w.size() % 2 == 0)       // array is even
        med = (w[w.size()/2] + w[w.size()/2 - 1])/2.;
    else
        med = w[w.size()/2];  // integer divison should not round up
    return med;
}



template <class T>
T median(const valarray<T>& v, vector<bool>& select) {

    if (select.size() != v.size()) {
        cerr << "template <class T> T median -> valarray and selection vector are not of the same size! " << v.size() << "  " << select.size() << endl;
        return EXIT_FAILURE;
    }

    size_t truth = 0;
    for (size_t i = 0; i < select.size(); i++)
        if (select[i])
            truth++;

    if (truth == select.size()) {
        cerr << "template <class T> T median -> selection vector has NOTHING left over processing! " << endl;
        return EXIT_FAILURE;
    }

    T med;
    valarray<T> w(select.size() - truth);
    cerr << w.size() << endl;
    for (size_t i = 0; i < select.size(); i++)
        if (!select[i])
            w[i] = v[i];

    sort(&w[0], &w[w.size()]);
    if (w.size() % 2 == 0)       // array is even
        med = (w[w.size()/2] + w[w.size()/2 - 1])/2.;
    else
        med = w[w.size()/2];  // integer divison should not round up
    return med;
}



/***************************************************************************
 *                                                                         *
 *   returns the MEAN of values which are pc                               *
 *   off from the median                                                   *
 *   this is something like a smoothed median or a method to stack         *
 *   not all values of a vector but only all values which are in a certain *
 *   range around the median                                               *
 *   example:                                                              *
 *   a spectrum oj 100*1024 points                                         *
 *   every 1024 you have the same frequency                                *
 *   construct a vector of 100 points to get 100 times the same frequency  *
 +   take median and median range to do statistics for that certain frequency
 *   negative values for pc will return the MEAN! this is a patch, sorry    *
 ***************************************************************************/

template <class T>
T median_range(const valarray<T>& v, T pc) {

    if (pc == 0)
        return median(v);
    if (pc == -1)
        return median(v);
    if (pc == -2)
        return v.sum()/v.size();

    int j = 0;
    T med, med_rg = 0;
    T upper, lower;
    med = median(v);
    if (med >= 0 ) {
        upper = med + med * pc;
        lower = med - med * pc;
    } else {
        upper = med - med * pc;
        lower = med + med * pc;
    }
    if ((upper == lower) && (upper == med)) {
        cerr << "median range -> invalid numbers? ";
    }

    //if (med >= 0) {
    for (size_t i = 0; i < v.size(); i++) {

        if ( (v[i] < upper) && (v[i] > lower) ) {
            med_rg += v[i];
            ++j;
        }
    }
    // }
    // mirrored for negative numbers
    /* else {
       for (size_t i = 0; i < v.size(); i++) {

         if ( (v[i] > upper) && (v[i] < lower) ) {
           med_rg += v[i];
           ++j;
         }
       }
     }
    */
    //  cerr << j << " ";
    if (j)
        return med_rg/j;
    else {
        cerr << "medianrange -> failed, simple median returned";
        cerr << " lower median upper " << lower << "  " << med << "  " << upper << endl;
        return med;
    }
}



/***************************************************************************
 *  returns the                                                            *
 *  standard deviation                                                     *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
template <class T>
T stddev(const valarray<T>& x)  {

    valarray<T> sqr = pow(x,2.);
    T sum_of_squares = sqr.sum();
    return sqrt( 1./(x.size()-1) * ( sum_of_squares - pow(x.sum(),2.)/x.size() ) );
}


template <class T>
T stddev_mean(const valarray<T>& x)  {

    valarray<T> sqr = x;
    sqr -= (sqr.sum()/sqr.size());
    T sum_of_squares = sqr.sum();
    return sqrt( 1./(x.size()-1) * ( sum_of_squares - pow(x.sum(),2.)/x.size() ) );
}



template <class T>
T stddev(valarray<T>& sqr, const valarray<T>& x)  {

    sqr = pow(x,2.);
    T sum_of_squares = sqr.sum();
    return sqrt( 1./(x.size()-1) * ( sum_of_squares - pow(x.sum(),2.)/x.size() ) );
}



template <class T> 
void maxmin_select(const valarray<T>& x, size_t length, T max_min, vector<bool>& select) {
 cerr << "template maxmin_select got length, max_min " << length << "  " << max_min << " for " << x.size() << endl;
size_t i =0, j = 0, k = 0, l = 0;
T testmax, testmin;

//vector<bool>::iterator ii = select.begin();
valarray<T> chop(length);
  while(i < x.size()-length) {
//  cerr << i << " " ;  
    chop = x[slice(i, length, 1)];
    if ( (chop.max() - chop.min() ) > max_min) {
      l = k = 0;
      testmax = chop.max() - 0.2 * chop.max();
      testmin = chop.min() + 0.2 * chop.min();
      while (k != length) {
        if ( (chop[k] > testmax) || (chop[k] < testmin) ) ++l;
        ++k;
      }
       
        if (l > length/100) {
          while (j != i + length) {
            select[j] = 1;
            ++j;
          }
          j = i;
       }
    }
    i += length;
    j += length;

  }

}
/***************************************************************************
 * calculate an ARRAY of standard deviations                               *
 * along a vector window by window                                         *
 * wl is the window length used for the stddev calculation                 *
 * ovr is the overlapping of these windows in points                       *
 * pos this vector can be used later to find out where the standard dev.   *
 * is different from the mean/median stddev                                *
 * std these segments of the vector will be different in some sense        *
 ***************************************************************************/
 
template <class T>
size_t stddev_array(const valarray<T>& x, valarray<double>& std, valarray<size_t>& pos,
                    size_t wl, size_t ovr) {

    if (wl <= ovr) {
        cerr << " stddev_array -> overlapping interval must be < window length " << endl;
        return 0;
    }
    size_t nwind = (x.size()-ovr)/(wl-ovr), i, j = 0; // how many windows
    valarray<T> dat(wl);                              // array for the stddev calculation
    std.resize(nwind);
    pos.resize(nwind);
    for (i = 0; i < x.size() - wl + 1; i++) {
        pos[j] = i;
        dat = x[slice(i, wl, 1)];
        std[j++] = stddev(dat);
        --i += (wl - ovr);
    }
    return nwind;
}


template <class T>
size_t stddev_array(const valarray<T>& x, valarray<double>& std, valarray<size_t>& pos,
                    size_t wl, size_t ovr, vector<bool>& select) {

    if (wl <= ovr) {
        cerr << " stddev_array -> overlapping interval must be < window length " << endl;
        return 0;
    }
    valarray<T> dat(wl);                              // where I calc the stdev from
    vector<double> vstd;                              // array for the stddev calculation
    vector<size_t> vpos;                              // keep window position
    size_t i = 0, count = 0;
    vector<bool>::iterator ii = select.begin();       // shoul also worke as size_t declaration

    do {
        ii = search_n(select.begin() + i, select.end(), wl, 0);
        i = (ii - select.begin());
        if ( i <= x.size() - wl) {
            vpos.push_back(i);
            dat = x[slice(count * (wl -ovr), wl, 1)];
            vstd.push_back(stddev(dat));
            count++;
        }
        i += (wl - ovr);
    } while (i <= select.size());

    std.resize(vstd.size());
    pos.resize(vpos.size());

    copy(&vstd[0], &vstd[vstd.size()], &std[0]);
    copy(&vpos[0], &vpos[vpos.size()], &pos[0]);

    return std.size();
}














/*
In the main program a valarray h with the length of pow(2,bit_resolution) is
used for the histogram; use 256 for 8 bit machine, 65536 for 16 bit and 16777216 for 24 bit

*/
template <class T>
int positive_bit_histogram(valarray<T>& h, const valarray<T>& b) {

    size_t i;

    // hence a vector of 24 bit would be 4 GBytes!!

    //h = 0; // zero h

    for (i = 0; i < b.size(); i++)
        h[b[i]] += 1;

    return 1;
}




/***************************************************************************
*                                                                         *
*                                                                         *
*    returns lower and upper index of a frequency list                    *
*    where parzening is possible; if beg and end are zero                 *
*    maxiumum bounds are returned                                         *
     if beg and and are given result will be inside these indices          *
     the new end is the NEXT iterator to the element
     if end is 7 you get ...6 as the last valid
     of in other words your loops will be (i = beg; i < end; i++)
     a later spectra valarray (slice!!) will have the same length as f!!
***************************************************************************/

template<class T>
size_t possible_parzening(const valarray<T>& f, const double& pr, size_t& beg, size_t& end) {

    size_t i, j, has_dc;
    i = j = has_dc = 0;
    T tmp_freq;


    // too small
    if (f.size() < 3) {
      cerr << "my_valarray::possible_parzening -> frequency valarray to small, min is 3, actual size: " << f.size() << endl;
      return 0;

    }

    // we can not change the sort order of f because all functions are written this way
    if ( f[2] < f[1]) {
        cerr << "my_valarray::possible_parzening -> frequencies in wrong sort order!" << endl;
        cerr << "input f is [1], [2]: " << f[1] << ", " << f[2] << " and that is wrong!" << endl;
        return 0;
    }

    // check lower bound
    if (beg > (f.size() - 1) ) {
      cerr << "my_valarray::possible_parzening -> begin exceeds maximum, reset to 0; beg: " << beg << ", f.size():" << f.size() << endl;
      beg = 0;
    }

    // check upper bound
    if (!end) end = f.size() - 1;

    if (end > (f.size() - 1) ) {
      cerr << "my_valarray::possible_parzening -> end exceeds maximum, reset to f.size()-1" << endl;
      end = f.size()-1;
    }

    // exclude DC part from parzening
    if (f[0] == 0.)
        has_dc = 1;      // do we have DC?
    if (beg) beg--;     // adjust for the while loop
		do {
			++beg;
    	tmp_freq = f[beg] - f[beg] * pr; // define the lower bound
    	i = (lower_bound(&f[0], &f[end], tmp_freq) - &f[0]);
//cerr << beg << "  " << i << endl;
		//	if (i < 2 || beg - i < i) i = beg;
		} while ((i < 2 ||( beg - i < 2)));
///cerr << "lower bound " << i << " b: " << f[beg] << "  t:"  << tmp_freq << "  f(i)" << f[i] << "  "  << beg << endl;

    // lower bound old way
    //    i = (find_if(&f[beg], &f[end], bind2nd(greater_equal<double>(), (f[beg] + f[beg] * pr)) - &f[0]);

    if (i == (f.size() - 1)) {
      cerr << "my_valarray::possible_parzening -> warning lower bound reaches maximum frequency -> return 0!" << endl;
      return 0;  // no more upper bound possible return 0 = error
    }
    else beg = i; // make new bound available

    if (beg >= end) {
      cerr << "my_valarray::possible_parzening -> warning lower bound reaches upper bound -> return 0!" << endl;
      return 0;  // no more upper bound possible return 0 = error

    }

    // upper bound; returns also f.size()-1 if nothing found!
    tmp_freq = f[end] - f[end] * pr; // define the upper bound
  //  cerr << "tmpfer: " << tmp_freq << endl;
    i = (upper_bound(&f[0], &f[end], tmp_freq) - &f[0]);

    if (i > beg && i < f.size()) {
      end = i; // make new bound available
      return end - beg;
    }

    else {
      cerr << "my_valarray::possible_parzening -> no valid upper bound - return 0!" << endl;
      return 0;

    }

    return 0;

}


/***************************************************************************
* returns upper and lower bound                                           *
*                                                                         *
* of frequency lists with parzen radius                                   *
*                                                                         *
* no checks and erro handling                                             *
*                                                                         *
***************************************************************************/

template <class T>
size_t return_bounds (const valarray<T>& f, const T& fn, const double& pr, const size_t& proved_low, const size_t& proved_high, size_t& beg, size_t& end) {

    T fu = fn + fn * pr; // upper
    T fl = fn - fn * pr; // lower
    beg = (lower_bound(&f[proved_low], &f[proved_high], fl) - &f[0]);
    end = (upper_bound(&f[proved_low], &f[proved_high], fu) - &f[0]);

    if ( (f[end-1] <= fu) && (f[beg] >= fl) ) return end - beg;
    else return 0;


  }



/***************************************************************************
*                                                                         *
* returns the beg and end index of the given list fn                      *
* end - beg is the length                                                 *
*                                                                         *
* return is [..) in a loop start ... < stop                               *
*                                                                         *
***************************************************************************/


  //returns the size of the valarray

template <class T> size_t parzen_length(const valarray<T>& f, const valarray<T>& fn, const double& pr, size_t& start, size_t& stop) {

    if (!f.size()) {
      cerr << "my_valarray::parzen_length -> frequencies of input spectra has zero length" << endl;
      return 0;
    }

    if (!fn.size()) {
      cerr << "my_valarray::parzen_length -> frequencies of frequency list has zero length" << endl;
      return 0;
    }

    if ( fn[2] < fn[1]) {
        cerr << "my_valarray::parzen_length -> frequencies of input list in wrong sort order!" << endl;
    }

    size_t beg = 0, end = 0;

    // get the range of f from the spectra where we can do parzening at all

    if (!possible_parzening(f, pr, beg, end)) {

        cerr << "my_valarray::parzen_length -> possible_parzening might fail" << endl;
        return 0;

    }

    start = (lower_bound(&fn[0], &fn[fn.size()-1], f[beg]) - &fn[0]);
    stop  = (upper_bound(&fn[0], &fn[fn.size()-1], f[end-1]) - &fn[0]);



    if (stop == fn.size() || start == fn.size()) {
      cerr << "my_valarray::parzen_length -> no freuqncies found inside the list" << endl;
      return 0;
    }


    if (start == stop) {
      cerr << "my_valarray::parzen_length -> frequency list has no frequencies inside possible parzening of spectral frequencies" << endl;
      return 0;
    }

    return stop - start;


}

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/



          // parzening of a e.g. complex vector;
          // f is the frequency vector of this complex vector
          // f must be ascending from f[0] (perhaps DC), f[1] to the highest freq f[n-1]
          // out will be the (e.g. smaller) result
          // fn contains only valid frequencies for parzening (use parzen_length to make a valarray!
template <class C, class T> int parzening(const valarray<C>& in, const valarray<C>& in2, valarray<C>& out,
                                          const valarray<T>& f,  const valarray<T>& fn, const double& pr) {

    if (in.size() != f.size()) {
        cerr << "my_valarray::parzening -> complex vector (input) and frequency vector are not of the same size" << endl;
        cerr << "input: " << in.size() << ", freqs: " << f.size();
        return 0;
    }

    if (out.size() != fn.size()) {
        cerr << "my_valarray::parzening -> complex vector (output) and frequency vector are not of the same size" << endl;
        cerr << "input: " << out.size() << ", freqs: " << fn.size();
        return 0;
    }


    if ( f[2] < f[1]) {
        cerr << "my_valarray::parzening -> frequencies in wrong sort order!" << endl;
        cerr << "input f is [1], [2]: " << f[1] << ", " << f[2] << " and that is wrong!" << endl;
        return 0;
    }


    if ( fn[2] < fn[1]) {
        cerr << "my_valarray::parzening -> frequencies of input list in wrong sort order!" << endl;
        return 0;
    }

    size_t j, k, beg, end, low, high, has_dc;
    double u, us = 0;
    beg = end = has_dc = 0;

    // die grenzen vom input realen f vector nehmen
    // dann die n�hst gr�eren / kleineren vom default f vector zum parzening verwenden..
    if (!possible_parzening(f, pr, beg, end)) {

        cerr << "my_valarray::parzening -> possible_parzening might fail" << endl;
        return 0;

    }




     for (j = 0; j < out.size(); j++) {

      if (return_bounds(f, fn[j], pr, beg, end, low, high)) {
  //      cerr << f[low] << " - " << f[high] << "  target "<< fn[j] << "  ";
        us = 0;
        for (k = low; k < high; k++) {
          u = (abs(fn[j] - f[k]) * M_PI) / (fn[j] * pr);

          //cerr << "test "  << in[k] << "  "  << in2[k] << "  "  << in[k] * in2[k] << "  " << (sin(u)/pow(u, 4.)) << endl;
          if (!u) out[j] += in[k] * in2[k];
          else {
            //u = (sin(u)/pow(u, 4.));
						u = pow((sin(u)/u), 4.);
            out[j] += (in[k] * in2[k]) * u;
          }
          us += u;
        }
        out[j] /= us;
      }

     }






    return 1;
}







/***************************************************************************
 *                                                                         *
 *   read ascii file section                                               *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

template <class T>
size_t read_ukwn_3(ifstream& ifs, valarray<T>& x, valarray<T>& y, valarray<T>& z) {

    vector<T> ix, iy, iz;
    T dx, dy, dz;
    if (!ifs) {
        cerr << "read_uknw_3 -> ifstream is not valid, exit(0)";
        exit(0);
    }
    while(ifs) {
        ifs >> dx >> dy >> dz;              // read temp variables
        // if last operation was successful
        if(ifs) {
            ix.push_back(dx);
            iy.push_back(dy);
            iz.push_back(dz);
        }
    }
    // resize valarrays; all data will be lost!!!
    x.resize(ix.size());
    y.resize(ix.size()), z.resize(ix.size());
    // copy data into valarrays
    copy(&ix[0], &ix[ix.size()], &x[0]);
    copy(&iy[0], &iy[ix.size()], &y[0]);
    copy(&iz[0], &iz[ix.size()], &z[0]);

    ix.resize(0);                          // can not delete????�    iy.resize(0);
    iz.resize(0);
    return x.size();
}



template <class T>
size_t read_ukwn_2(ifstream& ifs, valarray<T> &x, valarray<T> &y) {

    vector<T> ix, iy;
    T dx, dy;

    if (!ifs) {
        cerr << "read_uknw_2 -> ifstream is not valid, exit(0)";
        exit(0);
    }
    while(ifs) {
        ifs >> dx >> dy;              // read temp variables
        // if last operation was successful
        if(ifs) {
            ix.push_back(dx);
            iy.push_back(dy);
        }
    }
    // resize valarrays; all data will be lost!!!
    x.resize(ix.size());
    y.resize(ix.size());
    // copy data into valarrays
    copy(&ix[0], &ix[ix.size()], &x[0]);
    copy(&iy[0], &iy[ix.size()], &y[0]);

    ix.resize(0);                              // can not delete .. why: compilier
    iy.resize(0);
    return x.size();
}


template <class T>
size_t read_ukwn_1(ifstream& ifs, valarray<T>& x) {

    vector<T> ix, iy;
    T dx;
    if (!ifs) {
        cerr << "read_uknw_1 -> ifstream is not valid, exit(0)";
        exit(0);
    }
    while(ifs) {
        ifs >> dx;              // read temp variables
        // if last operation was successful
        if(ifs) {
            ix.push_back(dx);
        }
    }
    // resize valarrays; all data will be lost!!!
    x.resize(ix.size());
    // copy data into valarrays
    copy(&ix[0], &ix[ix.size()], &x[0]);

    ix.resize(0);
    return x.size();
}

// reads NOT in C notation that means first col is 1 !!!!
template <class T>
size_t read_nth_col(ifstream& ifs, unsigned int col, valarray<T>& x) {

    vector<T> ix;
    string rest;
    T dx;
    if (!ifs) {
        cerr << "read_nth_col -> ifstream is not valid, exit(0)";
        exit(0);
    }
    while(ifs) {

        for (unsigned int i = 0; i < col; i++)
            ifs >> dx;  // read temp variables
        // if last operation was successful
        if(ifs) {
            ix.push_back(dx);
            getline(ifs, rest, '\n');
        }

    }
    // resize valarrays; all data will be lost!!!
    x.resize(ix.size());
    // copy data into valarrays
    copy(&ix[0], &ix[ix.size()], &x[0]);

    ix.resize(0);

    //  for (unsigned int i = 0; i < x.size(); i++) cerr << x[i] << endl;
    //  cerr << endl;

    return x.size();
}





/***************************************************************************
 *                                                                         *
 *   write ascii file section                                              *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/


template <class T>
size_t vwrite(const valarray<T>& x, const string& name,
              size_t skip_first, size_t skip_last) {

    unsigned int i;
    ofstream ofs;

    ofs.open(name.c_str());
    if (!ofs) {
        cerr << "vwrite (1) -> can not open " << name << " as output, continue.." << endl;
        ofs.close();
        return 0;
    }

    if (skip_first >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples first: " << skip_first << endl;
        return 0;
    }

    if (skip_last >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples last: " << skip_last << endl;
        return 0;
    }

    if (skip_first >= x.size() - skip_last) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size()  << " skip samples first: " << skip_first
        << " skip samples last: " << skip_last << endl;
        return 0;
    }



    for (i = skip_first; i < x.size() - skip_last; i++)
        ofs <<  x[i] << endl;

    ofs.close();
    return x.size() - skip_first - skip_last;

}
template <class T>

size_t vwrite_first(const valarray<T>& x, const string& name,
              size_t skip_first, size_t skip_behind) {

    unsigned int i;
    ofstream ofs;

    ofs.open(name.c_str());
    if (!ofs) {
        cerr << "vwrite (1) -> can not open " << name << " as output, continue.." << endl;
        ofs.close();
        return 0;
    }

    if (skip_first >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples first: " << skip_first << endl;
        return 0;
    }

    if (skip_behind > x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples last: " << skip_behind << endl;
        return 0;
    }




    for (i = skip_first; i < skip_behind; i++)
        ofs <<  x[i] << endl;

    ofs.close();
    return x.size() - skip_first - skip_behind;

}

template <class T>
size_t vwrite(const valarray<T>& x, const valarray<T>& y, const string& name,
              const size_t skip_first = 0, const size_t skip_last = 0, const size_t suppress_zero_data = 0) {

    unsigned int i;
    ofstream ofs;
    if (x.size() != y.size()) {
        cerr << "x.size() = " << x.size() << "  and y.size() = " << y.size();
        cerr << " crash might follow immediatly; continue..." << endl;
    }
    ofs.open(name.c_str());
    if (!ofs) {
        cerr << "vwrite (2) -> can not open " << name << " as output, continue.." << endl;
        ofs.close();
        return 0;
    }

    if (skip_first >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples first: " << skip_first << endl;
        return 0;
    }

    if (skip_last >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples last: " << skip_last << endl;
        return 0;
    }

    if (skip_first >= x.size() - skip_last) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size()  << " skip samples first: " << skip_first
        << " skip samples last: " << skip_last << endl;
        return 0;
    }

    if (!suppress_zero_data) {
      for (i = skip_first; i < x.size() - skip_last; i++)
        ofs <<  x[i] << "\x9" << y[i] << endl;
    }
    else {
      for (i = skip_first; i < x.size() - skip_last; i++)
        if (y[i] && x[i]) ofs <<  x[i] << "\x9" << y[i] << endl;
    }
    ofs.close();
    return x.size() - skip_first - skip_last;

}

template <class T>
size_t vwrite(const valarray<T>& x, const valarray<T>& y, const valarray<T>& z,
              const string& name, size_t skip_first, size_t skip_last) {

    unsigned int i;
    ofstream ofs;
    if ( (x.size() != y.size()) || (x.size() != z.size()) ) {
        cerr << "x.size() = " << x.size() << "  and y.size() = " << y.size() << "  and z.size() = " << z.size();
        cerr << " crash might follow immediatly; continue..." << endl;
    }
    ofs.open(name.c_str());
    if (!ofs) {
        cerr << "vwrite (3) -> can not open " << name << " as output, continue.." << endl;
        ofs.close();
        return 0;
    }

    if (skip_first >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples first: " << skip_first << endl;
        return 0;
    }

    if (skip_last >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples last: " << skip_last << endl;
        return 0;
    }

    if (skip_first >= x.size() - skip_last) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size()  << " skip samples first: " << skip_first
        << " skip samples last: " << skip_last << endl;
        return 0;
    }

    for (i = skip_first; i < x.size() - skip_last; i++)
        ofs <<  x[i] << "\t" << y[i]  << "\t" << z[i] << endl;

    ofs.close();
    return x.size() - skip_first - skip_last;

}

template <class T>
size_t vwrite(const valarray<T>& x, const valarray<T>& y, const valarray<T>& z, const valarray<T>& u,
              const string& name, size_t skip_first, size_t skip_last) {

    unsigned int i;
    ofstream ofs;
    if ( (x.size() != y.size()) || (x.size() != z.size()) || (x.size() != u.size())) {
        cerr << "x.size() = " << x.size() << "  and y.size() = " << y.size() << "  and z.size() = " << z.size()  << "  and u.size() = " << u.size() ;
        cerr << " crash might follow immediatly; continue..." << endl;
    }
    ofs.open(name.c_str());
    if (!ofs) {
        cerr << "vwrite (4) -> can not open " << name << " as output, continue.." << endl;
        ofs.close();
        return 0;
    }

    if (skip_first >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples first: " << skip_first << endl;
        return 0;
    }

    if (skip_last >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples last: " << skip_last << endl;
        return 0;
    }

    if (skip_first >= x.size() - skip_last) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size()  << " skip samples first: " << skip_first
        << " skip samples last: " << skip_last << endl;
        return 0;
    }

    for (i = skip_first; i < x.size() - skip_last; i++)
        ofs <<  x[i] << "\t" << y[i]  << "\t" << z[i] << "\t" << u[i] << endl;

    ofs.close();
    return x.size() - skip_first - skip_last;

}


template <class T>
size_t vwrite_st_ust(const valarray<T>& x, const valarray<T>& y, const string& name, const int each_stack_as_file = 0, 
              const size_t skip_first = 0, const size_t skip_last = 0) {

    size_t i, j;
    ofstream ofs;
/*		
    if (x.size() != y.size()) {
        cerr << "x.size() = " << x.size() << "  and y.size() = " << y.size();
        cerr << " crash might follow immediatly; continue..." << endl;
    }
*/
		
		if (!each_stack_as_file) ofs.open(name.c_str());
    if (!ofs) {
        cerr << "vwrite (2) -> can not open " << name << " as output, continue.." << endl;
        ofs.close();
        return 0;
    }

    if (skip_first >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples first: " << skip_first << endl;
        return 0;
    }

    if (skip_last >= x.size()) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size() << " skip samples last: " << skip_last << endl;
        return 0;
    }

    if (skip_first >= x.size() - skip_last) {
        cerr << "vwrite -> ERROR: size of vector: " << x.size()  << " skip samples first: " << skip_first
        << " skip samples last: " << skip_last << endl;
        return 0;
    }

    for (j = 0; j < y.size() / x.size(); j++) {
      for (i = skip_first; i < x.size() - skip_last; i++) ofs <<  x[i] << "\x9" << y[i + j * x.size()] << endl;
		}
		
    if (!each_stack_as_file) ofs.close();
		
		else {
			
			for (j = 0; j < y.size() / x.size(); j++) {
		  string t_name = name + num2str(j);
		  ofs.open(t_name.c_str());
			if (!ofs) {
        cerr << "vwrite (2) -> can not open " << name << " as output, continue.." << endl;
        ofs.close();
        return 0;
      }
			
       for (i = skip_first; i < x.size() - skip_last; i++) ofs <<  x[i] << "\x9" << y[i + j * x.size()] << endl;
			ofs.close();
		}
		
			
			
		}
    return x.size() - skip_first - skip_last;

}






/***************************************************************************
 *                                                                         *
 *   utilities for stacking vectors                                        *
 *                                                                         *
 *   e.g. used for spectral processing                                     *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

// returns how many points were used for stacking;

template <class T>
unsigned int stack(valarray<T>& result, const valarray<T>& in,
                   const unsigned int length, const int scale, const int conjugate, size_t& stacks) {

    unsigned int i;
    stacks = in.size()/length;
    if (!result.size()) {
        cerr << "my_valarray::stack -> resize(" << length <<"), stacks " << stacks << endl;
        result.resize(length);
    } else if (result.size() != length) {
        cerr << "my_valarray::stack -> resize(" << length <<"), stacks " << stacks << endl;
        result.resize(length);
    }

    cerr << "my_valarray::stack -> " << result[2] << " " << in[2] << endl;
    for (i = 0; i < stacks; i++) {
        result += in[slice(i * length, length, 1)];
        //cerr << result[2] << " " << in[i*length + 2] << endl;
    }

    /*
    //    else for (i = 0; i < stacks; i++) result += conj(in[slice(i * length, length, 1)]);
        else {
          valarray<T> mc(in.size());
    //      for (i = 0; i < in.size(); i++) mc[i] = conj(in[i]);
          for (i = 0; i < stacks; i++) result += mc[slice(i * length, length, 1)];
          ~mc;
        }
    */

    if (scale)
        result /= stacks;

    return stacks * length;
}


/***************************************************************************
 *                                                                         *
 *   multiply and stack                                                    *
 *   assume x[1024], and y, z [10240]  -> max 10 stacks possible           *
 *   x = y * z or x = conj(y) * z or x = y * conj(z)                       *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

template <class T>
unsigned int stack_mul(valarray<T>& result, const valarray<T>& in, const valarray<T>& in2,
                       const unsigned int length, const int scale, const int conjugate, unsigned int& stacks) {


    unsigned int i;
    stacks = in.size()/length;
    // feherl unter lange
    result.resize(length);
    if (!conjugate)
        for (i = 0; i < stacks; i++)
            result += (in[slice(i * length, length, 1)]
                       * in2[slice(i * length, length, 1)]);

    else {

        if (conjugate == 1) {
            //valarray<T> mc(in.size());
            valarray<T> mc = in.apply(conj);
            for (i = 0; i < stacks; i++)
                result += (in[slice(i * length, length, 1)]
                           * (mc[slice(i * length, length, 1)]));
        } else {
            valarray<T> mc(in2.size());
            for (i = 0; i < stacks; i++)
                result += (in[slice(i * length, length, 1)]
                           * in2[slice(i * length, length, 1)]);
        }

    }


    if (scale)
        result /= scale;

    return stacks * length;

}

/***************************************************************************
 *                                                                         *
 *   utilities for stacking vectors                                        *
 *                                                                         *
 *   here used for so called pre-stacking or sub stacking                  *
 *   e.g. in time domain                                                   *
 *                                                                         *
 ***************************************************************************/

// returns how many points were used for stacking;

template <class T>
unsigned int substack(valarray<T>& result, const valarray<T>& in,
                      const unsigned int length, const int scale,
                      const int conjugate, const unsigned int substacks, size_t& stacks) {

    unsigned int i, j, k = 0;
    valarray<T> tstack(length);

    size_t tsize = (in.size()  / (length *substacks)) * length;
    stacks = tsize / length;

    if (!substacks) {
        cerr << "my_valarray::stack -> substacks must be greater zero" << endl;
        exit(0);
    }

    if (!result.size()) {
        cerr << "my_valarray::stack -> resize(" << tsize << "), stacks " << stacks << endl;
        result.resize(tsize);
    } else if (result.size() != tsize) {
        cerr << "my_valarray::stack -> resize(" << tsize << "), stacks " << stacks << endl;
        result.resize(tsize);
    }

    //  cerr << result[2] << " " << in[2] << endl;
    k = 0;
    for (i = 0; i < stacks; i++) {
        tstack = in[slice(k * length, length, 1)];
        for (j = 0; j < substacks; j++) {
  //          cerr << i << "  " << k << "  " << (k +j) * length << endl;
            tstack += in[slice( (k+j) * length, length, 1)];
        }
 //       cerr << i * length << endl;
        k += j;
        copy(&tstack[0], &tstack[tstack.size()], &result[i*length]);
        //    cerr << result[2] << " " << in[i*length + 2] << endl;
    }


    if (scale)
        result /= substacks;

    return stacks * length;
}

template <class T>
unsigned int substack2(valarray<T>& result, const valarray<T>& in,
                      const unsigned int length, const int scale,
                      const int conjugate, const unsigned int substacks, size_t& stacks) {

    unsigned int i, j, k = 0;
    valarray<T> tstack(length);
    valarray<T> tmpstack(length);

        //copy(&in[0], &in[length], &tstack[0]);


        copy(&in[0], &in[length], &tstack[0]);

        //tstack = in[slice(k * length, length, 1)];
        for (j = 1; j < substacks; j++) {
            //cerr << i << "  " << k << "  " << (k +j) * length << endl;
            //tstack += in[slice( (k+j) * length, length, 1)];
            copy(&in[j*length], &in[(j*length)+length], &tmpstack[0]);
            tstack += tmpstack;
        }

        tstack /= double (substacks);


        cerr << "fi: " << tstack.size() << endl;
        my_valarray::vwrite_first(tstack,"out.dat",0,length);



    size_t tsize = (in.size()  / (length *substacks)) * length;
    stacks = tsize / length;

    if (!substacks) {
        cerr << "my_valarray::stack -> substacks must be greater zero" << endl;
        exit(0);
    }

    if (!result.size()) {
        cerr << "my_valarray::stack -> resize(" << tsize << "), stacks " << stacks << endl;
        result.resize(tsize);
    } else if (result.size() != tsize) {
        cerr << "my_valarray::stack -> resize(" << tsize << "), stacks " << stacks << endl;
        result.resize(tsize);
    }

    //  cerr << result[2] << " " << in[2] << endl;
    k = 0;
    for (i = 0; i < stacks; i++) {
        tstack = in[slice(k * length, length, 1)];
        for (j = 1; j < substacks; j++) {
            //cerr << i << "  " << k << "  " << (k +j) * length << endl;
            tstack += in[slice( (k+j) * length, length, 1)];
        }
 //       cerr << i * length << endl;
        k += j;
        copy(&tstack[0], &tstack[tstack.size()], &result[i*length]);
        //    cerr << result[2] << " " << in[i*length + 2] << endl;
    }


    if (scale)
        result /= substacks;

    return stacks * length;
}



/***************************************************************************
 *  byte swap routine for valarry                                          *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

template <class T>
size_t tbswap (valarray<T>& v) {

    size_t i;
    unsigned char dst[sizeof(T)];

    for (i = 0; i < v.size(); i++) {
        const unsigned char *p = (const unsigned char*) &v[i];
        for (int j = 0; j < (int)sizeof(T); j++) {
            dst[j] = p[sizeof(T)-1-j];
        }
        v[i] = *((T*)dst);
    }
    return v.size();
}



/***************************************************************************
 * 4 basic routines for standard conversions:                              *
 *                                                                         *
 * complex into a real and a double                                        *
 * a real and a double into a complex                                      *
 * complex into amplitude and phase (degrees or radians)                   *
 * amplitude and phase (degrees or radians) into complex                  *
 ***************************************************************************/

// complex valarrays to real and imag
template <class T>
size_t cplx2ri(const valarray<complex<T> >& in,
               valarray<T>& treal, valarray<T>& timag) {

    if (in.size() != treal.size())
        treal.resize(in.size());
    if (in.size() != timag.size())
        timag.resize(in.size());

    for (size_t i = 0; i < in.size(); i++) {
        treal[i] = real(in[i]);
        timag[i] = imag(in[i]);
    }

    return treal.size();
}

// complex valarrays to amplitude and phase
template <class T>
size_t cplx2ap(const valarray<complex<T> >& in,
               valarray<T>& tamp, valarray<T>& tphase, bool deg) {

    if (in.size() != tamp.size())
        tamp.resize(in.size());
    if (in.size() != tphase.size())
        tphase.resize(in.size());


    if (deg) {
        for (size_t i = 0; i < in.size(); i++) {
            tamp[i] = abs(in[i]);
            tphase[i] = arg(in[i]) * 180 / M_PI;
        }
    } else {
        for (size_t i = 0; i < in.size(); i++) {
            tamp[i] = abs(in[i]);
            tphase[i] = arg(in[i]);
        }
    }

    return tamp.size();
}


/*!

 \brief real imaginary to amplitude and phase
*/
template <class T>
size_t ri2ap(const valarray<T>& inreal, const valarray<T>& inimag,
             valarray<T>& outampl, valarray<T>& outphase, bool deg) {

    if (inreal.size() != inimag.size()) {
        cerr << "ri2ap -> input length of real and imaginary must be the same!!" << endl;
        return 0;
    }
    if (inreal.size() != outampl.size())
        outampl.resize(inreal.size());
    if (inreal.size() != outphase.size())
        outphase.resize(inreal.size());

    complex<T> z;

    if (deg) {
        for (size_t i = 0; i < inreal.size(); i++) {
            z = complex<T>(inreal[i], inimag[i]);
            outampl[i] = abs(z);
            outphase[i] = arg(z) * 180. / M_PI;
        }
    } else {
        for (size_t i = 0; i < inreal.size(); i++) {
            z = complex<T>(inreal[i], inimag[i]);
            outampl[i] = abs(z);
            outphase[i] = arg(z);
        }
    }

    return outampl.size();
}

template <class T>
size_t ap2ri (const valarray<T>& inampl, const valarray<T>& inphase,
              valarray<T>& outreal, valarray<T>& outimag, bool deg) {

    if (inampl.size() != inphase.size()) {
        cerr << "ri2ap -> input length of real and imaginary must be the same!!" << endl;
        return 0;
    }
    if (inampl.size() != outreal.size())
        outreal.resize(inampl.size());
    if (inampl.size() != outimag.size())
        outimag.resize(inampl.size());

    complex<T> z;

    if (deg) {
        for (size_t i = 0; i < inampl.size(); i++) {
            z = polar(inampl[i], (inphase[i] /180.) * M_PI);
            outreal[i] = real(z);
            outimag = imag(z);
        }
    } else {
        for (size_t i = 0; i < inampl.size(); i++) {
            z = polar(inampl[i], inphase[i]);
            outreal[i] = real(z);
            outimag = imag(z);
        }
    }

    return outreal.size();
}



/***************************************************************************
*                                                                         *
*                                                                         *
* all the trans_if functions are only to reduce the amount of             *
* code lines in other modules                                             *
*                                                                         *
*                                                                         *
***************************************************************************/


// I use this to transfer valarrays if special flags in classes have been used
template <class T>
int trans_if (const char* action, const int do_it, const valarray<T>& in, valarray<T>& out) {

    if (!do_it)
        return 0;
    if (in.size() != out.size() ) {
        cerr << "my_valarray::trans_if ->input and output vectors have different lengths - return 0 " << endl;
        return 0;
    }


    if (do_it && !in.size() )  {

        cerr << "my_valarray::trans_if -> you want to copy but input vector has zero length " << endl;
        return 0;
    }
    else if (do_it) {
        if (!strcmp(action, "cp") && out.size() != in.size()) {
            if (typeid(in[0]) == typeid(out[0])) {
                out.resize(in.size());
                out = in;
            } else
                cerr << "my_valarray::trans_if cp -> RTTI different valarray types for copy, error, continue" << endl;
        } else if (!out.size()) {

            cerr << "my_valarray::trans_if -> you want to add/sub/mul/div but output vector has zero length " << endl;
            return 0;

        } else if (!strcmp(action, "add"))
            out += in;
        else if (!strcmp(action, "sub"))
            out -= in;
        else if (!strcmp(action, "mul"))
            out *= in;
        else if (!strcmp(action, "div"))
            out /= in;
        else {
            cerr << "my_valarray::trans_if -> operation not specified; use cp/add/sub/mul/div " << endl;
            return 0;

        }
        return 1;
    } else
        return 0;
}


// I use this to transfer valarrays if special flags in classes have been used
template <class T>
int trans_if ( const char* action, const int do_it, const valarray<T>& in, const valarray<T>& in2, valarray<T>& out) {


    if (!do_it)
        return 0;

    if ( (in.size() != out.size()) || (in.size() != in2.size()) ) {
        cerr << "my_valarray::trans_if ->input and output vectors have different lengths - return 0 " << endl;
        return 0;
    }


    if (do_it && !in.size() )  {

        cerr << "my_valarray::trans_if -> you want to copy but input vector has zero length " << endl;
        return 0;
    }
    else if (do_it) {
        if (!out.size()) {

            cerr << "my_valarray::trans_if -> you want to add/sub/mul/div but output vector has zero length " << endl;
            return 0;

        } else if (!strcmp(action, "add"))
            out = in + in2;
        else if (!strcmp(action, "sub"))
            out = in - in2;
        else if (!strcmp(action, "mul"))
            out = in * in2;
        else if (!strcmp(action, "div"))
            out = in / in2;
        else {
            cerr << "my_valarray::trans_if -> operation not specified; use add/sub/mul/div " << endl;
            return 0;

        }
        return 1;
    } else
        return 0;
}



template <class T, class S>
int trans_if ( const char* action, const int do_it, const S& in, valarray<T>& out) {


    if (!do_it)
        return 0;

    if(do_it) {
        if (!out.size()) {

            cerr << "my_valarray::trans_if -> you want to add/sub/mul/div but output vector has zero length " << endl;
            return 0;

        }
        else if (!strcmp(action, "add"))
            out += in;
        else if (!strcmp(action, "sub"))
            out -= in;
        else if (!strcmp(action, "mul"))
            out *= in;
        else if (!strcmp(action, "div"))
            out /= in;
        else {
            cerr << "my_valarray::trans_if -> operation not specified; use cp/add/sub/mul/div " << endl;
            return 0;

        }
        return 1;
    } else
        return 0;
}













/***************************************************************************
*                                                                         *
* generate a list of frequencies starting with a base frequency            *
*                                                                         *
* eo 0 = all harmonics, 1 even harm, 2 = odd harmonics                    *
* base = base frequency                                                   *
*                                                                         *
***************************************************************************/

template <class T>
size_t gen_flist_h (valarray<T>& flist, T base, double low, double high, size_t nharm, int eo) {

    vector<T> list;
    T freq;
    size_t i = 0;

    if (!base) {
        cerr << "my_valarray::size_t gen_flist_h -> you must provide a base frequency! " << endl;
        exit(0);
    }

    freq = base;

    if (low && high)
        cerr << "my_valarray::size_t gen_flist_h -> desired range is: " << low << " - " << high
        << " Hz, base: " << freq << endl;

    if (base < low)
        i =  (unsigned int) (low / base);
    ++i;

    do {
        if (eo == 2) {
            if (!(i % 2))
                i++;
        }
        if (eo == 1) {
            if ((i % 2))
                i++;
        }
        list.push_back(i++ * freq);

    } while (  (i * freq) <= high);

    flist.resize(list.size());
    copy(&list[0], &list[list.size()], &flist[0]);




    cerr << "my_valarray::size_t gen_flist_h -> returning " << flist.size() << " frequencies "
    << flist[0] << ", " << flist[1] << ", .. " << flist[flist.size()-2] << ", " << flist[flist.size()-1] << endl;
    return flist.size();
}



} // end of namespace valarray

#endif
