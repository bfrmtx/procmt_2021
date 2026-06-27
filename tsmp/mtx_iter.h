/** 
 * \file mtx_iter.h
 * 
 * \brief This file contains the major processing functions.
 *
 * This file contains the main processing functions, that can 
 * be applied to the TMatrix and slice_iter classes. The
 * functions, that are defined here are used in the ProcessingObject
 * classes to compute different types of processing functions on
 * the data-
 *
 * (c) 2008 by Metronix Messgereate und Elektronik GmbH
 *
 * \author BFR, MWI
 * \date 11.09.2008
 * \version 01.00
 *
 * - changed at: -
 * - changed by: -
 * - changes:    -
 */


// self include protection

#ifndef MTX_ITER_H
#define MTX_ITER_H


// includes

#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <cmath>
#include <valarray>
#include "slice_iter.h"
#include "c_slice_iter.h"
#include "allinclude.h"

//#include <../src/DebugConstants.h>

using namespace std;

/*!
 * @namespace  mtx
 * The algorithms will basically use indices wich are compatible to v.begin() and v.end(); in C notation
 * this is for an array v[10]: v[0] and v[10]. v.end() is a pointer BEHIND the last element. The reason
 * is that these algorithms use while(first != last) first++.<br>
 * STL alogorithms are also using first and last.<br>
 * first is the pointer, first++ increments; *first refers to the value<br>
 * most of these algorithms are designed for double, long double<br>
 * In multiclasses we could add rtty: <br>
 *  template<class T, class InputIterator, class L> xx (*fist, T &val, L &output) {
 *  L otherType;<br>
 *  if (typeid(otherType) == typeid(*first) ) cout << "same type used" << endl;<br>
 *  // cout << typeid(otherType).name() << endĺ;
 *  }<br>
 *
 * \author B. Friedrichs
 */
namespace mtx {

  template <class Iterator>  struct  mtxBuffer {
    Iterator first;
    Iterator last;
  };

/** 
 * \brief This function computes the dipole length out of the sensor positions.
 *
 * This function will determine the dipole length out of the 
 * sensor positioning data. The dipole length is the absolute
 * value of the vector, that is defined by the X, Y and Z 
 * coordinates, assuming that they are orthognal to each other.
 * The center of the coordinate system allways is the ADU-07
 * system itself.
 *
 * @param[in] T dPosX1 = X1 coordinate
 * @param[in] T dPosX2 = X2 coordinate
 * @param[in] T dPosY1 = Y1 coordinate
 * @param[in] T dPosY2 = Y2 coordinate
 * @param[in] T dPosZ1 = Z1 coordinate
 * @param[in] T dPosZ2 = Z2 coordinate
 * @param[out] T = dipole length
 *
 * \author MWI
 * \date 25.09.2008
 * \version 01.00
 */
template <class T> T dipole_length (T dPosX1, T dPosX2, T dPosY1, T dPosY2, T dPosZ1, T dPosZ2)
{
    // declaration of variables
    T dDeltaX    = (T) 0.0;
    T dDeltaY    = (T) 0.0;
    T dDeltaZ    = (T) 0.0;
    T dDipoleLen = (T) 0.0;
    
    dDeltaX = dPosX1 - dPosX2;
    dDeltaY = dPosY1 - dPosY2;
    dDeltaZ = dPosZ1 - dPosZ2;
    
    dDipoleLen = sqrt (pow (dDeltaX, 2) + pow (dDeltaY, 2) + pow (dDeltaZ, 2));
    
    return (dDipoleLen);
}


/** 
 * \brief This function will scale the input data with the numerator and divisor.
 *
 * This function will scale the input spectra with the
 * input parameters. This is done by multiplying with all
 * the entries with the numerator and divide it with the
 * divisor.
 *
 * @param[in] Iterator iterBegin = iterator on start of data array
 * @param[in] Iterator iterEnd = iterator on end of data array
 * @param[in] T dNumerator = numerator
 * @param[in] T dDivisor = divisor
 * @param[out] size_t = numbers of values that have been computed
 *
 * \author MWI
 * \date 25.09.2008
 * \version 01.00
 */
template <class T, class Iterator>
    size_t scale (Iterator iterBegin, const Iterator iterEnd, const T dNumerator, const T dDivisor)
{
    // declaration of variables
    size_t uiNumValues = 0;
    
    // iterate over all values and scale them
    while (iterBegin != iterEnd)
    {
        (*iterBegin) *= dNumerator;
        (*iterBegin) /= dDivisor;
        ++iterBegin;
        ++uiNumValues;
    }
    
    return (uiNumValues);
}


/** 
 * \brief This function computes a detrend on the incoming time series.
 *
 * This function will compute a detrend on the incoming time
 * series data. This means that all linear trends are removed
 * from it. The data buffer is overwritten with the processing
 * result.
 *
 * @param[in] Iterator first = iterator to start of time series buffer
 * @param[in] Iterator last = iterator to end of time series buffer
 *
 * \author BFR
 * \date 11.09.2008
 * \version 01.00
 */
template <class T, class Iterator> void detrend_procmt (Iterator first, const Iterator last) {

    ///* MWI implementation of detrend (test because detrend adds noise)
    // declaration of variables
    T dBias       = 0.0;
    T dTrend      = 0.0;
    T dTrendPart1 = 0.0;
    T dTrendPart2 = 0.0;
    T dWindowLen  = 0.0;
    T dTmpValue;
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
    
/**
 * removes  a trend from data; should be used before using hanning and FFT
 * @param first 
 * @param last 
 */
template <class T, class Iterator> void detrend (Iterator first, const Iterator last) {

  Iterator save = first;
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

  first = save;
  i=0;
  while (first != last) {
    tt = i * t_factor - 1.;
    (*first) -= (mean + tt * slope);                  // detrend
    ++first;
    ++i;
  }
}




/** 
 * \brief This function computes a hanning on the incoming time series.
 *
 * This function will scale the incoming data buffer with an hanning
 * window.
 *
 * @param[in] Iterator first = iterator to start of time series buffer
 * @param[in] Iterator last = iterator to end of time series buffer
 * @param[in] T h_scale = 0. = extra scaling factor
 *
 * \author BFR
 * \date 11.09.2008
 * \version 01.00
 */
template <class T, class Iterator> T hanning_procmt (Iterator first, const Iterator last, T h_scale = 0.) {

    // declaration of variables
    T dT0;
    T dCounter;
    
    // get periode length
    dT0 = (T) M_PI / ((last - first) / 2.0);
    
    // do hanning like described in PROCMT manual
    dCounter = (last - first) / 2.0;
    while (first != last)
    {
        (*first) = (*first) * (0.5 + (0.5 * cos (dCounter * dT0)));
        ++dCounter;
        ++first;
    }
    
}

/**
 * applies a von Hann (Hanning) window onto a time series<br>
 * This routine with h_scale 2.0 actaully returns 1 as integral of the
 * hanning window so scaling should not be neccessary<br>
 * hence that h_scale has to be re-used after using the fft in the sense
 * that the result of the ft has to multiplied with
 * sqrt(h_scale / (sampling_freq * window_length
 * h(x) = (cos²(PI*x / length) = 1/2 (1 + cos(PI*x / (length/2))  )
 * 
 * @param first begin of your window 
 * @param last  end of your window
 * @param h_scale scale the integral window ; defaults to 2.
 * @return scale used -> default 2.
 */
template <class T, class Iterator> T hanning (Iterator first, const Iterator last, T h_scale = 0.) {

  T i;

  T h_period = M_PI/ ((T)(last - first)), hc = (T)0., hw = (T)0.;

  if (!h_scale) h_scale = (T)2., i = 0.;
  while (first != last) {
    hc = cos(i * h_period);
    hw = h_scale * (1. - hc * hc);
    *first *= hw;                                   // hanning
    ++i;
    ++first;
  }
  return h_scale;
}


/**
 *  combines detrend and hanning (e.g. faster computation); documentation ref. to single functions
 * @param first 
 * @param last 
 * @param h_scale 
 * @return 
 */
template<class T, class Iterator> T detrend_and_hanning (Iterator first, const Iterator last,  T h_scale = 0.) {

    T i;
    T tt, slope = (T)0., sumtx = (T)0.;
    T h_period = M_PI/ ((T)(last - first)), hc = (T)0., hw = (T)0.;
    Iterator start = first;
    if (!h_scale) h_scale = (T)2.;

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
    return h_scale;
}



/**
 * refer to the functions above. This function is needed if you use OVERLAPPING windows: the input will be
 * partly used again and therfor can not be modified
 * @param first 
 * @param last 
 * @param out_first 
 * @param out_last 
 * @param h_scale 
 * @return 
 */
template<class T, class Iterator> T detrend_and_hanning_copy (Iterator first, const Iterator last,
    Iterator out_first, const Iterator out_last,  T h_scale = 0.) {

    T i;
    T tt, slope = (T)0., sumtx = (T)0.;
    T h_period = M_PI/ ((T)(last - first)), hc = (T)0., hw = (T)0.;
    Iterator start = first;
    Iterator out_start = out_first;
    if (!h_scale) h_scale = (T)2.;

// see comments on hanning above

    T t_factor = 2.0/((last - first) - 1);
    T mean = accumulate(first, last, 0.)/ (T)(last -first);


    i = 0.;
    while (first != last) {
        tt = i * t_factor - 1.;
        slope += tt * tt;
        sumtx += tt * *first;
        *out_first = *first;
        ++first;
        ++out_first;
        ++i;
    }
    slope = sumtx / slope;

    i = 0.;
    out_first = out_start;
    while (out_first != out_last) {
        tt = i * t_factor - 1.;
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        *out_first -= (mean + tt * slope);                  // detrend
        *out_first *= hw;                                   // hanning
        ++i;
        ++out_first;
    }
    return h_scale;
}


/**
 * calculate the mean <br>
 * see that n is casted into T before division
 * @param first
 * @param last
 * @return mean
 */
template<class T, class Iterator> T mean (Iterator first, const Iterator last) {

  size_t n = (last - first);
  T add = (T) 0;
  T sum = accumulate(first, last, add);
  return sum / (T)n ;

}

/**
 * calculate the median <br>
 * demonstrates the usage of valarrays internally and STL functions for valarrays<br>
 * hence that valarrays need less space and are fast even with no optimization
 * @param first
 * @param last
 * @return median
 */
template<class T, class Iterator> T median (Iterator first, const Iterator last) {

  
  T med;
  valarray<T> w (last - first);
  copy(first, last, &w[0]);
  sort(&w[0], &w[w.size()]);
  if (w.size() % 2 == 0)       // array is even
    med = (w[w.size()/2] + w[w.size()/2 - 1])/2.;
  else
    med = w[w.size()/2];  // integer divison should not round up
  ~w;
  return med;

}


/**
 *  check !
 * standard deviation
 * @param first
 * @param last
 * @return
 */
template <class T, class Iterator> T stddev(Iterator first, const Iterator last)  {

  valarray<T> sqr (last - first);
  size_t i = 0;
  
  T add = (T) 0;
  T sum = accumulate(first, last, add);

  // copy and modify
  while (first++ != last)  sqr[i++] = pow(*first, 2.);
  T sum_of_squares = sqr.sum();
  
  return sqrt( (1./(sqr.size()-1)) * ( sum_of_squares - pow(sum,2.)/sqr.size() ) );
}




/**
 * check ... wrong yet
 * standard deviation of the mean
 * @param first
 * @param last
 * @return
 */
/*
template <class T, class Iterator> T stddev_mean (Iterator first, const Iterator last)  {

  valarray<T> sqr (last - first);
  copy(first, last, &sqr[0]);
  
  T add = (T) 0;
  T sum = accumulate(first, last, add);
  
  sqr -= (sqr.sum()/(sqr.size()));
  T sum_of_squares = sqr.sum();

  cerr << sqr.size() << "xxx " << sum_of_squares << "xx " << sum << endl;
  
  return sqrt( 1./(sqr.size()-1) * ( sum_of_squares - pow(sum,2.)/sqr.size() ) );
}


template <class T, class Iterator> T stddev_mean (Iterator first, Iterator last, T& mean, T& stddevmean)  {

  valarray<T> sqr (last - first);
  copy(first, last, &sqr[0]);
  
  T add = (T) 0;
  T sum = accumulate(first, last, add);
  
  sqr -= (sqr.sum()/(sqr.size()));
  T sum_of_squares = sqr.sum();

  cerr << sqr.size() << "xxx " << sum_of_squares << "xx " << sum << endl;
  
  return sqrt( 1./(sqr.size()-1) * ( sum_of_squares - pow(sum,2.)/sqr.size() ) );
}

*/


/**
 * Calculates FIR filter coefficients for low pass, high pass, band pass and band stop<br>
 * as longer (last-first) the FIR filter (order of filter) is, as better the filter becomes, but also the
 * computation time increases<br>
 * coefficiencies vector must have odd length like 21 or 471
 * \author Jialong He, Bernhard Friedrichs
 * \date July 1999, April 2005
 *
 * (CutLow == 0.0) && (CutHigh < 1.0)                     is a Low Pass<br>
 * (CutLow > 0.0) && (CutHigh == 1.0)                     is a High Pass<br>
 * (CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh) is Band Pass<br>
 * (CutLow > 0.0) && (CutHigh < 1.0) && (CutLow > CutHigh) is Band Stop
 * @param first first element of the coefficient vector
 * @param last  last element of the coefficient vector
 * @param samplefreq
 * @param CutLow
 * @param CutHigh
 * @return
 */

template <class T, class Iterator > size_t fir_dsgn(Iterator first, Iterator last, const T samplefreq, T CutLow, T CutHigh) {


    /*---------------------------------------------*/
    /* adjust the number of coefficients to be ODD */
    /*---------------------------------------------*/

    if ( !((last - first) % 2)) {
        #ifdef C_ERROR_OUT_MTX_ITER
            cerr << "fir_dsgn-> coefficiencies vector must have odd length like 21 or 471; actual length: " << (last - first) << endl;
        #endif
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

//cerr << samplefreq << " " << CutLow << "  " << CutHigh << endl;

    HalfLen = (CoefNum - 1) / 2;



    /*------------------*/
    /*  Lowpass filter  */
    /*------------------*/
    if ((CutLow == 0.0) && (CutHigh < 1.0)) {

        first[HalfLen] = CutHigh;
        for (Cnt=1; Cnt<=HalfLen; Cnt++) {
            TmpFloat = M_PI * Cnt;
            first[HalfLen + Cnt] = sin(CutHigh * TmpFloat) / TmpFloat;
            first[HalfLen - Cnt] = first[HalfLen + Cnt];
        }

        /*------------------------------*/
        /* multiplying with a window    */
        /*------------------------------*/
        TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
        Sum = 0.0;
        for (Cnt=0; Cnt<CoefNum; Cnt++) {
            first[Cnt] *= (0.54 - 0.46 * cos(TmpFloat * Cnt));
            Sum += first[Cnt];
        }

        /*------------------------------*/
        /* Normalize GAIN to 1          */
        /*------------------------------*/
        for (Cnt=0; Cnt<CoefNum; Cnt++) {
            first[Cnt] /= fabs (Sum);
        }

        return (CoefNum);

    }  /* if Lowpass */


    /*------------------*/
    /* Highpass filter  */
    /*------------------*/
    else if ((CutLow > 0.0) && (CutHigh == 1.0)) {

        first[HalfLen] = CutLow;
        for (Cnt=1; Cnt<=HalfLen; Cnt++) {
            TmpFloat = M_PI * Cnt;
            first[HalfLen + Cnt] = sin(CutLow * TmpFloat) / TmpFloat;
            first[HalfLen - Cnt] = first[HalfLen + Cnt];
        }

        /*------------------------------*/
        /* multiplying with a window    */
        /*------------------------------*/
        TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
        Sum = 0.0;
        for (Cnt=0; Cnt<CoefNum; Cnt++) {
            first[Cnt] *= -(0.54 - 0.46 * cos(TmpFloat * Cnt));
            if (Cnt % 2 == 0) Sum += first[Cnt];  /* poly(-1), even +, odd -*/
            else Sum -= first[Cnt] ;
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

    } /* if HighPass */


    /*------------------*/
    /* Bandpass filter  */
    /*------------------*/
    else if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh)) {

        first[HalfLen] = CutHigh - CutLow;
        for (Cnt=1; Cnt<=HalfLen; Cnt++) {
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
        for (Cnt=0; Cnt<CoefNum; Cnt++) {
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
            cerr << "fir_dsgn -> wrong filter parameters selected!!!; likely that fmax > fsample/2 " << endl;
        #endif
        return 0;
    }

    return (CoefNum);    /* never reach here */
}



//template <class T, class Iterator> inline T* fold(Iterator first_in, const Iterator last_in, Iterator coef_first, const Iterator coef_last) {
template <class T, class Iterator, class Iter> inline T fold(Iterator first_in, const Iterator last_in, Iter coef_first, const Iter coef_last) {
  T sum = 0;
  //*first_out = 0;
  while (coef_first != coef_last) {
  //  *first_out += ((*first_in) * (*coef_first));
    sum  += ((*first_in) * (*coef_first));
    ++coef_first;
    ++first_in;
  }
  return sum;
 
}


/*!
Applies FIR filter coefficients for low pass, high pass, band pass and band stop on input<br>
can also be used for folding time series
@param[in] first_in begin of input vector
@param[in] last_in end of input vector
@param[in] first_out of filtred output vector
@param[in] last_out of filtred output vector
@param[in] first_coef begin of coefficients vector
@param[in] last_coef end of coefficients vector
 */
template <class T, class Iterator, class vIter > size_t fil_fir(Iterator first_in, const Iterator last_in, Iterator first_out, const Iterator last_out, vIter first_coef, const vIter last_coef) {

    size_t order = (last_coef - first_coef);



    if (order) {
        #ifdef C_DEBUG_OUT_MTX_ITER
            cerr << "fil_fir -> using external coefficients, order (" << order  << ")" << endl;
        #endif
    }

    if ( unsigned(last_in - first_in) < order) {
        #ifdef C_ERROR_OUT_MTX_ITER
            cerr << "fil_fir -> filter has " << order << " coefficients, your time series only "
            << last_in - first_in << " points; return 0" << endl;
        #endif
        return 0;
    }

    if ( ( unsigned(last_in - first_in) - order + 1) > (last_out - first_out)) {

        #ifdef C_ERROR_OUT_MTX_ITER
            cerr << "fir_fil -> output vector to small: needed size: " << ((last_in - first_in) - order + 1) << " actual size: " << (last_out - first_out) << endl;
        #endif
        return 0;
    }
    if ( ((last_in - first_in) - order + 1) < (last_out - first_out)) {

        #ifdef C_ERROR_OUT_MTX_ITER
            cerr << "fir_fil -> output vector to might be to BIG: needed size: " << ((last_in - first_in) - order + 1) << " actual size: "
            << (last_out - first_out) << " continued" << endl;
        #endif
    }
    
    #ifdef C_DEBUG_OUT_MTX_ITER
        cerr << "fil_fir output: " << (last_out - first_out) << " samples " << endl;
    #endif




    // fold the input signal with the filter function

    while (first_in != (last_in - order)) {

        *first_out = fold<T>(first_in, last_in, first_coef, last_coef);
        //fold(first_in, last_in, first_coef, last_coef, first_out);

        // move along the array positions
        ++first_out;
        ++first_in;
    }


    return order;
}


/**
 * Input fist/last is multiplied by coefficients; after that you must ++first until you reach (last_in - first_in) = (coef_last - coef_first)
 * @param first_in
 * @param last_in
 * @param coef_first
 * @param coef_last
 * @return sum of product
 */


/*!
* Applies FIR filter coefficients for reduction<br>
* can also be used for folding time series<br>
 * The reduced times seris has the size out.size() = (in.size() - coeff.size() ) /reduction<br>
 * the start time is late: datetime += long(coeff.size() / (2. *samplefreq))<br>
 * MTX Reduction filters are calculated as (example):<br>
 * 64 input frequency; 32x reduction ( = 2 Nyquist); 1 set to stop frequency, and 0.5 is pass through frequency
 * @param[in] first_in begin of input vector
* @param[in] last_in end of input vector
* @param[in] first_out of filtred output vector
* @param[in] last_out of filtred output vector
* @param[in] first_coef begin of coefficients vector
* @param[in] last_coef end of coefficients vector
* @param[in] reduction such as 2, 4, 8, 32 - same as func_move: input iterator will be increased by 32 increments
*/

template <class T, class Iterator, class vIter > size_t fil_fir_reduce(Iterator first_in, const Iterator last_in, Iterator first_out, const Iterator last_out, vIter first_coef, const vIter last_coef,
    const size_t& reduction) {

      size_t order = (last_coef - first_coef);
      size_t i = 0;


      if (order) {
#ifdef C_DEBUG_OUT_MTX_ITER
        cerr << "fil_fir_reduce -> using external coefficients, order (" << order  << ")" << endl;
#endif
      }

      if ((last_in - first_in) < order) {
#ifdef C_ERROR_OUT_MTX_ITER
        cerr << "fil_fir_reduce -> filter has " << order << " coefficients, your time series only "
            << last_in - first_in << " points; return 0" << endl;
#endif
        return 0;
      }

      if ( (((last_in - first_in) - order)/reduction ) > (last_out - first_out)) {

#ifdef C_ERROR_OUT_MTX_ITER
        cerr << "fil_fir_reduce -> output vector to small: needed size: " << ((last_in - first_in) - order + 1) << " actual size: " << (last_out - first_out) << endl;
#endif
        return 0;
      }
      if ( (((last_in - first_in) - order)/reduction )  < (last_out - first_out)) {

#ifdef C_ERROR_OUT_MTX_ITER
        cerr << "fil_fir_reduce -> output vector to might be to BIG: needed size: " << ((last_in - first_in) - order + 1) << " actual size: "
            << (last_out - first_out) << " continued" << endl;
#endif
      }
#ifdef C_DEBUG_OUT_MTX_ITER
      cerr << "fil_fir_reduce output: " << (last_out - first_out) << " samples " << endl;
#endif




    // fold the input signal with the filter function

      while (first_in != (last_in - order)) {

        //fold(first_in, last_in, first_coef, last_coef, first_out);
        *first_out = fold<T>(first_in, last_in, first_coef, last_coef);

        // move along the array positions
        ++first_out;
        first_in += reduction;
      }



      return order;
    }




//**************************************************AKIMA SPLINE ***************************

/**
 * little helper for Akima Splines ; dx(i) (x[i+1]-x[i]); dy(i) (y[i+1]-y[i]) and m = dy(i)/dx(i)
 * (random access seems not to want const qualifyer)
 * @param x abcissa in
 * @param y ordinate in
 * @param index index on abscissa
 * @return dy[i] / dx[i]
 *
 */
template <class T, class Iterator_x, class Iterator_y> T m(Iterator_x x, Iterator_y y, const size_t index) {

    T dx = x[index+1] - x[index];
    T dy = y[index+1] - y[index];
    return dy/dx;

}



/**
 * Calculates the akima spline parameters b, c, d for  given inputs (x ,y) where x[0] < x[1] < ... < x[n] <br>
 * so the x-axis numbers MUST be SORTED x[0] < x[1] < x[n]<br>
 * RANDOM ACCESS Iterators needed - this template can only be used where "[]" is defined --- and not only "++"
 *
 * x = complete frequency list from cal file
 * y = complete spectra (real/imag) from cal file
 * b = 1st coefficient (output) computed from input
 * c = 2nd coefficient (output) computed from input
 * d = 3rd coefficient (output) computed from input
 *
 * for each input pair in x/y one set of coefffiecents b/c/d is computed.
 *
 * @param x_first first x Iterator
 * @param x_last  last x Iterator
 * @param y_first first y Iterator corresponding with first x!  -> (&x[0], &x[n+1])
 * @param y_last  last y Iterator corresponding with last x!  -> (&x[n], &y[n+1])
 * @param b_first coefficients start  - same size as 2D input (x,y)
 * @param b_last  coefficients stop
 * @param c_first coefficients start  - same size as 2D input (x,y)
 * @param c_last  coefficients stop
 * @param d_first coefficients start  - same size as 2D input (x,y)
 * @param d_last  coefficients stop
 * @return size of input
 */
template <class T, class Iterator_x, class Iterator_y, class Iterator_s> size_t aspline(
    Iterator_x x_first, const Iterator_x x_last,
    Iterator_y y_first, const Iterator_y y_last,
    Iterator_s b_first, const Iterator_s b_last,
    Iterator_s c_first, const Iterator_s c_last,
    Iterator_s d_first, const Iterator_s d_last) {


    size_t n = (size_t)(x_last - x_first);
    size_t i;

    // check for duplicates!!!
    //    for (i = 0; i < n - 1; i++)
    //      if (x[i] == x[i+1]) cerr << "aspline -> duplicate entry on x-axis( " << x[i] << ", " << x[i+1] << "); //result unstable; continued ****************" << endl;
    //

    if ( (b_last - b_first) != n || (c_last - c_first) != n || (d_last - d_first) != n || (y_last - y_first) != n    ) {
        #ifdef C_ERROR_OUT_MTX_ITER
            cerr << "aspline size mismatch " << endl ;
        #endif
        exit(0);

    }


    T num, den;
    T m_m1, m_m2, m_p1, m_p2;
    T x_m1, x_m2, x_p1, x_p2;
    T y_m1, y_m2, y_p1, y_p2;


    if (n > 0) {



        x_m1 = x_first[0] + x_first[1] - x_first[2];
        y_m1 = (x_first[0]-x_m1) * (m<T>(x_first, y_first, 1) - 2 * m<T>(x_first, y_first, 0)) + y_first[0];

        m_m1 = (y_first[0]-y_m1)/(x_first[0]-x_m1);

        x_m2 = 2 * x_first[0] - x_first[2];
        y_m2 = (x_m1-x_m2) * (m<T>(x_first, y_first, 0) - 2 * m_m1) + y_m1;

        m_m2 = (y_m1-y_m2)/(x_m1-x_m2);

        x_p1 = x_first[n-1] + x_first[n-2] - x_first[n-3];
        y_p1 = (2 * m<T>(x_first, y_first, (n-2)) - m<T>(x_first, y_first, (n-3))) * (x_p1 - x_first[n-1]) + y_first[n-1];

        m_p1 = (y_p1-y_first[n-1])/(x_p1-x_first[n-1]);

        x_p2 = 2 * x_first[n-1] - x_first[n-3];
        y_p2 = (2 * m_p1 - m<T>(x_first, y_first, (n-2))) * (x_p2 - x_p1) + y_p1;

        m_p2 = (y_p2-y_p1)/(x_p2-x_p1);

        // i = 0
        num=fabs(m<T>(x_first, y_first, 1) - m<T>(x_first, y_first, 0)) * m_m1 + fabs(m_m1 - m_m2) * m<T>(x_first, y_first, 0);
        den=fabs(m<T>(x_first, y_first, 1) - m<T>(x_first, y_first, 0)) + fabs(m_m1 - m_m2);

        if (den != 0.0) b_first[0] = num / den;
        else            b_first[0] = 0.0;

        //i = 1
        num=fabs(m<T>(x_first, y_first, 2) - m<T>(x_first, y_first, 1)) * m<T>(x_first, y_first, 0) + fabs(m<T>(x_first, y_first, 0) - m_m1) * m<T>(x_first, y_first, 1);
        den=fabs(m<T>(x_first, y_first, 2) - m<T>(x_first, y_first, 1)) + fabs(m<T>(x_first, y_first, 0) - m_m1);

        if (den != 0.0) b_first[1] = num / den;
        else            b_first[1] = 0.0;

        for (i=2; i < n-2; i++) {

            num=fabs(m<T>(x_first, y_first, (i+1)) - m<T>(x_first, y_first, i)) * m<T>(x_first, y_first, (i-1)) + fabs(m<T>(x_first, y_first, (i-1)) - m<T>(x_first, y_first, (i-2))) * m<T>(x_first, y_first, i);
            den=fabs(m<T>(x_first, y_first, (i+1)) - m<T>(x_first, y_first, i)) + fabs(m<T>(x_first, y_first, (i-1)) - m<T>(x_first, y_first, (i-2)));

            if (den != 0.0) b_first[i]=num / den;
            else            b_first[i]=0.0;
        }

        // i = n - 2
        num=fabs(m_p1 - m<T>(x_first, y_first, (n-2))) * m<T>(x_first, y_first, (n-3)) + fabs(m<T>(x_first, y_first, (n-3)) - m<T>(x_first, y_first, (n-4))) * m<T>(x_first, y_first, (n-2));
        den=fabs(m_p1 - m<T>(x_first, y_first, (n-2))) + fabs(m<T>(x_first, y_first, (n-3)) - m<T>(x_first, y_first, (n-4)));

        if (den != 0.0) b_first[n-2]=num / den;
        else        b_first[n-2]=0.0;

        // i = n - 1
        num = fabs(m_p2 - m_p1) * m<T>(x_first, y_first, (n-2)) + fabs(m<T>(x_first, y_first, (n-2)) - m<T>(x_first, y_first, (n-3))) * m_p1;
        den = fabs(m_p2 - m_p1) + fabs(m<T>(x_first, y_first, (n-2)) - m<T>(x_first, y_first, (n-3)));

        if (den != 0.0) b_first[n-1]=num / den;
        else        b_first[n-1]=0.0;

        for (i=0; i < n-1; i++) {
            double dxv = x_first[i+1] - x_first[i];
            c_first[i]=(3 * m<T>(x_first, y_first, i) - 2 * b_first[i] - b_first[i+1]) / dxv;
            d_first[i]=(b_first[i] + b_first[i+1] - 2 * m<T>(x_first, y_first, i)) / (dxv * dxv);
        }
    }

    return n;
}




/**
 * Calculates a cubic spline for newly given u ("x-axis") and returns a new v ("y-axis")
 *
 * <br>
 * seval v = y(i) + b(i)*(u-x(i)) + c(i)*(u-x(i))**2 + d(i)*(u-x(i))**3<br>
 * where  x(0) < u < x(n), using horner's rule<br>
 * if  u < x(0) then  i = 1  is used<br>
 * if  u > x(n) then  i = n  is used<br>
 * u = the abscissa at which the spline is to be evaluated<br>
 * x,y = the arrays of data abscissas and ordinates<br>
 * b,c,d = arrays of spline coefficients computed by spline / aspline<br>
 *
 * u = target frequency value (from input spectra / not cal spectra)
 * v = output value (interpolation result) for new target frequency u
 * x = frequency list from cal file
 * y = spectral values from cal file
 * b, c, d = coefficients taken priorly computed with function aspline (...)
 *
 * @param u new value u
 * @param x_first
 * @param x_last
 * @param y_first
 * @param y_last
 * @param b_first
 * @param b_last
 * @param c_first
 * @param c_last
 * @param d_first
 * @param d_last
 * @return the interpolated v (u,v)
 */
template <class T, class Iterator_u, class Iterator_x, class Iterator_y, class Iterator_s>
                              T seval(const Iterator_u u,
        Iterator_x x_first, const Iterator_x x_last,
        Iterator_y y_first, const Iterator_y y_last,
        Iterator_s b_first, const Iterator_s b_last,
        Iterator_s c_first, const Iterator_s c_last,
        Iterator_s d_first, const Iterator_s d_last) {


    size_t n = (x_last - x_first);
    size_t i, j, k;
    T dx;

// binary search is performed to determine the proper interval.
//binary search

    if (u < x_first[0]) i = 0;
    else if (u >= x_first[n - 1]) i = n - 1;
    else {
        i = 0;
        j = n;

        do {
            k = (i + j) / 2;
            if (u < x_first[k])
                j = k;
            if (u >= x_first[k])
                i = k;
        } while (j > i + 1);
    }

    dx = u - x_first[i];

    return (y_first[i] + dx * (b_first[i] + dx * (c_first[i] + dx * d_first[i])));
}

/**
 * Same as seval; except that if u > x or u < x :  y_first[n] or y_first[0] will be returned
 * @param u
 * @param x_first
 * @param x_last
 * @param y_first
 * @param y_last
 * @param b_first
 * @param b_last
 * @param c_first
 * @param c_last
 * @param d_first
 * @param d_last
 * @return
 */
template <class T, class Iterator> T seval_const_borders(const Iterator u, Iterator x_first, const Iterator x_last,
        Iterator y_first, const Iterator y_last,
        Iterator b_first, const Iterator b_last,
        Iterator c_first, const Iterator c_last,
        Iterator d_first, const Iterator d_last) {


    size_t n = (x_last - x_first);
    size_t i, j, k;
    T dx;

    if (u < x_first[0]) return y_first[0];
    else if (u >= x_first[n - 1]) return y_first[n-1];
    else {
        i = 0;
        j = n;
        do {
            k = (i + j) / 2;
            if (u < x_first[k])
                j = k;
            if (u >= x_first[k])
                i = k;
        } while (j > i + 1);
    }

    dx = u - x_first[i];

    return (y_first[i] + dx * (b_first[i] + dx * (c_first[i] + dx * d_first[i])));
}





} // end namespace mtx

#endif
