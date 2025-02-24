#include "adunetwork.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QSslKey>

/**
 * if set, we are working in HTTP mode instead of HTTPS
 */
// #define C_MODE_HTTP 1

/*!
 * \brief Creates a adunetwork object.
 * \param parent
 */
adunetwork::adunetwork(QObject *parent) :
    QObject(parent),
    m_netMgr(new QNetworkAccessManager(this)),
    m_timer(new QTimer(this)),
    m_jobsTimer(new QTimer(this)) {
  connect(m_timer, SIGNAL(timeout()), this, SLOT(fullFetch()));
  connect(m_jobsTimer, SIGNAL(timeout()), this, SLOT(fetchJobs()));
}

/*!
 * \brief Deletes a adunetwork object.
 */
adunetwork::~adunetwork() {
  stop();
  // m_netMgr is bound to lifetime of adunetwork, see CTOR
  // m_timer is bound to lifetime of adunetwork, see CTOR
}

/*!
 * \brief Sets the url where adunetwork will expect the adu web service.
 * \param url
 *
 * The url is expected to point at the base of the web service. E.g.:
 * - Web service: http://myadu:8181/ws/AduWebService.php
 * - Proper url: http://myadu:8181
 *
 * MWI: only accepts https, as ADU-08e now only supports https
 */
void adunetwork::setUrl(QString url) {
#ifdef C_MODE_HTTP
  url.replace("https://", "http://", Qt::CaseInsensitive);
#else
  url.replace("http://", "https://", Qt::CaseInsensitive);
#endif

  m_url = url;

  qDebug() << "[" << Q_FUNC_INFO << "] using URL:" << m_url;

#ifndef C_MODE_HTTP
  // try to load SSL certificates from host
  this->getHostCert(this->clHostCerts, this->m_url);
#endif
}

/**
 * @brief Returns the current url connection string
 * @return
 */
QString adunetwork::urlString() const {
  return m_url;
}

/*!
 * \brief Sets the path to a directory where fetched resources should be stored.
 * \param dir This should be an absolute Path to an existing, writable directory.
 *
 * E.g.:
 * - C:\Users\public\metronix\adu_yyyy_MM_dd
 * - QStandardPath::writableLocation(QStandardPaths::DocumentsLocation)
 */
void adunetwork::setSaveDir(QString dir) {
  m_saveDir = dir;
}

/*!
 * \brief Starts continuous fetching of a predefined set of resources.
 *
 * This will cause adunetwork query the web service for HwConfig, HwDatabase,
 * HwStatus and Selftest once a second until either stop() is called or
 * the adunetwork object gets deleted.
 *
 * The resulting data will be written into files in the directory set via
 * setSaveDir(). The files will be overridden once a second. Each time a file
 * is written resultFetched() will be emmitted.
 */
void adunetwork::run() {
  // MWI: update only every 5s. split the job update and the status update
  m_timer->start(5000);
  QThread::msleep(2500);
  m_jobsTimer->start(5000);
}

/*!
 * \brief Stops adunetwork from sending any more requests to the web service.
 *
 * Requests that have already been sent will not be aborted and the respective
 * results will still be written to local storage when they arrive.
 */
void adunetwork::stop() {
  m_timer->stop();
  m_jobsTimer->stop();
}

/*!
 * \brief Pauses the given activity
 * \param activity
 */
void adunetwork::pause(adunetwork::Activity activity) {
  switch (activity) {
  case All:
    stop();
    break;
  case JobFetch:
    m_jobsTimer->stop();
    break;
  case ConfigFetch:
    m_timer->stop();
  }
}

/*!
 * \brief Resumes the given activity
 * \param activity
 */
void adunetwork::resume(adunetwork::Activity activity) {
  switch (activity) {
  case All:
    run();
    break;
  case JobFetch:
    m_jobsTimer->start(4000);
    break;
  case ConfigFetch:
    m_timer->start(2000);
    break;
  }
}

/*!
 * \brief Fetches a single resource via the adu web service.
 * \param resourceType The kind of xml data you want to retrieve.
 *
 * Sends a single request for the according resource type. If the request
 * completes successfully resourceFetched() will be emitted to inform you about
 * the location of the received data.
 *
 * Note: While a request for jobs is running further fetch requests to "jobs" are being
 *  ignored until all subrequests have returned.
 */
void adunetwork::fetchResource(adunetwork::ResourceType resourceType) {
  QMutexLocker lock(&m_mutex2);
  QNetworkReply *aPromise = nullptr;

  QString qstrRequestString;

  // prepare request string
  switch (resourceType) {
  case HwConfig: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=hwConfig";
    break;
  }
  case HwDatabase: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=hwDatabase";
    break;
  }
  case HwStatus: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=hwStatus";
    break;
  }
  case ChannelConfig: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=channelConfig";
    break;
  }
  case Jobs: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=jobs";
    break;
  }
  case FutureJobs: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=futureJobs";
    break;
  }
  case Selftest: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=selftest";
    break;
  }
  case StopJob: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=stopJob";
    break;
  }
  case ClearJobs: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=clearJobs";
    break;
  }
  case ClearFutureJobs: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=clearFutureJobs";
    break;
  }
  case CopyTSToUSB: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=startCopyTSToUSB";
    break;
  }
  case EjectSD: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=ejectTSDataSDCard";
    break;
  }
  case InsertSD: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=insertTSDataSDCard";
    break;
  }
  case FormatSD: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=formatTSDataSDCard";
    break;
  }
  case RequestSensorDetection: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=requestSensorDetection";
    break;
  }
  case RequestStartSensorTest: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=restartSensorTest";
    break;
  }
  case JobListNamesADU: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=joblistnames";
    break;
  }
  case ToggleGPSDynaMode: {
    qstrRequestString = m_url + "/ws/AduWebService.php?resource=toggleGPSDynaMode";
    break;
  }
  default:
    break;
  }

  // create request and configure SSL
  QNetworkRequest clRequest = QNetworkRequest(QUrl(qstrRequestString));

#ifndef C_MODE_HTTP
  QSslConfiguration config = clRequest.sslConfiguration();

  config.setCaCertificates(this->clHostCerts);
  if (this->clHostCerts.size() > 0) {
    config.setLocalCertificate(this->clHostCerts.at(0));
    config.setPrivateKey(this->clHostCerts.at(0).publicKey());
    config.setProtocol(QSsl::AnyProtocol);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    clRequest.setSslConfiguration(config);
  } else {
    qDebug() << "no valid host certs found -> communication broken!";
    return;
  }
#endif

  QUrl clUrl = clRequest.url();
  clUrl.setUserName(C_WEBSERVICE_USER);
  clUrl.setPassword(C_WEBSERVICE_PASS);
  clRequest.setUrl(clUrl);
  clRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml; charset=utf-8");

  switch (resourceType) {
  case HwConfig: {
    QNetworkReply *hwConfigPromise = m_netMgr->get(clRequest);
    aPromise = hwConfigPromise;
    connect(hwConfigPromise, SIGNAL(finished()), this, SLOT(onGetHwConfigFinished()));
    break;
  }
  case HwDatabase: {
    QString hwDatabaseRequest = m_url + "/ws/AduWebService.php?resource=hwDatabase";
    QNetworkReply *hwDatabasePromise = m_netMgr->get(clRequest);
    aPromise = hwDatabasePromise;
    connect(hwDatabasePromise, SIGNAL(finished()), this, SLOT(onGetHwDatabaseFinished()));
    break;
  }
  case HwStatus: {
    QString hwStatusRequest = m_url + "/ws/AduWebService.php?resource=hwStatus";
    QNetworkReply *hwStatusPromise = m_netMgr->get(clRequest);
    aPromise = hwStatusPromise;
    connect(hwStatusPromise, SIGNAL(finished()), this, SLOT(onGetHwStatusFinished()));
    break;
  }
  case ChannelConfig: {
    QString channelConfigRequest = m_url + "/ws/AduWebService.php?resource=channelConfig";
    QNetworkReply *channelConfigPromise = m_netMgr->get(clRequest);
    aPromise = channelConfigPromise;
    connect(channelConfigPromise, SIGNAL(finished()), this, SLOT(onGetChannelConfigFinished()));
    break;
  }
  case JobListNamesADU: {
    QString jobListNamesRequest = m_url + "/ws/AduWebService.php?resource=joblistnames";
    QNetworkReply *jobListNamesPromise = m_netMgr->get(clRequest);
    aPromise = jobListNamesPromise;
    connect(jobListNamesPromise, SIGNAL(finished()), this, SLOT(onGetJoblistNamesADUFinished()));
    break;
  }
  case Jobs: {
    if (!m_pendingRequests.isEmpty()) {
      return;
    }
    QString jobsRequest = m_url + "/ws/AduWebService.php?resource=jobs";
    QNetworkReply *jobsPromise = m_netMgr->get(clRequest);
    aPromise = jobsPromise;
    connect(jobsPromise, SIGNAL(finished()), this, SLOT(onGetJobsFinished()));
    break;
  }
  case FutureJobs: {
    if (!m_pendingRequests.isEmpty()) {
      return;
    }
    QString futureJobsRequest = m_url + "/ws/AduWebService.php?resource=futureJobs";
    QNetworkReply *futureJobsPromise = m_netMgr->get(clRequest);
    aPromise = futureJobsPromise;
    connect(futureJobsPromise, SIGNAL(finished()), this, SLOT(onGetJobsFinished()));
    break;
  }
  case Selftest: {
    QString selftestRequest = m_url + "/ws/AduWebService.php?resource=selftest";
    QNetworkReply *selftestPromise = m_netMgr->get(clRequest);
    aPromise = selftestPromise;
    connect(selftestPromise, SIGNAL(finished()), this, SLOT(onGetSelftestFinished()));
    break;
  }
  case StopJob: {
    QString stopJobRequest = m_url + "/ws/AduWebService.php?resource=stopJob";
    QNetworkReply *stopJobPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = stopJobPromise;
    connect(stopJobPromise, SIGNAL(finished()), this, SLOT(onPostStopJobFinished()));
    break;
  }
  case CopyTSToUSB: {
    QString copyTSToUSBRequest = m_url + "/ws/AduWebService.php?resource=startCopyTSToUSB";
    QNetworkReply *copyTSToUSBPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = copyTSToUSBPromise;
    connect(copyTSToUSBPromise, SIGNAL(finished()), this, SLOT(onPostCopyTSToUSBFinished()));
    break;
  }
  case EjectSD: {
    QNetworkReply *ejectSDPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = ejectSDPromise;
    connect(ejectSDPromise, SIGNAL(finished()), this, SLOT(onPostEjectTSDataSDCardFinished()));
    break;
  }
  case InsertSD: {
    QNetworkReply *insertSDPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = insertSDPromise;
    connect(insertSDPromise, SIGNAL(finished()), this, SLOT(onPostInsertTSDataSDCardFinished()));
    break;
  }
  case FormatSD: {
    QNetworkReply *formatSDPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = formatSDPromise;
    connect(formatSDPromise, SIGNAL(finished()), this, SLOT(onPostFormatTSDataSDCardFinished()));
    break;
  }
  case ToggleGPSDynaMode: {
    QNetworkReply *toggleGPSDynaModePromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = toggleGPSDynaModePromise;
    connect(toggleGPSDynaModePromise, SIGNAL(finished()), this, SLOT(onPostToggleGPSDynaModePromiseFinished()));
    break;
  }
  case RequestSensorDetection: {
    QNetworkReply *requestSensorDetectionPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = requestSensorDetectionPromise;
    connect(requestSensorDetectionPromise, SIGNAL(finished()), this, SLOT(onPostRequestSensorDetectionFinished()));
    break;
  }
  case RequestStartSensorTest: {
    QNetworkReply *requestStartSensorTestPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = requestStartSensorTestPromise;
    connect(requestStartSensorTestPromise, SIGNAL(finished()), this, SLOT(onPostRequestStartSensorTestFinished()));
    break;
  }
  case ClearJobs: {
    // TUT 7: ClearJobs already existed;
    //   the network replies need to be handled manually qt will not delete them automatically;
    //   hence the connection, now the webservice needs to be checked
    QString clearJobsRequest = m_url + "/ws/AduWebService.php?resource=clearJobs";
    QNetworkReply *clearJobsPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = clearJobsPromise;
    connect(clearJobsPromise, SIGNAL(finished()), this, SLOT(onClearJobsFinished()));
    break;
  }
  case ClearFutureJobs: {
    // TUT 17: A new case for a new request, it'll be a POST since it will change data;
    //   We'll need a new handler;
    QString clearFutureJobsRequest = m_url + "/ws/AduWebService.php?resource=clearFutureJobs";
    QNetworkReply *clearFutureJobsPromise = m_netMgr->post(clRequest, QByteArray());
    aPromise = clearFutureJobsPromise;
    connect(clearFutureJobsPromise, SIGNAL(finished()), this, SLOT(onClearFutureJobsFinished()));
    break;
  }
  default:
    break;
  }

  if (m_doTracking && aPromise != nullptr) {
    startTracking(*aPromise);
  }
}

/*!
 * \brief Override of fetchResource to allow requesting multiple resources at once.
 * \param resourceTypes A list of resource types that you want to request.
 *
 * A single resource will be requested multiple times if the resourceTypes list
 * contains duplicates.
 */
void adunetwork::fetchResource(QList<adunetwork::ResourceType> resourceTypes) {
  for (auto resource : resourceTypes) {
    fetchResource(resource);
  }
}

QByteArray *adunetwork::hwConfigQba() {
  return &m_hwConfig;
}

QByteArray *adunetwork::hwDatabaseQba() {
  return &m_hwDatabase;
}

QByteArray *adunetwork::hwStatusQba() {
  return &m_hwStatus;
}

QByteArray *adunetwork::channelConfigQba() {
  return &m_channelConfig;
}

QByteArray *adunetwork::selftestQba() {
  return &m_selftest;
}

void adunetwork::sendJob(QString jobxml) {
  QString postJobRequest = m_url + "/ws/AduWebService.php?resource=job";

  QByteArray const data = jobxml.toUtf8();

  QUrl url(postJobRequest);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml; charset=utf-8");

  QNetworkReply *postJobPromise = m_netMgr->post(request, data);

  if (m_doTracking) {
    startTracking(*postJobPromise);
  }
  connect(postJobPromise, SIGNAL(finished()), this, SLOT(onPostJobFinished()));
}

void adunetwork::sendChannelConfig(QString channelConfigXml) {
  QString postChannelConfigRequest = m_url + "/ws/AduWebService.php?resource=channelConfig";
  QNetworkReply *postChannelConfigPromise = m_netMgr->post(QNetworkRequest(QUrl(postChannelConfigRequest)), channelConfigXml.toUtf8());
  if (m_doTracking) {
    startTracking(*postChannelConfigPromise);
  }
  connect(postChannelConfigPromise, SIGNAL(finished()), this, SLOT(onPostChannelConfigFinished()));
}

void adunetwork::startJoblistADU(QString jobListParamsXml) {
  QString postStartJoblistADURequest = m_url + "/ws/AduWebService.php?resource=startJoblist";
  QNetworkReply *postStartJoblistADUPromise = m_netMgr->post(QNetworkRequest(QUrl(postStartJoblistADURequest)), jobListParamsXml.toUtf8());
  if (m_doTracking) {
    startTracking(*postStartJoblistADUPromise);
  }
  connect(postStartJoblistADUPromise, SIGNAL(finished()), this, SLOT(onPostStartJoblistADUFinished()));
}

/*!
 * \brief A private method to create a request for each predefined resource.
 */
void adunetwork::fullFetch() {
  QList<adunetwork::ResourceType> resourcesToFetch;
  resourcesToFetch << adunetwork::HwConfig
                   << adunetwork::HwDatabase
                   << adunetwork::HwStatus
                   << adunetwork::Selftest
                   << adunetwork::ChannelConfig
                   << adunetwork::JobListNamesADU;
  fetchResource(resourcesToFetch);
}

void adunetwork::fetchJobs() {
  QList<adunetwork::ResourceType> resourcesToFetch;
  resourcesToFetch << adunetwork::FutureJobs;
  fetchResource(resourcesToFetch);
}

/*!
 * \brief A private method to handle the result of a request for the HwConfig.
 *
 * Only called when the according request finishes successfully. Will write, or
 * override, the file "HwConfig.xml" with new data.
 *
 * Emmits resourceFetched() upon completion.
 */
void adunetwork::onGetHwConfigFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting HwConfig:" << reply->error() << reply->errorString();
    return;
  }

  // writing data
  m_hwConfig = reply->readAll();
  QString hwConfigData = (QString)m_hwConfig;
  QFile file(m_saveDir + "/HwConfig.xml");
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out << hwConfigData;
  }
  file.close();

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // freeing the resource
  reply->deleteLater();

  // signalling
  emit resourceFetched(adunetwork::HwConfig, file.fileName());
}

/*!
 * \brief A private method to handle the result of a request for the HwStatus.
 *
 * Only called when the according request finishes successfully. Will write, or
 * override, the file "HwStatus.xml" with new data.
 *
 * Emmits resourceFetched() upon completion.
 */
void adunetwork::onGetHwStatusFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting HwStatus:" << reply->error() << reply->errorString();
    return;
  }

  // writing data
  m_hwStatus = reply->readAll();
  QString hwStatusData = (QString)m_hwStatus;
  QFile file(m_saveDir + "/HwStatus.xml");
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out << hwStatusData;
  }
  file.close();

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // freeing the resource
  reply->deleteLater();

  // signalling
  emit resourceFetched(adunetwork::HwStatus, file.fileName());
}

/*!
 * \brief A private method to handle the result of a request for the HwDatabase.
 *
 * Only called when the according request finishes successfully. Will write, or
 * override, the file "HwDatabase.xml" with new data.
 *
 * Emmits resourceFetched() upon completion.
 */
void adunetwork::onGetHwDatabaseFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting HwDatabase:" << reply->error() << reply->errorString();
    return;
  }

  // writing data
  m_hwDatabase = reply->readAll();
  QString hwDatabaseData = (QString)m_hwDatabase;
  QFile file(m_saveDir + "/HwDatabase.xml");
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out << hwDatabaseData;
  }
  file.close();

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // freeing the resource
  reply->deleteLater();

  // signalling
  emit resourceFetched(adunetwork::HwDatabase, file.fileName());
}

/*!
 * \brief A private method to handle the result of a request for the ChannelConfig.
 *
 * Only called when the according request finishes successfully. Will write, or
 * override, the file "ChannelConfig.xml" with new data.
 *
 * Emmits resourceFetched() upon completion.
 */
void adunetwork::onGetChannelConfigFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting ChannelConfig:" << reply->error() << reply->errorString();
    return;
  }

  // writing data
  m_channelConfig = reply->readAll();
  QString channelConfigData = (QString)m_channelConfig;
  QFile file(m_saveDir + "/ChannelConfig.xml");
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out << channelConfigData;
  }
  file.close();

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // freeing the resource
  reply->deleteLater();

  // signalling
  emit resourceFetched(adunetwork::ChannelConfig, file.fileName());
}

/*!
 * \brief A private method to handle the result of a request for the Selftest.
 *
 * Only called when the according request finishes successfully. Will write, or
 * override, the file "Selftest.xml" with new data.
 *
 * Emmits resourceFetched() upon completion.
 */
void adunetwork::onGetSelftestFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting HwConfig:" << reply->error() << reply->errorString();
    return;
  }

  // writing data
  m_selftest = reply->readAll();
  QString selftestData = (QString)m_selftest;
  QFile file(m_saveDir + "/Selftest.xml");
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out << selftestData;
  }
  file.close();

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // freeing the resource
  reply->deleteLater();

  // signalling
  emit resourceFetched(adunetwork::Selftest, file.fileName());
}

/*!
 * \brief A private method to handle the result of a request for the joblist names.
 *
 * Only called when the according request finishes successfully. Will write, or
 * override, the file "Selftest.xml" with new data.
 *
 * Emmits resourceFetched() upon completion.
 */
void adunetwork::onGetJoblistNamesADUFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting Joblist Names from ADU:" << reply->error() << reply->errorString();
    return;
  }

  // writing data
  m_JobListNamesADU = reply->readAll();
  QString qstrData = (QString)m_JobListNamesADU;
  QFile file(m_saveDir + "/JobListNamesADU.xml");
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out << qstrData;
  }
  file.close();

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // freeing the resource
  reply->deleteLater();

  // signalling
  emit resourceFetched(adunetwork::JobListNamesADU, file.fileName());
}

/*!
 * \brief A private method to handle the result of a request for the list of jobs.
 *
 * Expects a xml jobdescriptor list as a result.
 * E.g.:
 * <jobdescriptors>
 *  <jobdescriptor id="" start="" stop="" isMeasurement="" freq="">
 * </jobdescriptors>
 *
 * For each entry in the list a request to the webservice will be made.
 */
void adunetwork::onGetJobsFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting HwConfig:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // evaluate count
  QString jobsString = (QString)reply->readAll();
  reply->deleteLater();

  QList<adujobdescriptor> jobdescriptors = adujobdescriptor::buildListFromXml(jobsString);
  for (adujobdescriptor entry : jobdescriptors) {
    m_pendingRequests.insert(entry.m_id, entry);
  }

  // wipe data
  QDir dir(m_saveDir);
  if (dir.cd("jobs")) {
    dir.removeRecursively();
    dir.cdUp();
  }
  dir.mkdir("jobs");

  // if there are no jobs, we update with an empty directory
  if (m_pendingRequests.isEmpty()) {
    emit resourceFetched(adunetwork::Jobs, m_saveDir + "/jobs");
    return;
  }

  // request the jobs
  for (int id : m_pendingRequests.keys()) {
    QString jobRequest = m_url + "/ws/AduWebService.php?resource=job&id=%1";
    jobRequest = jobRequest.arg(id);
    QNetworkReply *jobPromise = m_netMgr->get(QNetworkRequest(QUrl(jobRequest)));
    jobPromise->setProperty("id", id);
    if (m_doTracking) {
      startTracking(*jobPromise);
    }
    connect(jobPromise, SIGNAL(finished()), this, SLOT(onGetJobFinished()));
  }
}

/*!
 * \brief A private method to handle the result of a job subrequest
 */
void adunetwork::onGetJobFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // evaluate id
  QVariant idProperty = reply->property("id");
  bool ok;
  int id = idProperty.toInt(&ok);
  if (!ok) {
    reply->deleteLater();
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while getting Job:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  // check isMeasurement
  const adujobdescriptor descriptor = m_pendingRequests.value(id);
  if (descriptor.m_isMeasurement) {
    // write data
    m_jobs[id] = reply->readAll();
    QString jobData = (QString)m_jobs[id];
    QString startString = descriptor.m_start.toString("yyyy-MM-dd_hh-mm-ss");
    QString freqString = QString::number((int)descriptor.m_freq);
    QFile file(m_saveDir + QString("/jobs/%1_%2H.xml").arg(startString, freqString));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
      QTextStream out(&file);
      out << jobData;
    } else {
      qDebug() << "can't write job!!";
    }
    file.close();
  }

  reply->deleteLater();
  m_pendingRequests.remove(id);
  if (m_pendingRequests.isEmpty()) {
    emit resourceFetched(adunetwork::Jobs, m_saveDir + "/jobs");
  }
}

/*!
 * \brief A private method to handle the result of a stopJob request
 */
void adunetwork::onPostStopJobFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting StopJob:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::StopJob, "");
}

void adunetwork::onPostCopyTSToUSBFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting CopyTSToUSB:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::CopyTSToUSB, "");
}

void adunetwork::onPostEjectTSDataSDCardFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting EjectTSDataSDCard:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::EjectSD, "");
}

void adunetwork::onPostInsertTSDataSDCardFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting InsertTSDataSDCard:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::InsertSD, "");
}

void adunetwork::onPostFormatTSDataSDCardFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting FormatTSDataSDCard:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::FormatSD, "");
}

void adunetwork::onPostRequestSensorDetectionFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting RequestSensorDetection:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::RequestSensorDetection, "");
}

void adunetwork::onPostRequestStartSensorTestFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting RequestStartSensorTest:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::RequestStartSensorTest, "");
}

void adunetwork::onPostToggleGPSDynaModePromiseFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting Toggle GPS Dyna Mode:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::ToggleGPSDynaMode, "");
}

void adunetwork::onPostJobFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting job:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
}

void adunetwork::onPostChannelConfigFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting hwConfig:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
}

void adunetwork::onPostStartJoblistADUFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while starting ADU joblist:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
}

void adunetwork::onClearJobsFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting clearJobs:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::ClearJobs, "");
  // TUT 11: Every request should have some slot for reply handling;
  //  every reply needs to be deleted, otherwise they'll pile up on the heap;
  //  I also emit a signal informing adurt or anyone who cares to listen about successful replies;
  //  the handling of the clearJobs reply is trivial, other replies require more work;
}

void adunetwork::onClearFutureJobsFinished() {
  // Checking validity of reply
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply == 0) {
    qDebug() << "Sender was not a QNetworkReply";
    return;
  }

  // check error
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error while posting clearFutureJobs:" << reply->error() << reply->errorString();
    return;
  }

  if (m_doTracking) {
    finishTracking(*reply);
  }

  reply->deleteLater();
  emit resourceFetched(adunetwork::ClearFutureJobs, "");
  // TUT 18: It's basically same code over again, this really needs to be refactored;
  //   back to the web service;
}

/**
 * @brief Tracks a NetworkRequest
 * @param aPromise - Not null
 * Appends a line to [documents]/metronix/connectionLog.txt
 */
void adunetwork::startTracking(QNetworkReply &aPromise) {
  QMutexLocker lock(&m_mutex);
  aPromise.setProperty("trackId", m_conId);
  QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  QFile conFile(docsPath + "/metronix/connectionLog.txt");
  if (conFile.open(QFile::Append | QFile::WriteOnly)) {
    QTextStream out(&conFile);
    out << QDateTime::currentMSecsSinceEpoch() << ";";  // timestamp
    out << m_conId << ";";                              // conId
    out << "request" << ";";                            // direction
    out << aPromise.request().url().toString() << "\n"; // url
  }
  conFile.close();
  ++m_conId;
}

/**
 * @brief Tracks reception of an answer
 * @param aPromise - Not null
 * Appends a line to [documents]/metronix/connectionLog.txt
 */
void adunetwork::finishTracking(QNetworkReply &aPromise) {
  QMutexLocker lock(&m_mutex);
  unsigned int conId = aPromise.property("trackId").toUInt();
  QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  QFile conFile(docsPath + "/metronix/connectionLog.txt");
  if (conFile.open(QFile::Append | QFile::WriteOnly)) {
    QTextStream out(&conFile);
    out << QDateTime::currentMSecsSinceEpoch() << ";";  // timestamp
    out << conId << ";";                                // conId
    out << "answer" << ";";                             // direction
    out << aPromise.request().url().toString() << "\n"; // url
  }
  conFile.close();
}

QList<adunetwork::adujobdescriptor> adunetwork::adujobdescriptor::buildListFromXml(const QString &xml) {
  QList<adunetwork::adujobdescriptor> result;
  tinyxml2::XMLDocument xmldoc;
  tinyxml2::XMLError error = xmldoc.Parse(xml.toStdString().c_str());
  if (error != tinyxml2::XML_SUCCESS) {
    return result;
  }
  tinyxml2::XMLHandle handle(&xmldoc);
  tinyxml2::XMLElement *xmldescriptor = handle.FirstChildElement("jobdescriptors").FirstChildElement("jobdescriptor").ToElement();
  while (xmldescriptor) {
    QString idData(xmldescriptor->Attribute("id"));
    QString startData(xmldescriptor->Attribute("start"));
    QString stopData(xmldescriptor->Attribute("stop"));
    QString isMeasurementData(xmldescriptor->Attribute("isMeasurement"));
    QString freqData(xmldescriptor->Attribute("freq"));

    int id = idData.toInt();
    QDateTime start = QDateTime::fromString(startData, "yyyy-MM-dd hh:mm:ss");
    start.setTimeZone(QTimeZone::utc());
    QDateTime stop = QDateTime::fromString(stopData, "yyyy-MM-dd hh:mm:ss");
    stop.setTimeZone(QTimeZone::utc());
    bool isMeasurement = (isMeasurementData == "1");
    double freq = freqData.toDouble();

    adunetwork::adujobdescriptor jobdescriptor; // For some reason I can't use a initializer List here
    jobdescriptor.m_id = id;
    jobdescriptor.m_start = start;
    jobdescriptor.m_stop = stop;
    jobdescriptor.m_isMeasurement = isMeasurement;
    jobdescriptor.m_freq = freq;
    result << jobdescriptor;

    xmldescriptor = xmldescriptor->NextSiblingElement("jobdescriptor");
  }
  return result;
}

bool adunetwork::getHostCert(QList<QSslCertificate> &clCerts, const QString qstrUrl) {
  bool bRetValue = false;
  QSslSocket clSocket;
  QString qstrTemp;
  QList<QSslCertificate> clHostCertsTemp;

  qstrTemp = qstrUrl;
  qstrTemp = qstrTemp.right(qstrTemp.size() - qstrTemp.lastIndexOf("/") - 1);

  clCerts.clear();

  qDebug() << clSocket.protocol();

  // clSocket.setProtocol(QSsl::TlsV1_1);
  clSocket.setProtocol(QSsl::TlsV1_2OrLater);

  clSocket.setPeerVerifyMode(QSslSocket::QueryPeer);

  clSocket.connectToHostEncrypted(qstrTemp, 443);
  clSocket.waitForConnected(2000);
  clSocket.waitForEncrypted(2000);
  if (clSocket.isValid() == true) {
    clHostCertsTemp = clSocket.peerCertificateChain();

    for (const QSslCertificate &cert : clHostCertsTemp) {
      qDebug() << cert.issuerInfo(QSslCertificate::Organization);
      qDebug() << cert.issuerInfo(QSslCertificate::OrganizationalUnitName);
      qDebug() << cert.issuerInfo(QSslCertificate::CountryName);

      clCerts.push_back(clHostCertsTemp.at(0));
      //            if ((cert.issuerInfo(QSslCertificate::Organization).contains            (C_SSL_CERT_ORGANIZATION)           == true) &&
      //                (cert.issuerInfo(QSslCertificate::OrganizationalUnitName).contains  (C_SSL_CERT_ORGANIZATION_UNIT_NAME) == true))
      //            {
      //                clCerts.push_back(cert);
      //                bRetValue = true;
      //            }
    }
    clSocket.close();
  } else {
    bRetValue = false;
  }

  return (bRetValue);
}

QByteArray *adunetwork::joblistNamesADUQba() {
  return (&this->m_JobListNamesADU);
}
