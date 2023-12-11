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

#ifndef MEASDOC_FROM_MC_DATA_H
#define MEASDOC_FROM_MC_DATA_H

#include "measdoc_from_mc_data_global.h"
#include <iostream>
#include <QObject>
#include <QDebug>
#include <vector>
#include <QDir>
#include <QString>
#include <QMap>

#include "mc_data.h"
#include "measdocxml.h"
#include "adulib.h"
#include "eqdatetime.h"
#include "calibration.h"

class measdoc_from_mc_data  : public QObject
{
  Q_OBJECT

public:
    measdoc_from_mc_data(std::shared_ptr<mc_data> mxcd, QObject *parent = Q_NULLPTR);
    measdoc_from_mc_data(const QList<QFileInfo> qfis, QObject *parent = Q_NULLPTR);

    std::shared_ptr<measdocxml> make_measdoc_xml(bool &success);


    ~measdoc_from_mc_data();

private slots:
    void slot_qmap_created(const QMap<QString,QVariant> &data_map);

private:

    void connect_measdoc();

    void get_calibration_xml();

    bool prepare_adu_from_skeleton();

    bool data_from_ats_files();

    bool can_continue = false;

    std::unique_ptr<adulib> adujob = nullptr;
    std::shared_ptr<measdocxml> measdoc;
    QFile measdocxml_xml;
    std::shared_ptr<mc_data> mxcd;

    // for the job

    QString xml_cnf_basedir;
    QString HWConfig;
    QString HwDatabase;
    QString HwStatus;
    QString ChannelConfig;
    QString outputjobsdir;

    QFileInfo *qfiHWConfig = nullptr;
    QUrl *qrlHWConfig = nullptr;
    QByteArray *qbaHWConfig = nullptr;

    QFileInfo *qfiHwDatabase = nullptr;
    QUrl *qrlHwDatabase = nullptr;
    QByteArray *qbaHwDatabase = nullptr;


    QFileInfo *qfiChannelConfig = nullptr;
    QUrl *qrlChannelConfig = nullptr;
    QByteArray *qbaChannelConfig = nullptr;

    QFileInfo *qfiHwStatus;

    QFileInfo calibration_db;
    std::vector<int> used_channels;



};

#endif // MEASDOC_FROM_MC_DATA_H
