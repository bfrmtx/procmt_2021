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

#ifndef MK_MASTER_CAL_H
#define MK_MASTER_CAL_H

#include <QMainWindow>
#include <QFileInfo>
#include <QDrag>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QFileInfo>

#include "gui_items.h"
#include "calibration.h"
#include "xmlcallib.h"
#include "prc_com_table.h"

#include "plotlib.h"
#include "xmlcallib.h"

namespace Ui {
class mk_master_cal;
}

class mk_master_cal : public QMainWindow
{
    Q_OBJECT

public:
    explicit mk_master_cal(QWidget *parent = 0);
    ~mk_master_cal();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);



private slots:
    void on_sensor_box_currentTextChanged(const QString &arg1);

    void rx_cal_message(const int &channel, const int &slot, const QString &message);

    void on_pushButton_make_master_clicked();

private:
    Ui::mk_master_cal *ui;

    std::unique_ptr<sensors_combo> scbox = nullptr;
    QFileInfo qfi_db;

    QFileInfo qfi_out;


    std::unique_ptr<calibration> extf = nullptr;
    std::unique_ptr<calibration> inmaster = nullptr;
    std::shared_ptr<calibration> outmaster = nullptr;

    std::unique_ptr<xmlcallib> xmlcal = nullptr;

    bool is_ready = false;

    std::vector<double> outer_fon;
    std::vector<double> outer_foff;
    std::vector<double> inner_fon;
    std::vector<double> inner_foff;

    prc_com_table pctab;

    QString coilname;                       //!< real coils name like MFS-06e MFS-07e FGS-03e
    QString message;

};

#endif // MK_MASTER_CAL_H
