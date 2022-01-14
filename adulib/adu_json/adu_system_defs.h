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

#ifndef ADU_SYSTEM_DEFS_H
#define ADU_SYSTEM_DEFS_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QVariant>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QDir>
// setting up mass data from SQL file
#include <QSqlDatabase>
#include <QSqlQuery>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

/*!
 * \brief app_homepath get the path either for application or debug
 * \return
 */
inline QString app_homepath(const QString &what_to_open) {
    QFileInfo qfinew;
#ifdef  QT_DEBUG
    qfinew.setFile(QString(PMTSQLPATH) + "/bin/" + what_to_open);
    return qfinew.absoluteFilePath();
#else
    qfinew.setFile(QCoreApplication::applicationDirPath() + "/" + what_to_open);  // typical windows / linux
    std::cerr << qfinew.absoluteFilePath().toStdString();
    if (qfinew.exists()) return qfinew.absoluteFilePath();
    qfinew.setFile(QString(PMTSQLPATH_UNIX) + "/" + what_to_open); // on MacOS the SQL is not exactly where the binary is

#endif
    /// #ifdef Q_OS_MACOS Q_OS_UNIX Q_OS_WIN
    std::cerr << qfinew.absoluteFilePath().toStdString();
    return qfinew.absoluteFilePath();

}

/*!
 * \brief map_hairdresser - if you need control of the QVariants types; don't want to check later in GUI or so
 * \param cutter convert client's types to cutter's types
 * \param client - gets the same types from  the cutter
 */
inline void map_hairdresser(const QMap<QString, QVariant> &cutter,  QMap<QString, QVariant> &client) {

    if (cutter.size() != client.size()) return;
    QStringList cutter_keys(cutter.keys());
    QStringList client_keys(client.keys());
    int i;
    // check same keys
    for (i = 0; i < cutter.size(); ++i) {
        if (cutter_keys.at(i).compare(client_keys.at(i), Qt::CaseSensitive) ) {
            qDebug() << Q_FUNC_INFO << "not the same keys - wron maps!";
            return;
        }
    }

    QMap<QString, QVariant>::const_iterator cuts = cutter.constBegin();
    QMap<QString, QVariant>::iterator clie = client.begin();

    // what to to with "N/A" ? in case value is number?

    while (cuts != cutter.constEnd()) {
        if (cuts->typeId() != clie->typeId()) {
            qDebug() << cuts.key() << cuts->typeId();

            QVariant qv(clie.value());
//            bool ok = false;
//            ok = qv.convert(cuts->typeId());
            //if (ok)
                clie->setValue(qv);
//            else {
//                qDebug() << Q_FUNC_INFO << "can not convert no native type - you may run into trouble later!";
//            }
        }
        ++cuts;
        ++clie;
    }
}

namespace adu_defs {
enum adu_defs : uint {
    nothing                 = 0,
    max_channels            = 20,       //!< protect against overflow
    no_channel              = 100000,   //!< number indicates that no channel is meant
    max_cal_entries         = 800,      //!< protect against overflow
    max_qmap_size           = 200,      //!< protect against overflow
    channel_map             = 1,        //!< key to access QVariant Map
    system_map              = 2,        //!< key to access QVariant Map
    sensor_map              = 3         //!< key to access QVariant Map

};

/**
 * This enum defines the system types in the global system information.
 */
enum t_SystemTypes
{
    ADU07       =   0,     /// ADU-07 system
    ADU08       =   1,     /// ADU-08e 5,6-CH system
    ADU08_2CH   =   2,     /// ADU-08e 2-CH system
    TXB08       =   3,     /// TXB-08 system
    ADU09       =   4,     /// ADU-08e 5-CH system system
    ADU10       =   5,     /// ADU-08e 5-CH system system
    ADU_UNKNOWN = 0xFF     /// unknown or invalid ADB board type
};
}

/*!
 * \brief object_to_struct splits a synthetic string back to values
 * \param object_name adu_defs::map++key++channel for channel/sensor and adu_defs::map++key for the system
 * \param invalue the value for a key <-> value pair generated from above
 */
template<typename T>
struct object_name_to_struct
{

    object_name_to_struct(const QString& object_name, const T& invalue) {


        this->key = QString("");
        this->channel = adu_defs::adu_defs::no_channel;
        this->varmap = adu_defs::adu_defs::nothing;

        if ((object_name.size() < 4) || (!object_name.contains("++")) ) return;
        this->value = QVariant(invalue);



        // check the string object name which shall be "adu_defs::map++key++channel" for a channel / sensor object
        // and "adu_defs::map++key" for objects not related to a channel
        QStringList split(object_name.split("++"));

        if ((split.size() < 2) || split.size() > 3) {
            this->value = QString("");
            return;
        }
        bool ok = false;

        this->varmap = split.at(0).toUInt(&ok);
        if (!ok) {
            this->varmap = adu_defs::adu_defs::nothing;
            this->value = QString("");
            return;
        }
        if (this->varmap == adu_defs::adu_defs::nothing ) {
            this->value = QString("");
            return;
        }
        this->key = split.at(1);
        if (!key.size()) {
            this->channel = adu_defs::adu_defs::no_channel;
            this->varmap = adu_defs::adu_defs::nothing;
            this->key = QString("");;
            this->value = QString("");
            return;
        }

        // have channel / sensor object
        if (split.size() == 3) {
            this->channel = split.last().toUInt(&ok);
            if (!ok) {
                this->channel = adu_defs::adu_defs::no_channel;
                this->varmap = adu_defs::adu_defs::nothing;
                this->key = QString("");;
                this->value = QString("");
                return;
            }
        }
    }

    uint channel;
    uint varmap;
    QString key;
    QVariant value;



};

/*!
 * \brief The adu_row_struct struct contains the table headers of the info.sql3 for xml_adu and xml_channel
 *
 */
struct adu_row_struct
{
    adu_row_struct() {}

    adu_row_struct(const adu_row_struct& rhs ) {
        this->key = rhs.key;
        this->value = rhs.value;
        this->label = rhs.label;
        this->value_type = rhs.value_type;
        this->editable = rhs.editable;
        this->tooltip = rhs.tooltip;

    }

    /*!
     * \brief clear clear contents
     */
    void clear() {
        this->key = "";
        this->value = "";
        this->label = "";
        this->value_type = "";
        this->xmlnode = "";
        this->editable = "";
        this->tooltip = "";

    }

    QString select_substr_channel() const {
        return QString ("SELECT xml_channel.key, xml_channel.value, xml_channel.label, xml_channel.value_type, xml_channel.xmlnode, xml_channel.editable, xml_channel.tooltip FROM xml_channel");
    }

    QString select_substr_system() const {
        return QString ("SELECT xml_adu.key, xml_adu.value, xml_adu.label, xml_adu.value_type, xml_adu.xmlnode, xml_adu.editable, xml_adu.tooltip FROM xml_adu");
    }

    /*!
     * \brief set_query_result for a LIMIT 1 query - which is mostly used, otherwise last is used!
     * \param query
     */
    void set_query_result (std::unique_ptr<QSqlQuery> &query) {
        while (query->next()) {
            qDebug() << query->value(0).toInt() <<  query->value(1).toString() <<  query->value(2).toString() << query->value(3).toString()<< query->value(4).toString()<< query->value(5).toString();
            this->key = query->value(0).toString();
            this->value = query->value(1).toString();
            this->label = query->value(2).toString();
            this->value_type = query->value(3).toString();
            this->xmlnode = query->value(4).toString();
            this->editable = query->value(5).toString();
            this->tooltip = query->value(6).toString();
        }
    }

    void set_query_results (std::unique_ptr<QSqlQuery> &query, QList<adu_row_struct>& adu_row_structs) {
        adu_row_structs.clear();
        while (query->next()) {
            qDebug() << query->value(0).toInt() <<  query->value(1).toString() <<  query->value(2).toString() << query->value(3).toString()<< query->value(4).toString()<< query->value(5).toString();
            this->key = query->value(0).toString();
            this->value = query->value(1).toString();
            this->label = query->value(2).toString();
            this->value_type = query->value(3).toString();
            this->xmlnode = query->value(4).toString();
            this->editable = query->value(5).toString();
            this->tooltip = query->value(6).toString();
            adu_row_structs.append(adu_row_struct(*this));
        }

    }


    QString key;
    QString value;
    QString label;
    QString value_type;
    QString xmlnode;
    QString editable;
    QString tooltip;
};



#endif // ADU_SYSTEM_DEFS_H
