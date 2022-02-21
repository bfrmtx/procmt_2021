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


#ifndef ITERATOR_TEMPLATES
#define ITERATOR_TEMPLATES

#include <numeric>
#include <valarray>
#include <cmath>
#include <iostream>



namespace iter {

/*!
 * \brief detends a vector or array
 * \param first  = first point
 * \param last   < last data point
 */

template <typename T, typename Iterator> void detrend (Iterator first, const Iterator last) {
// old procmt detrend - adopted by Martin
   // declaration of variables
    T dBias       = 0.0;
    T dTrend      = 0.0;
    T dTrendPart1 = 0.0;
    T dTrendPart2 = 0.0;
    T dWindowLen  = 0.0;

    T dCounter;

    // determine the window length
    dWindowLen = (last - first);

    // determine trend
    Iterator iterMidth = first + ((size_t) (dWindowLen / 2));
    dTrendPart1        = accumulate (iterMidth, last,  0.0);
    dTrendPart2        = accumulate (first, iterMidth, 0.0);
    dTrend             = (4.0 / (dWindowLen * dWindowLen)) * (dTrendPart1 - dTrendPart2);

    // detrend
    dCounter = 0.0;
    Iterator iterBegin = first;
    Iterator iterEnd   = last;
    while (iterBegin != iterEnd)
    {
        (*iterBegin) -= (dCounter * dTrend);
        ++dCounter;
        ++iterBegin;
    }

    // determine Bias value
    dBias  = accumulate (first, last, 0.0);
    dBias /= dWindowLen;

    // remove bias
    iterBegin = first;
    iterEnd   = last;
    while (iterBegin != iterEnd)
    {
        (*iterBegin) -= dBias;
        ++iterBegin;
    }

}

/*!
 * \brief detends and hanning a vector or array in one step
 * \param first  = first point
 * \param last   < last data point
 */
template <typename T, typename Iterator> void detrend_and_hanning (Iterator first, const Iterator last) {
// old procmt detrend - adopted by Martin
   // declaration of variables
    T dBias       = 0.0;
    T dTrend      = 0.0;
    T dTrendPart1 = 0.0;
    T dTrendPart2 = 0.0;
    T dWindowLen  = 0.0;

    T dCounter;

    // determine the window length
    dWindowLen = (last - first);

    // determine trend
    Iterator iterMidth = first + ((size_t) (dWindowLen / 2));
    dTrendPart1        = accumulate (iterMidth, last,  0.0);
    dTrendPart2        = accumulate (first, iterMidth, 0.0);
    dTrend             = (4.0 / (dWindowLen * dWindowLen)) * (dTrendPart1 - dTrendPart2);

    // detrend
    dCounter = 0.0;
    Iterator iterBegin = first;
    Iterator iterEnd   = last;
    while (iterBegin != iterEnd)
    {
        (*iterBegin) -= (dCounter * dTrend);
        ++dCounter;
        ++iterBegin;
    }

    // determine Bias value
    dBias  = accumulate (first, last, 0.0);
    dBias /= dWindowLen;

    // remove bias
    iterBegin = first;
    iterEnd   = last;
    while (iterBegin != iterEnd)
    {
        (*iterBegin) -= dBias;
        ++iterBegin;
    }

    T i = T(0);

    T h_period = M_PI/ ((T)(last - first)), hc = (T)0., hw = (T)0.;

    T h_scale = (T)2.;
    while (first != last) {
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        *first *= hw;                                   // hanning
        ++i;
        ++first;
    }

}


/*!
 * \brief removes  a trend from data; should be used before using hanning and FFT; old version
 * \param first  = first point
 * \param last   < last data point
 */
template <typename T, typename Iterator> void simple_detrend (Iterator first, const Iterator last) {

    Iterator start = first;
    size_t i = 0;
    T tt = 0, slope = 0, sumtx = 0;
    T t_factor = 2.0/(last -first -1);
    T mean = accumulate(first, last, 0.)/ (T)(last -first);

    while (first != last) {

        tt = i * t_factor - 1.;
        slope += tt * tt;
        sumtx += tt * *first;
        ++first;
        ++i;
    }

    slope = sumtx / slope;

    first = start;
    i=0;
    while (first != last) {
        tt = i * t_factor - 1.;
        (*first) -= (mean + tt * slope);                  // detrend
        ++first;
        ++i;
    }
}

/*!
 \brief applies a von Hann (Hanning) window onto a time series<br>
 * This routine with h_scale 2.0 actaully returns 1 as integral of the
 * hanning window so scaling should not be neccessary<br>
 * hence that h_scale has to be re-used after using the fft in the sense
 * that the result of the ft has to multiplied with
 * sqrt(h_scale / (sampling_freq * window_length
 * h(x) = (cos²(PI*x / length) = 1/2 (1 + cos(PI*x / (length/2))  )
 *
 * \param first  = first point
 * \param last   < last data point

 */
template <typename T, typename Iterator> void hanning (Iterator first, const Iterator last) {

    T i = T(0);

    T h_period = M_PI/ ((T)(last - first)), hc = (T)0., hw = (T)0.;

    T h_scale = (T)2.;
    while (first != last) {
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        *first *= hw;                                   // hanning
        ++i;
        ++first;
    }
}



/*!
 * \brief (old )combines detrend and hanning (e.g. faster computation); documentation ref. to single functions - old version
 * \param first  = first point
 * \param last   < last data point


 */
template<typename T, typename Iterator> void simple_detrend_and_hanning (Iterator first, const Iterator last) {

    T i;
    T tt, slope = (T)0., sumtx = (T)0.;
    T h_period = M_PI/ ((T)(last - first)), hc = (T)0., hw = (T)0.;
    Iterator start = first;
    T h_scale = (T)2.;

    // see comments on hanning above

    T t_factor = 2.0/((last - first) - 1);
    T mean = accumulate(first, last, 0.)/ (T)(last -first);


    i = 0.;
    while (first != last) {
        tt = i * t_factor - 1.;
        slope += tt * tt;
        sumtx += tt * *first;
        ++first;
        ++i;
    }
    slope = sumtx / slope;

    i = 0.;
    first = start;
    while (first != last) {
        tt = i * t_factor - 1.;
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        *first -= (mean + tt * slope);                  // detrend
        *first *= hw;                                   // hanning
        ++i;
        ++first;
    }
}

/*!
 * calculate the mean <br>
 * see that n is casted into T before division
 * \param first  = first point
 * \param last   < last data point
 * \param add adds a constant
 * \return mean
 */
template<typename T, typename Iterator> T mean (Iterator first, const Iterator last, const T& add = T(0)) {

    size_t n = (last - first);
    T sum = accumulate(first, last, add);
    return sum / (T)n ;

}

/*!
 * calculate the median <br> of a COPY
 * \param first  = first point
 * \param last   < last data point
 * \return median
 */
template<typename T, typename Iterator> T median (Iterator first, const Iterator last) {


    T med;
    std::valarray<T> w (last - first);
    copy(first, last, &w[0]);
    sort(&w[0], &w[w.size()]);
    if (w.size() % 2 == 0)       // array is even
        med = (w[w.size()/2] + w[w.size()/2 - 1])/2.;
    else
        med = w[w.size()/2];  // integer divison should not round up
    return med;
}

/*!
 * \brief find max and min at the same time and avoid double iteration
 * \param first  = first point
 * \param last   < last data point

*/
template <typename T, typename Iterator>
void max_and_min (Iterator first, const Iterator last,  T &max, T& min) {

    T* xmax = first;
    T* xmin = first;
    if (first == last) {
        xmax = *first;
        xmin = *first;
    }
    while (++first != last) {
        if (*first < *xmin) xmin = first;
        if (*first > *xmax) xmax = first;
    }

    max = *xmax;
    min = *xmin;
}
/*!
 * \brief calculate the variance of a distribution
 * \param first  = first point
 * \param last   < last data point
 * \param mean get the mean of the distribution
 * \return variance
 */


template <typename T, typename Iterator> T variance(Iterator first, const Iterator last, T& mean )  {


    if ( !(last - first)) {
        mean = T(0);
        return T(0);
    }

    size_t i = 0, j;

    T mysize  = T(last - first);
    T one = T(1);

    T add = T (0);
    T sum = accumulate(first, last, add);
    T powsum = T(0);
    T sum_of_squares = T (0);

    while (first != last) {
        sum_of_squares += pow(*first++, 2.);
    }

    powsum = pow(sum,2.);
    powsum /= mysize;

    mean = sum;
    mean /= mysize;

    // precision of double is not perfect
    if (abs(sum_of_squares- powsum) < 1E-10) {
        return 0;
    }


    return ( (one/( mysize - one)) * ( sum_of_squares - powsum )) ;
}

/*!
 * \brief calculate the standard deviation of a distribution
 * \param first  = first point
 * \param last   < last data point
 * \param mean get the mean of the distribution
 * \return standard deviation
 */


template <typename T, typename Iterator> T stddev(Iterator first, const Iterator last, T& mean )  {

    T var = variance<T>(first, last, mean) ;

    return sqrt(var);
}


/*!
 * \brief fold two vectors - each element by each element and sum up, CHeCK SZIES IN ADVANCE, no error here
 * no check if vectors have same length
 * \param first  = first point
 * \param last   < last data point
 * \param coef_first  = first point of other vector
 * \param coef_last   < last data point of other vector
 * \return  sum of product
 */

template <typename T, typename Iterator> inline T fold(Iterator first, const Iterator last, Iterator coef_first, const Iterator coef_last) {
  T sum = 0;
  //*first_out = 0;
  while ( (coef_first != coef_last) ) {
    sum  += ((*first) * (*coef_first));
    ++coef_first;
    ++first;
  }
  return sum;

}


/*!
 * \brief Calculates FIR filter coefficients for low pass, high pass, band pass and band stop<br>
 * as longer (last-first) the FIR filter (order of filter) is, as better the filter becomes, but also the
 * computation time increases<br>
 * coefficiencies vector must have odd length like 21 or 471 <br>
 * example for 4096Hz and 8193 coeff:
 * 0 1000 0   is low pass <br>
 * 100 4096 0 is high pass <br>
 * 100 1000 0 is bandpass <br>
 * 55 45 0    is notch / bandstop <br>
 *
 *
 *
 * \author Jialong He, Bernhard Friedrichs
 * \date July 1999, April 2005, September 2016
 *
 * (CutLow == 0.0) && (CutHigh < 1.0)                      is a Low Pass<br>
 * (CutLow > 0.0) && (CutHigh == 1.0)                      is a High Pass<br>
 * (CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh) is Band Pass<br>
 * (CutLow > 0.0) && (CutHigh < 1.0) && (CutLow > CutHigh) is Band Stop / Notch
 * \param first first element of the coefficient vector
 * \param last  last element of the coefficient vector
 * \param samplefreq
 * \param CutLow
 * \param CutHigh
 * \return coefficients size
 */
template <typename T, typename Iterator > size_t fir_dsgn(Iterator first, Iterator last, const T samplefreq, T CutLow, T CutHigh) {


    /*---------------------------------------------*/
    /* adjust the number of coefficients to be ODD */
    /*---------------------------------------------*/

    if ( !((last - first) % 2)) {
            std::cerr << "fir_dsgn-> coefficiencies vector must have odd length like 21 or 471; actual length: " << (last - first) << std::endl;
        return 0;
    }


    double Sum, TmpFloat;
    size_t CoefNum, HalfLen, Cnt;


    CoefNum = (last -first);

    //adjust but why? with 2??
    CutLow = (CutLow / samplefreq) * 2.;

    if (CutHigh > samplefreq/2.) {
        CutHigh = samplefreq/2.;
    }

    CutHigh = (CutHigh / samplefreq) * 2.;
    HalfLen = (CoefNum - 1) / 2;

    /*------------------*/
    /*  Lowpass filter  */
    /*------------------*/
    if ((CutLow == 0.0) && (CutHigh < 1.0)) {

        first[HalfLen] = CutHigh;
        for (Cnt = 1; Cnt <= HalfLen; Cnt++) {
            TmpFloat = M_PI * Cnt;
            first[HalfLen + Cnt] = sin(CutHigh * TmpFloat) / TmpFloat;
            first[HalfLen - Cnt] = first[HalfLen + Cnt];
        }

        /*------------------------------*/
        /* multiplying with a window    */
        /*------------------------------*/
        TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
        Sum = 0.0;
        for (Cnt = 0; Cnt < CoefNum; Cnt++) {
            first[Cnt] *= (0.54 - 0.46 * cos(TmpFloat * Cnt));
            Sum += first[Cnt];
        }

        /*------------------------------*/
        /* Normalize GAIN to 1          */
        /*------------------------------*/
        for (Cnt = 0; Cnt < CoefNum; Cnt++) {
            first[Cnt] /= fabs (Sum);
        }

        return (CoefNum);

    }  /* if Lowpass */


    /*------------------*/
    /* Highpass filter  */
    /*------------------*/
    else if ((CutLow > 0.0) && (CutHigh == 1.0)) {

        first[HalfLen] = CutLow;
        for (Cnt = 1; Cnt <= HalfLen; Cnt++) {
            TmpFloat = M_PI * Cnt;
            first[HalfLen + Cnt] = sin(CutLow * TmpFloat) / TmpFloat;
            first[HalfLen - Cnt] = first[HalfLen + Cnt];
        }

        /*------------------------------*/
        /* multiplying with a window    */
        /*------------------------------*/
        TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
        Sum = 0.0;
        for (Cnt = 0; Cnt < CoefNum; Cnt++) {
            first[Cnt] *= -(0.54 - 0.46 * cos(TmpFloat * Cnt));
            if (Cnt % 2 == 0) Sum += first[Cnt];  /* poly(-1), even +, odd -*/
            else Sum -= first[Cnt] ;
        }

        first[HalfLen] += 1;
        Sum += 1;

        /*------------------------------*/
        /* Normalize GAIN to 1          */
        /*------------------------------*/
        for (Cnt = 0; Cnt < CoefNum; Cnt++) {
            first[Cnt] /= fabs (Sum);
        }

        return (CoefNum);

    } /* if HighPass */


    /*------------------*/
    /* Bandpass filter  */
    /*------------------*/
    else if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh)) {

        first[HalfLen] = CutHigh - CutLow;
        for (Cnt = 1; Cnt <= HalfLen; Cnt++) {
            TmpFloat = M_PI * Cnt;
            first[HalfLen + Cnt] = 2.0 * sin( (CutHigh - CutLow) / 2.0 * TmpFloat) *
                                   cos( (CutHigh + CutLow) / 2.0 * TmpFloat) / TmpFloat;
            first[HalfLen - Cnt] = first[HalfLen + Cnt];
        }

        /*------------------------------*/
        /* multiplying with a window    */
        /*------------------------------*/
        TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
        Sum = 0.0;
        for (Cnt = 0; Cnt < CoefNum; Cnt++) {
            first[Cnt] *= (0.54 - 0.46 * cos(TmpFloat * Cnt));
            Sum += first[Cnt];
        }
        return (CoefNum);

    } /* if */

    /*------------------*/
    /* Bandstop filter  */
    /*------------------*/
    else if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow > CutHigh)) {

        first[HalfLen] = CutLow - CutHigh;
        for (Cnt=1; Cnt<=HalfLen; Cnt++) {
            TmpFloat = M_PI * Cnt;
            first[HalfLen + Cnt] = 2.0 * sin( (CutLow - CutHigh) / 2.0 * TmpFloat) *
                                   cos( (CutHigh + CutLow) / 2.0 * TmpFloat) / TmpFloat;
            first[HalfLen - Cnt] = first[HalfLen + Cnt];
        }

        /*------------------------------*/
        /* multiplying with a window    */
        /*------------------------------*/
        TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
        Sum = 0.0;
        for (Cnt=0; Cnt<CoefNum; Cnt++) {
            first[Cnt] *= -(0.54 - 0.46 * cos(TmpFloat * Cnt));
            Sum += first[Cnt];
        }

        first[HalfLen] += 1;
        Sum += 1;

        /*------------------------------*/
        /* Normalize GAIN to 1          */
        /*------------------------------*/
        for (Cnt=0; Cnt<CoefNum; Cnt++) {
            first[Cnt] /= fabs (Sum);
        }


        return (CoefNum);

    }  /* if */
    else {
        #ifdef C_ERROR_OUT_MTX_ITER
            cerr << "fir_dsgn -> wrong filter parameters selected!!!; likely that fmax > fsample/2 " << std::endl;
        #endif
        return 0;
    }

    return (CoefNum);    /* never reach here, avoid empty return warning */
}




} // end namespace

#endif
