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

#include <QDebug>
#include <QMainWindow>
#include <QRadioButton>
#include <QButtonGroup>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void slot_sys_changed(int id);

    void on_spinBox_sens_res_valueChanged(int arg1);

    void on_checkBox_coil_clicked(bool checked);

private:
    Ui::MainWindow *ui;

    QButtonGroup *sys;
    QButtonGroup *adb;
    QButtonGroup *div;
    QButtonGroup *gain_1;
    QButtonGroup *dac;
    QButtonGroup *gain_2;
    QButtonGroup *rf;
    QButtonGroup *filter_lf;
    QButtonGroup *filter_hf;

    QList<QRadioButton *> sys_btns;


    QMap<QString, int> all_btns;

    void set_default();



};
#endif // MAINWINDOW_H
