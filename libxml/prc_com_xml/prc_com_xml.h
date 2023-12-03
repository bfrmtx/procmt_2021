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

#ifndef PRC_COM_XML_H
#define PRC_COM_XML_H


#include "prc_com_xml_global.h"
#include "tinyxml2.h"
#include "prc_com_xml_global.h"
#include "tinyxmlwriter.h"

#include "prc_com_xml_global.h"
#include <QMap>
#include "prc_com_xml_global.h"
#include <QFileInfo>
#include "prc_com_xml_global.h"
#include <QFile>
#include "prc_com_xml_global.h"
#include <QDir>
#include "prc_com_xml_global.h"
#include <QDebug>
#include "prc_com_xml_global.h"
#include <QDateTime>
#include "prc_com_xml_global.h"
#include <QString>
#include "prc_com_xml_global.h"
#include <QStringList>
#include "prc_com_xml_global.h"
#include <QMap>
#include "prc_com_xml_global.h"
#include <QList>
#include "prc_com_xml_global.h"
#include <QVariant>
#include "prc_com_xml_global.h"
#include <QTextStream>
#include "prc_com_xml_global.h"
#include <QByteArray>
#include "prc_com_xml_global.h"
#include <QUrl>
#include "prc_com_xml_global.h"
#include <vector>
#include "prc_com_xml_global.h"
#include <string>
#include "prc_com_xml_global.h"
#include <mutex>
#include "prc_com_xml_global.h"
#include "measdocxml.h"
#include "prc_com_xml_global.h"
#include <memory>

/*!
 * \brief The prc_com_xml class read/writes QMAP<QString, QVariant> as XML
 */

class prc_com_xml  : public QObject
{
  Q_OBJECT


public:
    // prc_com_xml(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr, QObject *parent = Q_NULLPTR);
    prc_com_xml(QObject *parent = Q_NULLPTR);

    bool open(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);

    QMap<QString, QVariant> read(const QString &topnode, const QString &node);

    /*!
     * \brief read_existing reads a given qmap and tries to convert to native types while reading
     * \param topnode e.g. procmt_processing
     * \param node e.g. proc
     * \param map e.g. cmdline
     */
    void read_existing(const QString &topnode, const QString &node, QMap<QString, QVariant> &map);

    bool create(const QString root_node, const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);

    int add_main(const QString node_name, const QMap<QString, QVariant> &map);

    int add_sub(const QString node_name, const QMap<QString, QVariant> &map);

    bool close_and_write();



protected:

    //QVariant get_native(const QVariant &in, const QVariant &native);

    void clear();

    QFileInfo qfi;


    tinyxmlwriter tix;              //!< main document
    tinyxmlwriter tixsub;

    std::unique_ptr<measdocxml> mdxml;

    QString main_node;
    QString root_node;

    void write_map(tinyxmlwriter &tixml, const QMap<QString, QVariant> &map);


//    this->empty_calibration();
//    tix.insert(this->xcal.getXmlStr());
//    xcal.clear(false);

};

#endif // PRC_COM_XML_H
