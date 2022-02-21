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

#ifndef VECTOR_TEMPLATES_H
#define VECTOR_TEMPLATES_H
/**
 * @file vector_templates.h
 * @brief vector templates based of vector::iterators or const iterators
 *
 */


#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <iomanip>
// we make plots with Qt later
#include <QString>
#include <QStringList>
#include <map>
#include <unordered_map>
#include "statmaps.h"




namespace vect {



void lin_round_min(const double& min,
                   const double one_ten_hundred_etc, double& new_min) {

    new_min = min / one_ten_hundred_etc;
    new_min = floor(new_min) * one_ten_hundred_etc;

}

void lin_round_max(const double& max,
                   const double one_ten_hundred_etc, double& new_max) {

    new_max = max / one_ten_hundred_etc;
    new_max = ceil(new_max) * one_ten_hundred_etc;


}



void lin_round_min_max(const double& min, const double& max,
                       const double one_ten_hundred_etc, double& new_min, double& new_max) {

    new_min = min / one_ten_hundred_etc;
    new_min = floor(new_min) * one_ten_hundred_etc;
    new_max = max / one_ten_hundred_etc;
    new_max = ceil(new_max) * one_ten_hundred_etc;


}


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
                        const size_t min_valid_elements = 4, const int control = 0) {

    int xdist = std::distance(xcbeg, xcend);
    if ( xdist < min_valid_elements) {
        std::cerr << "x is smaller than: " << xdist << " required: " << min_valid_elements << std::endl;
        return false;
    }
    else if (control == 2) {
        return true;
    }

    int seldist = std::distance(selected_cbeg, selected_cend);

    if (xdist != seldist) {
        std::cerr << " x and selection vector have different sizes: " << xdist << " " << seldist << std::endl;
        return false;

    }

    // don't check trues now
    if (control != 1) {
        int trues = std::count(selected_cbeg, selected_cend, true);
        if (trues < min_valid_elements) {
            std::cerr << "selection vector has only true selections: " << trues << " required: " << min_valid_elements << std::endl;
            return false;

        }
    }
    return true;

}

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
                         const size_t min_valid_elements = 4, const int control = 0) {

    int xdist = std::distance(xcbeg, xcend);
    if ( xdist < min_valid_elements) {
        std::cerr << "x is smaller than: " << xdist << " required: " << min_valid_elements << std::endl;
        return false;
    }

    int ydist = std::distance(ycbeg, ycend);
    if ( xdist != ydist) {
        std::cerr << "x and y not of same size: " << xdist << " " << ydist << std::endl;
        return false;
    }
    else if (control == 2) {
        return true;
    }



    int seldist = std::distance(selected_cbeg, selected_cend);
    if (xdist != seldist) {
        std::cerr << " x and selection vector have different sizes: " << xdist << " " << seldist << std::endl;
        return false;

    }
    // don't check trues now
    if (control != 1) {
        int trues = std::count(selected_cbeg, selected_cend, true);
        if (trues < min_valid_elements) {
            std::cerr << "selection vector has only true selections: " << trues << " required: " << min_valid_elements << std::endl;
            return false;

        }
    }
    return true;

}


double dmean(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
             std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
             const size_t min_valid_elements = 4, const int control = 0) {

    if (!vector_size_if_min(xcbeg, xcend, selected_cbeg, selected_cend, min_valid_elements, control)) {
        return DBL_MAX;
    }
    // x only
    if (control == 2) {
        double mymean = std::accumulate(xcbeg, xcend, 0.0);
        return mymean / double(std::distance(xcbeg, xcend));
    }
    else {
        double sum = 0.0;
        size_t cnt = 0;
        for ( ; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
            if (*selected_cbeg) {
                sum += *xcbeg;
                ++cnt;
            }
        }
        return sum / double(cnt);
    }

    return DBL_MAX;

}



/*!
 *  mk_variance_data contains all data to transport from statistiks using
 * variance
 * \return map initialized with DBL_MAX
 */
statmap mk_variance_data(bool make_y = false) {

    statmap datamap;
    int i = 0;
    // th y data starts directly after x
    if (make_y) i = g_stat::xstat_size;

    // we could loop here but this is more easy to understand
    datamap[g_stat::d_n_x + i] =  DBL_MAX ;
    datamap[g_stat::sum_x + i] =  DBL_MAX;
    datamap[g_stat::mean_x + i] =  DBL_MAX;
    datamap[g_stat::median_x + i] =  DBL_MAX;
    datamap[g_stat::variance_population_x + i] =  DBL_MAX;
    datamap[g_stat::variance_x + i] =  DBL_MAX;
    datamap[g_stat::stddev_population_x + i] =  DBL_MAX;
    datamap[g_stat::stddev_x + i] =  DBL_MAX;
    datamap[g_stat::skewness_x + i] =  DBL_MAX;
    datamap[g_stat::kurtosis_x + i] =  DBL_MAX;
    datamap[g_stat::min_x + i] =  DBL_MAX;
    datamap[g_stat::max_x + i] =  DBL_MAX;
    datamap[g_stat::range_x + i] =  DBL_MAX;


    return datamap;

}

/*!
 * \brief mk_regression_data combines tow variance data and adds regresssion template such as slope etc
 * \param xdata variance data (deep copy)
 * \param ydata varaiance data (deep copy)
 * \return a statmap
 */
statmap mk_regression_data(const statmap &x_variance_data, const statmap &y_variance_data, bool &is_ok) {

    statmap datamap;

    int i;

    // at his moment y data war treated befor as x because it was calculated independently
    if ( (x_variance_data.at(g_stat::d_n_x) == y_variance_data.at(g_stat::d_n_x)) && (x_variance_data.at(g_stat::d_n_x) >= 4.00) ) {


        for (i = 0; i < g_stat::xstat_size; ++i) {
            datamap[i] = x_variance_data.at(i);
        }


        for (i = 0; i < g_stat::xstat_size; ++i) {
            datamap[g_stat::xstat_size + i] = y_variance_data.at(i);
        }



        datamap[g_stat::covariance_xy] = DBL_MAX;
        datamap[g_stat::correlation_xy] = DBL_MAX;
        datamap[g_stat::slope_xy] =  DBL_MAX;
        datamap[g_stat::abscissa_xy] = DBL_MAX;
        datamap[g_stat::sum_alldist_from_slope_xy] = DBL_MAX;

        is_ok = true;
    }
    else is_ok = false;


    return datamap;

}

/*!
 * \brief mk_regression_result prepares data for regression from mk_regression_data
 * \param regression_data
 * \return a statmap with the distribution (Student T upper Quantile) depending results
 */
statmap mk_regression_result(const statmap &regression_data, bool &is_ok) {

    statmap datamap;


    if ( (regression_data.at(g_stat::d_n_x) == regression_data.at(g_stat::d_n_y)) && (regression_data.at(g_stat::d_n_x) >= 4.00) ) {

        for ( auto it = regression_data.cbegin(); it != regression_data.cend(); ++it ) {
            datamap[it->first] = it->second;
        }

        // copy convinience
        datamap[g_stat::delta_slope_xy] = DBL_MAX;
        datamap[g_stat::student_t_upper_quantile] =  DBL_MAX;
        datamap[g_stat::student_t_inv] = DBL_MAX;
        datamap[g_stat::student_t_upper_confidence_xy] = DBL_MAX;
        datamap[g_stat::student_t_lower_confidence_xy] = DBL_MAX;
        is_ok = true;
    }
    else {
        is_ok = false;
    }

    return statmap();
}




struct regplot_data
{



    // cache variables;
    double d_n;
    double d_slope;
    double d_abscissa;
    double d_lower_confidence;
    double d_upper_confidence;
    double d_varx;
    double d_student_t_inv;
    double d_meanx;
    double d_minx;
    double d_maxx;

    regplot_data() {}


    /*! \brief mkplot_slope refer to regplot::mkplot_slope_if()
     *
     */
    bool mkplot_slope(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                      std::vector<double> &x, std::vector<double> &y,
                      const statmap &regression_data,
                      const int ntimes_size = 1,
                      const double newmin = 0.0, const double newmax = 0.0,
                      const double center_x = 0.0, const double center_y = 0.0,
                      const int slope_type_0s_1u_2l = 0, const double extra_offsety = 0.0) {

        std::vector<bool> selected;
        return this->mkplot_slope_if(xcbeg, xcend, x, y, selected.cbegin(), selected.cend(), regression_data,
                                     ntimes_size, newmin, newmax, center_x, center_y, slope_type_0s_1u_2l, extra_offsety);

    }

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
                         const int slope_type_0s_1u_2l = 0, const double extra_offsety = 0.0)
    {

        if (std::distance(xcbeg, xcend) < 4 ) {
            std::cerr << "x is smaller than: " << 4 << std::endl;
            return false;
        }


        this->d_n = regression_data.at(g_stat::d_n_x);
        this->d_slope = regression_data.at(g_stat::slope_xy);
        this->d_abscissa = regression_data.at(g_stat::abscissa_xy);
        this->d_lower_confidence = regression_data.at(g_stat::student_t_lower_confidence_xy);
        this->d_upper_confidence = regression_data.at(g_stat::student_t_upper_confidence_xy);
        this->d_varx = regression_data.at(g_stat::variance_x);
        this->d_student_t_inv = regression_data.at(g_stat::student_t_inv);
        this->d_meanx = regression_data.at(g_stat::mean_x);
        this->d_minx = regression_data.at(g_stat::min_x);
        this->d_maxx = regression_data.at(g_stat::max_x);

        double step = 0.0;
        double xmin, xmax;

        if (ntimes_size < 0) {
            x.resize(2);
            y.resize(2);


            if ( (newmin != newmax) && newmin < newmax ) {
                xmin = newmin;
                xmax = newmax;
                step = newmax - newmin;

            }
            else {
                xmin = this->d_minx;
                xmax = this->d_maxx;
                step = this->d_maxx - this->d_minx;
            }
        }

        else if (ntimes_size == 0) {
            x.resize(std::distance(xcbeg, xcend));
            y.resize(std::distance(xcbeg, xcend));
            std::copy(xcbeg, xcend, x.begin());
        }
        else {
            x.resize(std::distance(xcbeg, xcend) * ntimes_size);
            y.resize(std::distance(xcbeg, xcend) * ntimes_size);

            if ( (newmin != newmax) && newmin < newmax ) {
                xmin = newmin;
                xmax = newmax;
                step = (xmax - xmin) / (double (x.size() -1.));

            }
            else {
                auto resultxy = std::minmax_element (xcbeg, xcend);
                step = (*resultxy.second - *resultxy.first) / (double (x.size() -1.));
                xmin = *resultxy.first;
                xmax = *resultxy.second;

            }

        }

        // plot the center slope y = mx + b
        // this line should went through xmean and ymean
        if (slope_type_0s_1u_2l == 0) {

            // plot value y by x
            if (step == 0.0) {
                // plot
                if (selected_cbeg == selected_cend) {
                    for (size_t i = 0; i < x.size(); ++i) {
                        y[i] = this->d_slope * x[i] + this->d_abscissa + extra_offsety;
                    }
                }
                // plot all x, y according to selection vector
                else {
                    for (size_t i = 0; i < x.size(); ++i, ++selected_cbeg) {
                        if(*selected_cbeg) y[i] = this->d_slope * x[i] + this->d_abscissa + extra_offsety;
                    }
                }
            }

            // plot with steps resolution
            else {
                for (size_t i = 0; i < x.size(); ++i) {
                    x[i] =  xmin + (double(i) * step);
                    y[i] = this->d_slope * x[i] + this->d_abscissa + extra_offsety;

                }
            }
        }

        // plot with upper // lower confidence
        if ((slope_type_0s_1u_2l == 1) || (slope_type_0s_1u_2l == 2)) {

            double myconfidence = this->d_lower_confidence;
            if (slope_type_0s_1u_2l == 2) myconfidence = this->d_upper_confidence;
            if (step == 0.0) {
                // no selection == all plotted
                if (selected_cbeg == selected_cend) {
                    for (size_t i = 0; i < x.size(); ++i) {
                        y[i] = myconfidence* (x[i] - center_x)  + center_y + extra_offsety;
                    }
                }
                // plot according to selection vector
                else {
                    for (size_t i = 0; i < x.size(); ++i, ++selected_cbeg) {
                        if(*selected_cbeg) y[i] = myconfidence * (x[i] - center_x)  + center_y + extra_offsety;
                    }
                }
            }
            // plot by resolution given
            else {
                for (size_t i = 0; i < x.size(); ++i) {
                    x[i] =  xmin + (double(i) * step);
                    y[i] = myconfidence * (x[i] - center_x)  + center_y + extra_offsety;
                }
            }
        }


        if ((slope_type_0s_1u_2l == 3) || (slope_type_0s_1u_2l == 4)  ) {

            double dist_from_slope;
            double denom = (this->d_n - 1.0) * this->d_varx;
            double factor = this->d_student_t_inv * sqrt(this->d_abscissa) / sqrt((this->d_n - 2.0));
            if (step == 0.0) {

                if(selected_cbeg == selected_cend) {
                    for (size_t i = 0; i < x.size(); ++i) {
                        dist_from_slope = factor * sqrt( ( (pow((x[i] - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                                + extra_offsety;
                        if (slope_type_0s_1u_2l == 3) dist_from_slope *= -1.0;
                        //std::cout << x[i] << " " << dist_from_slope << std::endl;
                        y[i] = this->d_slope * x[i] + this->d_abscissa + dist_from_slope;
                    }
                }
                else {
                    for (size_t i = 0; i < x.size(); ++i, ++selected_cbeg) {
                        if (*selected_cbeg) {
                            dist_from_slope = factor * sqrt( ( (pow((x[i] - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                                    + extra_offsety;
                            if (slope_type_0s_1u_2l == 3) dist_from_slope *= -1.0;

                            //std::cout << x[i] << " " << dist_from_slope << std::endl;
                            y[i] = this->d_slope * x[i] + this->d_abscissa + dist_from_slope;
                        }
                    }
                }

            }
            else {
                for (size_t i = 0; i < x.size(); ++i) {
                    x[i] =  xmin + (double(i) * step);
                    dist_from_slope = factor * sqrt( ( (pow((x[i] - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                            + extra_offsety;
                    if (slope_type_0s_1u_2l == 4) dist_from_slope *= -1.0;
                    //std::cout << x[i] << " " << dist_from_slope << std::endl;
                    y[i] = this->d_slope * x[i] + this->d_abscissa + dist_from_slope;

                }
            }
        }



        return true;
    }   // regplot

};

struct two_pass_variance
{
    size_t n;
    double d_n;             //!< count as double
    double d_sum;
    double d_mean;
    double d_variance_population;
    double d_variance;
    double d_stddev_population;
    double d_stddev;
    double d_skewness;
    double d_kurtosis;
    double d_dmean;
    double ep;
    double d_min;
    double d_max;
    double d_range;

    statmap variance_data;

    two_pass_variance() {
        this->n = 0;
        this->d_n = 0.0;
        this->d_mean = 0.0;
        this->d_sum = 0.0;
        this->d_variance_population = 0.0;
        this->d_variance = 0.0;
        this->d_stddev_population = 0.0;
        this->d_stddev = 0.0;
        this->d_skewness = 0.0;
        this->d_kurtosis = 0.0;
        this->ep = 0.0;
        this->d_min = 0.0;
        this->d_max = 0.0;
        this->d_range = 0.0;
    }

    statmap variance(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                     const double median_or_mean_initvalue = 0.0) {


        if (std::distance(xcbeg, xcend) < 4 ) {
            std::cerr << "x is smaller than: " << 4 << std::endl;
            return statmap();
        }

        std::vector<double>::const_iterator xcbeg_save = xcbeg;
        this->d_n = std::distance(xcbeg, xcend);
        this->n = std::distance(xcbeg, xcend);



        if (median_or_mean_initvalue != 0.0) {
            for (; xcbeg != xcend; ++xcbeg) {
                this->d_sum += *xcbeg - median_or_mean_initvalue;
            }
            this->d_sum += this->d_n * median_or_mean_initvalue;
            this->d_mean = median_or_mean_initvalue;
        }

        else {
            for (; xcbeg != xcend; ++xcbeg) {
                this->d_sum += *xcbeg;
            }
            this->d_mean = this->d_sum / this->d_n;

        }

        double sdiffs = 0.0;
        double moment = 0.0;

        xcbeg = xcbeg_save;
        this->d_min = *xcbeg;
        this->d_max = *xcbeg;
        for (; xcbeg != xcend; ++xcbeg) {
            if (*xcbeg < this->d_min) this->d_min = *xcbeg;
            if (*xcbeg > this->d_max) this->d_max = *xcbeg;
            sdiffs = *xcbeg - this->d_mean;
            this->d_dmean += fabs(sdiffs);
            this->ep += sdiffs;
            this->d_variance += (moment = sdiffs * sdiffs); // m2
            this->d_skewness += (moment *= sdiffs);         // m3
            this->d_kurtosis += (moment *= sdiffs);         // m4
        }


        this->calc_all();

        return this->variance_data;

    }

    statmap variance_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                        std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                        const double median_or_mean_initvalue = 0.0) {

        if (!vector_size_if_min(xcbeg, xcend, selected_cbeg, selected_cend, 4)) {

            return statmap();
        }

        std::vector<double>::const_iterator xcbeg_save = xcbeg;
        std::vector<bool>::const_iterator   selected_cbeg_save = selected_cbeg;

        if (median_or_mean_initvalue != 0.0) {
            for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
                if(*selected_cbeg) {
                    this->d_sum += *xcbeg - median_or_mean_initvalue;
                    this->n++;
                }
            }
            this->d_n = (double)this->n;
            this->d_sum += this->d_n * median_or_mean_initvalue;
            this->d_mean = median_or_mean_initvalue;
        }

        else {
            for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
                if(*selected_cbeg) {

                    this->d_sum += *xcbeg;
                    this->n++;
                }

            }
            this->d_n = (double)this->n;

            this->d_mean = this->d_sum / this->d_n;

        }

        double sdiffs = 0.0;

        double moment = 0.0;
        xcbeg = xcbeg_save;
        this->d_min = *xcbeg;
        this->d_max = *xcbeg;
        selected_cbeg = selected_cbeg_save;
        for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
            if(*selected_cbeg) {
                if (*xcbeg < this->d_min) this->d_min = *xcbeg;
                if (*xcbeg > this->d_max) this->d_max = *xcbeg;
                sdiffs = *xcbeg - this->d_mean;
                this->d_dmean += fabs(sdiffs);
                this->ep += sdiffs;
                this->d_variance += (moment = sdiffs * sdiffs); // m2
                this->d_skewness += (moment *= sdiffs);         // m3
                this->d_kurtosis += (moment *= sdiffs);         // m4
            }
        }


        this->calc_all();

        return this->variance_data;

    }



    void calc_all() {


        this->d_dmean /= this->d_n;
        this->d_range = this->d_max - this->d_min;

        this->d_variance_population = (this->d_variance - (ep * ep / this->d_n)) / (this->d_n);
        this->d_variance = (this->d_variance - (ep * ep / this->d_n)) / (this->d_n - 1.0);
        this->d_stddev_population = sqrt(this->d_variance_population);
        this->d_stddev = sqrt(this->d_variance);
        if (this->d_variance != 0.0) {
            this->d_skewness /= this->d_n * this->d_variance * this->d_stddev;
            this->d_kurtosis = (this->d_kurtosis /((this->d_n - 1.0)* this->d_variance * this->d_variance)) - 3.0;
        }
        else {
            this->d_skewness = 0;
            this->d_kurtosis = 0;
        }

        this->variance_data[g_stat::d_n_x] = this->d_n ;
        this->variance_data[g_stat::sum_x] = this->d_sum;
        this->variance_data[g_stat::min_x] = this->d_min;
        this->variance_data[g_stat::max_x] = this->d_max;
        this->variance_data[g_stat::mean_x] = this->d_mean;
        this->variance_data[g_stat::range_x] = this->d_range;
        this->variance_data[g_stat::variance_population_x] = this->d_variance_population;
        this->variance_data[g_stat::variance_x] = this->d_variance;
        this->variance_data[g_stat::stddev_population_x] = this->d_stddev_population;
        this->variance_data[g_stat::stddev_x] = this->d_stddev;
        this->variance_data[g_stat::skewness_x] = this->d_skewness;
        this->variance_data[g_stat::kurtosis_x] = this->d_kurtosis;
    }

    friend std::ostream& operator<<(std::ostream& out,
                                    const two_pass_variance& a) {
        if (4 > a.d_n) {
            out << "two_pass_variance NOT VALID, size must be > 3" << std::endl;
            return out;

        }

        for ( auto it = a.variance_data.cbegin(); it != a.variance_data.cend(); ++it ) {
            if (it->second > 0) {
                out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "    " << it->second << std::endl;
            }
            else {
                out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "   " << it->second << std::endl;

            }
        }

        return out;
    }
};

struct two_pass_linreg
{

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


    statmap regression_data; //!< contains all recursion results

    two_pass_linreg() : S_xy(0), n(0), d_n(0.0) {

        this->d_covariance = 0.0;
        this->d_correlation = 0.0;
        this->d_slope = 0.0;
        this->d_abscissa = 0.0;
        this->d_sum_alldist_from_slope = 0.0;
        this->has_data = false;
        this->d_test_against_other_slope = DBL_MAX;

    }

    two_pass_variance x;
    two_pass_variance y;

    statmap linreg(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                   std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                   const double median_or_mean_subtract_y = 0.0) {


        int xdist = std::distance(xcbeg, xcend);
        if (xdist < 4 ) {
            std::cerr << "x is smaller than: " << 4 << std::endl;
            return statmap();
        }
        int ydist = std::distance(ycbeg, ycend);


        if (xdist != ydist) {
            std::cerr << "x and y not of same size: " << xdist << " " << ydist << std::endl;
            return statmap();
        }
        this->d_n = std::distance(xcbeg, xcend);

        std::vector<double>::const_iterator xcbeg_save = xcbeg;
        std::vector<double>::const_iterator ycbeg_save = ycbeg;


        x.variance(xcbeg, xcend);
        y.variance(ycbeg, ycend, median_or_mean_subtract_y);


        xcbeg = xcbeg_save;
        ycbeg = ycbeg_save;
        for (; xcbeg != xcend; ++xcbeg, ++ycbeg) {

            this->S_xy += *xcbeg * *ycbeg;
        }


        if (!this->calc_all() ) {
            return statmap();
        }

        return this->regression_data;
    }

    statmap linreg_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                      std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                      std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                      const double median_or_mean_subtract_y = 0.0) {



        if (!vector_sizes_if_min(xcbeg, xcend, ycbeg, ycend, selected_cbeg, selected_cend, 4)) {

            return statmap();
        }

        std::vector<double>::const_iterator xcbeg_save = xcbeg;
        std::vector<double>::const_iterator ycbeg_save = ycbeg;
        std::vector<bool>::const_iterator   selected_cbeg_save = selected_cbeg;


        x.variance_if(xcbeg, xcend, selected_cbeg, selected_cend);
        selected_cbeg = selected_cbeg_save;
        y.variance_if(ycbeg, ycend, selected_cbeg, selected_cend, median_or_mean_subtract_y);

        xcbeg = xcbeg_save;
        ycbeg = ycbeg_save;
        selected_cbeg = selected_cbeg_save;

        for (; xcbeg != xcend; ++xcbeg, ++ycbeg, ++selected_cbeg) {

            if (*selected_cbeg) {
                this->S_xy += *xcbeg * *ycbeg;
                this->n++;
            }
        }

        this->d_n = (double) this->n;

        if (!this->calc_all() ) {
            return statmap();
        }

        return this->regression_data;
    }

    bool calc_all() {


        this->regression_data = mk_regression_data(this->x.variance_data, this->y.variance_data, this->is_ok);
        if (!this->is_ok) {
            std::cerr << "two_pass_linreg::linerar regression NOT VALID, can not join x & y" << std::endl;
            return false;
        }


        this->S_xy = (this->S_xy - this->x.d_sum * this->y.d_sum /this->d_n)  / (this->d_n - 1.0);

        this->d_slope = this->S_xy / this->x.d_variance;
        this->d_abscissa = this->y.d_mean - this->d_slope * this->x.d_mean;
        this->d_covariance = this->S_xy;
        this->d_correlation = this->S_xy / ( (this->x.d_stddev * this->y.d_stddev) );
        this->d_sum_alldist_from_slope = (this->d_n - 1.0) * (this->y.d_variance - pow(this->d_slope, 2.0) * this->x.d_variance);

        this->regression_data[g_stat::covariance_xy] =  this->d_covariance;
        this->regression_data[g_stat::correlation_xy] =  this->d_correlation;
        this->regression_data[g_stat::slope_xy] =  this->d_slope;
        this->regression_data[g_stat::abscissa_xy] =  this->d_abscissa;
        this->regression_data[g_stat::sum_alldist_from_slope_xy] =  this->d_sum_alldist_from_slope;

        this->has_data = true;

        return true;


    }

    double test_against_other_slope(const double& other_slope, statmap &result) {
        if (this->is_ok && this->has_data) {

            this->d_test_against_other_slope =
                    sqrt(this->x.d_variance * (this->d_n - 1.0) * (this->d_n - 2.0) ) *
                    (this->d_slope - other_slope) / sqrt(this->d_sum_alldist_from_slope);

            this->regression_data[g_stat::student_t_test_against_other_slope] = this->d_test_against_other_slope;
            result[g_stat::student_t_test_against_other_slope] = this->d_test_against_other_slope;
            return this->d_test_against_other_slope;

        }

        else return DBL_MAX;
    }


    friend std::ostream& operator<<(std::ostream& out,
                                    const two_pass_linreg& a)
    {



        for ( auto it = a.regression_data.cbegin(); it != a.regression_data.cend(); ++it ) {
            if (it->second > 0) {
                out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "    " << it->second << std::endl;
            }
            else {
                out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "   " << it->second << std::endl;

            }
        }

        return out;
    }





};


struct select_in_out_reg
{



    // cache variables;
    double d_n;
    double d_slope;
    double d_abscissa;
    double d_lower_confidence;
    double d_upper_confidence;
    double d_varx;
    double d_student_t_inv;
    double d_meanx;

    select_in_out_reg() {}

    bool mkselect_in_out_reg(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                             std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                             std::vector<bool>::iterator selected_cbeg, std::vector<bool>::iterator selected_cend,
                             const statmap &regression_data,
                             const double center_x = 0.0, const double center_y = 0.0,
                             const int slope_type_0s_1u_2l = 0, double extra_offsety = 0.0)
    {



        if (!vector_sizes_if_min(xcbeg, xcend, ycbeg, ycend, selected_cbeg, selected_cend, 4)) {

            return false;
        }

        this->d_n = regression_data.at(g_stat::d_n_x);
        this->d_slope = regression_data.at(g_stat::slope_xy);
        this->d_abscissa = regression_data.at(g_stat::abscissa_xy);
        this->d_lower_confidence = regression_data.at(g_stat::student_t_lower_confidence_xy);
        this->d_upper_confidence = regression_data.at(g_stat::student_t_upper_confidence_xy);
        this->d_varx = regression_data.at(g_stat::variance_x);
        this->d_student_t_inv = regression_data.at(g_stat::student_t_inv);
        this->d_meanx = regression_data.at(g_stat::mean_x);




        if ( (slope_type_0s_1u_2l == 1) || (slope_type_0s_1u_2l == 2) ) {
            for (; xcbeg != xcend; ++xcbeg, ++ycbeg, ++selected_cbeg) {
                double ymax = this->d_upper_confidence * (*xcbeg - center_x)  + center_y + extra_offsety;
                double ymin = this->d_lower_confidence * (*xcbeg - center_x)  + center_y + extra_offsety;
                std::cout << ymin << "  " << *ycbeg << "  " << ymax <<std::endl;
                if ((ymin < *ycbeg) && (*ycbeg < ymax) ) *selected_cbeg = true;
                else *selected_cbeg = false;
            }
            return true;
        }


        else if ( (slope_type_0s_1u_2l == 3) || (slope_type_0s_1u_2l == 4) ) {

            double dist_from_slope;
            double denom = (this->d_n - 1.0) * this->d_varx;
            double factor = this->d_student_t_inv * sqrt(this->d_abscissa) / sqrt((this->d_n - 2.0));

            for (; xcbeg != xcend; ++xcbeg, ++ycbeg, ++selected_cbeg) {
                dist_from_slope = factor * sqrt( ( (pow((*xcbeg - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                        + extra_offsety;
                double ymax = this->d_slope * *xcbeg + this->d_abscissa + dist_from_slope;
                double ymin = this->d_slope * *xcbeg + this->d_abscissa - dist_from_slope;
                std::cout << *xcbeg << "  " << ymin << "  " << *ycbeg << "  " << ymax <<std::endl;
                if ((ymin < *ycbeg) && (*ycbeg < ymax) ) *selected_cbeg = true;
                else *selected_cbeg = false;

                std::cout << *xcbeg << " " << dist_from_slope << std::endl;


            }
            return true;
        }



        return false;
    }   // select_in_out_reg

};

} // end namespace


#endif // VECTOR_TEMPLATES_H
