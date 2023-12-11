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

#ifndef SPC_DB_H
#define SPC_DB_H

#include "spc_db_global.h"
#include "prc_com.h"
#include "qtx_templates.h"
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
#include <unordered_map>
#include <utility>

#include "mt_data.h"

class spc_db :  public QObject
{
    Q_OBJECT
public:
    spc_db(const QFileInfo qfi_base, const QString subdir_str = "dump", QObject *parent = Q_NULLPTR);
    spc_db(QObject *parent = Q_NULLPTR);
    QFileInfo create_new_db(const QString &dbname_name_only, const std::vector<acsp> &ac_spectra, std::vector<prc_com> &prcs);

    size_t fetch_all(const QFileInfo qfi, std::vector<acsp> &ac_spectra, std::vector<prc_com> &prcs);

    size_t table_names();

    size_t row_count(const QString table_name);

    size_t ac_ssp_columns(const QString table_name, QStringList &column_names);

    void clear();

    ~spc_db();
private:


    QStringList tables;
    QList<double> freqs;
    QStringList columns;
    QStringList channel_tables;
    QList<QMap<QString, QVariant>> dip_1, dip_2;
    QFileInfo dbname;
    QFileInfo qfi_base;                                             //!< survey base dir - we need to append "dump" by default
    QSqlDatabase spcdb;
    QSqlDatabase info_db;
    QFileInfo info_filedb;
    QDir dir;

};

#endif // SPC_DB_H
