#include "tsnetworkhandler.h"
#include <iostream>

/**
 * defines username and password for webservice access
 */
#define WEB_USER                            "aduuser"
#define WEB_PASS                            "neptun"
#define SSL_CERT_ORGANIZATION               "Metronix GmbH"
#define SSL_CERT_ORGANIZATION_UNIT_NAME     "Geophysics"

constexpr int ATS_HEADER_SIZE = 1024;

QList<QSslCertificate> ATSNetworkManager::getHostCertificates(const QString & host_address) {
    QList<QSslCertificate> certificates;

    QSslSocket socket;
    QList<QSslCertificate> clHostCertsTemp;


    auto connection_url = host_address.right(host_address.size() - host_address.lastIndexOf("/") - 1);

    certificates.clear();

    socket.setPeerVerifyMode(QSslSocket::QueryPeer);
    socket.connectToHostEncrypted(connection_url, 443);
    socket.waitForConnected(2000);
    socket.waitForEncrypted(2000);

    if (socket.isValid() == true) {
        clHostCertsTemp = socket.peerCertificateChain();

        for (QSslCertificate & cert : clHostCertsTemp) {
            qDebug() << "--- certificate found begin ---";
            qDebug() << cert.issuerInfo(QSslCertificate::Organization);
            qDebug() << cert.issuerInfo(QSslCertificate::OrganizationalUnitName);
            qDebug() << cert.issuerInfo(QSslCertificate::CountryName);
            qDebug() << cert.issuerInfo(QSslCertificate::CommonName);
            qDebug() << "---- certificate found end ----";

            if ((cert.issuerInfo(QSslCertificate::Organization).contains(SSL_CERT_ORGANIZATION) == true) &&
                (cert.issuerInfo(QSslCertificate::OrganizationalUnitName).contains(SSL_CERT_ORGANIZATION_UNIT_NAME) == true)) {
                certificates.push_back(cert);
            }
        }
        socket.close();
    }
    return certificates;
}

ATSNetworkManager::ATSNetworkManager(const QString & host_address, QObject * parent) : QObject(parent) {
    QTimer::singleShot(100, this, [this, host_address](){
        m_certificates = getHostCertificates(host_address);
        if(m_certificates.isEmpty()) {
            emit connectionVerificationFailed();
        } else {
            emit connectionVerified();
        }
    });
}

QNetworkRequest ATSNetworkManager::getSSLConfiguredRequest(QString const & target) {
    QNetworkRequest request((QUrl(target)));

    QSslConfiguration config    = request.sslConfiguration();
    config.setCaCertificates(m_certificates);

    if(m_certificates.size() > 0) {
        config.setLocalCertificate  (m_certificates.at(0));
        config.setPrivateKey        (m_certificates.at(0).publicKey());
        config.setProtocol          (QSsl::AnyProtocol);
        config.setPeerVerifyMode    (QSslSocket::VerifyNone);
        request.setSslConfiguration(config);
    } else {
        qDebug () << "no valid host certs found -> communication broken!";
    }

    QUrl url = request.url();
    url.setUserName   ("aduuser");
    url.setPassword   ("neptun");
    request.setUrl    (url);

    return request;
}

void ATSNetworkManager::handleBufferedHeaderAnswer(const QString & uri, const QByteArray &array) {
    --m_expected_packages;
    m_buffers.push_back({uri, array});
    if(m_expected_packages == 0) {
        emit atsHeadersReceived(m_buffers);
        m_buffers.clear();
    }
}

void ATSNetworkManager::handleBufferedAnswer(const QString &uri, QNetworkReply *reply) {
    --m_expected_packages;
    int32_t tmp = 0;

    std::vector<int32_t> data;

    QDataStream ds(reply);

    int counter = 0;
    while(!ds.atEnd()) {
        ++counter;
        ds >> tmp;
        data.push_back(tmp);
    }

    m_buffers_vector_data.push_back({uri, data});

    if(m_expected_packages == 0) {
        emit atsDataReceived(m_buffers_vector_data);
        m_buffers_vector_data.clear();
    }
}

void ATSNetworkManager::execSecuredGetRequests(const QStringList URIs, size_t begin, size_t end) {
    m_expected_packages = URIs.size();
    for(auto & uri : URIs) {
        execSecuredGetRequestRaw(uri, begin, end, [this, uri](QNetworkReply * reply) {
            this->handleBufferedAnswer(uri, reply);
        });
    }
}

void ATSNetworkManager::execGetATSHeadersRequest(const QStringList URIs) {
    m_expected_packages = URIs.size();
    for(auto & uri : URIs) {
        execSecuredGetRequest(uri, 0, ATS_HEADER_SIZE - 1, [this, uri](QByteArray const & answer) {
            this->handleBufferedHeaderAnswer(uri, answer);
        });
    }
}
