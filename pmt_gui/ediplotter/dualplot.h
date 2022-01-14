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

#ifndef DUALPLOT_H
#define DUALPLOT_H

#include <QWidget>
#include <complex>

#include <buckets.h>
#include <qcustomplot.h>

namespace Ui {
class DualPlot;
}

class DualPlot : public QWidget
{
    Q_OBJECT

public:
    explicit DualPlot(QWidget *parent = nullptr);
    ~DualPlot();

    void handle_readout(int x, int y);
    void fit_rho_ranges();
    void fit_phi_ranges();
    void fit_ranges();
    void set_phi_range(double min, double max);
    void set_rho_range(double min, double max);
    void add_plots(QString const & name, mt_data_res<std::complex<double>> const & data, size_t a, size_t b);
    void remove_plots(QString const & name);
    void remove_all_plots();
    void set_plots_visible(QString const & name, bool value);

public slots:
    void slot_line_for_crosshair(const int graph_number);


signals:

    void signal_set_x_axis_label(const QString& label_x_axis);
    void signal_set_y_axis_label(const QString& label_y_axis);
    void signal_line_for_crosshair(const int graph_number);



private:
    Ui::DualPlot *ui;
};

#endif // DUALPLOT_H
