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

#ifndef SELECT_ITEMS_FROM_DB_SENSORS_H
#define SELECT_ITEMS_FROM_DB_SENSORS_H

#include <QDialog>
#include <QMap>
#include <QPushButton>
#include <QString>
#include <QSettings>
#include <QVariant>
#include <QKeyEvent>
#include <QDebug>
#include <QSettings>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileInfo>
#include <memory>

#include "db_sensor_query.h"

namespace Ui {
class select_items_from_db_sensors;
}


// forward declaration
class sens_button;

class select_items_from_db_sensors : public QDialog
{
    Q_OBJECT

public:
    explicit select_items_from_db_sensors(const QStringList &selections, const int &cols, QWidget *parent = Q_NULLPTR);
    ~select_items_from_db_sensors();

    void clear();

    QString get_selection() const;

public slots:
    void set_selection(const QString &selection);

    void activate_button(const QString &button_name);

private:

    int cols;
    QStringList selections;
    Ui::select_items_from_db_sensors *ui;


    QString selection = "empty";

    QList<QHBoxLayout* > hzls;
    QList<sens_button*>  btns;

    uint nlayout = 0;
    uint nbutton = 0;

};



class sens_button : public QObject
{

    Q_OBJECT

public:

    sens_button(const uint &nbutton, const uint &nlayout, QBoxLayout *layout, const QString &label_text, select_items_from_db_sensors *gui);

    select_items_from_db_sensors *gui;
    QString label_text;
    QBoxLayout *layout;
    uint nlayout;
    uint nbutton;

    QPushButton *btn;
    QLabel *lbl;

    ~sens_button();


public slots:
    void on_clicked();

signals:

    QString selection_done(const QString);

};

#endif // SELECT_ITEMS_FROM_DB_SENSORS_H
