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

#ifndef TSDATA_HEADER_H
#define TSDATA_HEADER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <prc_com.h>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QDateTime>
#include <QList>
#include "atsfile.h"
#include "qtx_templates.h"
#include "prc_com.h"
#include <cmath>
#include "vector_utils.h"


///@todo we need the chopper setting isn't it?


class tsdata_header : public QObject, public QFileInfo, public prc_com
{
    Q_OBJECT
public:
    explicit tsdata_header(const QFileInfo &qfi, QObject *parent = 0);

    // use set file from QFileinfo here
    explicit tsdata_header();

    ~tsdata_header();

    bool open();

    size_t read();

    bool guess_values();

    int create_global_header();

    quint64 create_ats_files();

signals:

public slots:

private:

    QFile file;
    bool isok = false;

    QMap<QString, QVariant> global_header;
    //QList<prc_com> headers;

    // list of files
    // let the files send the maps
    // scan the maps and convert native
    // translate
    // update the headers
    // open write the headers
    // read the columns
    // check
    // append write the ats file


    QMap<QString, QString> translator;

    std::vector<double> x, y, a, b, c;
    std::vector<bool> bx, by, ba, bb, bc;
    double gt_not_number = 1.0E+31;
    double gt_number_replace = 1.0;
    size_t size_hint = 1048576;
    QTextStream qts;

    QDateTime start_time, stop_time;
    size_t nsamples;
    double sample_freq;
    double mina, minb, minc, minx, miny;
    double maxa, maxb, maxc, maxx, maxy;


};

#endif // TSDATA_HEADER_H
