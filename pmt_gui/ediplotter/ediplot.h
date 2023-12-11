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

#ifndef EDIPLOT_H
#define EDIPLOT_H

#include <qcustomplot.h>
#include <buckets.h>
#include <QVector>

struct PlotData {
    mt_data_res<std::complex<double>> m_data;
    QVector<QCPErrorBars*> m_error_bars;
    QVector<QCPGraph*> m_graphs;
};

enum class EdiPlotType {
    rho,        //!< log f, log rho
    phi,        //!< log f, lin phi, e.g. min max -180, 180, default 0-90
    coh,        //!< coherency log f, lin coh - values can not exceed 0 - 1; ddisplay default 0 - 1.1 for readability
    tip,        //!< tipper log f KÖNNEN WIR PFEILE MACHEN???
    ai,         //!< anisotroy log f, lin ai
    strk,       //!< strike log f lin y
    rszs        //!< rho* - z*, log rho* as x axis, log z* as y axis (log log)
};

class EdiPlot : public QCustomPlot {
    Q_OBJECT

public:
    explicit EdiPlot(QWidget * parent = nullptr);
    void add_edi_plot(QString const & name, const mt_data_res<std::complex<double> > & data, size_t index, EdiPlotType type, const QPen & pen);
    void remove_edi_plots(QString const & name);
    void reset_edi_data(QString const & name);
    void set_edi_plots_visible(QString const & name, bool value);
    void remove_all_plots();

signals:
    void signal_set_x_axis_label(const QString& label_x_axis);
    void signal_set_y_axis_label(const QString& label_y_axis);

public slots:
    void map_phase(bool map_phase);
    void slot_line_for_crosshair(const int graph_number);

private:
    void handle_readout(double x, double y);
    void set_cursor_coords(double x, double y);

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

private:
    QMap<QString, mt_data_res<std::complex<double>>> m_mt_data;
    QMap<QString, QVector<QCPErrorBars*>> m_error_bars;
    QMap<QString, QVector<QCPGraph*>> m_graphs;
    QCPItemLine * m_vertical_line = nullptr;
    QCPItemLine * m_horizontal_line = nullptr;
    bool phase_is_mapped = true;

    int graph_number_chc = 0;           //!< which for plotting a cross hair cursor


};

#endif // EDIPLOT_H
