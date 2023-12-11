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

#ifndef CAL_STAT_DISPLAY_LIB_H
#define CAL_STAT_DISPLAY_LIB_H

#include "cal_stat_display_lib_global.h"
#include <QMainWindow>
#include <QList>
#include <vector>
#include <memory>
#include <QPushButton>

#include "calibration.h"
#include "calstat.h"
#include "db_sensor_query.h"
#include "xmlcallib.h"

#include "plotlib.h"


namespace Ui {
class cal_stat_display_lib;
}

class cal_stat_display_lib : public QMainWindow
{
    Q_OBJECT

public:
    explicit cal_stat_display_lib(const QFileInfo &info_db, const QFileInfo &master_cal_db, QWidget *parent = Q_NULLPTR);

    ~cal_stat_display_lib();

    int set_calstat(const QList<calstat> &in);

    std::unique_ptr<calibration> sumcal;


private slots:

    void slot_plot(const double &f);

   // void slot_plot_off(const double &f);

    void xrange_changed(const QCPRange &newRange);

    void on_chopper_chk_clicked(bool checked);

    void on_pushButton_rescale_clicked();

private:
    Ui::cal_stat_display_lib *ui;

    double f_tmp;



    int count_me = 0;
    int chopper = 1;

    QSizeF wsize;

//    QList<calstat> ons;
//    QList<calstat> offs;

    std::vector<std::unique_ptr<plotlib>> plots;
    plotlib *anyplot = nullptr;

    std::vector<calstat> calstats;

//    std::shared_ptr<plotlib> amp_plt_on;
//    std::shared_ptr<plotlib> phz_plt_on;

//    std::shared_ptr<plotlib> amp_plt_off;
//    std::shared_ptr<plotlib> phz_plt_off;


//
    std::vector<calstat>::const_iterator citb, cite;






};

#endif // CAL_STAT_DISPLAY_LIB_H
