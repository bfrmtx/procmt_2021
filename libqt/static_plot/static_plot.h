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

#ifndef STATIC_PLOT_H
#define STATIC_PLOT_H

#include <QMainWindow>

#include "qcustomplot.h"
#include <vector>
#include <complex>
#include <cfloat>
#include <map>
#include <QString>
#include <QScrollBar>
#include <QWidget>
#include "iterator_complex_templates.h"


namespace Ui {
class static_plot;
}

/*!
 * \brief The static_plot class small wrapper class to plot my ac- and single spectra
 */
class static_plot : public QMainWindow
{
    Q_OBJECT

public:
    explicit static_plot(QWidget *parent = nullptr);
    ~static_plot();


    /*!
      * \brief set_data
      * \param plot_number 0 ...8 - protect me agains fail loops wtih 1000 plots
      * \param x x-axis
      * \param y y-axis
      * \param pen take QPen(Qt::blue) or QPen(Qt::red)) and so on
      * \param shape take QCPScatterStyle::ScatterShape::ssSquare or QCPScatterStyle::ScatterShape::ssCircle and so on
      */
     void set_data(const int plot_number, const std::vector<double> &x, const std::vector<double> &y, const QPen pen, const QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssCircle);

     void set_data_complex(const int plot_number, const std::vector<double> &x, const std::vector<std::complex<double>> &y, const int real_1_imag_2_ampl_3_phase_4, const QPen pen, const QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssCircle);

     void set_data_ops(const int plot_number, const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &y2, int add_1_mul_2_div_3_sub_4, const QPen pen, const QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssCircle);

     void set_data_complex_ops(const int plot_number, const std::vector<double> &x, const std::vector<std::complex<double>> &y, const std::vector<std::complex<double>> &y2 , int add_1_mul_2_div_3_sub_4, const int real_1_imag_2_ampl_3_phase_4, const QPen pen, const QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssCircle);



     void set_tile(const QString &window_title);
     void set_x_y_label(const int plot_number, const QString &xlabel, const QString &ylabel);

public slots:

     void plot();
     void clear();
     void create_scrollbar();


private slots:
     void on_pushButton_reset_x_clicked();

     void on_pushButton_reset_y_clicked();

     void on_pushButton_reset_xy_clicked();

     void on_xmin_spin_valueChanged(double arg1);

     void on_xmax_spin_valueChanged(double arg1);

     void on_ymin_spin_valueChanged(double arg1);

     void on_ymax_spin_valueChanged(double arg1);

     void change_x_axis_and_spins(const QCPRange &newRange);

     void change_y_axis_and_spins(const QCPRange &newRange);



private:
    Ui::static_plot *ui;


    size_t max_plots = 8;
    std::vector<std::unique_ptr<QCustomPlot>> plots;

    QScrollBar *scrollbar = nullptr;


    std::vector<int> active_plots;
    std::vector<double> mins_x;
    std::vector<double> maxs_x;
    std::vector<double> mins_y;
    std::vector<double> maxs_y;

    std::vector<double> act_mins_x;
    std::vector<double> act_maxs_x;
    std::vector<double> act_mins_y;
    std::vector<double> act_maxs_y;



    void set_x_min_max(const double &xmin, const double &xmax);

    void replot();


};

class multi_static_plots : public QObject, public std::map<QString, std::unique_ptr<static_plot>>
{
    Q_OBJECT
public:
    multi_static_plots(QObject *parent = Q_NULLPTR);
    ~multi_static_plots();

    void insert(const QString &plotname, QWidget *where_to_add_buttons = Q_NULLPTR);
    void clear();

    std::vector<QPushButton *> what_to_plot_buttons;

    const char *prop_what_to_plot = "plotme";

signals:

    void plotting(const QString &what);

public slots:

    void slot_plot_button();
    void slot_plot(const QString &what);
    void close();

private:

    QWidget *where_to_add_buttons = Q_NULLPTR;

};

#endif // STATIC_PLOT_H
