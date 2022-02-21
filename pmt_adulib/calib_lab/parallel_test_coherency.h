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

#ifndef PARALLEL_TEST_COHERENCY_H
#define PARALLEL_TEST_COHERENCY_H

#include <vector>
#include <QString>

struct parallel_test_coherency {

    // data
    std::vector<double> f;
    std::vector<double> ampl;
    std::vector<double> err;
    std::vector<double> coh;
    double f_sample;
    size_t wl;

    // cal - not inperpolated
    int chopper = -1;  // can only be 0 or 1; -1 == undefined
    std::vector<double> cal_f;
    std::vector<double> cal_ampl;
    std::vector<double> cal_phase;
    QString sensortype;
    int sernum = -1;
    QString channel_type;

};

#endif // PARALLEL_TEST_COHERENCY_H
