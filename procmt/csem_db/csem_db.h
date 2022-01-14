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

#ifndef CSEM_DB_H
#define CSEM_DB_H

#include "prc_com.h"

#include "procmt_alldefines.h"
#include <climits>

#include <vector>

#include <QObject>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMap>
#include <QList>
#include <QVariant>

class csem_db :  public QObject
{
    Q_OBJECT
public:
    csem_db(const QFileInfo qfi_base, const QString subdir_str = "csem", QObject *parent = Q_NULLPTR);

    ~csem_db();

    QFileInfo create_new_db(const QString &dbname_name_only);

    QFileInfo filename() const;

    QFileInfo open(const QString &dbname_name_only);

    void close();

    void clear();
    /*!
     * \brief update_true_or_insert_false
     * \param data
     * \param dip1_1__dip2_2
     * \param rowid
     * \return
     */
    bool update_true_or_insert_false(const QMap<QString, QVariant> &data, const int &dip1_1__dip2_2, int &rowid) const;

public slots:

    int slot_insert_update(const QMap<QString, QVariant> &data, const int &dip1_1__dip2_2);

    int write_data_file();

private:

    int QMAP_to_SQL(const QMap<QString, QVariant> &data, QString &colnames, QString &colvalues);

    bool db_has_data_keys(const QMap<QString, QVariant> &data);

    QStringList tables;
    QStringList columns;
    QList<QMap<QString, QVariant>> dip_1, dip_2;
    QFileInfo dbname;
    QFileInfo qfi_base;                                             //!< survey base dir - we need to append "csem" by default
    QSqlDatabase csemdb;
    QDir dir;

};

#endif // CSEM_DB_H
