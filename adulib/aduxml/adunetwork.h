#ifndef ADUNETWORK_H
#define ADUNETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QTimer>
#include <QSslCertificate>
#include <QList>

#include "tinyxml2.h"
#include "adulib.h"


/**
 * defines the valid values for host certificates for organization
 * and OrganizationalUnitName.
 */
#define C_SSL_CERT_ORGANIZATION             "Metronix GmbH"
#define C_SSL_CERT_ORGANIZATION_UNIT_NAME   "Geophysics"


/**
 * defines username and password for webservice access
 */
#define C_WEBSERVICE_USER   "aduuser"
#define C_WEBSERVICE_PASS   "neptun"


// Add DEFINES += MTX_LOG_CONNECTIONS in some reachable .pro or .pri file to
//  get a logfile written to <documents>/metronix/connectionLog.txt
#ifdef MTX_LOG_CONNECTIONS
    #define C_MTX_DO_LOG_CONNECTIONS false
#else
    #define C_MTX_DO_LOG_CONNECTIONS true
#endif
// TODO : This file based exchange can be improved to use in memory only,
//          which would require adulib to be able to directly accept strings
//          for initialisation.
/*!
 * \brief The adunetwork class fetches and writes xml resources from and to
 *        an ADU.
 *
 * The ADU holds configuration data as xml-CLOBs in a database. This data is
 * available through a web service. Adunetwork can be used to asynchronously
 * fetch this data to a local file storage.
 *
 * Two use cases are currently supported:
 * - specifically fetching a certain configuration file or a set of files once
 * -- Use the two versions of fetchResource() and connect a slot to
 *    resourceFetched() for handling the incoming results.
 * - continuously fetching a predefined set of configuration files once a second
 * -- Use run() and stop() to control the fetching process and connect a slot
 *    to resourceFetched() to handle the incoming results.
 *
 * To use the above functionality first set the base url of the web service
 * via setUrl() and set the directory path where results should be stored via
 * setSaveDir().
 *
 * Possible resources for fetching are enumerated in ResourceType.
 */
class adunetwork : public QObject
{
    Q_OBJECT

    /*!
     * \brief A private helper struct to represent meta information about jobs
     *        as returned from the first step of asking the web service about
     *        jobs.
     * Offers a parsing function buildListFromXml().
     *
     * When adunetwork is told to fetch jobs from the ADU it first asks the
     * web service to enumerate all availabe entries in the job table. This
     * creates a new list of descriptors which is used to build separate
     * requests only for measurements and keeping track of the requests that
     * are still in flight.
     */
    struct adujobdescriptor
    {
        static QList<adujobdescriptor> buildListFromXml(const QString &xml);

        int m_id = -1; //!< The job's id in the database.
        QDateTime m_start;
        QDateTime m_stop;
        bool m_isMeasurement = false; //!< Some jobs in the database might not be measurements.
        double m_freq = 0.0;
    };

public:
    /*!
     * \brief The ResourceType enum names all resources that adunetwork can
     *        handle.
     */
    enum ResourceType {
        HwConfig,
        HwDatabase,
        HwStatus,
        ChannelConfig,
        Jobs,
        FutureJobs,
        Selftest,
        StopJob,
        ClearJobs,
        ClearFutureJobs,
        CopyTSToUSB,
        EjectSD,
        InsertSD,
        FormatSD,
        RequestSensorDetection,
        JobListNamesADU,
        ToggleGPSDynaMode,
        RequestStartSensorTest
        // TUT 16: Here all the resource types are listed GET and POST; parametrized resources are not listed
    };

    /*!
     * \brief The Activity enum names activities that adunetwork can pause and resume.
     */
    enum Activity {
        ConfigFetch,
        JobFetch,
        All
    };

    explicit adunetwork(QObject *parent = 0);
    ~adunetwork();

    void setUrl(QString url);
    QString urlString() const;
    void setSaveDir(QString dir);

    void run();
    void stop();
    void pause(Activity activity);
    void resume(Activity activity);

    void fetchResource(ResourceType resourceType);
    void fetchResource(QList<ResourceType> resourceTypes);

    /*!
     * The getter functions return pointers to the data that adunetwork
     * has fetched already. The underlying arrays might be empty if fetchResource
     * hasn't been called yet or the initial requests failed.
     *
     * The underlying data is only overwritten when the according requests finish
     * successfully.
     *
     * The returned pointers will never be NULL. Also do not try to delete them.
     *
     * Intended use:
     * - fetchResource()
     * - wait for signal resourceFetched()
     * - now call the according getter
     */
    QByteArray *hwConfigQba();
    QByteArray *hwDatabaseQba();
    QByteArray *hwStatusQba();
    QByteArray *channelConfigQba();
    QByteArray *selftestQba();
    QByteArray *joblistNamesADUQba();

    void sendJob           (QString jobxml);
    void sendChannelConfig (QString channelConfigXml);
    void startJoblistADU   (QString jobListParamsXml);


signals:
    /*!
     * \brief Emitted when a request completes successfully and the data has
     *        been written to local storage.
     * \param resourceType Information about what kind of resource has arrived.
     * \param filePath The absolute Path to the newly written file.
     *
     * Code that uses adunetwork should connect a slot to this signal. This slot
     * would evaluate the provided file based on the resourceType.
     *
     * E.g.: Track arriving files until a HwConfig and a HwDatabase is available
     * and then intialize an adulib object based on those files.
     */
    void resourceFetched(adunetwork::ResourceType resourceType, QString filePath);

private slots:
    void fullFetch();
    void fetchJobs();
    void onGetHwConfigFinished();
    void onGetHwStatusFinished();
    void onGetHwDatabaseFinished();
    void onGetChannelConfigFinished();
    void onGetSelftestFinished();
    void onGetJobsFinished();
    void onGetJobFinished();
    void onGetJoblistNamesADUFinished();
    void onPostStopJobFinished();
    void onPostJobFinished();
    void onPostChannelConfigFinished();
    void onClearJobsFinished();
    void onClearFutureJobsFinished();
    void onPostCopyTSToUSBFinished();
    void onPostEjectTSDataSDCardFinished();
    void onPostInsertTSDataSDCardFinished();
    void onPostFormatTSDataSDCardFinished();
    void onPostRequestSensorDetectionFinished();
    void onPostStartJoblistADUFinished();
    void onPostToggleGPSDynaModePromiseFinished();
    void onPostRequestStartSensorTestFinished();

private:
    QNetworkAccessManager *m_netMgr = nullptr; //!< Used for http requests
    QTimer *m_timer = nullptr; //!< Used for triggering fullFetch() continuously
    QTimer *m_jobsTimer = nullptr; //!< Separate timer for fetching jobs to give them lower prority
    QMap<int, adujobdescriptor> m_pendingRequests; //!< Tracks pending requests for jobs

    QString m_url; //!< The ADU-server URL
    QString m_saveDir; //!< The directory where fetched data will be stored. See setSaveDir().

    QByteArray m_hwConfig;
    QByteArray m_hwDatabase;
    QByteArray m_hwStatus;
    QByteArray m_channelConfig;
    QByteArray m_selftest;
    QByteArray m_JobListNamesADU;
    QMap<int, QByteArray> m_jobs;

    const bool m_doTracking = C_MTX_DO_LOG_CONNECTIONS; //!< control variable for logging connections
    unsigned int m_conId = 0; //!< incremented if logging connections
    void startTracking(QNetworkReply &aPromise);
    void finishTracking(QNetworkReply &aPromise);
    QMutex m_mutex;
    QMutex m_mutex2;


    /**
     * list of all valid host certificates
     */
    QList<QSslCertificate> clHostCerts;

    /**
     * \brief this function tries to load the Hosts SSL certificate.
     *
     * If certificate contents for authority, company and common name fit to
     * ADU-08e, the certificate is accepted and used to encrypt the connection.
     * Internally uses the QSslSocket class to build up initial connection and
     * read the hosts cert files.
     *
     * @param[in] QList<QSslCertificate>& clCerts = reference to list of certificates, all valid certs from host shall be stored in
     * \return bool = true: found valid cert / false: no valid cert found
     */
    bool getHostCert (QList<QSslCertificate>& clCerts, const QString qstrUrl);


};

#endif // ADUNETWORK_H
