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

#ifndef SELECT_COIL_H
#define SELECT_COIL_H

#include <QDialog>
#include <QDebug>
#include <QString>
#include <QMap>
#include <QMultiMap>
#include <QListWidget>
#include <QListWidgetItem>

#include "calibration.h"

namespace Ui {
class select_coil;
}

class select_coil : public QDialog
{
    Q_OBJECT

public:
    explicit select_coil(QWidget *parent = Q_NULLPTR);
    ~select_coil();

public slots:

    void on_Sensor_comboBox_currentTextChanged(const QString &arg1);
    void on_Sensor_comboBox_activated(const QString &arg1);

    void preselect(const QString  &arg1);

    void set_coils(const QStringList &coils, const QMultiMap<QString, int> &alldb);
signals:
    void coil_selcted(const QString &coil, const int &serno);

private slots:

    void on_Serial_spinBox_valueChanged(int arg1);

    void on_select_coil_accepted();


    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:

    int extract_serials();

    Ui::select_coil *ui;
    QString coil;
    int serno;
    QMultiMap<QString, int> alldb;
};

#endif // SELECT_COIL_H
