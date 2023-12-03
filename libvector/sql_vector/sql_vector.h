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

#ifndef SQL_VECTOR_H
#define SQL_VECTOR_H

#include "sql_vector_global.h"
#include <QDebug>
#include "sql_vector_global.h"
#include <QtSql>
#include "sql_vector_global.h"
#include <QSqlDatabase>
#include "sql_vector_global.h"
#include <QSqlQuery>
#include "sql_vector_global.h"
#include <QSqlError>
#include "sql_vector_global.h"
#include <QFileInfo>
#include "sql_vector_global.h"
#include <memory>
#include "sql_vector_global.h"
#include <vector>
#include "sql_vector_global.h"
#include <iostream>


/*!
 * \brief The SQL_vector class is a simple vector loader for SQLite database
 */
class  SQL_vector
{

public:


    explicit SQL_vector(const QFileInfo &dbname, const QString &connection_name = "SQL_vector_query", const QString &db_type = "QSQLITE");
    ~SQL_vector();

    size_t get_vector(const QString& table, const QString& col, std::vector<double> &v);
    size_t get_vector(const QString& table, const QString& colx, const QString& coly,
                      std::vector<double> &x, std::vector<double> &y);
    size_t get_vector(const QString& table, const QString& colx, const QString& coly, const QString& colz,
                      std::vector<double> &x, std::vector<double> &y, std::vector<double> &z);


private:
    std::unique_ptr<QSqlDatabase> db = nullptr;
    QString connection_name;
    QString db_type;
    bool is_open = false;
    QString enquote(const QString& str) const;

};

#endif // SQL_VECTOR_H
