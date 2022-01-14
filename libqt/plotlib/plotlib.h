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

#ifndef PLOTLIB_H
#define PLOTLIB_H

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QButtonGroup>
#include <QList>
#include <QSignalMapper>
#include <QWidget>
#include "qcustomplot.h"
#include <algorithm>
#include <memory>
#include <cfloat>

#include "procmt_alldefines.h"


/*!
 * \brief The plotlib class cover QCustomplot for specific plots; the plot name is covered by Qt's objectName
 */
class plotlib : public QCustomPlot
{
Q_OBJECT
public:
    explicit plotlib(const QString &object_name = "plotlib", QWidget *parent = Q_NULLPTR);

    ~plotlib();

    void set_ScaleType_log_xy(const QString leftaxislabel);
    void set_ScaleType_log_x_lin_y(const QString leftaxislabel);
    void set_ScaleType_xy(const QString leftaxislabel);


    void add_right_logx_liny(const QString rightaxislabel);

    void clear_all_data();

    /*!
     * \brief clear_downto clears ALL data all graphs, removes all except first (default == 1, graph 0 remains)
     * \param i
     */
    void clear_downto(const int i = 1);

    void add_hz_line(const double &y, const double &x_start, const double &x_stop, const QPen &pen);


    QSharedPointer<QCPAxisTickerLog> logTicker = Q_NULLPTR;
    QSharedPointer<QCPAxisTickerFixed> fixedTicker = Q_NULLPTR;


    QSharedPointer<QGroupBox> create_rho_ranges();

    QSharedPointer<QGroupBox> create_phi_ranges();

    QList<QPushButton*> rho_ranges;
    QSharedPointer<QGroupBox> rho_ranges_box = Q_NULLPTR;
    QList<QPushButton*> phi_ranges;
    QSharedPointer<QGroupBox> phi_ranges_box = Q_NULLPTR;

    QSharedPointer<QSignalMapper> signalMapper_rho_ranges = Q_NULLPTR;
    QSharedPointer<QSignalMapper> signalMapper_phi_ranges = Q_NULLPTR;


    QList<QPushButton*> fon_ranges;
    QSharedPointer<QSignalMapper> signalMapper_fon_ranges = Q_NULLPTR;
    QSharedPointer<QScrollArea> fon_ranges_area = Q_NULLPTR;
    QSharedPointer<QScrollArea> create_fon_ranges(std::vector<double> &vf, const int iwidth = 200);

    QList<QPushButton*> foff_ranges;
    QSharedPointer<QSignalMapper> signalMapper_foff_ranges = Q_NULLPTR;
    QSharedPointer<QScrollArea> foff_ranges_area = Q_NULLPTR;
    QSharedPointer<QScrollArea> create_foff_ranges(std::vector<double> &vf, const int iwidth = 200);

    QList<QPushButton*> f_ranges;
    QSharedPointer<QSignalMapper> signalMapper_f_ranges = Q_NULLPTR;
    QSharedPointer<QScrollArea> f_ranges_area = Q_NULLPTR;
    QSharedPointer<QScrollArea> create_f_ranges(std::vector<double> &vf, const int iwidth = 200);







    double xmin, xmax, ymin, ymax;

    void xmax_xmin(const std::vector<double> &x);
    void ymax_ymin(const std::vector<double> &y);

    void std_regression_plot(const QString what, const std::vector<double> &x, const std::vector<double> &y, const std::vector<bool> &selected, const bool alreadySorted = false);
    void std_regression_plot(const QString what, const double &x, const double &y);
    void title(const QString text);
    void subtitle(const QString text);

    void set_linticks(const QString axisname, const double factors);

    void set_chopper(const int chopper);

    int get_chopper() const;

    int chopper   = cal::chopper_undef;                 //!< plt::chopper_on, chopper_off
    int plot_type = cal::nothing;                       //!< say later plt::amplitude, phase  and so on
    int caltype   = cal::nothing;                       //!< say later plt::calibration, mastercal, interpolated_cal

public slots:

    void rho_range_clicked(const QString &str);
    void phi_range_clicked(const QString &str);

    void f_on_range_clicked(const int &f);
    void f_off_range_clicked(const int &f);
    void f_range_clicked(const int &f);

    void rescale_xaxis(const double &xmin, const double &xmax);

signals:
    void signalMapper_rho_ranges_clicked(const QString);
    void signalMapper_phi_ranges_clicked(const QString);

    void signalMapper_fon_ranges_clicked(const int);
    void signalMapper_foff_ranges_clicked(const int);
    void signalMapper_f_ranges_clicked(const int);


    void f_on_selected(const double f);
    void f_off_selected(const double f);
    void f_selected(const double f);

    void signal_x_y_pos(const double &x, const double &y);


protected:
    void mousePressEvent(QMouseEvent * event) override;
   // void mouseReleaseEvent(QMouseEvent * event) override;

private:

    // needed for signal mapper
    std::vector<double> f;
    std::vector<double> f_on;
    std::vector<double> f_off;




    void delete_pushbuttons(QList<QPushButton*> &pbl);

};

#endif // PLOTLIB_H
