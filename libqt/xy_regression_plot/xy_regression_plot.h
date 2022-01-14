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

#ifndef XY_REGRESSION_PLOT_H
#define XY_REGRESSION_PLOT_H


#include <QObject>
#include <QMessageBox>
#include <QMainWindow>
#include <cmath>
#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <QDebug>
#include <QMap>

#include <gui_items.h>

#include "plotlib.h"

#include "iterator_templates.h"
#include "statmaps.h"

#include "boost_student.h"
#include "math_vector.h"

class xy_regression_plot : public QMainWindow
{

    Q_OBJECT

public:
    explicit xy_regression_plot(QWidget *parent = Q_NULLPTR);

    void clear();



    void set_data(const std::vector<double> &x, const std::vector<double> &y);


    std::unique_ptr<plotlib> plot_tp;

private:

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> plot_x, plot_y;

    std::unique_ptr<two_pass_linreg> tplrg;
    std::unique_ptr<regplot_data> rgplt_data;

    std::unique_ptr<boost_student> stdlrg;

    statmap tp_regressiondata;
    statmap tp_regressionresults;

    double inner_min, inner_max, outer_minx, outer_maxx, outer_miny, outer_maxy;

    std::unique_ptr<message_window> msg;
    std::vector<bool> allselected;
    QString note;
};

#endif // XY_REGRESSION_PLOT_H
