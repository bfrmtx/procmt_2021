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

#ifndef QEXTNETWORKACCESSMANAGER_H
#define QEXTNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QObject>

class QExtNetworkAccessManager : public QNetworkAccessManager {
    Q_OBJECT

public:
    enum class Depth {
        ZERO,
        ONE,
        INF
    };

    QNetworkReply * propfind(QNetworkRequest const & request, QByteArray const & body);
    QNetworkReply * propfind(QNetworkRequest request, Depth depth, QByteArray const & body);
    QNetworkReply * get(QNetworkRequest request, size_t begin, size_t end);
    QNetworkReply * get(QNetworkRequest const & request);
};

#endif // QEXTNETWORKACCESSMANAGER_H
