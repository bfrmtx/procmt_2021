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

#ifndef GOTOSAMPLEDIALOG_H
#define GOTOSAMPLEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QRegularExpressionValidator>

#include <eqdatetime.h>

namespace Ui {
class GotoSampleDialog;
}

class LineEditNumber : public QLineEdit {
    Q_OBJECT

public:
    explicit LineEditNumber(QWidget * parent);

protected:
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

private slots:
    void slot_on_text_changed(QString const & new_text);

};

class GotoSampleDialog : public QDialog {
    Q_OBJECT

public:
    void set_start_time(eQDateTime time) { m_start_time = time; }
    void set_min_max(qlonglong min, qlonglong max);
    void set_value(qlonglong value);
    explicit GotoSampleDialog(QWidget *parent = nullptr);
    ~GotoSampleDialog();

private:
    Ui::GotoSampleDialog * ui;
    qlonglong m_min_val = std::numeric_limits<qlonglong>::min();
    qlonglong m_max_val = std::numeric_limits<qlonglong>::max();
    qlonglong m_current = 0;
    eQDateTime m_start_time;

private slots:
    void on_button_min_clicked();
    void on_button_max_clicked();

    void on_time_dateTimeChanged(const QDateTime &dateTime);

signals:
    void value_changed(qlonglong value);
};

#endif // GOTOSAMPLEDIALOG_H
