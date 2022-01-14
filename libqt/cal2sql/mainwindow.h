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
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QListWidget>
#include <QList>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <memory>
#include <vector>
#include <QFileDialog>
#include <QMap>
#include <QMultiMap>
#include <QTextStream>

#include <iostream>
#include "procmt_alldefines.h"
#include "calibration.h"
#include "select_coil.h"
#include "xmlcallib.h"
#include "xy_regression_plot.h"
#include "calstat.h"
#include "cal_stat_display_lib.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);



signals:

    void plot_cal(std::shared_ptr<calibration> calib, const bool nooverlay = true);

private slots:

    void rx_cal_message(const int &channel, const int &slot, const QString &message);
    void scan_db();

    void dlg_coil_selected(const QString &coil, const int &serno);

    void on_actionOpenDataBase_triggered();

    void on_actionGetCalibration_triggered();

    void on_actionDumpAll_triggered();

    void on_actionOpenCreate_triggered();

    void on_actionQuit_triggered();

    void on_plot_pushButton_clicked();

    void on_getcal_pushButton_clicked();

    void on_plot_append_pushButton_clicked();

    void on_actionStats_triggered();

    void on_qlw_itemDoubleClicked(QListWidgetItem *item);

    void on_actionWrite_LogFile_triggered();

private:
    Ui::MainWindow *ui;

    QFileInfo qfi;

    QFileInfo qfi_db;
    QFileInfo qfi_master;

    QFileInfo qfi_log;
    QStringList qts_buffer;




    std::shared_ptr<calibration> cal;
    QFileDialog *dialog;
    QString dirname;


    QString coil;
    int serno;

    select_coil *selcoil;

    xmlcallib *callib;
    xy_regression_plot *xy_plot;

    QMultiMap<QString, int> alldb;

    std::unique_ptr<cal_stat_display_lib> csdpl;



    int mode = -1;  //!< 0 read/write,  1 create/read/write, -1 close/not open , -2 open from drag and drop

};

#endif // MAINWINDOW_H
