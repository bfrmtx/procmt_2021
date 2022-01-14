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

#ifndef ADD_RANGE_LABELS_H
#define ADD_RANGE_LABELS_H

#include <QDialog>
#include <cfloat>
#include <QVector>
#include <QSharedPointer>
#include <qcustomplot.h>

namespace Ui {
class add_range_labels;
}

class add_range_labels : public QDialog
{
    Q_OBJECT

public:
    explicit add_range_labels(QWidget *parent = nullptr);
    ~add_range_labels();

    void set_data(QCPAxis *axis, const QVector<double> dnums_labels, const QVector<QString> strnums_labels);

signals:

    void signal_range_label_updated();

private slots:
    void on_spin_lower_valueChanged(double arg1);

    void on_spin_upper_valueChanged(double arg1);

    void on_buttonBox_rejected();

private:
    Ui::add_range_labels *ui;
    QSharedPointer<QCPAxisTickerText> textTicker;
    QCPAxis *axis;
    QVector<double>  dnums_labels;
    QVector<QString> strnums_labels;

    int upper_counts = 0;
    int lower_counts = 0;


};

#endif // ADD_RANGE_LABELS_H
