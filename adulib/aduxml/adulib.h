#ifndef ADULIB_H
#define ADULIB_H

#include <iostream>
#include <QDebug>
#include <vector>
#include <algorithm>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QAbstractTableModel>
#include <QFile>
#include <QFileInfo>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QUrl>
#include <QByteArray>

#include <mutex>

#include <memory>
#include "tinyxml2.h"
#include "tinyxmlwriter.h"
#include "measdocxml.h"
#include "geocoordinates.h"
#include "eqdatetime.h"
#include "gps.h"
#include "txm.h"

#include "channel.h"




/*
 * Notizen
   alles muss von 0 ... 4 oder ...5 oder 9 laufen, wenn die ADU gleich bestückt ist
   d.h 0...9 sollte NICHT orkommen, wenn die ADU-07 mit HF UND LF bestückt ist:
   das setzt voraus, dass die RadioFilter AUTOMATISCH gesetzt werden müssen
   board 255 = not found
   sensor 203 = not found -> in case E for 0,1 or MFS-06 for 2,3,4




*/
/*! \mainpage

  The \ref adulib class is a container for a system with n boards.<br>
  First a ADU07HwConfig.xml file or database is scanned to detect the available boards.<br>
  The the HwDatabase.xml file or database is scanned to fill the detected boards with their capabilities sich as gains, smapling frequency and so on.<br>
  The adulib contains n channels of \ref channel.<br>
  \ref channel is again a container for a \ref aduslot : that is a hardware desciption of a board or a combination of LF & HF board.<br>
  In a ADU-08e system a slot == channel, in a ADU-07e system a slot (can be) == (LF channel + HF channel). The handling is transparent in the sense of:
  setting sample_freq to 512Hz will set a ADU-08e board to 512Hz. In a ADU-07e system the HF board will be switched of and the LF board will be witched on and set to 512Hz.<br>
  <br>
  <br>
  The adulib shall contain ALL XML data. The class will read all necessary information; adulib will not modify the XML tree - but create a XML file completely new out of scratch
  <br>
  The \ref adulib class maintains four logical set conditions:<br>
  <ul>

  <li> set a channel individually - for example the <b>dac_val, gain_stage1, gain_stage2</b> is for each channel different , and different for HF and LF </li>
  <li> set a HF and LF channel correspondingly - for example for LF <i>channel 0</i> and HF <i>channel 5</i> we use the same sensor, same sensor position ("pos_XX"),
       typical <b>filter_type, pos_x1, pos_x2, pos_y1, pos_y2, pos_z1, pos_z2, channel_type</b>. <br>This feature is NOT needed for the ADU-08e.</li>
  <li> set all channels - tries to set all channels with the same value; the board will decide what to do: setting a filter to
       "ADU07_LF_RF_4" will set all board. Having HF and ADU-07e this will
        do nothimg for the HF channels</li>
  <li> special settings - for example the sampling frequency <b>sample_freq</b> - which belongs to "set all channels" BUT: at the end -
       if sampling frequency could be set - we have to use a fallback strategy and set the lowest
       sampling frequency from slot/channel 0.</li>
   <li> in the source code \ref adulib::values_needs_individual_channel_setting (DAC, gains) and \ref adulib::values_needs_mapping_channel_setting (pos_x1, sensor_type) need to changed! when adding values to QMap<br>
       Values of QMap NOT belonging to this (sample_freq) will be set to ALL chanels! In case of frequency the boards will switch on/off individually</li>

  </ul>

  <ul>
    <li> the name of channel (channel_type) is in adulib::set() value "Ey", 1 for Ey </li>
    <li> the name of the sensor (sensor_type) is in adulib::set() value "MFS-06e", 2 for Hx </li>
    <li> find out if we treat a channel as electric (needs dipole length) of magnetic, us get("is_electric", 1) or get("is_magnetic", 1) ; returns QVariant(bool)

  </ul>


*/


/**
 * This enum defines the system types in the global system information.
 */
enum t_SystemTypes
{
    ADU07       =   0,     /// ADU-07 system
    ADU08_5CH   =   1,     /// ADU-08e 5-CH system
    ADU08_2CH   =   2,     /// ADU-08e 2-CH system
    TXB08       =   3,     /// TXB-08 system
    ADU09_10    =   4,     /// ADU-09u / ADU-10e system
    ADU_UNKNOWN = 0xFF     /// unknown or invalid ADB board type
};

/**
 * This enum defines the system family versions
 */
enum t_SystemFamily
{
    GMS07 =   7,     /// ADU-07 system
    GMS08 =   8,     /// ADU-08 system
    GMS09 =   9,     /// ADU-09 system
    GMS10 =  10      /// ADU-10 system
};


// set sample_freq this activates max 5 or 6 channels - and that does the trick
// if you have HF and LF only HF or LF will be active after this

/*!
   \brief The adulib class maintains 0, 1, 2, 3, 4 (5) channels or (0,5) (1,6) ... (4,9) channels<br>
   The adulib contains a \ref adulib::start_time and a \ref adulib::duration - that is together with \ref aduslot::data [sample_freq] and others
   the essential description of a job.<br>
   A joblist contains of a list of adulibs.
 */
class adulib : public QAbstractTableModel  {

    Q_OBJECT
public:

    adulib(qint64 grid_time, QObject *parent = nullptr);

    ~adulib();

    adulib(const adulib& newjob);

    adulib& operator = (const adulib &rhs);


    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);


    /*!
       \brief operator < compares start time and makes jobs sortable
       \param rhs
       \return class with earlier start time
     */
    bool operator < (const adulib &rhs);

    /*!
       \brief operator - compares stop_time and start_time of rhs and makes jobs diffs
       \param rhs
       \return diff in second
     */
    quint64 operator - (const adulib &rhs);


    /*!
       \brief operator == compare start times
       \param rhs
       \return class with equal start time
     */
    bool operator == (const adulib& rhs);

    /*!
     * \brief size returns the size we want to iterate over;
     * \return 0...5 (ADU-08) and 0...4 (ADU-07 with LF/HF) and 0 .. 9 (ADU-07 with LF OR HF only)
     */
    size_t size() const;

    /*!
     * \brief total_slots returns the number of used slots; DO NOT use in general, use adulib::size()
     * \return slots used
     */
    size_t total_slots() const;


    /*!
     * \brief get returns the value for the "what", where what is the xml node from a job file
     *  sensor_sernum is used for the sensor - but originates from HWConfig as Serial
     * \param what xml node name like pos_x1
     * \param ichan channel (0...9) , in case of pos_x1 ichan 0 and 5 can be the same (mapped); -1 is first active channel
     * \return QVariant
     */

    QVariant get(const QString &what, const int &ichan) const;

    /*!
     * \brief set sets data through all channel: sensor -> cable -> adbboard -> aduslot <br>
     *  emits in case \ref adulib::QTableViewChanged to update all jobst in case this signal is connected
     *  if (!set()) you should check if aduslot::has_no_set_error() returns 2 (auto corrected)
     *  sensor_sernum is NOT written back in HWConfig if set manually
     * \param what  string value like pos_x1; via mapping in \ref sensor::map_names North, South, East, West are supported as alias for pos_XXX
     * \param value  QVariant like -50 (for pos_x1 a channel number ichan is required)
     * \param ichan  channel number - only for a certain channel, like (sensor_type, MFS-06, 2) or (pos_x1, -50, 0);
     * \return false in case of what == not existing or value == not allowed or value == auto-corrected : ref \ref aduslot::has_no_set_error()
     *
     * mbk: Notes for ui, should also pertain to get()
     * north => "pos_x2"
     * south => "pos_x1"
     * east  => "pos_y2"
     * west  => "pos_y1"
     * ????  => "channel_type" - Names of buttons in CFG (Ex, Ey, Hx, Hy, Hz, Ey1 ...), we don't set this from ui
     * ????  => "sensor_type" - Shown below H-Channels in CFG, selectable via radio buttons for each channel
     * ????  => "sample_freq" - not relevant until job list management, we need a getter though
     * do NOT use for duration (aka recording time)
     */
    bool set(const QString &what, const QVariant &value, const int &ichan, const bool emits = true);

    /*!
     * \brief get_dipole_length
     * \param channel no (for example channel_map.value("Ex")
     * \return  dipole length
     */
    double get_dipole_length(const int &ichan);

    /*!
     * \brief get_channel_map
     * \return the actual channel map; hence that the signal adulib::channel_map_changed() can be emitted
     */
    QMap<QString, int> get_channel_map() const;


    QString get_sample_file_string() const;



    bool fetch_hwstat(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);





    QMap<QString, QVariant> fetch_selftest(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);

    QMap<QString, QVariant> fetch_global_config(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);


    /*!
     * \brief fetch_recording fetches the recording sections form a jobfile or measdocxml
     * \param filename
     * \param sysurl
     * \return
     */
    QMap<QString, QVariant> fetch_recording(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);

    int fetch_atswriter_part(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);


    void fetch_autogains_part(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);




    /*!
     * \brief step three : build_channels read the channels from HWConfig and set the GMS/RevMain/RevSub/Type for each board and sensor; this we need to find them in the HWDatabase
     */
    void build_channels();

    /*!
     * \brief step four: build_hardware
     */
    void build_hardware();

    /*!
     * \brief info get base infor on rev main / sub and names
     * \param classname like sensor
     * \return
     */
    QStringList info(const QString classname) const;

    /*!
     * \brief info_two about sensor pos and f
     * \return
     */
    QStringList info_two();

    bool is_active(const int channel_no) const;

    /*!
     * \brief get_avail_channel_types
     * \return list of implemented channel types such as Ex, Ex2, Ey ... can be used for drop down; use the signal \ref adulib::channel_map_changed() to update your buttons; the signal will be emitted during creation of adulib
     */
    QStringList get_avail_channel_types() const;

    /*!
     * \brief set_start_time explicitly sets the start time and emit the change; FOR A JOBLIST you may connect this with adulib::set_min_start_time()
     * \param start_time start time of this job in UTC (not local time), see also \ref adulib::on_set_duration()
     */
    void set_start_time(const QDateTime &start_time, const bool emits = true);


    /*!
     * \brief set_min_start_time set the minium time delay to a previous job; does NOT throw a signal; use when append a job or when a list item has changed start time or duration
     * \param previous_stop_time
     * \param previous_min_delta_start
     * \return
     */
    QDateTime set_min_start_time(const int &index, const QDateTime &previous_stop_time, const qint64 &previous_min_delta_start, const qint64 &previous_diff_secs);

    QDateTime get_start_time() const;

    QDateTime get_stop_time() const;

    qint64 seconds_to_midnight(const qint64 offset_seconds) const;



    /*!
     * \brief set_delta_start time between jobs
     * \param delta_start - in case 0, the job starts at next full minute; if 64 we add 128s
     * \return actual (in case corrected) delta_start
     */
    qint64 set_delta_start(const qint64 &delta_start);

    qint64 get_delta_start() const;

    /*!
     * \brief get_min_delta_start; this time is calculated by adulib
     * \return mimum time between two jobs (set by adulib)
     */
    qint64 get_min_delta_start() const;

    /*!
     * \brief set_min_delta_start according to the sampling freuency; emits a signal min_delta_start_changed that later jobs may e shifted
     */
    void set_min_delta_start(const bool emits = true );

    /*!
     * \brief update_channel_map updates adulib::channel_map ;
     * \param emits ste false in case working on a LIST
     */
    void update_channel_map(bool emits = true);

    /*!
     * \brief want_change_for_all_jobs
     * \param what like pos_x1 or site_name which will be the same for all jobs; this function will find out for you
     * \return
     */
    bool want_change_for_all_jobs(const QString &what) const;

    /*!
     * \brief get_index returns index in case I am member of a list or vector
     * \return index
     */
    int get_index() const;

    /*!
     * \brief set_index index of a list; when some signals will be emitted the index will be emitted as well in order to see who was the sender
     * \param index
     */
    void set_index(const int index);

    /*!
     * \brief get_sample_freq - convinience
     * \return sample frequency
     */
    double get_sample_freq() const;
    QVector<double> get_selectable_frequencies() const;

    int get_active_channels() const;

    int get_first_active_channel() const;


    /*!
     * \brief poll_system_status
     * \return
     */
    QMap<QString, QVariant> poll_system_status();


    /*!
     * \brief detect_sensors submits a special job
     * \return
     */
    bool detect_sensors(bool emits = true);


    /*!
     * \brief fetch_hwcfg step one: create empty max channels, typeless  - nothing else
     * \param qfi
     * \param qurl
     * \param qba
     * \return
     */
    bool fetch_hwcfg(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);

    bool fetch_hwcfg_virtual(const size_t n_channels, const double &sample_freq, const int &Type = 1, const QString &Version = "2.0", const QString &Name = "ADU-08e");
    /*!
     * \brief fetch_hwdb step two: open the database // connect
     * \param qfi
     * \param qurl
     * \param qba
     * \return
     */
    bool fetch_hwdb(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);

    bool is_submitted_adujobtable() const;

    void submitted_to_adu_jobtable(const bool submitted_true_false);


    unsigned int get_gms  (void);
    unsigned int get_type (void);

    void set_special_list(const QString special_list_name);

    QString get_special_list() const;

    qint64 utcoffset_for_midnight_usage = 0;    //!< for the poor people want to record locally midnight and not intl.

    void setGridTime(const qint64 grid_time);


public slots:


    bool on_write_job(const bool set_astwriter_finhished = false);
    QString on_build_xml(const bool set_astwriter_finhished = false);

    QString on_build_channel_config();

    void on_set_filename(const QString &filename);

    /*!
     * \brief set_start_time_now gets the local time and changes the time whe slot is called
     * \param secs_from_now add some seconds (120 default); start time is rounded up to full minute
     * \param emits
     */
    void set_start_time_now(const int secs_from_now = 120, const bool emits = true);



    /*!
     * \brief on_set_duration set the runtime of this job; do no use the set(what, value) here
     * \param duration
     */
    void on_set_duration(const qint64& duration, const bool emits = true);

    void on_set_long_duration(const int &days, const int &hh, const int &mm, const int &ss, const bool emits = true);

    /*!
     * \brief get_duration
     * \return job recording time in seconds
     */
    qint64 get_duration() const;


    /*!
     * \brief get_H_sensors als sensors for magnetic field (blue)
     * \return
     */
    QStringList get_H_sensors(QStringList *alias_names = nullptr, QList<int> *input_divider = nullptr) const;

    /*!
     * \brief get_E_sensors all sensors for electric field (yellow)
     * \return
     */
    QStringList get_E_sensors(QStringList *alias_names = nullptr, QList<int> *input_divider = nullptr) const;

    int get_selftest_table(QList<QString> &what, QList<QString> &limits_strs, QList<QVariant> &le, QList<QVariant> &ge) const;


    int slot_adbboard_data_qmap_created(const QMap<QString, QVariant> &data_map);


    void slot_atswriter_comments_qmap_created(const QMap<QString, QVariant> &data_map);
    void slot_atswriter_configuration_qmap_created(const QMap<QString, QVariant> &data_map);
    void slot_atswriter_output_file_qmap_created(const QMap<QString, QVariant> &data_map);

    /*!
     * \brief slot_gps_status_qmap_created parses the GPS section from XML and converts to readable strings; also sets the button color in case
     * \param data_map
     */
    void slot_gps_status_qmap_created(const QMap<QString, QVariant> &data_map);

    void slot_recording_status_qmap_created(const QMap<QString, QVariant> &data_map);

    void slot_system_status_qmap_created(const QMap<QString, QVariant> &data_map);

    void slot_on_keep_grid(const bool &is_checked);





signals:

    /*!
     * \brief timing_changed will be always emitted if sample_freq, duration or start_time has changed;
     */
    void timing_changed();

    /*!
     * \brief timing_change_attempt will be always emitted BEFORE! sample_freq, duration or start_time try to change;
     * this may be needed to catch the old status BEFoRE change
     */
    void timing_change_attempt();

    void sample_freq_changed(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq);
    /*!
     * \brief selectable_frequencies_changed will be thrown by build hardware; this signal should occure only once
     * \param selectable_frequencies
     */
    void selectable_frequencies_changed(const std::vector<double> &selectable_frequencies);
    void channel_map_changed(const QMap<QString, int> &channel_map);
    void sensor_names_changed(const QStringList &sensor_names);
    void start_time_changed(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq);

    /*!
     * \brief min_delta_start_changed after changing sample_freq we may have to change the min time bewtween jobs, this can cause a re-assembly of all following start times of other jobs
     * \param index my list index
     * \param start_time start time of this lib
     * \param duration duration of this lib
     * \param sample_freq sample frequency of this lib
     */
    void min_delta_start_changed(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq);
    void duration_changed(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq);
    /*!
     * \brief QTableViewChanged emits all paramter - to be used in case of job list: chnage all instead of one in case adulib::want_change_for_all_jobs returns true
     * \param what
     * \param value
     * \param ichan
     * \param jobid
     * \param emits
     */
    void QTableViewChanged(const QString &what, const QVariant &value,  const int &ichan, const int &jobid, const bool emits);

    void sensors_detected(const bool finished);

    void state_changed();



private:



    /*!
       \brief map_xml_channels in case we have ADU-07 (->GMS = 7)
     */
    void map_xml_channels();



    void channel_calibration();
    void sensor_calibration();

    void delete_tinyxml2();


    /////////////// DATA BEGIN
    ///
    ///

    int index;
    QFileInfo qfi;
    int GMS;                                                //!< GMS version 7 or 8
    int Type = 0;                       //!< database ID
    QString Name;                       //!< database ID - readable name for System
    QString Version;                    //!< Version of the metronix xml file

    QMap<QString, QVariant> atswriter_comments;
    QMap<QString, QVariant> atswriter_output_file;
    QMap<QString, QVariant> atswriter_configuration;

    bool bool_is_submitted_adujobtable = false;             //! is job is on the adu jobtable; adu should say next job: or wait start time


    QMap<QString, QVariant> system_status;                  //!< contains data from HW status and Selftest
    QMap<QString, QVariant> gps_status;                     //!< contains data from GPS status
    QMap<QString, QVariant> recording_status;               //!< contains data from Recording status
    QMap<QString, QVariant> selftest_result;                //!< contains data from Selftest results


    //!< @todo connect changes with signals
    QMap<QString, QVariant> tmp_recording;                  //! contains data from recording section, used for parsing only -> will be analysed and become class data
    QMap<QString, QVariant> tmp_global_config;              //! contains data from global_config section, used for parsing only -> will be copied into adbboard class

    QStringList sensor_names;
    QMap<QString, int> channel_map;                         //!< attach a name like Ey to channel 1; HAS TO USE Ex2, Ey2 for EMAP

    std::vector<channel> channels;                          //!< internal administration of channels

    QStringList items_want_change_for_all_jobs;             //!< contains values for all items to be changed in job lists; will be used in joblists for example: change one -> change all \ref adujoblist::on_set

    QDateTime start_time;                                   //!< start time where this object comes to live; the adulib will act as a job!
    qint64 duration;                                        //!< duration of job in seconds, live time of this object
    qint64 delta_start;                                     //!< delta start is the min time between to jobs - 30s for LF jobs
    qint64 min_delta_start;                                 //!< min delta start is the min time between to jobs which can not be underrun - 30s for up to 64kHz jobs, 3min above 64kHz
    QVector<double> selectable_frequencies;

    QString resistivity_mode = "normal";                    //!< low, normal, high

    QString special_list =          "";                     //!< up to know: "JLL", "CEA", "CEA-daily" only

    QStringList values_needs_individual_channel_setting;    //!< must be set individually! like dac_val or gains
    QStringList values_needs_mapping_channel_setting;       //!< pos_x1 shall be set for 0 and 5 at the same time (only needed for ADU-07


    ///////////////// DATA END

    std::shared_ptr<measdocxml> hwdb = nullptr;
    std::shared_ptr<measdocxml> hwcfg = nullptr;
    std::shared_ptr<measdocxml> hwstat = nullptr;
    std::unique_ptr<measdocxml> measdoc_template = nullptr;
    std::shared_ptr<measdocxml> channel_cfg = nullptr;

    std::mutex lock;

    bool bfetch_complete_adu_status = false;                //!< chceck if all system status data was successfully read



    tinyxmlwriter tix;                                      //!< writes the job file
    tinyxmlwriter xcal;                                     //!< append the empty calibration section

    QSqlDatabase infodb;
    geocoordinates coordinates;

    QDateTime  local_time;
    eQDateTime utc_time_base;



    bool keep_grid = true;                                  //!< when true and job time decreases the following jobs start earlier (default)

    GPS gps;
    TXM txm;

    qint64 grid_time;                                       //!< prefer a 64s raster for RR

};


#endif // ADULIB_H
