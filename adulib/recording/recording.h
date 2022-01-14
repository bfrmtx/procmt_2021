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

#ifndef RECORDING_H
#define RECORDING_H

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QTimeZone>
#include <QString>

#include <QSpinBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>



class recording : public QWidget
{
    Q_OBJECT

public:
    recording(const qint64 sample_freq = 512, const qint64 max_sample_freq = 512, QWidget *parent = Q_NULLPTR);




    QDateTimeEdit *starts;
    QDateTimeEdit *stops;
    QSpinBox *days;
    QSpinBox *hours;
    QSpinBox *minutes;
    QSpinBox *seconds;

    QRadioButton *qrb_64;
    QRadioButton *qrb_60;
    QRadioButton *qrb_1;

    QPushButton *until_midnight;




    qint64 duration_from_spins() const;
    void duration_to_spins();




public slots:

    void spins_valueChanged();
    void grid_changed();
    void starts_changed(QDateTime dt);
    void stops_changed(QDateTime dt);
    void duration_to_midnight();



signals:

    QDateTime actual_start_time(const QDateTime&);
    qint64 actual_duration(const qint64&);
    QDateTime actual_stop_time(const QDateTime&);




private:


    qint64 duration;
    qint64 sample_freq;     //!< works for recordings >= 1 Hz
    qint64 max_sample_freq;
    qint64 min_sample_freq = 1;


};

#endif // RECORDING_H
