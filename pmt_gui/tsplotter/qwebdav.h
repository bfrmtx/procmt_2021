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

#ifndef QWEBDAV_H
#define QWEBDAV_H

#include <stdint.h>
#include <QDate>
#include <QDateTime>
#include <QSharedPointer>
// #include <QtXml>

namespace webdav {
enum class ContentType {
    Unknown = 0,
    Application = 1,
    Httpd = 2,
    Image = 3,
    Text = 4,
    Audio = 5,
    Multipart = 6
};

enum class ContentSubtype {
    Unknown = 0,
    UnixDirectory = 1,
    OctetStream = 2,
    PDF = 3,
    XML = 4,
    JSON = 5,
    DOC = 6,
};

enum class Status {
    Unknown = 0,
    Ok = 200,
    Forbidden = 403,
    Conflict = 409,
    FailedDependency = 422,
    Locked = 423,
};

struct Response {
    Response() = default;
    ~Response() = default;
    Response(const Response &) = default;
    Response &operator=(const Response &) = default;

    int64_t                     content_length = 0;
    QSharedPointer<QDateTime>   creation;
    QSharedPointer<QDateTime>   last_modified;
    QString                     eTag;
    ContentType                 type = ContentType::Unknown;
    ContentSubtype              subtype = ContentSubtype::Unknown;

    QString ref;

    operator QString() const;
};

class WebDavResponse {
public:
    static WebDavResponse parseResponse(QString const & response);
    QVector<Response> const & getResponses() const;
    operator QString() const;

private:
    explicit WebDavResponse(QString const & response);

private:
    QVector<Response> m_responses;
};
}

Q_DECLARE_METATYPE(webdav::Response)

#endif // QWEBDAV_H
