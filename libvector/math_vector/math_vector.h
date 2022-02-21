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

#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include <QObject>
#include <numeric>
#include <limits>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <complex>
#include <algorithm>
#include "iterator_templates.h"
#include "statmaps.h"
#include <utility>
#include <QDebug>


// this is an early lib - do not try to include latet projects


/*!
 * \brief The math_vector class contains vector functions for doubles
 */
class math_vector : public QObject
{
    Q_OBJECT
public:
    math_vector(QObject *parent = Q_NULLPTR);

    ~math_vector();

    size_t akima_vector(const std::vector<double> &vcalfreqs, const std::vector<std::complex<double> > &vcalentries,
                        const std::vector<double> &newfreqs, std::vector<std::complex<double> > &calibipl) const;

    size_t akima_vector_double(const std::vector<double> &vcalfreqs, const std::vector<double>  &vcalentries,
                               const std::vector<double> &newfreqs, std::vector<double> &calibipl) const;

    /*!
     * \brief merge_sorted_low_to_high_outside merges two vectors o and i to iiioooi ; vector items are sorted from small to large
     * \param orig input data like measured cal frequencies
     * \param inmerge input data like master cal data
     * \param out a vector consisting of measured cal data inside and master data outside in a new vector
     * \return merged vector
     */
    std::vector<double> merge_sorted_low_to_high_outside(const std::vector<double> &orig, const std::vector<double> &inmerge);


    int merge_sorted_low_to_high_outside_join(const std::vector<double> &forig, const std::vector<double> &finmerge, std::vector<double> &foutmerge,
                                              const std::vector<double> &aorig, const std::vector<double> &ainmerge, std::vector<double> &aoutmerge,
                                              const std::vector<double> &porig, const std::vector<double> &pinmerge, std::vector<double> &poutmerge,
                                              const bool overlapping = true);

    /*!
     * \brief sort_triple sort three vectors by f from low to high
     * \param f
     * \param a
     * \param p
     */


    bool sort_vectors(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p, std::vector<double> &da, std::vector<double> &dp);

    bool sort_vectors(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p);

    bool sort_vectors(std::vector<int> &l, std::vector<double> &a, std::vector<double> &p);

    bool sort_vectors(std::vector<int> &l, std::vector<double> &a);

    bool sort_vectors(std::vector<double> &f, std::vector<double> &a);


    /*!
     * \brief remove_duplicates_from_sorted f is a sorted low to high vector
     * \param f duplicates to be removed
     * \param a acompanied vector
     * \return new size
     */
    bool remove_duplicates_from_sorted(std::vector<double> &f, std::vector<double> &a, const bool emit_message = true);


    bool remove_duplicates_from_sorted(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p, const bool emit_message = true);

    bool remove_duplicates_from_sorted(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p,
                                       std::vector<double> &da, std::vector<double> &dp,
                                       const bool emit_message = true);


    double mean(const std::vector<double> &data);

    int channel;

    /*!
     * \brief gen_equidistant_logvector generate a equally logarithic spaced vector
     * \param start from start (included)
     * \param stop   to stop (included)
     * \param steps_per_decade steps per decade (like 11 for MSF-06e or 7 for MFS-07e)
     * \return vector of frequencies (or null vector if failed)
     */
    std::vector<double> gen_equidistant_logvector(const double start, const double stop, const size_t steps_per_decade);


    void lin_round_min(const double& min,
                       const double one_ten_hundred_etc, double& new_min);



    void lin_round_max(const double& max,
                       const double one_ten_hundred_etc, double& new_max);


    void lin_round_min_max(const double& min, const double& max,
                           const double one_ten_hundred_etc, double& new_min, double& new_max);


    /*!
     * \brief vector_size_if_min check for minimum sizes, especially for a selection vector
     * \param xcbeg
     * \param xcend
     * \param selected_cbeg
     * \param selected_cend
     * \param min_valid_elements
     * \param control <br>
     *  0: x.size = true.size, trues and sizes >=  min_valid_elements <br>
     *  1: x.size = true.size, sizes >=  min_valid_elements, DON'T count trues <br>
     *  2: x.size = sizes >=  min_valid_elements, DON'T check select and DON'T count trues <br>
     *
     * \return true or false
     */
    bool vector_size_if_min(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                            std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                            const size_t min_valid_elements = 4, const int control = 0);


    bool vector_size_if_min_cplx(std::vector<std::complex<double>>::const_iterator xcbeg, std::vector<std::complex<double>>::const_iterator xcend,
                                 std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                 const size_t min_valid_elements = 4, const int control = 0);





    /*!
     * \brief vector_sizes_if_min for minimum sizes, especially for a selection vector; this version always checks x.size = y.size
     * \param xcbeg
     * \param xcend
     * \param ycbeg
     * \param ycend
     * \param selected_cbeg
     * \param selected_cend
     * \param min_valid_elements
     * \param control <br>
     *  0: x.size = true.size, trues and sizes >=  min_valid_elements <br>
     *  1: x.size = true.size, sizes >=  min_valid_elements, DON'T count trues <br>
     *  2: x.size = sizes >=  min_valid_elements, DON'T check select and DON'T count trues <br>
     *
     * \return true or false
     */
    bool vector_sizes_if_min(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                             std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                             std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                             const size_t min_valid_elements = 4, const int control = 0);



    double dmean(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                 std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                 const size_t min_valid_elements = 4, const int control = 0);

    /*!
     *  mk_variance_data contains all data to transport from statistiks using
     * variance
     * \return map initialized with DBL_MAX
     */
    statmap mk_variance_data(bool make_y = false);


    /*!
     * \brief mk_regression_data combines tow variance data and adds regresssion template such as slope etc
     * \param xdata variance data (deep copy)
     * \param ydata varaiance data (deep copy)
     * \return a statmap
     */
    statmap mk_regression_data(const statmap &x_variance_data, const statmap &y_variance_data, bool &is_ok);


    /*!
         * \brief mk_regression_result
         * \param regression_data
         * \return a statmap with the distribution (Student T upper Quantile) depending results
         */
    statmap mk_regression_result(const statmap &regression_data, bool &is_ok);



public slots:

    /*!
     * \brief slot_parzen_vector
     * \param freqs input fft freqs
     * \param target_freqs tartget MT (EDI) freqs
     * \param prz_radius like 0.1; if too small it will be changed adaptively
     * \return
     */
    size_t slot_parzen_vector(const std::vector<double> &freqs, const std::vector<double> &target_freqs, const double &prz_radius);

    size_t take_five(const std::vector<double> &freqs, const std::vector<double> &target_freqs, const size_t odd_min_lines, const double &bw);

signals:

    void frequencies_selected(const std::vector<double> &selfreqs);
    void frequencies_fft_indices_selected(const std::vector<size_t> &selindices, const std::vector<size_t> &selindices_sizes);

    void parzenvector_created(const double &f, const std::vector<double> &przv);
    void parzen_radius_changed(const double &old_parzen_radius, const double &parzen_radius);
    void math_message(const int &channel, const QString &message);
    void harmonic_f_vector_created(const std::vector<double> &selfreqs);


};


class regplot_data
{
public:
    regplot_data();

    ~regplot_data();


    /*! \brief mkplot_slope refer to regplot::mkplot_slope_if()
     *
     */
    bool mkplot_slope(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                      std::vector<double> &x, std::vector<double> &y,
                      const statmap &regression_data,
                      const int ntimes_size = 1,
                      const double newmin = 0.0, const double newmax = 0.0,
                      const double center_x = 0.0, const double center_y = 0.0,
                      const int slope_type_0s_1u_2l = 0, const double extra_offsety = 0.0);





    /*!
     * \brief mkplot_slope_if
     * \param xcbeg                 data x vector begin
     * \param xcend                 data x vector end
     * \param x                     newly created plot vector x
     * \param y                     newly created plot vector y
     * \param selected_cbeg         selection vector begin (true == slected for plot)
     * \param selected_cend         selection vector end   (true == slected for plot)
     * \param regression_data       data needed for calculating y from x
     * \param ntimes_size           plot vector will have higher resolution (6 means from 8 input plotvecor will 48); selection vector will be ignored; if -1 slope will have two points for a straight line
     * \param newmin                if ntimes_size we can lot with new min (w/o selection vector)
     * \param newmax                if ntimes_size we can lot with new max (w/o selection vector)
     * \param center_x              middle of slope from where we draw; e.g. mean x or 0;
     * \param center_y              middle of slope from where we draw; e.g. mean y or abscissa;
     * \param slope_type_0s_1u_2l   0: slope y = mx + b; one by one (w w/o selected) or ntimes_size<br>
     * 1: slope with lower confidence ref. 2)<br>
     * 2: slope with upper confidence (e.g. with center_x = mean x and center_y = mean y OR 0, abscissa),<br>
     * 3: slope with lower confidence including the confidence interval of the mean (line does NOT run through the center but above/below the center) <br>
     * 4: slope with upper ...
     * \param extra_offsety adds a distance to the slope - this increases the confidence intervall if you say so; center x/y NOT to be added (ignored)
     * \return
     */

    bool mkplot_slope_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                         std::vector<double> &x, std::vector<double> &y,
                         std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,

                         const statmap &regression_data,
                         const int ntimes_size = 1,
                         const double newmin = 0.0, const double newmax = 0.0,
                         const double center_x = 0.0, const double center_y = 0.0,
                         const int slope_type_0s_1u_2l = 0, const double extra_offsety = 0.0);



    // cache variables;
    double d_n = 0.0;
    double d_slope = 0.0;
    double d_abscissa = 0.0;
    double d_lower_confidence = 0.0;
    double d_upper_confidence = 0.0;
    double d_varx = 0.0;
    double d_student_t_inv = 0.0;
    double d_meanx = 0.0;
    double d_minx = 0.0;
    double d_maxx = 0.0;

};


class two_pass_variance
{
public:
    two_pass_variance();

    ~two_pass_variance();


    size_t n = 0;
    double d_n = 0.0;             //!< count as double
    double d_sum = 0.0;
    double d_mean = 0.0;
    double d_variance_population = 0.0;
    double d_variance = 0.0;
    double d_stddev_population = 0.0;
    double d_stddev = 0.0;
    double d_skewness = 0.0;
    double d_kurtosis = 0.0;
    double d_dmean = 0.0;
    double ep = 0.0;
    double d_min = 0.0;
    double d_max = 0.0;
    double d_range = 0.0;

    statmap variance_data;

    statmap variance(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                     const double median_or_mean_initvalue = 0.0);

    statmap variance_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                        std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                        const double median_or_mean_initvalue = 0.0);

    friend std::ostream& operator<<(std::ostream& out,
                                    const two_pass_variance& a);

    void clear();

private:
    void calc_all();
};

class select_in_out_reg
{
public:
    select_in_out_reg();

    ~select_in_out_reg();


    bool mkselect_in_out_reg(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                             std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                             std::vector<bool>::iterator selected_cbeg, std::vector<bool>::iterator selected_cend,
                             const statmap &regression_data,
                             const double center_x = 0.0, const double center_y = 0.0,
                             const int slope_type_0s_1u_2l = 0, double extra_offsety = 0.0);


    double d_n = 0.0;
    double d_slope = 0.0;
    double d_abscissa = 0.0;
    double d_lower_confidence = 0.0;
    double d_upper_confidence = 0.0;
    double d_varx = 0.0;
    double d_student_t_inv = 0.0;
    double d_meanx = 0.0;
};


class two_pass_linreg
{
public:
    two_pass_linreg();

    ~two_pass_linreg();


    statmap linreg(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                   std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                   const double median_or_mean_subtract_y = 0.0);

    statmap linreg_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                      std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                      std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                      const double median_or_mean_subtract_y = 0.0);


    double test_against_other_slope(const double& other_slope, statmap &result);


    double S_xy;    //!< (covaiance * n) of this sample
    size_t n;       //!< total elements
    double d_n;    //!< above as double

    double d_covariance;
    double d_correlation;
    double d_slope;
    double d_abscissa;
    double d_sum_alldist_from_slope;
    double d_test_against_other_slope;
    bool is_ok;
    bool has_data;

    // init goes through the constructor; so when deleting you see the destrcutor
    two_pass_variance x;
    two_pass_variance y;

    statmap regression_data; //!< contains all recursion results

private:
    bool calc_all();

};

class two_pass_variance_cplx
{
public:
    two_pass_variance_cplx();

    ~two_pass_variance_cplx();


    size_t n = 0;
    double d_n = 0.0;             //!< count as double
    std::complex<double> d_sum = 0.0;
    std::complex<double> d_mean = 0.0;
    std::complex<double> d_variance_population = 0.0;
    std::complex<double> d_variance = 0.0;
    std::complex<double> d_stddev_population = 0.0;
    std::complex<double> d_stddev = 0.0;
    std::complex<double> d_skewness = 0.0;
    std::complex<double> d_kurtosis = 0.0;
    std::complex<double> d_dmean = 0.0;
    std::complex<double> ep = 0.0;
    std::complex<double> d_min = 0.0;
    std::complex<double> d_max = 0.0;
    std::complex<double> d_range = 0.0;

    statmap variance_data;

    statmap variance(std::vector<std::complex<double>>::const_iterator xcbeg, std::vector<std::complex<double>>::const_iterator xcend,
                     const std::complex<double> median_or_mean_initvalue = 0.0);

    statmap variance_if(std::vector<std::complex<double>>::const_iterator xcbeg, std::vector<std::complex<double>>::const_iterator xcend,
                        std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                        const std::complex<double> median_or_mean_initvalue = 0.0);

    friend std::ostream& operator<<(std::ostream& out,
                                    const two_pass_variance& a);

    void clear();

private:
    void calc_all();
};


#endif // MATH_VECTOR_H
