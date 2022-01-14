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

#ifndef DB_SENSOR_QUERY_H
#define DB_SENSOR_QUERY_H

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <memory>
#include "msg_logger.h"

/*!
 * \brief The db_sensor_query class a tiny sensor info tool; <br>
 * this class can not be copied; create a new class instead and use dbname_info, channel and connection etc.; then copy data if needed; <br>
 * this is maybe only needed when using derived classes such as calibration
 *
 */
class db_sensor_query :  public QObject
{
    Q_OBJECT
public:

    /*!
     * \brief db_sensor_query
     * \param dbname_info database name, pointing to something like /procs/procmt/bin/info.sql3; e.g. where the executable is
     * \param channel -1 for general, 0, ..., 100 for channels
     * \param slot -1 for general, 0, ..., 100 for slot / thread

     * \param connection_name keep EMPTY for auto connection (recommended for channels, here we generate calinfo_query_N for nth channel)
     * \param parent in case NOT Q_NULLPTR parent MUST HAVE / SHOULD HAVE a SLOT  rx_cal_message(const int &channel, const QString &message)
     * \param parent in case NOT nullptr it will connect to messenger; if parent is a std::shared_ptr the above does not work!
     */

    explicit db_sensor_query(const QFileInfo &dbname_info, const int channel = -1, const int slot = -1, const bool open_db = true, const QString &connection_name = "db_sensor_query",
                             QObject *parent = Q_NULLPTR, std::shared_ptr<msg_logger> msg = nullptr);
    //explicit db_sensor_query(QSqlDatabase &dbinuse, QString &message);
    ~db_sensor_query();

    /*!
     * \brief open_db call this only in case the DB is a clone! Otherwise use the constructor!
     */
    void open_db(const QString &connection_name = "");



    /*!
     * \brief get_H_sensors
     * \return a valid H sensor name like MFS-06e on
     */
    QStringList get_H_sensors() const;

    /*!
     * \brief get_E_sensors
     * \return a valid E sensor name like EFP-06 and so on
     */
    QStringList get_E_sensors() const;

    /*!
     * \brief get_H_sensors_atsnames
     * \return a valid H sensor name like MFS06e on for inside atsheader or cal BIOS - hence that mostly the "-" is missing or name is shortened
     */
    QStringList get_H_sensors_atsnames() const;

    /*!
     * \brief get_E_sensors_atsnames
     * \return a valid E sensor name like EFP06 on for inside atsheader or cal BIOS - hence that mostly the "-" is missing or name is shortened
     */
    QStringList get_E_sensors_atsnames() const;

    /*!
     * \brief get_connection_name
     * \return connection name of QSQL Database
     */
    QString get_connection_name() const;

    /*!
     * \brief get_dbname_info
     * \return QFileInfo of used database
     */
    QFileInfo get_dbname_info() const;

    /*!
     * \brief get_channel_no
     * \return actual channel number
     */
    int get_channel_no() const;

    int get_slot_no() const;




    /*!
     * \brief set_atsheader_sensor
     * \param inatsheadername like MFS06 or EFP06
     * \return the offical name like MFS-06e or EFP-06 as we want to use it in Software
     */
    QString set_atsheader_sensor(const QString& inatsheadername);

    /*!
     * \brief set_sensor
     * \param inname Like MFS-06e EFP-06
     * \return MFS06 or EFP06 compatible with atsfile and BIOS
     */
    QString set_sensor(const QString &inname);


signals:
    void tx_cal_message(const int &channel, const int &slot, const QString &message);



protected:

    QSqlDatabase info_db;
    QString connection_name;
    QString db_type;
    QFileInfo dbname_info;
    int channel = -1;
    int slot = -1;

    void init_db_sensor_query(QSqlDatabase &dbinuse, QString &message);
    QList<QString> names_e;
    QList<QString> atsheadernames_e;
    QString name_e;
    QString atsheadername_e;
    QList<QString> names_h;
    QList<QString> atsheadernames_h;
    QString name_h;
    QString atsheadername_h;

    QList<QString> sensor_aliases;                  //!< map strange name, must be exactly ORDERD as inserted; no QMAP here!
    QList<QString> sensor_aliases_true_names;       //!< corresponding List for the sensor aliases np QMAP here!

};

#endif // DB_SENSOR_QUERY_H
