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

#ifndef SQL_BASE_TOOLS_H
#define SQL_BASE_TOOLS_H
#include "sql_base_tools_global.h"
#include <QString>
#include "sql_base_tools_global.h"
#include <QStringList>
#include "sql_base_tools_global.h"
#include <QtSql>
#include "sql_base_tools_global.h"
#include <QSqlDatabase>
#include "sql_base_tools_global.h"
#include <QSqlQuery>
#include "sql_base_tools_global.h"
#include <QSqlRecord>
#include "sql_base_tools_global.h"
#include <QSqlError>
#include "sql_base_tools_global.h"
#include <QDebug>
#include "sql_base_tools_global.h"
#include <QVector>

/*!
 * \brief The sql_base_tools class SQL queries for re-use in other libs - SQL DB must be already opened
 */
class sql_base_tools
{

public:
    sql_base_tools();
    /*!
     * \brief copy_complete_table copy a complete table from indb and create complete in outdb
     * \param indb
     * \param in_table_name
     * \param outdb
     * \param out_table_name
     * \param message - error or empty string in case of success
     * \return
     */
    int copy_complete_table(QSqlDatabase &indb, const QString &in_table_name, QSqlDatabase &outdb,
                            const QString &out_table_name, QString &message);
};

#endif // SQL_BASE_TOOLS_H
/*
QSqlQuery query("SELECT * FROM newTab");
QVector<QStringList> lst;
while (query.next())
{
    QSqlRecord record = query.record();
    QStringList tmp;
    for(int i=0; i < record.count(); i++)
    {
        tmp << record.value(i).toString();
    }
    lst.append(tmp);
}
foreach (const QStringList &var, lst) {
    qDebug() << var;
}

Output with QVector will very similar to table. Each iteration means each row with all fields inside it(QStringList):

("0", "Name1", "5000")
("1", "Name2", "1000")
("2", "Name3", "2000")
*/

