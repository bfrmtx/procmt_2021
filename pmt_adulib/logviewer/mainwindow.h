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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

#include <QtSql>
#include <QSqlDatabase>
#include <QCheckBox>
#include <QSqlQuery>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QDateTime>
#include <QVariant>
#include <QXmlStreamReader>
#include <memory>
#include <QVector>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void add_gps();
    void add_temperature();
    void add_batt_volt();
    void add_batt1_curr();
    void add_batt2_curr();

    void estimate_scale();

private slots:
    void on_dateTimeEdit_from_dateTimeChanged(const QDateTime &dateTime);

    void on_dateTimeEdit_to_dateTimeChanged(const QDateTime &dateTime);

    void on_pushButton_3_clicked();

    void on_dial_from_sliderMoved(int position);

    void on_dial_to_sliderMoved(int position);

private:
    Ui::MainWindow *ui;

    void create_plot();

    void reserves(const size_t size);

    void read_message(const QSqlQuery &query);

    void min_max_time();

    std::unique_ptr<QCustomPlot> customPlot;
    QString dbname;
    std::unique_ptr<QSqlDatabase> db = nullptr;


    std::vector<double> num_sats_sats;
    std::vector<double> num_sats_datetimes;

    std::vector<double> batt1_currs;
    std::vector<double> batt1_curr_datetimes;

    std::vector<double> batt2_currs;
    std::vector<double> batt2_curr_datetimes;

    std::vector<double> batt_volts;
    std::vector<double> batt_volt_datetimes;

    std::vector<double> batt_states;
    std::vector<double> batt_state_datetimes;

    std::vector<double> temperatures;
    std::vector<double> temperature_datetimes;

    std::vector<qint64> all_datetimes;


    QXmlStreamReader xmlreader;

    QStringList tokens;

    QDateTime maxtime;
    QDateTime mintime;

    QDateTime old_from;
    QDateTime old_to;

    bool last30 = false;
    int dial_from_pos = 0;
    int dial_to_pos = 0;

    int nplot = 0;

};
#endif // MAINWINDOW_H
