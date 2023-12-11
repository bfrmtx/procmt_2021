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

#ifndef CALSTAT_H
#define CALSTAT_H

#include "calstat_global.h"
#include <QDebug>
#include <QMap>
#include <vector>
#include "procmt_alldefines.h"
#include "math_vector.h"
#include "statmaps.h"

class calstat {

public:

    /*!
     * \brief calstat initialize the class with
     * \param f frequency you want
     * \param chopper chopper status you want
     * \param minmax_percentage percentage - 3 means 3% and covers 97 ... 100 .. 103 as 100
     */
    calstat(const double &f, const cal::cal &chopper, const double &minmax_percentage = 3);

    calstat(const calstat &rhs);

    calstat& operator = (calstat const &rhs);


    bool add_cal(const double &f, const int &serial, const double &ampl, const double &phz);

    std::vector<double> serials_to_double() const;

    double median_variance_ampl();

    double median_variance_phase() ;

    double f, f_min, f_max;
    int chopper = cal::chopper_undef;
    std::vector<double> ampls;
    std::vector<double> phzs;
    std::vector<int> serials;

    statmap variance_phzs_data;
    statmap variance_ampls_data;


};

#endif // CALSTAT_H
