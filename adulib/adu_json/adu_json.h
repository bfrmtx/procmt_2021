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

#ifndef ADU_JSON_H
#define ADU_JSON_H

#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QVariant>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QDir>


#include <QDate>
#include <QTime>
#include <QDateTime>

// setting up data from SQL file
#include <QSqlDatabase>
#include <QSqlQuery>

#include <cmath>
#include <vector>
#include <memory>
#include <iostream>
#include <cfloat>

#include "adu_system_defs.h"



class adu_json  : public QObject
{
    Q_OBJECT

public:
    explicit adu_json(QObject *parent = nullptr);

    adu_json(const adu_json &rhs);

    adu_json& operator = (const adu_json &rhs);

    ~adu_json();

    /*!
     * \brief init_channels
     * \param chans how many channels the ADU has; call directly to create an empty ADU; otherwise this is used while loading JSON and is init from meas_channels
     * \return
     */
    bool init_channels(const uint& chans);

    void prepare();

    QByteArray toJsonByteArray(QJsonDocument::JsonFormat format = QJsonDocument::Indented);

    bool fromQBytearray(const QByteArray &json_bytes);

    bool to_JSON_file(const QFileInfo &qfi);

    bool to_XML_file(const QFileInfo &qfi);

    QByteArray toXMLByteArray();

    bool from_JSON(const QFileInfo *qfi = nullptr, const QByteArray *qba = nullptr);

    bool from_XML(const QFileInfo *qfi = nullptr);

    /*!
     * \brief size who many channels
     * \return channels of ADU
     */
    uint size() const;

    void clear();

    QVariant get_channel_value(const uint &channel, const QString &key) const;

    bool set_channel_value(const uint &channel, const QString &key, const QVariant &value);

    bool set_sensor_value(const uint &channel, const QString &key, const QVariant &value);

    bool get_selftest_channels(QList<QStringList>& txt_values, QStringList& out_keys, QStringList& out_tooltips) const;

    QStringList get_channel_node_keys(const QString xmlnode) const;

    bool set_system_value(const QString &key, const QVariant &value);

    QVariant get_system_value(const QString &key) const;

    double dipole_length(const uint &channel, double &angle, bool *ok = nullptr) const;


    // these are needed for const inititalization of other maps
    //
    std::vector<QVariantMap> get_channels() const;
    QVariantMap get_system() const;
    std::vector<QVariantMap> get_sensors() const;


    //    friend bool    compstartime_lt(const std::unique_ptr<adu_json>& lhs, const std::unique_ptr<adu_json>& rhs);
    //    friend bool    compstartime_gt(const std::unique_ptr<adu_json>& lhs, const std::unique_ptr<adu_json>& rhs);
    //    friend bool    compstartime_eq(const std::unique_ptr<adu_json>& lhs, const std::unique_ptr<adu_json>& rhs);
    //    friend int64_t stop_lhs_starts_rhs(const std::unique_ptr<adu_json>& lhs, const std::unique_ptr<adu_json>& rhs);


    // needed for my comp functions - that can be used for you local - but not remote ADU
    QDateTime get_start_time() const;           //!< returns UTC start time
    QDateTime get_stop_time() const;            //!< stop_time when loaded from files
    QDateTime get_stop_time_calc(const int64_t duration) const;       //!< stop_time is calculated from start_time, duration and sample_frequency
    uint64_t get_samples() const;           //!< often used - calculated
    double get_sample_freq() const;         //!< often used - returns the sampling frequency

    // ------- this is the time ! ----------------- //
    uint64_t get_duration() const;          //!< duration is the driving force; often used - returns time of job; sub second range is ignored
    bool set_duration(const qint64 duration);   //!< duration is the driving force; often used - returns time of job; sub second range is ignored

    void set_edit_mode(const bool on_off);

public slots:

    void slot_sensortype_of_serial_changed(const QString& sensortype, const int& existing_sernum);

private:

    std::vector<QVariantMap> channels;
    void channels_to_json();


    std::vector<QVariantMap> sensors;
    //    void sensors_to_json();


    QVariantMap system;
    void system_to_json();

    std::unique_ptr<QJsonObject> root_obj;
    std::unique_ptr<QJsonDocument> json_doc;
    QByteArray xmlqba;

    qint64 duration = 0;

    bool edit_mode = false;                 //!< edit an existing file; values can be set which are not needed for job creation

    QString GMS="8";        // 9 10
    QString Version="2.0";  // selftest 08 / 09 / 10
    QString Name="ADU-08e"; //
    QString Type="1";       // egal



};

#endif // ADU_JSON_H

/*  possible variables in JASON - int can be covered by double aka "number"
a string
a number
an object (JSON object)
an array
a boolean
null
*/

/*  copy a database manually
 *
 * bool hwbase::populate_db()
{
    QFileInfo info_db_file(app_homepath("info.sql3"));
    QString keys, labels, value_types, xmlnodes, editables;


    if (info_db_file.exists()) {
        QSqlDatabase info_db = QSqlDatabase::addDatabase("QSQLITE", "populate");
        info_db.setConnectOptions("QSQLITE_OPEN_READONLY");
        info_db.setDatabaseName(info_db_file.absoluteFilePath());

        if (info_db.open() ) {
            QSqlQuery xquery(info_db);
            QString querystr("SELECT xml_channel.key, xml_channel.label, xml_channel.value_type, xml_channel.xmlnode, xml_channel.editable FROM xml_channel;");
            qDebug() << querystr;
            if (xquery.exec(querystr)) {
                while (xquery.next()) {
                    keys = (xquery.value(0).toString());
                    //values are not needed
                    labels = (xquery.value(1).toString());
                    value_types = (xquery.value(2).toString());
                    xmlnodes = (xquery.value(3).toString());
                    editables = (xquery.value(4).toString());

                    // cop to runtime DB
                    QString xquerystr = "INSERT INTO " + this->table + " (`key`, `label`, `value_type`, `xmlnode`, `editable`) VALUES ('";
                    xquerystr.append(keys + "', '" + labels + "', '" + value_types + "', '"  + xmlnodes + "', '"  + editables + "')");
                    qDebug() << xquerystr;
                    if (!query->exec(xquerystr)) {
                        qDebug() << "failed to populate hw";
                    }

                }
                qDebug() << "data base channel prepared";
            }
            if (info_db.isOpen()) info_db.close();

        }
    }

    QSqlDatabase::removeDatabase("populate"); // correct doing this after getting out of scope

    return false;

}

*/
