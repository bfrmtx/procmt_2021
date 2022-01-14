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

#ifndef MANUAL_SCALES_H
#define MANUAL_SCALES_H

#include <QDialog>
#include "qcustomplot.h"
#include <cfloat>

namespace Ui {
class manual_scales;
}

class manual_scales : public QDialog
{
    Q_OBJECT

public:
    explicit manual_scales(QWidget *parent = nullptr);
    ~manual_scales();

    void set_data(QCPRange &ampl, QCPRange &phz, QCPRange &freq);
signals:

    void ayrange_changed(QCPRange range);
    void pyrange_changed(QCPRange range);
    void xrange_changed(QCPRange range);

private slots:
    void on_a_min_valueChanged(double arg1);

    void on_a_max_valueChanged(double arg1);

    void on_phz_min_valueChanged(double arg1);

    void on_phz_max_valueChanged(double arg1);

    void on_f_min_valueChanged(double arg1);

    void on_f_max_valueChanged(double arg1);

private:
    Ui::manual_scales *ui;
    QCPRange ampl;
    QCPRange phz;
    QCPRange freq;
};

#endif // MANUAL_SCALES_H
