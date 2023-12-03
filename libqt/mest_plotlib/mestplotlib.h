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

#ifndef MESTPLOTLIB_H
#define MESTPLOTLIB_H

#include "mestplotlib_global.h"
#include <QMainWindow>
#include "mestplotlib_global.h"
#include "qcustomplot.h"
#include "mestplotlib_global.h"
#include <vector>
#include "mestplotlib_global.h"
#include "iterator_complex_templates.h"


namespace Ui {
class mest_plotlib;
}

class mest_plotlib : public QMainWindow
{
    Q_OBJECT

public:
    explicit mest_plotlib(QWidget *parent = nullptr);
    ~mest_plotlib();

    void set_realpart_estimator(const std::vector<double> &mhxr, const std::vector<double> &mhyr, const std::vector<double> &d_itersr);
    void set_imagpart_estimator(const std::vector<double> &mhxi, const std::vector<double> &mhyi, const std::vector<double> &d_itersi);


private:
    Ui::mest_plotlib *ui;




    std::unique_ptr<QCustomPlot> plot_tr = nullptr ;
    std::unique_ptr<QCustomPlot> plot_ti = nullptr ;
};

#endif // MESTPLOTLIB_H
