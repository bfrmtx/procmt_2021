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

#ifndef TSNETWORKHANDLER_H
#define TSNETWORKHANDLER_H

#include <iostream>

#include <QString>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslCertificate>
#include <QSslKey>
#include <QTimer>
#include "qextnetworkaccessmanager.h"
#include "qwebdav.h"

class ATSNetworkManager : public QObject {
    Q_OBJECT

private:
    QList<QSslCertificate> getHostCertificates(QString const & host_address);


private:
    void handleMeasurementFolderReply(const QByteArray & reply);

    template<typename FUNC>
    void bindReply(QNetworkReply * reply, FUNC func) {
        connect(reply, &QNetworkReply::finished, this, [this, func]() {
            auto reply = qobject_cast<QNetworkReply*>(sender());
            func(reply->readAll());
            reply->deleteLater();
        });
    }

    template<typename FUNC>
    void bindRawReply(QNetworkReply * reply, FUNC func) {
        connect(reply, &QNetworkReply::finished, this, [this, func]() {
            auto reply = qobject_cast<QNetworkReply*>(sender());
            func(reply);
            reply->deleteLater();
        });
    }

    template<typename FUNC>
    void bindHeadReply(QNetworkReply * reply, FUNC func) {
        connect(reply, &QNetworkReply::finished, this, [this, func]() {
            auto reply = qobject_cast<QNetworkReply*>(sender());
            func(reply->rawHeaderPairs());
            reply->deleteLater();
        });
    }

    template<typename FUNC>
    void bindFullReply(QNetworkReply * reply, FUNC func) {
        connect(reply, &QNetworkReply::finished, this, [this, func]() {
            auto reply = qobject_cast<QNetworkReply*>(sender());
            func(reply);
            reply->deleteLater();
        });
    }

private:
    void handleBufferedHeaderAnswer(const QString & uri, QByteArray const & array);
    void handleBufferedAnswer(const QString &uri, QNetworkReply * reply);

signals:
    void atsHeadersReceived(QVector<QPair<QString, QByteArray>> const & headers);
    void atsDataReceived(QVector<QPair<QString, std::vector<int32_t>>> const & data);
    void connectionVerified();
    void connectionVerificationFailed();

public:
    ATSNetworkManager(QString const & host_address, QObject * parent = nullptr);
    QNetworkRequest getSSLConfiguredRequest(QString const & target);

    /**
     * @brief execSecuredGetRequest executes a get request for the given url. The reply (QByteArray) will be
     * directed to the function given by parameter func.
     * @param url request url for the get request
     * @param func target function which will be called delayed with the reply
     */
    template<typename FUNC>
    void execSecuredGetRequest(const QString url, FUNC func) {
        QNetworkRequest request = getSSLConfiguredRequest(url);
        bindReply(m_network_access_manager.get(request), func);
    }

    template<typename FUNC>
    void execSecuredGetRequest(const QString url, size_t begin, size_t end, FUNC func) {
        QNetworkRequest request = getSSLConfiguredRequest(url);
        bindReply(m_network_access_manager.get(request, begin, end), func);
    }

    template<typename FUNC>
    void execSecuredGetRequestRaw(const QString url, size_t begin, size_t end, FUNC func) {
        QNetworkRequest request = getSSLConfiguredRequest(url);
        bindRawReply(m_network_access_manager.get(request, begin, end), func);
    }

    void execSecuredGetRequests(const QStringList URIs, size_t begin, size_t end);
    void execGetATSHeadersRequest(const QStringList URLs);

    template<typename FUNC>
    QNetworkReply * execSecuredGetRequestObj(const QString url, FUNC func) {
        QNetworkRequest request = getSSLConfiguredRequest(url);
        QNetworkReply * reply = m_network_access_manager.get(request);
        bindFullReply(reply, func);
        return reply;
    }

    template<typename FUNC>
    void execSecuredHeadRequest(const QString url, FUNC func) {
        QNetworkRequest request = getSSLConfiguredRequest(url);
        bindHeadReply(m_network_access_manager.head(request), func);
    }

    template<typename FUNC>
    void execSecuredPropfindRequest(const QString request_string, QByteArray const & body, FUNC func) {
        QNetworkRequest request = getSSLConfiguredRequest(request_string);

        qDebug() << QString("http request: %1").arg(request_string);
        bindReply(m_network_access_manager.propfind(request, QExtNetworkAccessManager::Depth::ONE, body), func);
    }

    void getContent(const QString folder_path, std::function<void(webdav::WebDavResponse const &)> function) {
        execSecuredPropfindRequest(folder_path, "", [function, folder_path](QByteArray const & rep) {
            function(webdav::WebDavResponse::parseResponse(rep));
        });
    }

    void getContentLength(const QString path, std::function<void(quint64)> function) {
        execSecuredHeadRequest(path, [function](QList<QNetworkReply::RawHeaderPair> const & data){
            for(auto & header : data) {
                if(header.first == "Content-Length") {
                    function(header.second.toULongLong());
                }
            }
        });
    }

public:
    QExtNetworkAccessManager m_network_access_manager;
    QList<QSslCertificate> m_certificates;
    QString m_host_address;
    QVector<QPair<QString, QByteArray>> m_buffers;
    QVector<QPair<QString, std::vector<int32_t>>> m_buffers_vector_data;
    int m_expected_packages = 0;
};


#endif // TSNETWORKHANDLER_H
