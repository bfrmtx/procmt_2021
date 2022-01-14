#ifndef ADUJOBLIST_H
#define ADUJOBLIST_H

#include <QObject>
#include <QTableView>
#include <QtGlobal>
#include <QAbstractTableModel>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>

#include "adulib.h"
#include "adutable.h"
#include "stoptime_dialog.h"

struct jp {
    int duration;
    double sample_freq;
    QString filter_type_main;
    int input;
    QString special_list;
};



class adujoblist : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit adujoblist(qint64 grid_time, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    ~adujoblist();

    /*!
     * \brief add_job
     * \param duration is min 1 second
     * \param freq -1 does not set a sample freq
     * \param qfi
     * \param qurl
     * \param qba
     */

    void add_job(const qint64 duration = 1, const double freq = 512, const QString filter_type_main = "", const int input = 0,
                 const QString special_list = "", const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);

    /*!
     * \brief init_from_files will also ALWAYS init a first job containg config and HW information
     * \param ADUHwConfig
     * \param HwDatabase
     */
    void init_from_files(const QFileInfo* qfiHwConfig = nullptr, const QUrl* qurlHwConfig = nullptr, const QByteArray *qbaHwConfig = nullptr,
                         const QFileInfo* qfiHwDatabase = nullptr, const QUrl* qurlHwDatabase = nullptr, const QByteArray *qbaHwDatabase = nullptr);

    void init_from_adulib(adulib adu);
    /*!
     * \brief update_from_jobdir
     * \param dirPath
     * \return number of job files
     */
    int update_from_jobdir(const QString &dirPath, const bool is_from_adu);

    bool set_save_absoloute_basedir_url(const QString &basedir, const QUrl &sysurl = QUrl() );


    /*!
     * \brief operator [] implements the index operator : gui.joblist->operator [](i), use \ref adujoblist::job(i) instead
     * \param i
     * \return
     */
    adulib& operator [] (int i);

    /*!
     * \brief job index operator
     * \param i
     * \return
     */
    adulib& job(const int &i);

    int size() const;

    /*!
     * \brief clear clears the job list, except the fist (otherwise we cant work)
     */
    void clear();

    /*!
     * \brief set_start_time sets the startime of the list
     * \param dt
     */
    void set_start_time(const QDateTime &dt, const uint jobno);

    /*!
     * \brief set_start_time_now ref ADULib; call this to update from time to time
     * \param secs_from_now
     * \param emits
     */
    void set_start_time_now(const int secs_from_now = 120, const bool emits = true);

    void  setGridTime (const qint64 grid_time);


signals:

    void sample_freq_changed(const double &freq);

    /*!
     * \brief selectable_frequencies_changed forwards a signal from e.g. first job initialisation to a GUI or other maintainer
     * \param selectable_frequencies
     */
    void selectable_frequencies_changed(const std::vector<double> &selectable_frequencies);
    void start_times_adjusted(adujoblist *ptr);
    void jobs_updated();
    void job_zero_created();
    void job_zero_changed();


public slots:

    void set_button_sample_freq_value(const double f);

    void adjust_start_times(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq);

    void on_clicked(const QModelIndex &index);

    /*!
     * \brief on_regrid_clicked re-arrange the start times to a minimum
     */
    void on_regrid_clicked();


    void on_sensor_names_changed(const QStringList &sensor_names);
    void on_selectable_frequencies_changed(const std::vector<double> &selectable_frequencies);

    void on_channel_map_changed(const QMap<QString, int> &channel_map );

    void on_button_sample_freq_value_accepted(const double f);

    void on_removeAt(const QModelIndex &index);

    /*!
     * \brief removeAt deletes a job - except there is ONLY one remaining; joblist MUST contain always 1 job;
     * if a new list is loaded old jobs will be deleted that way
     * \param index
     */
    void removeAt(int index);

    void slot_update_grid();

    void slot_utcoffset_for_midnight_usage(const qint64 &secs_from_utc);                    //!< for the poor people want to record locally midnight and not intl.




    /*!
     * \brief on_set
     * \param what
     * \param value
     * \param ichan
     * \param ijob
     * \param emits
     * \return
     */
    bool on_set(const QString &what, const QVariant &value, const int &ichan, const int &ijob = 0, const bool emits = true);

    QVariant on_get(const QString &what, const int &ichan, const int &ijob = 0);

    void on_duration_changed(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq);

    int on_save(const QFileInfo* qfiHwConfig = nullptr, const QUrl* qurlHwConfig = nullptr, const QByteArray *qbaHwConfig = nullptr);

    void on_swap_up(const int &list_index);

    void on_swap_down(const int &list_index);

    void on_set_TargetDirectory(const QString& TargetDirectory = "default");
    void on_set_TimeFormat (const QString& TimeFormat = "adjusted" );

    void on_set_StorageMode (const QString& StorageMode = "USB_TO_DEFAULT");
    void on_set_AdaptConfig (const QString& AdaptConfig = "FALSE");
    void on_set_AdaptSensorSpacing(const QString& AdaptSensorSpacing = "TRUE");
    void on_set_AdaptSensorType (const QString& AdaptSensorType = "E_SERIES");
    void on_set_AdaptChannelConfig (const QString& AdaptChannelConfig = "TRUE");
    void on_set_CleanJobTable (const QString& CleanJobTable = "TRUE");





private:

    QMutex mutex;

    QList<adulib> jobs;

    jp jobparms;

    std::vector<double> selectable_frequencies;

    QFileInfo qfi;
    QDir top_jobdir;

    bool want_set_first = true;

    QList<qint64> grid_space;                               //!< contains timing between jobs


    QMap<QString, int> channel_map;                         //!< attach a name like Ey to channel 1; HAS TO USE Ex2, Ey2 for EMAP

    std::unique_ptr<stoptime_dialog> stops_at = nullptr;

    QMap<int, QString> stops_at_cols;

    QString HWConfig_stream_string;
    QString HwDatabase_stream_string;

    QStringList TargetDirectories;                          //!< JLE component
    QString TargetDirectory = "";                           //!< JLE component

    QStringList TimeFormats;                                    //!< JLE component
    QString TimeFormat = "adjusted";                        //!< JLE component

    QString TimeOffset = "0";                               //!< JLE component

    QStringList StorageModes;                               //!< JLE component
    QString StorageMode = "USB_TO_DEFAULT";                 //!< JLE component

    QStringList AdaptConfigs;                               //!< JLE component
    QString AdaptConfig = "FALSE";                          //!< JLE component

    QStringList AdaptSensorSpacings;                        //!< JLE component
    QString AdaptSensorSpacing  = "TRUE";                   //!< JLE component

    QStringList AdaptSensorTypes;                           //!< JLE component
    QString AdaptSensorType = "E_SERIES";                   //!< JLE component

    QStringList AdaptChannelConfigs;                        //!< JLE component
    QString AdaptChannelConfig = "TRUE";                    //!< JLE component

    QStringList CleanJobTables;                             //!< JLE component
    QString CleanJobTable = "TRUE";                         //!< JLE component


     qint64 grid_time;              //!< prefer a 64s raster for RR

};

#endif // ADUJOBLIST_H
