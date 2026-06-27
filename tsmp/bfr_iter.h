
#ifndef BFR_ITER_H
#define BFR_ITER_H
#include "allinclude.h"


/*! 
\file bfr_iter.h
*/

// for complex
//if (typeid(l) == typeid(std::complex<double>) ) cerr << "complex used" << endl;

/*!
\brief STL like iterator functions designed for valarray and vector; NOTE v[10], v.begin() = v[0]; v.end() = v[10]!!
The algorithms will basically use indices wich are compatible to v.begin() and v.end(); in C notation
this is for an array v[10]: v[0] and v[10]! v.end() is a pointer BEHIND the last element. The reason
is that these algorithms use while(begin != end) begin++.<br>
STL alogorithms are also using begin and end.<br>
begin is the pointer, begin++ increments; *begin refers to the value
\author B. Friedrichs
\date 24.02.2005
*/
namespace bfrtsmp {

/*!
Input a sorted frequency list, returns N frequencies used for parzening
@param[in]  f_low lower bound of frequency vector, first index you want to use from input
@param[in]  f_high upper bound of frequency vector, last index you want to use from input
@param[in]  fn target frequency (center frequency for parzening)
@param[in]  pr parzen radius around target frequency; somethinf between 0 and 0.5
@param[out] beg first index of input frequency vector used for parzening
@param[out] end last index of input frequency vector used for parzening
*/



// high speed version without any extras; especiall in main() you do not need slicing and copying
// HX will be a a big output array, then define complex<double> *result;
// and let result = &HX[i*mt_pts]; inside your loops


/*!
Does the parzening of two input vectors like hx,hx or hx ey etc.
@param[in]  in_beg start index data vector 1
@param[in]  in_end end index data vector 1
@param[in]  in2_beg start index data vector 2
@param[in]  in2_end end index data vector 2
@param[out] out_beg start index of result vector
@param[out] out_end end index of result vector
@param[in]  f_base fist index of the TOTAL input frequency vector
@param[in]  f_beg index of frequency vector where parzening can start
@param[in]  f_end index of frequency vector where parzening has to stop
@param[in]  fn_beg start index of list with target frequencies for parzening
@param[in]  fn_end stop index of list with target frequencies for parzening
@param[in]  pr parzen radius
*/
template<class T, class S> size_t parzening ( const T* in_beg,  const T* in_end,
                                              const T* in2_beg, const T* in2_end,
                                                    T* out_beg,       T* out_end,
                                              const S* f_base,        S* f_beg,
                                                    S* f_end,         S* fn_beg,  const S* fn_end,
                                              const S pr) {

    size_t j, k;
    double u, us = 0.;
    S* low  = 0, *lowsave = 0;
    S* high = 0;  // high points possibly behind last frequency; TEST!
    lowsave = f_beg;                                                         // remeber first position
    while (fn_beg != fn_end) {                                               // go along the target frequency list
      low = lower_bound(lowsave, f_end, (*fn_beg - *fn_beg * pr));            // fist input freq matches can be used for parzen
      lowsave = low;                                                          // copy the result
      high = upper_bound(low, f_end, (*fn_beg + *fn_beg * pr));               // last input freq can be used for parzen
      if (high > low && high <= f_end) {

//	      cerr << *low << " - " << *high << "  target "<< *fn_beg << "  " << (low - f_base) << endl;
        us = 0;
        while (low != high) {                                                 // now do the parzening of the input frequencies
          k = (low - f_base);                                                 // k is the first index of the input frequencies
          u = (abs(*fn_beg - *low) * M_PI) / (*fn_beg * pr);
          if (!u) *out_beg += *(in_beg + k) * *(in2_beg + k);
          else {
            //u = (sin(u)/pow(u, 4.));
						u = pow((sin(u)/u), 4.);
            *out_beg += (*(in_beg + k) * *(in2_beg + k)) * u;
          }
          us += u;
          low++;                                                              // herewith we INCREMENT k
        }
        *out_beg /= us;
      }
//			cerr << *fn_beg << "  "  << *out_beg << endl;
      ++fn_beg;    // next frequency
      ++out_beg;   // and next element in output
    }

    return 1;
}



/*!
Simple sum
@param[in] beg pointer to first element
@param[in] end pointer behind last element
returns the sum of all elements

*/
template<class T> T sum(T* beg,  const T* end) {
  
  T sum = 0;
  while (beg != end) {
    sum += *beg;
    ++beg;
  }
  return sum;

}




/*!
Detrends a vector and applies a hanning window<br> Idea taken from GMT sources.
@param[in] beg pointer to first element
@param[in] end pointer behind last element
*/
template<class T> T detrend_and_hanning (T* beg, const T* end) {

    T i;
    T tt, slope = (T)0., sumtx = (T)0.;
    T h_period = M_PI/double(end - beg), h_scale = (T)2., hc = (T)0., hw = (T)0.;
    T* start = beg;
    //h_scale = sqrt(8./3.); this was originally used by GMT fft routine

    /* hence that h_scale has to be re-used after using the fft in the sense
      that the result of the ft has to multiplied with
      sqrt(h_scale / (sampling_freq * window_length))

      This routine with h_scale 2.0 actaully returns 1 as integral of the
      hanning window so scaling should not be neccessary

    */

    T t_factor = 2.0/((end - beg) - 1);
    T mean = sum(beg, end)/ (end - beg);


    i = 0;
    while (beg != end) {
        tt = i * t_factor - 1.;
        slope += tt * tt;
        sumtx += tt * *beg;
        ++beg;
        ++i;
    }
    slope = sumtx / slope;
    i = 0;
    beg = start;
    while (beg != end) {
        tt = i * t_factor - 1.;
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        *beg -= (mean + tt * slope);                  // detrend
        *beg *= hw;                                   // hanning
        ++i;
        ++beg;
    }
    return h_scale;
}

/*!
Detrends a vector; Sources from GMT
@param[in] beg start of part to detrend
@param[in] end end

*/
template <class T> int detrend (T* beg, T* end) {

    T i;
    T* start = beg;
    
    T tt, slope = (T)0., sumtx = (T)0.;
    
    T t_factor = 2.0/((end - beg) - 1);
    T mean = sum(beg, end)/ (end - beg);
    
  i = 0.;
  while (beg != end) {
        tt = i * t_factor - 1.;
        slope += tt * tt;
        sumtx += tt * *beg;
        ++beg;
        ++i;
    }
    slope = sumtx / slope;
    i = 0.;
    beg = start;
    while (beg != end) {
        tt = i * t_factor - 1.;
        *beg -= (mean + tt * slope);                  // detrend
        ++i;
        ++beg;
    }    

    return 1;
}

/*!
Hanning window on a vector; Sources from GMT
@param[in] beg start of part to hanning
@param[in] end end

*/
template <class T> int hanning (T* beg, T* end) {

    T i;
    T tt, slope = (T)0., sumtx = (T)0.;
    T h_period = M_PI/double(end - beg), h_scale = (T)2., hc = (T)0., hw = (T)0.;
    //h_scale = sqrt(8./3.); this was originally used by GMT fft routine

    /* hence that h_scale has to be re-used after using the fft in the sense
      that the result of the ft has to multiplied with
      sqrt(h_scale / (sampling_freq * window_length))

      This routine with h_scale 2.0 actaully returns 1 as integral of the
      hanning window so scaling should not be neccessary
    */

    i = 0.;
    while (beg != end) {
        hc = cos(i * h_period);
        hw = h_scale * (1. - hc * hc);
        *beg *= hw;                                   // hanning
        ++i;
        ++beg;
    }
    return h_scale;
}



// Author   : Jialong He, 
// Date     : July, 1999. 
//
// Contact  : Jialong_He@bigfoot.com, Jialong_He@homemail.com
// Web Page : www.bigfoot.com/~Jialong_He
// modified : Bernhard Friedrichs, April 2005
/*!
Calculates FIR filter coefficients for low pass, high pass, band pass and band stop
@param[in] beg begin of coef vector
@param[in] end end of coef vector

*/
template <class T> size_t fir_dsgn(T* beg, T* end, const double samplefreq, double CutLow, double CutHigh) {


  /*---------------------------------------------*/
  /* adjust the number of coefficients to be ODD */
  /*---------------------------------------------*/

  if ( !((end - beg) % 2)) {
    cerr << "fir_dsgn-> coefficiencies vector must have odd length like 21 or 471; actual length: " << (end - beg) << endl;
    return 0;
  }


  double Sum, TmpFloat;
  size_t CoefNum, HalfLen, Cnt;
  

  CoefNum = (end -beg);

  //adjust but why? with 2??
  CutLow = (CutLow / samplefreq) * 2.;

if (CutHigh > samplefreq/2) {
    CutHigh = samplefreq/2;
}

  CutHigh = (CutHigh / samplefreq) * 2.;

//cerr << samplefreq << " " << CutLow << "  " << CutHigh << endl;
  
  HalfLen = (CoefNum - 1) / 2;



  /*------------------*/
  /*  Lowpass filter  */
  /*------------------*/
  if ((CutLow == 0.0) && (CutHigh < 1.0)) {

      beg[HalfLen] = CutHigh;
      for (Cnt=1; Cnt<=HalfLen; Cnt++) {
        TmpFloat = M_PI * Cnt;
        beg[HalfLen + Cnt] = sin(CutHigh * TmpFloat) / TmpFloat;
        beg[HalfLen - Cnt] = beg[HalfLen + Cnt];
      }

      /*------------------------------*/
      /* multiplying with a window    */
      /*------------------------------*/
      TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
      Sum = 0.0;
      for (Cnt=0; Cnt<CoefNum; Cnt++) {
        beg[Cnt] *= (0.54 - 0.46 * cos(TmpFloat * Cnt));
        Sum += beg[Cnt];
      }

      /*------------------------------*/
      /* Normalize GAIN to 1          */
      /*------------------------------*/
      for (Cnt=0; Cnt<CoefNum; Cnt++) {
        beg[Cnt] /= fabs (Sum);
      }

  return (CoefNum);

  }  /* if Lowpass */


  /*------------------*/
  /* Highpass filter  */
  /*------------------*/
  else if ((CutLow > 0.0) && (CutHigh == 1.0)) {

      beg[HalfLen] = CutLow;
      for (Cnt=1; Cnt<=HalfLen; Cnt++) {
        TmpFloat = M_PI * Cnt;
        beg[HalfLen + Cnt] = sin(CutLow * TmpFloat) / TmpFloat;
        beg[HalfLen - Cnt] = beg[HalfLen + Cnt];
      }

      /*------------------------------*/
      /* multiplying with a window    */
      /*------------------------------*/
      TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
      Sum = 0.0;
      for (Cnt=0; Cnt<CoefNum; Cnt++) {
        beg[Cnt] *= -(0.54 - 0.46 * cos(TmpFloat * Cnt));
        if (Cnt % 2 == 0) Sum += beg[Cnt];  /* poly(-1), even +, odd -*/
        else Sum -= beg[Cnt] ;
      }

      beg[HalfLen] += 1;
      Sum += 1;

      /*------------------------------*/
      /* Normalize GAIN to 1          */
      /*------------------------------*/
      for (Cnt=0; Cnt<CoefNum; Cnt++) {
        beg[Cnt] /= fabs (Sum);
      }   

  return (CoefNum);

  } /* if HighPass */


  /*------------------*/
  /* Bandpass filter  */
  /*------------------*/
  else if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh)) {

      beg[HalfLen] = CutHigh - CutLow;
      for (Cnt=1; Cnt<=HalfLen; Cnt++) {
        TmpFloat = M_PI * Cnt;
        beg[HalfLen + Cnt] = 2.0 * sin( (CutHigh - CutLow) / 2.0 * TmpFloat) *
            cos( (CutHigh + CutLow) / 2.0 * TmpFloat) / TmpFloat;
        beg[HalfLen - Cnt] = beg[HalfLen + Cnt];
      }

      /*------------------------------*/
      /* multiplying with a window    */
      /*------------------------------*/
      TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
      Sum = 0.0;
      for (Cnt=0; Cnt<CoefNum; Cnt++) {
        beg[Cnt] *= (0.54 - 0.46 * cos(TmpFloat * Cnt));
        Sum += beg[Cnt];
      }
  return (CoefNum);

  } /* if */

  /*------------------*/
  /* Bandstop filter  */
  /*------------------*/
  else if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow > CutHigh)) {

      beg[HalfLen] = CutLow - CutHigh;
      for (Cnt=1; Cnt<=HalfLen; Cnt++) {
        TmpFloat = M_PI * Cnt;
        beg[HalfLen + Cnt] = 2.0 * sin( (CutLow - CutHigh) / 2.0 * TmpFloat) *
            cos( (CutHigh + CutLow) / 2.0 * TmpFloat) / TmpFloat;
        beg[HalfLen - Cnt] = beg[HalfLen + Cnt];
      }

      /*------------------------------*/
      /* multiplying with a window    */
      /*------------------------------*/
      TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
      Sum = 0.0;
      for (Cnt=0; Cnt<CoefNum; Cnt++) {
        beg[Cnt] *= -(0.54 - 0.46 * cos(TmpFloat * Cnt));
        Sum += beg[Cnt];
      }

      beg[HalfLen] += 1;
      Sum += 1;

      /*------------------------------*/
      /* Normalize GAIN to 1          */
      /*------------------------------*/
      for (Cnt=0; Cnt<CoefNum; Cnt++) {
        beg[Cnt] /= fabs (Sum);
      }


  return (CoefNum);

  }  /* if */
  else {
    cerr << "fir_dsgn -> wrong filter parameters selected!!!; likely that fmax > fsample/2 " << endl;
    return 0;
  }

  return (CoefNum);    /* never reach here */
}


/*!
Applies FIR filter coefficients for low pass, high pass, band pass and band stop on input<br>
can also be used for folding time series
@param[in] in_beg begin of input vector
@param[in] in_end end of input vector
@param[in] begin of filtred output vector
@param[in] end of filtred output vector
@param[in] beg begin of coef vector
@param[in] end end of coef vector
*/
template <class T> size_t fil_fir(T* in_beg, const T* in_end, T* out_beg, const T* out_end, T* coef_beg, const T* coef_end) {

size_t order = (coef_end - coef_beg);


// save beginnings of all arrays
T* coef_start = coef_beg;
T* in_start = in_beg;
T* out_start = out_beg;
T xsum = 0;
// data for the filter size
T *myfil = new T[order];
T* my_start = myfil;
T* my_stop = my_start + order;
  if (order){
    cerr << "filfir -> using external coefficients, order (" << order  << ")" << endl;
  }

  if ((in_end - in_beg) < order) {
    cerr << "fil_fir -> filter has " << order << " coefficients, your time series only "
      << in_end - in_beg << " points; return 0" << endl;
    return 0;
  }

  if ( ((in_end - in_beg) - order + 1) > (out_end - out_beg)) {

      cerr << "fir_fil -> output vector to small: needed size: " << ((in_end - in_beg) - order + 1) << " actual size: " << (out_end - out_beg) << endl;
      return 0;
  }
  if ( ((in_end - in_beg) - order + 1) < (out_end - out_beg)) {

      cerr << "fir_fil -> output vector to might be to BIG: needed size: " << ((in_end - in_beg) - order + 1) << " actual size: " 
      << (out_end - out_beg) << " continued" << endl;
  }  
  cerr << "fil_fir output: " << (out_end - out_beg) << " samples " << endl;




  // fold the input signal with the filter function

  while (in_beg != (in_end - order)) {

      while (coef_beg != coef_end) *myfil++ = *in_beg++ * *coef_beg++;

      // reset position
      myfil = my_start;
      
      while (myfil != my_stop){
          xsum += *myfil;
          myfil++;
      }
      *out_beg = xsum;
      xsum = 0;
      // reset position again
      myfil = my_start;
      coef_beg = coef_start;

      // move along the array positions
      ++out_beg;
      //in_beg = ++in_start;
      in_start += 2;
      in_beg = in_start;
    }
    
    delete[] myfil;

return order;
}







} // end namespace
#endif

