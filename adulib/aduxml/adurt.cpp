#include "adurt.h"


adurt::adurt(QObject *parent) : QObject(parent)
{
    m_network = new adunetwork(this);
    this->joblistsdescriptors = std::make_unique<tinyxml2::XMLDocument>();
    this->grid_time = grid_time_rr;
}

void adurt::connect_online(QUrl url)
{
    // initialize state management
    m_network->stop();
    disconnect(m_network, SIGNAL(resourceFetched(adunetwork::ResourceType,QString)),
               this, SLOT(onInitialResourceFetched(adunetwork::ResourceType,QString)));
    setConnectionType(Disconnected);
    m_hasHwConfig = false;
    m_hasHwDb = false;
    m_hasChannelConfig = false;

    // prepare data directory for information from webservice
    QDir saveDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString subdir = url.toString();
    subdir.replace(QRegularExpression("[:./ ]"), "_");
    saveDir.mkpath("metronix/" + subdir);
    saveDir.cd("metronix/" + subdir);

    // initiate requests for hwConfig and hwDatabase
    connect(m_network, SIGNAL(resourceFetched(adunetwork::ResourceType,QString)),
            this, SLOT(onInitialResourceFetched(adunetwork::ResourceType,QString)),
            Qt::UniqueConnection);
    m_network->setUrl(url.toString());
    m_network->setSaveDir(saveDir.absolutePath());
    m_network->fetchResource(QList<adunetwork::ResourceType>()
                             << adunetwork::HwConfig
                             << adunetwork::HwDatabase
                             << adunetwork::ChannelConfig);
    // At this point we wait for the results which will be handled in
    // onInitialResourceFetched().
}

void adurt::connect_filebased(QString path)
{
    disconnectAdu();

    QStringList fileNames{
        "HwConfig.xml",
        "HwDatabase.xml",
        "ChannelConfig.xml",
        "HwStatus.xml",
        "Selftest.xml"
    };

    QDir baseDir(path);
    for (QString fileName : fileNames) {
        if (!baseDir.exists(fileName)) {
            qDebug() << "Cannot connect file based. Missing file:" << fileName;
            return;
        }
    }

    QFileInfo hwConfigQfi(baseDir.absoluteFilePath("HwConfig.xml"));
    QFileInfo hwDatabaseQfi(baseDir.absoluteFilePath("HwDatabase.xml"));
    QFileInfo channelConfigQfi(baseDir.absoluteFilePath("ChannelConfig.xml"));
    QFileInfo hwStatusQfi(baseDir.absoluteFilePath("HwStatus.xml"));
    QFileInfo selftestQfi(baseDir.absoluteFilePath("Selftest.xml"));


    m_joblist = new adujoblist(this->grid_time, this);
    m_joblist->init_from_files(&hwConfigQfi, nullptr, nullptr,
                               &hwDatabaseQfi, nullptr, nullptr);
    m_joblist->job(0).fetch_atswriter_part(&channelConfigQfi, nullptr, nullptr);
    m_joblist->job(0).on_set_duration(60);
    m_joblist->on_set("sample_freq", 512, 0);
    m_joblist->set_start_time_now();

    m_shadowList = new adujoblist(this->grid_time, this);
    m_shadowList->init_from_files(&hwConfigQfi, nullptr, nullptr,
                                  &hwDatabaseQfi, nullptr, nullptr);
    m_shadowList->job(0).fetch_atswriter_part(&channelConfigQfi, nullptr, nullptr);

    //m_joblist->job(0) = &(*m_joblist)[0];
    setConnectionType(ConnectedOffline);

    m_joblist->job(0).fetch_hwstat(&hwStatusQfi, nullptr, nullptr);
    m_joblist->job(0).fetch_selftest(&selftestQfi, nullptr, nullptr);
}

void adurt::disconnectAdu()
{
    setConnectionType(Disconnected);
    m_network->stop();
    m_joblist->deleteLater();
    m_joblist = nullptr;
    m_shadowList->deleteLater();
    m_shadowList = nullptr;
}

QDateTime adurt::get_adu_time()
{
    if (m_joblist && m_connectionType == ConnectedOnline) {
        return m_joblist->job(0).get("GPSDateTime",-1).toDateTime();
    } else {
        return QDateTime();
    }
}

/*!
 * \brief Sends a single request to the remote system to clear all its jobs.
 */
void adurt::clearJobs()
{
    m_network->fetchResource(adunetwork::ClearJobs);
}

void adurt::clearFutureJobs()
{
    // TUT 15: We'll introduce a new resource type
    m_network->fetchResource(adunetwork::ClearFutureJobs);
}

void adurt::clearAllJobsOnAdu()
{
    // TUT 6: adunetwork is used for actually sending the request;
    //  simple requests without parameters work via fetchResource;
    //  for parametrized requests I would introduce separate functions like adunetwork::sendJob().
    m_network->fetchResource(adunetwork::ClearJobs);
}

/*!
 * \brief Sends a single request to the remote system for all its jobs.
 * @post: If everything goes well, the adujoblist will be updated once the new jobs arrive.
 */
void adurt::requestJobs()
{
    m_network->fetchResource(adunetwork::FutureJobs);
}

/*!
 * \brief Sends a stopJob request to the remote system via adunetwork.
 */
void adurt::stopCurrentJob()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::StopJob);
}


void adurt::startCopyTSToUSB ()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::CopyTSToUSB);
}


void adurt::ejectTSDataSDCard ()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::EjectSD);
}


void adurt::toggleGPSDynaMode ()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::ToggleGPSDynaMode);
}


void adurt::insertTSDataSDCard ()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::InsertSD);
}


void adurt::formatTSDataSDCard ()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::FormatSD);
}


void adurt::requestSensorDetection()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::RequestSensorDetection);
}


void adurt::requestStartSensorTest()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::RequestStartSensorTest);
}


/*!
 * \brief Sends the given xml string to the remote system via adunetwork.
 * \param jobxml
 */
void adurt::sendNewJob(QString jobxml)
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->sendJob(jobxml);
}

/*!
 * \brief Will request all future jobs from the remote system.
 */
void adurt::fetchFutureJobs()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_network->fetchResource(adunetwork::FutureJobs);
}

/*!
 * \brief Pauses the fetching of jobs from the remote system.
 *
 * Doesn't do anything if no fetching wasn't running or adurt isn't connected online.
 */
void adurt::pauseJobFetch()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }
    //    m_network->pause(adunetwork::JobFetch);
    m_isPaused = true;
}

/*!
 * \brief Resumes the fetching of jobs from the remote system.
 */
void adurt::resumeJobFetch()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }
    //    m_network->resume(adunetwork::JobFetch);
    m_isPaused = false;
}

/*!
 * \brief Replaces all future jobs on the remote system with the current jobs in the
 *          adujoblist.
 */
void adurt::submitJobs()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }

    m_isSubmitting = true; // continue with submitJobs_step2 once cleared
    //clearJobs();
    // TUT 13: We don't want to clear all jobs anymore on submitting but only future ones;
    //   let's add a new function for that, maybe some one wants to do this directly in the future;
    clearFutureJobs();
}

void adurt::submitChannelConfig()
{
    if (m_connectionType != ConnectedOnline) {
        return;
    }
    QString xmlString = m_joblist->job(0).on_build_channel_config();
    /*
#ifndef QT_NO_DEBUG
    QFile testFile("ChannelConfig_test.xml");
    if (testFile.exists() && testFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = testFile.readAll();
        xmlString = data;
    }
    QFile originFile("ChannelConfig_origin.xml");
    if (originFile.exists() && originFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = originFile.readAll();
        xmlString = data;
    }
#endif
*/
    //qDebug() << xmlString;
    if (!xmlString.isEmpty()) {
        m_network->sendChannelConfig(xmlString);
    }
}

/*!
 * \brief Returns the current state of connectivity.
 * \return ConnectionType
 */
adurt::ConnectionType adurt::connectionType() const
{
    return m_connectionType;
}

/*!
 * \brief Returns the URL to the currently connected ADU
 *
 * If not connected, returns QString::null
 */
QString adurt::connectionUrl()
{
    if (connectionType() == ConnectedOnline) {
        return m_network->urlString();
    } else {
        return QString();
    }
}

/*!
 * \brief Returns the adulib object that represents the hardware configuration
 *          for the current connection.
 * \return Pointer to adulib, might be nullptr. Do not delete, adurt will take care of it.
 */
adulib *adurt::adu() const
{
    if (m_joblist) {
        return &(m_joblist->job(0));
    } else {
        return nullptr;
    }
}

/*!
 * \brief Returns the adujoblist object that represents the joblist for the current
 *          connection.
 * \return Pointer to adujoblist, might be nullptr. Do not delete, adurt will take care of
 *          it.
 */
adujoblist *adurt::joblist() const
{
    return m_joblist;
}

adujoblist *adurt::shadowlist() const
{
    return m_shadowList;
}

/**
 * @brief Handles initial connection request.
 * Basically tracks when both HwConfig and HwDatabase are available and then
 * initializes the system in online mode.
 */
void adurt::onInitialResourceFetched(adunetwork::ResourceType resourceType, QString filePath)
{
    if (resourceType == adunetwork::HwConfig) {
        m_hasHwConfig = true;
    } else if (resourceType == adunetwork::HwDatabase) {
        m_hasHwDb = true;
    } else if (resourceType == adunetwork::ChannelConfig) {
        m_hasChannelConfig = true;
    }

    if (m_hasHwConfig && m_hasHwDb && m_hasChannelConfig) {
        initializeOnlineSystem();
    }
}

/*!
 * \brief Handles new resources supplied by adunetwork.
 * \param resourceType The kind of the new resource
 * \param filePath Further information on where adunetwork stored the new resource
 *
 * Consider filePath to be more like additional information. It needn't be a path to a
 * file. Eg.: For Jobs it's a directory.
 */
void adurt::onResourceFetched(adunetwork::ResourceType resourceType, QString filePath)
{
    if (!m_joblist) {
        return;
    }

    if (resourceType == adunetwork::HwStatus) {
        m_joblist->job(0).fetch_hwstat(nullptr, nullptr, m_network->hwStatusQba());
    } else if (resourceType == adunetwork::Selftest) {
        m_joblist->job(0).fetch_selftest(nullptr, nullptr, m_network->selftestQba());
    } else if (resourceType == adunetwork::Jobs) {
        m_shadowList->update_from_jobdir(filePath, true);
        bool listsMatch = true;
        if (m_shadowList->size() != m_joblist->size()) {
            listsMatch = false;
        }
        if (listsMatch) {
            for (int i = 0; i < m_shadowList->size(); i++) {
                if ( (m_shadowList->job(i) != m_joblist->job(i)) &&
                        m_shadowList->job(i).is_submitted_adujobtable() != m_joblist->job(i).is_submitted_adujobtable()) {
                    listsMatch = false;
                    break;
                }
            }
        }
        if (!m_isPaused && !listsMatch) {
            m_joblist->update_from_jobdir(filePath, true); // for jobs the filePath is actually a dirPath
        }
    } else if (resourceType == adunetwork::ClearFutureJobs && m_isSubmitting) { // TUT 22: Change from ClearJobs to ClearFutureJobs and we should be done;
        // TUT 12: Here is the handling of the ClearJobs success;
        //   it's only interesting if we are in the process of submitting;
        //   this also shows that we're not clearing future jobs yet as requested;
        //   let's do that now;
        submitJobs_step2();
    } else if (resourceType == adunetwork::ClearJobs) {
        for (int i = 0; i < m_joblist->size(); i++) {
            m_joblist->job(i).submitted_to_adu_jobtable(false);
        }
        m_joblist->jobs_updated();
    }
}

/*!
 * \brief Sets the current state of connectivity.
 * \param connectionType
 *
 * Emits connectionTypeChanged if the new value differs from the old value.
 */
void adurt::setConnectionType(adurt::ConnectionType connectionType)
{
    if (connectionType != m_connectionType) {
        m_connectionType = connectionType;
        emit connectionTypeChanged(m_connectionType);
    }
}

/*!
 * \brief Once adunetwork has all the resources for the configuration of the remote system
 *          this method can be called to enter the online mode of adurt.
 * @pre: connect_online has been called and hwConfig, hwDatabase, channelConfig are available.
 * @post: adurt is in online mode, cyclic updates are running
 */
void adurt::initializeOnlineSystem()
{
    m_joblist = new adujoblist(this->grid_time, this);
    m_joblist->init_from_files(nullptr, nullptr, m_network->hwConfigQba(),
                               nullptr, nullptr, m_network->hwDatabaseQba());

    m_joblist->job(0).fetch_atswriter_part(nullptr, nullptr, m_network->channelConfigQba());
    m_joblist->job(0).on_set_duration(60);
    m_joblist->on_set("sample_freq", 512, 0);
    m_joblist->set_start_time_now();

    m_shadowList = new adujoblist(this->grid_time, this);
    m_shadowList->init_from_files(nullptr, nullptr, m_network->hwConfigQba(),
                                  nullptr, nullptr, m_network->hwDatabaseQba());
    m_shadowList->job(0).fetch_atswriter_part(nullptr, nullptr, m_network->channelConfigQba());

    // subscribe to changes of job 0 to relay the change to any users
    connect(m_joblist, SIGNAL(job_zero_changed()), SIGNAL(aduChanged()));

    // start cyclic updates
    disconnect(m_network, SIGNAL(resourceFetched(adunetwork::ResourceType,QString)),
               this, SLOT(onInitialResourceFetched(adunetwork::ResourceType,QString)));
    connect(m_network, SIGNAL(resourceFetched(adunetwork::ResourceType,QString)),
            this, SLOT(onResourceFetched(adunetwork::ResourceType,QString)));
    setConnectionType(ConnectedOnline);
    m_network->run();
}

void adurt::submitJobs_step2()
{
    for (int i = 0; i < m_joblist->size(); i++) {
        adulib &localJob = m_joblist->job(i);
        localJob.submitted_to_adu_jobtable(true);
        QString xmlstring = localJob.on_build_xml();
        sendNewJob(xmlstring);
    }
    m_joblist->jobs_updated();
    m_isSubmitting = false;
}


QStringList adurt::fetchADUJoblistNames (void)
{
    QStringList qstrlRetValue;


    // get joblist names from network class and parse
    QString qstrXml (*(this->m_network->joblistNamesADUQba()));
    if (!qstrXml.size()) return qstrlRetValue;




    //    QDomDocument clXMLDoc;
    //    clXMLDoc.setContent(qstrXml);
    //    QDomNodeList clNodeList = clXMLDoc.elementsByTagName("joblistsdescriptor");
    //    for (int iNodeID = 0; iNodeID < clNodeList.size(); iNodeID++)
    //    {
    //        qstrlRetValue.append(clNodeList.at(iNodeID).toElement().attribute("name"));
    //    }
    //    clXMLDoc.clear();




    this->joblistsdescriptors->Parse(this->m_network->joblistNamesADUQba()->data());
    tinyxml2::XMLElement *tag = this->joblistsdescriptors->FirstChildElement("joblistsdescriptors");
    if (!tag) return qstrlRetValue;
    tag = tag->FirstChildElement("joblistsdescriptor");
    while(tag) {
        qstrlRetValue.append(QString(tag->Attribute("name")));
        tag = tag->NextSiblingElement();
    }
    this->joblistsdescriptors->Clear();
    return qstrlRetValue;
}


void adurt::startADUJoblist (const QString qstrJoblistName, const QString qstrStartDate, const QString qstrStartTime)
{
    QString qstrXml;

    qDebug () << "[ADURT] starting joblist: " << qstrJoblistName << qstrStartDate << qstrStartTime;

    qstrXml  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><joblist><name>";
    qstrXml += qstrJoblistName;
    qstrXml += "</name><startdate>";
    qstrXml += qstrStartDate;
    qstrXml += "</startdate><starttime>";
    qstrXml += qstrStartTime;
    qstrXml += "</starttime></joblist>";

    this->m_network->startJoblistADU (qstrXml);
}


qint64 adurt::getGridTime() const
{
    return this->grid_time;
}


void adurt::setGridTime(const qint64 grid_time)
{

    this->grid_time = grid_time;

   // not supported in MSVC qDebug() << "[" << __PRETTY_FUNCTION__ << "] new grid time:" << this->grid_time;

   m_joblist->setGridTime(grid_time);
   m_shadowList->setGridTime(grid_time);

    // @BFR: hier setzen der neuen Grid Time einbauen

}


