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

#ifndef BOOST_STUDENT_H
#define BOOST_STUDENT_H

#include "boost_student_global.h"
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include "statmaps.h"
#include <QString>
#include <QDebug>


/*!
 * \brief The boost_student class covers the BOOST Student T distribution<br>
 *  the mk_regression_data math vector is "re-used" but not linked in order to keep indepency
 */
class boost_student
{

public:
    /*!
     * \brief boost_student constructor
     * \param regression_data input data from regression
     * \param upper_quantile
     * \param result contains the result of the Student T distribution
     */
    boost_student();

    ~boost_student();

    statmap student_t(const statmap &regression_data, const double upper_quantile);
    double d_delta_slope;
    double d_student_t_inv;
    double d_upper_confidence;
    double d_lower_confidence;
    double d_upper_quantile;
    bool is_ok;
    statmap regression_result;

    /*!
         * \brief mk_regression_result A COPY FOM MATH_VECTOR !!
         * \param regression_data
         * \return a statmap with the distribution (Student T upper Quantile) depending results
         */
    statmap mk_regression_result(const statmap &regression_data, bool &is_ok);


};

#endif // BOOST_STUDENT_H
