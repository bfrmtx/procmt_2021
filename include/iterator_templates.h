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


#ifdef _msvc
#define _USE_MATH_DEFINES
#endif

#include <numeric>
#include <valarray>
#include <vector>
#include <cmath>
#include <iostream>
#include <cfloat>
#include <iterator>


namespace iter {
  
template <typename T, typename const_iterator>
std::vector<T> vector_from_indices(const_iterator v, const std::vector<std::size_t> &sel_size, const std::vector<std::size_t> &indices, const size_t &sel_size_index_pos  ) {

  std::vector<T> data(sel_size[sel_size_index_pos]);   // create vector of actual size
  std::size_t sum = 0;
  for (size_t i = 0; i < sel_size_index_pos; ++i) sum += sel_size[i];
  // sum points now to the indices[sum] where my indices are starting

  size_t j = 0;
  for (size_t i = sum; i < sum + sel_size[sel_size_index_pos]; ++i) {
    data[j++] = *(v + std::ptrdiff_t(indices[i]));
  }

  return data;
}


template <typename T, typename const_iterator>
std::vector<T> tilt_E_vector(std::vector<T> v, const std::vector<std::size_t> &sel_size, const std::vector<std::size_t> &indices, const std::vector<double> &freqs, const size_t &sel_size_index_pos  ) {
  
  std::vector<T> fs(sel_size[sel_size_index_pos]);   // create vector of actual size
  std::size_t sum = 0;
  for (size_t i = 0; i < sel_size_index_pos; ++i) sum += sel_size[i];
  
}  
  

/*!
 * force first element ONLY comparison ; the default operator checks in case == second in C++14
 */
template<typename T> struct cmp_pair_first_lt
{
    bool operator()(const T &lhs, const T &rhs)
    {
        return (lhs.first < rhs.first);
    }
};

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
    Iterator iterMidth = first + ((std::size_t) (dWindowLen / 2));
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
    Iterator iterMidth = first + ( (std::size_t(dWindowLen) / 2));
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
    std::size_t i = 0;
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

    std::size_t n = (last - first);
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
    std::vector<T> w (std::distance(first, last));
    copy(first, last, w.begin());
    sort(w.begin(), w.end());
    if (w.size() % 2 == 0)       // array is even
        med = (w[w.size()/2] + w[w.size()/2 - 1])/T(2);
    else
        med = w[w.size()/2];  // integer divison should not round up
    return med;
}

template<typename T, typename Iterator> T median_if (Iterator first, const Iterator last, std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend) {


    T med;
    std::size_t a(std::distance(first, last));
    std::size_t b(std::distance(selected_cbeg, selected_cend));

    if(a != b) exit(1);

    std::vector<T> w;
    w.reserve(a);


    while (first != last) {
        if (*selected_cbeg++) w.emplace_back(*first);
        ++first;
    }

    sort(w.begin(), w.end());
    if (w.size() % 2 == 0)       // array is even
        med = (w[w.size()/2] + w[w.size()/2 - 1])/T(2);
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



template <typename T, typename T2=T>
struct acc_var
{
    T2 sums;

    double M1, M2, M3, M4;
    std::size_t n;

    // default constructor initializes all values
    acc_var() : sums(0), M1(0), M2(0), M3(0), M4(0), n(0) { }

    // add another number
    T2 operator()(const T& x)
    {
        std::size_t n1 = n;
        ++n;
        sums += x;
        double delta = x - M1;
        double delta_n = delta / n;
        double delta_n2 = delta_n * delta_n;
        double term1 = delta * delta_n * n1;
        M1 += delta_n;
        M4 += term1 * delta_n2 * (n*n - 3*n + 3) + 6. * delta_n2 * M2 - 4. * delta_n * M3;
        M3 += term1 * delta_n * (n - 2) - 3. * delta_n * M2;
        M2 += term1;

        return sums;
    }

    void clear() {
        sums = (T)0;
        n = 0;
        M1 = 0;
        M2 = 0;
        M3 = 0;
        M4 = 0;
    }

    T sum() const {
        return sums;
    }
    double mean() const
    {
        return M1;
    }

    double skewness() const
    {
        return sqrt(double(n)) * M3 / pow(M2, 1.5);
    }

    double kurtosis() const
    {
        return   ((double(n) * M4) /( n * (this->M2 *  this->M2))) - 3.0;
    }

    double variance() const
    {
        return M2 / ( ((double)n) - 1.0);
    }

    T stddev() const
    {
        return sqrt(this->variance());
    }

    // operator<< to print the statistics to screen:
    // denoted friend just to be able to write this inside
    // the class definition and thus not to need to write
    // the template specification of accumulator...
    friend std::ostream& operator<<(std::ostream& out,
                                    const acc_var& a)
    {
        out << " n         = " << a.n << std::endl
            << " sum       = " << a.sum() << std::endl
            << " mean      = " << a.mean() << std::endl
            << " variance  = " << a.variance() << std::endl
            << " std dev   = " << a.stddev() << std::endl
            << " skewness  = " << a.skewness() << std::endl
            << " kurtosis  = " << a.kurtosis() << std::endl;
        return out;
    }

};







template <typename T>
struct acc_var_t
{
    T sums;

    double M1, M2, M3, M4;
    std::size_t n;

    // default constructor initializes all values
    acc_var_t() : sums(0), M1(0), M2(0), M3(0), M4(0), n(0) { }

    // add another number
    T operator()(const T& x, T m)
    {
        std::size_t n1 = n;
        ++n;
        sums += x;
        double delta;
        if (n == 1) delta = x - m/9.;
        else delta = x - M1;
        double delta_n = delta / n;
        double delta_n2 = delta_n * delta_n;
        double term1 = delta * delta_n * n1;
        M1 += m/9.;
        M4 += term1 * delta_n2 * (n*n - 3*n + 3) + 6. * delta_n2 * M2 - 4. * delta_n * M3;
        M3 += term1 * delta_n * (n - 2) - 3. * delta_n * M2;
        M2 += term1;

        return sums;
    }

    void clear() {
        sums = (T)0;
        n = 0;
        M1 = 0;
        M2 = 0;
        M3 = 0;
        M4 = 0;
    }

    T sum() const {
        return sums;
    }
    double mean() const
    {
        return M1;
    }

    double skewness() const
    {
        return sqrt(double(n)) * M3 / pow(M2, 1.5);
    }

    double kurtosis() const
    {
        return   ((double(n) * M4) /( n * (this->M2 *  this->M2))) - 3.0;
    }

    double variance() const
    {
        return M2 / ( ((double)n) - 1.0);
    }

    T stddev() const
    {
        return sqrt(this->variance());
    }

    // operator<< to print the statistics to screen:
    // denoted friend just to be able to write this inside
    // the class definition and thus not to need to write
    // the template specification of accumulator...
    friend std::ostream& operator<<(std::ostream& out,
                                    const acc_var_t& a)
    {
        out << " n         = " << a.n << std::endl
            << " sum       = " << a.sum() << std::endl
            << " mean      = " << a.mean() << std::endl
            << " variance  = " << a.variance() << std::endl
            << " std dev   = " << a.stddev() << std::endl
            << " skewness  = " << a.skewness() << std::endl
            << " kurtosis  = " << a.kurtosis() << std::endl;
        return out;
    }

};











template <typename T, typename T2=T>
struct acc_covar
{
    acc_var<T> x_stats;
    acc_var<T> y_stats;
    double S_xy;
    std::size_t n;

    // default constructor initializes all values
    acc_covar() : S_xy(0), n(0) { }

    // add another number
    void operator()(const T& x, const T& y)
    {
        S_xy += (x_stats.mean() -x)*(y_stats.mean() - y)*double(n)/double(n+1);

        x_stats(x);
        y_stats(y);
        n++;


    }


    double slope() const {
        double S_xx = x_stats.variance()*(n - 1.0);

        return S_xy / S_xx;
    }


    double abscissa() const {
        return y_stats.mean() - this->slope()*x_stats.mean();

    }

    double covariance() const {
        return S_xy / (n -1.0);
    }

    double correlation () const {
        double t = x_stats.stddev() * y_stats.stddev();
        return S_xy / ( (n-1.0) * t );
    }



    friend std::ostream& operator<<(std::ostream& out,
                                    const acc_covar& a)
    {
        out << " n         = " << a.n << std::endl
            << " slope     = " << a.slope() << std::endl
            << " abscissa  = " << a.abscissa() << std::endl
            << " correl.   = " << a.correlation() << std::endl
            << " covariance= " << a.covariance() << std::endl
            << " x: " << std::endl
            << a.x_stats
            << " y: " << std::endl
            << a.y_stats;
        return out;
    }


};




template <typename T, typename Iterator>
T variance_welford(Iterator first, const Iterator last, T& mean)
{
    std::size_t n = 0;
    T elem = 0, s = 0, sum = 0, elem_prev = 0;
    while (first != last) {
        ++n;
        elem_prev = elem;
        elem += (*first - elem_prev) / n;
        s += (*first - elem_prev) * (*first - elem);
        sum += *first;
        first++;
    }
    mean = sum / n;
    return s / (n-1);
}
/*!
 * \brief calculate the variance of a distribution
 * \param first  = first point
 * \param last   < last data point
 * \param mean get the mean of the distribution
 * \return variance
 */


template <typename T, typename Iterator> T variance_old(Iterator first, const Iterator last, T& mean )  {


    if ( !(last - first)) {
        mean = T(0);
        return T(0);
    }


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

    T var = variance_welford<T>(first, last, mean) ;

    return sqrt(var);
}

template <class T, class Iterator> bool variance2D(Iterator firstx, const Iterator lastx, Iterator firsty, const Iterator lasty,
                                                   T &varx, T& vary, T& stdx, T& stdy, T& meanx, T& meany, T& Qx, T& Qy, T& Qxy, std::size_t& n, T& covxy,
                                                   T& corr_coeff, T& reg_coeff, T& stdxy, T& stdyx,
                                                   T& abscissa, T& std_abscissa, T& std_reg_coeff)  {



    if ( !(lastx - firstx) || !(lasty - firsty)) {

        varx = T(0);
        vary = T(0);
        stdx = T(0);
        stdy = T(0);
        meanx = T(0);
        meany = T(0);
        Qx = T(0);
        Qy = T(0);
        Qxy = T(0);
        n = 0;
        covxy = T(0);
        corr_coeff = T(0);
        reg_coeff = T(0);
        stdxy = T(0);
        stdyx = T(0);
        abscissa = T(0);
        std_abscissa = T(0);
        std_reg_coeff = T(0);
        return false;


    }


    T add = T (0);
    T sumx = accumulate(firstx, lastx, add);
    T sumy = accumulate(firsty, lasty, add);

    n = (std::size_t)(lastx - firstx);

    T powsumx = T(0);
    T powsumy = T(0);
    T sum_of_squaresx = T (0);
    T sum_of_squaresy= T (0);

    T sum_of_product = T (0);

    T mysize  = T(lastx - firstx);
    T one = T(1);
    T two = T (2);

    abscissa = T(0);
    varx = T (0);
    stdx = T (0);
    vary = T (0);
    stdy = T (0);
    covxy = T (0);
    stdxy = T(0);
    stdyx = T (0);
    std_abscissa = T(0);
    std_reg_coeff = T(0);


    corr_coeff = T(0);
    reg_coeff  = T(0);

    Qxy = T(0);
    Qx = T(0);
    Qy = T(0);



    // copy and modify
    while (firstx != lastx) {

        sum_of_product  += *firstx * *firsty;
        sum_of_squaresx += pow(*firstx++, 2.);
        sum_of_squaresy += pow(*firsty++, 2.);

    }

    powsumx = pow(sumx,2.);
    powsumx /= mysize;

    powsumy = pow(sumy,2.);
    powsumy /= mysize;

    meanx = sumx;
    meanx /= mysize;

    meany = sumy;
    meany /= mysize;

    Qx = sum_of_squaresx - ((powsumx));
    Qy = sum_of_squaresy - ((powsumy));
    Qxy = sum_of_product - ((sumx * sumy)/mysize);




    // precision of double is not perfect
    if (abs(Qx) < 1E-10) {
        Qx = 0;


    }
    else {
        varx = ( (one/(mysize - one)) * Qx) ;
        stdx = sqrt(varx);
    }

    // precision of double is not perfect
    if (abs(Qy) < 1E-10) {
        Qy = 0;

    }
    else {
        vary = ( (one/(mysize - one)) * Qy) ;
        stdy = sqrt(vary);

    }

    if (abs(Qxy) < 1E-10) {
        Qxy = 0;

    }
    else {
        covxy = Qxy / (mysize - one);

    }

    if (stdy && stdy) {

        corr_coeff = covxy / (stdx * stdy);
    }

    if (stdy && stdy && Qxy) {

        reg_coeff = Qxy / Qx;
    }

    if (Qy && Qy && Qxy) {

        stdyx = sqrt(  (Qy -( pow(Qxy, two) / Qx ))/ (mysize - two));
        stdxy = sqrt(  (Qx -( pow(Qxy, two) / Qy ))/ (mysize - two));
        std_abscissa = stdyx * sqrt ( one/mysize + (pow(meanx,two)/Qx));
        std_reg_coeff = stdxy / sqrt(Qx);
        abscissa = meany - reg_coeff * meanx;

    }
    return true;


}


/*!
 * \brief fold two vectors - each element by each element and sum up
 * no check if vectors have same length
 * \param first  = first point
 * \param last   < last data point
 * \param coef_first  = first point of other vector
 * \param coef_last   < last data point of other vector
 * \return  sum of product
 */

template <typename T, typename Iterator> inline T fold(Iterator first, const Iterator last, Iterator coef_first, const Iterator coef_last) {

    if (std::distance(first, last) < std::distance(coef_first, coef_last)) return T(0);

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
template <typename T, typename Iterator > std::size_t fir_dsgn(Iterator first, Iterator last, const T samplefreq, T CutLow, T CutHigh) {


    /*---------------------------------------------*/
    /* adjust the number of coefficients to be ODD */
    /*---------------------------------------------*/

    if ( !((last - first) % 2)) {
        std::cerr << "fir_dsgn-> coefficiencies vector must have odd length like 21 or 471; actual length: " << (last - first) << std::endl;
        return 0;
    }


    double Sum, TmpFloat;
    std::size_t CoefNum, HalfLen, Cnt;


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
template <class T, class const_iterator_x> T m(const const_iterator_x x, const const_iterator_x y, const std::size_t index) {

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

template <class T, class const_iterator_x, class iterator_s> std::size_t aspline(
        const const_iterator_x x_first, const const_iterator_x x_last,
        const const_iterator_x y_first, const const_iterator_x y_last,
        iterator_s b_first, iterator_s b_last,
        iterator_s c_first, iterator_s c_last,
        iterator_s d_first, iterator_s d_last) {


    long n = (std::size_t)(x_last - x_first);
    std::size_t i;

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

        for (i = 2; i < (std::size_t)n - 2; i++) {

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

        for (i = 0; i < (std::size_t)n - 1; i++) {
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
 * @param b_first
 * @param c_first
 * @param d_first
 * @return the interpolated v (u,v)
 */
template <class T, class const_iterator_x>
T seval(const const_iterator_x u,
        const_iterator_x x_first, const const_iterator_x x_last,
        const_iterator_x y_first,
        const_iterator_x b_first,
        const_iterator_x c_first,
        const_iterator_x d_first) {


    std::size_t n = (x_last - x_first);
    std::size_t i, j, k;
    T dx;

    // binary search is performed to determine the proper interval.
    //binary search

    if (*u < x_first[0]) i = 0;
    else if (*u >= x_first[n - 1]) i = n - 1;
    else {
        i = 0;
        j = n;

        do {
            k = (i + j) / 2;
            if (*u < x_first[k])
                j = k;
            if (*u >= x_first[k])
                i = k;
        } while (j > i + 1);
    }

    dx = *u - x_first[i];

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


    std::size_t n = (x_last - x_first);
    std::size_t i, j, k;
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







} // end namespace

#endif
