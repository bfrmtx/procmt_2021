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

#ifndef XMLREADER_H
#define XMLREADER_H

#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>

#include "namespace.h"

TS_NAMESPACE_BEGIN

struct measurement_output_channel {
    std::string id = "";
    std::string channeltype = "";
    std::string filename = "";
    long long samplecount = 0;
    double frequency = 0;
};

struct measurement_xml_data {
    std::vector<measurement_output_channel> channel_data;
    std::string filepath = "";
};

measurement_xml_data read_xml_file(QString const & filename);

TS_NAMESPACE_END

std::ostream & operator<<(std::ostream & os, const TS_NAMESPACE_NAME::measurement_output_channel & data);


#endif // XMLREADER_H
