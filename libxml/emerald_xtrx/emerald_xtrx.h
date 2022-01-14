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

#ifndef EMERALD_XTRX_H
#define EMERALD_XTRX_H

#include "measdocxml.h"

class emerald_xtrx : public measdocxml
{
  Q_OBJECT


public:

    emerald_xtrx(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr, QObject *parent = Q_NULLPTR);

    // msvc problem
    //emerald_xtrx(QObject *parent = Q_NULLPTR);

    ~emerald_xtrx();

    void create_channel(const bool emits = true);
    void create_emerald(const bool emits = true);
    void create_site(const bool emits = true);


    // read the pseudo XML for each channel
    void get_emerald_channel_section();

    // read the header and site description
    QString get_emeral_emerald_section();

signals:

    // for each channel
    void signal_emerald_channel_data_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_got_emerald_channel(const QMap<QString, QVariant> &data_map);

    // site description
    void signal_emerald_emerald_data_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_got_emerald_emerald(const QMap<QString, QVariant> &data_map);



private:

    QMap<QString, QVariant> emerald_Channel_data;
    QMap<QString, QVariant> emerald_EmeraldData_data;
    QMap<QString, QVariant> emerald_Site_data;


};

#endif // EMERALD_XTRX_H
