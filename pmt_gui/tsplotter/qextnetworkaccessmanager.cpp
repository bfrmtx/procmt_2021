#include "qextnetworkaccessmanager.h"

QNetworkReply *QExtNetworkAccessManager::propfind(const QNetworkRequest & request, const QByteArray &body) {
    return sendCustomRequest(request, "PROPFIND", body);
}

QNetworkReply *QExtNetworkAccessManager::propfind(QNetworkRequest request, Depth depth, const QByteArray & body) {
    switch (depth) {
        case Depth::ONE:  request.setRawHeader("Depth", "1"); break;
        case Depth::ZERO: request.setRawHeader("Depth", "0"); break;
        case Depth::INF:  request.setRawHeader("Depth", "Infinity"); break;        
    }
    return sendCustomRequest(request, "PROPFIND", body);
}

QNetworkReply * QExtNetworkAccessManager::get(QNetworkRequest request, size_t begin, size_t end) {
    request.setRawHeader("Range", QString("bytes=%1-%2").arg(begin).arg(end).toLatin1());
    return QNetworkAccessManager::get(request);
}

QNetworkReply * QExtNetworkAccessManager::get(QNetworkRequest const & request) {
    return QNetworkAccessManager::get(request);
}
