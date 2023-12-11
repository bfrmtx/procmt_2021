#ifndef ADURT_H
#define ADURT_H

#include <QObject>
#include <QStandardPaths>

#include "adujoblist.h"
#include "adulib.h"
#include "adunetwork.h"

/**
 * @brief The adurt class provides an environment for interacting with an ADU.
 *
 * An adurt Object represents a connection to an actual or virtual ADU. It takes care
 * of setting up the collaboration between adulib, adujoblist and adunetwork.
 *
 * After a connection has been established adurt provides access to a designated adulib
 * object which represents the ADU-Hardware used and the joblist associated with that ADU.
 */
class adurt : public QObject
{
    Q_OBJECT
public:
    enum ConnectionType {
        Disconnected, //!< There is no connection
        ConnectedOnline, //!< adurt is connected to an acutal ADU
        ConnectedOffline //!< adurt is in offline mode backed by a local file system
    };
    Q_ENUM(ConnectionType)

    explicit adurt(QObject *parent = 0);
    void connect_online(QUrl url);
    void connect_filebased(QString path);
    void disconnectAdu();

    QDateTime get_adu_time();

    void clearJobs();
    void clearFutureJobs(); // TUT 14: The declaration
    void clearAllJobsOnAdu(); // TUT 5: declare new function for use case
    void requestJobs();
    void stopCurrentJob();
    void sendNewJob(QString jobxml);

    QStringList fetchADUJoblistNames (void);
    void startADUJoblist (const QString qstrJoblistName, const QString qstrStartDate, const QString qstrStartTime);

    void fetchFutureJobs();
    void pauseJobFetch();
    void resumeJobFetch();
    void submitJobs();
    void submitChannelConfig();
    void startCopyTSToUSB ();
    void ejectTSDataSDCard ();
    void insertTSDataSDCard ();
    void formatTSDataSDCard ();
    void requestSensorDetection ();
    void requestStartSensorTest ();
    void toggleGPSDynaMode ();

    qint64 getGridTime () const;            // qint64 is default for QtDateTime class
    void   setGridTime (const qint64 grid_time);

    ConnectionType connectionType() const;
    QString connectionUrl();

    adulib *adu() const;
    adujoblist *joblist() const;
    adujoblist *shadowlist() const;

    adunetwork *m_network = nullptr;



signals:
    void connectionTypeChanged(adurt::ConnectionType connectionType);
    void aduChanged();

private slots:
    void onInitialResourceFetched(adunetwork::ResourceType resourceType, QString filePath);
    void onResourceFetched(adunetwork::ResourceType resourceType, QString filePath);

private:
    void setConnectionType(ConnectionType connectionType);
    void initializeOnlineSystem();

    void submitJobs_step2();

private:
    ConnectionType m_connectionType = Disconnected;
    //adulib *m_joblist->job(0) = nullptr; //!< This represents the ADU that adurt is manipulating
    adujoblist *m_joblist = nullptr; //!< This is the joblist on the currently connected ADU
    adujoblist *m_shadowList = nullptr; //!< Shadow joblist that is not being edited (nextJobInfo)

    bool m_hasHwConfig = false; //!< Temporary cache during connection, will be reset after initialization of m_connectedAdu
    bool m_hasHwDb = false; //!< Temporary cache during connection, will be reset after initialization of m_connectedAdu
    bool m_hasChannelConfig = false; //!< Temporary cache during connection, will be reset after initialization of m_connectedAdu

    bool m_isSubmitting = false;
    bool m_isPaused = false;

    std::unique_ptr<tinyxml2::XMLDocument> joblistsdescriptors = nullptr;

    qint64 grid_time;              //!< prefer a 64s raster for RR

};

#endif // ADURT_H
