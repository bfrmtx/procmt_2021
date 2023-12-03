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

#ifndef PRC_COM_TABLE_H
#define PRC_COM_TABLE_H

#include "prc_com_table_global.h"
#include <iostream>
#include "prc_com_table_global.h"
#include <QDebug>
#include "prc_com_table_global.h"
#include <QVariant>
#include "prc_com_table_global.h"
#include <QVariantList>
#include "prc_com_table_global.h"
#include <QString>
#include "prc_com_table_global.h"
#include <QStringList>
#include "prc_com_table_global.h"
#include <QMap>
#include "prc_com_table_global.h"
#include <mutex>

#include "prc_com_table_global.h"
#include <cfloat>
#include "prc_com_table_global.h"
#include <climits>
#include "prc_com_table_global.h"
#include <QFileInfo>
#include "prc_com_table_global.h"
#include <QFile>
#include "prc_com_table_global.h"
#include <QVariant>
#include "prc_com_table_global.h"
#include <QSharedPointer>

#include "prc_com_table_global.h"
#include <QPair>

#include "prc_com_table_global.h"
#include <QAbstractTableModel>

#include "prc_com_table_global.h"
#include "qtx_templates.h"
#include "prc_com_table_global.h"
#include "iterator_templates.h"

class prc_com_table : public QAbstractTableModel
{
    Q_OBJECT

public:
    prc_com_table(QObject *parent = Q_NULLPTR);
    ~prc_com_table();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool get_row_key_value(const QModelIndex &index, QString &key, QVariant &value) const;
    bool set_row_key_value(const QModelIndex &index, const QVariant &value);

    int set_map(const QMap<QString, QVariant> &inmap);

    QMap<QString, QVariant> get_map() const;

    int set_includes(const QStringList &includes);

    QStringList get_includes() const;

    int set_excludes(const QStringList &excludes);

    QStringList get_excludes() const;

    void clear();

    /*!
     * \brief read_cvs_table reads a CSV table as columns
     * \param qfi
     * \return
     */
    int read_cvs_table(const QFileInfo &qfi, const int channel = -1, const int slot = -1);

    /*!
     * \brief sort_csv_table sorts a CSV table by column; try double if you know the col_name contains numbers;
     *
     * \param col_name column name from header
     * \param use_double try for double as sort
     * \return
     */
    int sort_csv_table(const QString col_name, const bool use_double = true, const int channel = -1, const int slot = -1);

    int sort_csv_table_with_chopper(const QString col_name, const QString chopper_name, const int channel = -1, const int slot = -1);

    int remove_duplicates_from_sorted(const QString col_name, const QString qlistvariant_name = "", const bool use_double = true, const int channel = -1, const int slot = -1);

    int split_chopper(const QString col_name, const QString chopper_name, const int channel = -1, const int slot = -1);

    /*!
     * \brief fetch_dvector_column
     * \param colname column name of CSV
     * \param qlistvariant_name - no name is all data (csv_data), in case you have splitted the data try csv_data_on or csv_data_off
     * \return
     */
    std::vector<double> get_dvector_column(const QString colname, const QString qlistvariant_name = "", const int channel = -1, const int slot = -1) const;

    QList<QVariant> get_csv_column(const QString colname, const QString qlistvariant_name = "", const int channel = -1, const int slot = -1) const;

    void clear_variant_lists();

signals:

    void prc_com_table_message(const int &channel, const int &slot, const QString &message) const;


private:

    QMap<QString, QVariant> map;
    QMap<int, QString> header;
    QStringList includes;
    QStringList excludes;

    QMap<QString, int> csv_header_cols;
    QList<QSharedPointer<QList<QVariant>>> csv_data;

    // for the lovely chopper
    QList<QSharedPointer<QList<QVariant>>> csv_data_on;
    QList<QSharedPointer<QList<QVariant>>> csv_data_off;

    /*!
     * \brief clear_and_resize_llv clears and destroys "rebuildme" completely bey resettimg the pointers
     * \param rebuildme QList<QSharedPointer<QList<QVariant>>> data to modify
     * \param prepare_again if true we have a same size of list of pointers, column data is empty
     */
    void clear_and_resize_llv(QList<QSharedPointer<QList<QVariant>>> &rebuildme, bool prepare_again = true);

    void hardcopy(QList<QSharedPointer<QList<QVariant> > > &in, QList<QSharedPointer<QList<QVariant>>> &out, const bool append_to_cols_only = false);

};

#endif // PRC_COM_TABLE_H
