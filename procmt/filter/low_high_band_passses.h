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

#ifndef LOW_HIGH_BAND_PASSSES_H
#define LOW_HIGH_BAND_PASSSES_H

#include <QDialog>
#include <QDebug>
#include <QVariant>
#include <memory>
#include "gui_items.h"
namespace Ui {
class low_high_band_passses;
}

class low_high_band_passses : public QDialog
{
    Q_OBJECT

public:
    explicit low_high_band_passses(std::shared_ptr< QMap<QString, QVariant> > extrafilters_status, QWidget *parent = 0);
    ~low_high_band_passses();

signals:


public slots:

    void set_f_min_max_lp(const double fmin, const double fmax);
    void set_f_min_max_hp(const double fmin, const double fmax);
    void set_initValue_lp(const double f);
    void set_initValue_hp(const double f);


private slots:

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::low_high_band_passses *ui;

    hz_s_spinbox *spnblowpass = Q_NULLPTR;
    hz_s_spinbox *spnbhighpass = Q_NULLPTR;

    QString filter_name;
    std::shared_ptr<QMap<QString, QVariant>> extrafilters_status;

};

#endif // LOW_HIGH_BAND_PASSSES_H
