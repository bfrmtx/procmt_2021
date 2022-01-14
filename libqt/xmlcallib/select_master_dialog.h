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

#ifndef SELECT_MASTER_DIALOG_H
#define SELECT_MASTER_DIALOG_H

#include <QDialog>
#include "gui_items.h"
#include <memory>
#include <QComboBox>
#include <QDebug>
#include <QString>

namespace Ui {
class select_master_dialog;
}

class select_master_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit select_master_dialog(const QString& dbname, const QChar E_or_H, QWidget *parent = nullptr);


    ~select_master_dialog();

    QString current_master_selection() const;

signals:

    QString master_sensor_selected(const QString master_sensor);

private slots:
    void on_comboBox_sensortype_currentIndexChanged(const QString &arg1);

private:
    Ui::select_master_dialog *ui;

    QString dbname;

    std::unique_ptr<sensors_combo> allsensors = nullptr;
};

#endif // SELECT_MASTER_DIALOG_H
