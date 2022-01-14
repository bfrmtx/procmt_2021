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

#ifndef ATSHEADER_H
#define ATSHEADER_H

#include "prc_com.h"
#include "atsheader_80_def.h"

#include "procmt_alldefines.h"
#include <climits>

#include <vector>

#include <QObject>
#include <QDataStream>
#include <QFileInfo>
#include <QtEndian>
#include <QDir>
#include <QDebug>
#include <QString>

#include <memory>
#include "calibration.h"
#include "msg_logger.h"

#include <eqdatetime.h>


/*!
   \brief The atsheader_80 class reads and writes the binary header(s)
   Internally the hashmap prc_com is used for all I/O (so the prc_com may contain values
   which are not flushed into the header data).<br>
   OLD The maximum storage is uint32_t samples that is <b>max 16 GiB per channel or 17GB per channel</b><br>
   New (testing) The maximum storage is now uint64_t samples - <b> more than the max file size on your system </b><br>
   For programmers: check that all timings/seconds are in qint64 and all samples are in quint64

 */
class atsheader :  public QObject, public QFileInfo, public prc_com
{
    Q_OBJECT


public:

    explicit atsheader(const QFileInfo &qfi, const short int &header_version = 0, QObject *parent = Q_NULLPTR);
    explicit atsheader(const QString &dir, const QString &name, const short int &header_version = 0, QObject *parent = Q_NULLPTR);
    explicit atsheader(const atsheader &atsh);

    explicit atsheader(const QFileInfo &qfi, const QByteArray &qba, const bool &scope_mode = true, QObject *parent = Q_NULLPTR);

    /*!
     * \brief atsheader create a fake in order to contruct QMAP
     * \param parent
     */
    explicit atsheader(QObject *parent = Q_NULLPTR);




    // this may cause a cdup with newly generated measdirs
    /*!
     * \brief set_basedir the algorithm wil "cd up" and land in Site_99 in order to be ready to create a new measdir or same measdir
     * \param basedir like Site_99/meas_2017-04-05_10-33-20;
     */
    void set_basedir(QDir const &basedir);

    /*!
     * \brief set_basedir_fake_deep same as above; BUT having a empty site you dont have a measdir yet; appends a fake maeasdir so that we end up under Site_99 again and not in ts dir
     *
     * \param basedir
     */
    void set_basedir_fake_deep(QDir const &basedir);

    bool dip_to_pos(const double length, const double decangle = 0.0);


    /*!
     * \brief atsheader  creates an EMPTY header; for filename creation<br>
     *  008_V01_C00_R011_TEx_BL_32H.ats <br>
     *
     *
     * \param file_version  1 -> V01
     * \param HeaderVers 80, 81, 1080(sliced) not visible in file name
     * \param system_serial_number 8 -> 008
     * \param sample_freq 32 -> 32H, 0.25 -> 4s
     * \param run_number 11 -> R011
     * \param channel_number 0 -> C01
     * \param channel_type Ex -> TEx
     * \param ADB_board_type LF, HF, MF, BB -> BL, BH, BM, BB; if empty will be set BL <= 4096Hz, BH else
     * \param observatory 21007 -> 21007 observatory number - only visible for CEA file format
     * \param ceadate YYYYMMDD -> only visible for CEA file format
     * \param ceameastype ns, ca, cp -> ns, ca, cp : natural source, controlled ante meridiem, controlled post meridiem
     */
    explicit atsheader(const int &file_version, const int header_version, const int &system_serial_number, const double &sample_freq,
                       const int &run_number, const int &channel_number,
                       const QString &channel_type, const QString &ADB_board_type = "", const QString &observatory = "",
                       const QString &ceadate = "", const QString &ceameastype = "");

    ~atsheader();

    /*!
       \brief parse_filename_header_version - checks in brief the consistancy of filename and header
       \return
     */
    short int parse_filename_header_version();

    /*!
       \brief clear resets the header to empty
       \param header_version
     */
    void clear(const short &header_version = 0);

    /*!
       \brief get_header_size size of header; you should need it because the class knows it
       \return size of headers in bytes;
     */
    short int get_header_size() const;

    /*!
       \brief get_header_version returns the header version - which also indicates if this is a scliced file
       \return
     */
    short int get_header_version() const;

    /*!
       \brief readHeader
       \return number of total samples in ats file
     */
    quint64 read_header(const bool &silent = false, const bool keep_open_for_append = false);

    quint64 write_header_qba(const QByteArray &qba, const bool &scope_mode, const bool &silent = false, const bool keep_open_for_append = false);


    /*!
     * \brief write_header; some changes on the fly (like changing the sampling freq by a filter (is in in file) will be may here at out file
     * \param rewrite_close re-write the header - eg. after finishing the data write we update samples and close the file
     * \return
     */
    size_t write_header(const bool rewrite_close);

    /*!
     * \brief scan_header_close reads (opens) and closes immedeately
     * \param silent
     * \return number of samples in the files
     */
    quint64 scan_header_close(const bool &silent = true);

    /*!
       \brief console_site_name console output of the UTF-8 site name (you can't use the QString here).
       \return
     */
    std::string console_site_name() const;

    std::string console_site_name_rr() const;

    std::string console_site_name_emap() const;


    /*!
     * \brief get_start_datetime_ui
     * \return  start time in UNSIGNED Unix timestamp format; secs since 1970; check your system if UNSIGNED is supported, otherwise use MSecsSinceEpoch() for Qt
     */
    quint32 get_start_datetime_ui() const;


    /*!
     * \brief get_stop_datetime_ui
     * \return  stop time in UNSIGNED Unix timestamp format; secs since 1970; check your system if UNSIGNED is supported, otherwise use MSecsSinceEpoch() for Qt
     */
    quint32 get_stop_datetime_ui() const;

    /*!
     * \brief get_start_datetime get a save start time
     * \return
     */
    eQDateTime get_start_datetime() const;

    /*!
     * \brief get_stop_datetime get a save stop time form f, samples : calculated
     * \return
     */
    eQDateTime get_stop_datetime() const;

    /*!
     * \brief set_start_date_time_ui you must use UTC time
     * \param secs_since_1970 in UTC
     */
    void set_start_date_time_ui(const quint32 &secs_since_1970);


    /*!
     * \brief set_start_date_time
     * \param qdt make sure that your QDateTime is already in UTC!
     */
    void set_start_date_time(const QDateTime &qdt);


    /*!
       \brief get_sample_freq
       \return sample frequency
     */
    double get_sample_freq() const;


    /*!
     * \brief set_sample_freq sets the sampling frequncy and prepares a possible new filename
     * \param sample_freq
     */
    QString set_sample_freq(const double &sample_freq, const bool set_Hchopper_auto, const bool set_Echopper_auto);

    /*!
     * \brief set_LSBMV lsb_mv * int32 sample = mV in double
     * \param lsb_mv
     */
    void set_LSBMV(const double &lsb_mv);

    /*!
     * \brief set_LSBMV_BB_LF_default - sets the default, needed when creating files from double data input
     */
    void set_LSBMV_BB_LF_default();

    /*!
     * \brief set_LSBMV_BB_HF_default - sets the default, needed when creating files from double data input
     */
    void set_LSBMV_BB_HF_default();


    void set_e_pos(const double &x1, const double &x2, const double &y1, const double &y2, const double &z1, const double &z2);

    void sample_freq_to_file_str(const double &sample_freq, int &fname_sample_period, QString &fname_sample_period_unit);


    /*!
       \brief size
       \return returns amount samples; convinience
     */
    quint64 size() const;

    /*!
       \brief channel_type should return Ex, Ex, Hx, Hy, Ez; in case of remote prepend you must use chan_type for mapping hx(2) to rhx(7)
       \return
     */
    QString channel_type() const;



    QFileInfo get_qfile_info() const;


    /*!
       \brief prc_com_to_classmembers copies class variables to hash
     */
    void prc_com_to_classmembers();

    /*!
       \brief classmembers_to_prc_com copies all / part of the hash to the class
     */
    void classmembers_to_prc_com();

    /*!
       \brief get_num_samples
       \return number of samples, in case of sliced header: all samples
     */
    quint64 get_num_samples() const;

    /*!
     * \brief set_num_samples DO NOT DO IT the samples should be there already; BUT when we make a header copy it can be useful before starting a thread
     * \param new_samples
     */
    void set_num_samples(const quint64 new_samples);

    /*!
       \brief get_Filter returns filter type
       \return
     */
    QStringList get_filter() const;

    void set_filter(const QStringList &filterstrings);

    /*!
       \brief activate_scalable - if type is E channel and dipole != 0 then set true and atsfile can return mV/km
       \return
     */
    bool activate_scalable();


    atsheader& operator = (atsheader const &atsh);

    /*!
       \brief get_dipole_length
       \return dipole length from coordinates
     */
    double get_dipole_length() const;

    /*!
       \brief scale_dipole_length scales all 3 coordinates x, y, z
       \param factor
     */
    void scale_dipole_length(const double factor);

    /*!
       \brief angle in RADIANS
       \return the angle from North -> East of the E-Field or sensor coodinates; z component is NOT used
     */
    double angle() const;

    /*!
       \brief set_start_seconds_offset - positive offset in seconds to RR site; a slide to "0" will be NOT after header BUT synchronioulsy to the RR site
       \param seconds_offset_to_rr
     */
    void set_start_seconds_offset(const qint64 &start_seconds_offset);

    /*!
       \brief get_start_seconds_offset
       \return offset seconds to RR site
     */
    quint32 get_start_seconds_offset() const;

    /*!
       \brief set_samples_offset set the filepos offset to n samples; IF YOU USE this function be aware that you have a PROBLEM
       \param samples_offset
     */
    void set_samples_offset(const quint64 &samples_offset);

    QDataStream::Status get_QDataStream_status() const;

    /*!
     * \brief set_run
     * \param negative_increments_positive_sets run is 12; -1 -> 13, -4 ->16, 22 -> 22
     */
    QString set_run(const int &negative_increments_positive_sets);


    /*!
     * \brief get_new_filename get a synthetic filename
     * \param change_measdir - force output to different measdir if starttime changes
     * \param type 06, 07, 08, cea
     * \return filename
     */
    QString get_new_filename(const bool bchange_measdir = true, const QString &type = "07");

    /*!
     * \brief get_atswriter_section returns data for XML
     * \return
     */
    QMap<QString,QVariant> get_atswriter_section() const;

    QMap<QString,QVariant> get_gps_section() const;

    void set_suffix(const QString &suffix);

    void set_change_measdir(const bool true_or_false);

    void set_extra_signals(const bool &true_false);

    void notify_xml();

    QString get_measdoc_name() const;

    QString get_xmlcal() const;

    /*!
     * \brief get_syscal_string the calibration can read a specially formatted string like TRF_ADU-08e_ADB08e-LF_DIV-1_Gain1-4_DAC-off_Gain2-1_LF-RF-2_LF-LP-off_HF-HP-off_SensorResistance-250.txt
     * \return
     */
    QString get_syscal_string() const;

    /*!
     * \brief set_ats_options sets all ats options (which have been pre-defined; no prepend class name so fupper not ats_fupper
     * \param cmdline
     */
    void set_ats_options(const std::shared_ptr<prc_com> cmdline);

    /*!
     * \brief set_ats_options sets all ats options (which have been pre-defined; no prepend class name so fupper not ats_fupper
     * \param cmdline
     */
    void set_ats_options(const prc_com &cmdline);

    /*!
     * \brief set_ats_option set a single option on demand; key maust already exist; no prepend class
     * \param key
     * \param value
     */
    void set_ats_option(const QString &key, const QVariant &value);

    QVariant get_ats_option(const QString &key) const;

    void init_calibration(const int auto0_txt1_sql2_xml3 = 0, const bool enable_adb_cal = false);

    /*!
     * \brief force_calibration with database, sensor and ser_num for crappy files
     * \param force_calib
     */
    void force_calibration(const QString which_caldb_absolute_filepath, const QString which_sensor_type, const int which_sensor_sernum);


    void cp_essentials_to(std::shared_ptr<atsheader> ats, const int i_6_7_8 = 6) const;

    /*!
     * \brief enum_chan_type corresponds to the index of pmt::channel_types; here we can map hx (2) to rhx (7) in order to store / find this data in vectors
     */
    size_t chan_type = SIZE_MAX;

    /*!
     * \brief subname - part of new filename
     * \return
     */
    QString subname_file() const;
    QString subname_xml() const;

    std::shared_ptr<calibration> get_cal() const;

    friend bool compsamples_ats_lhs_lt(const atsheader& lhs, const atsheader& rhs);
    friend bool compsamples_ats_lhs_lt_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

    friend bool compstartime_ats_lhs_lt(const atsheader& lhs, const atsheader& rhs);
    friend bool compstartime_ats_lhs_lt_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

    friend bool compstoptime_ats_lhs_lt(const atsheader& lhs, const atsheader& rhs);
    friend bool compstoptime_ats_lhs_lt_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

    friend int64_t delta_stop_start(const atsheader& lhs, const atsheader& rhs);
    friend int64_t delta_stop_start_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

    friend bool same_recording(const atsheader& lhs, const atsheader& rhs);
    friend bool same_recording(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

public slots:

    /*!
     * \brief close closes the file and emits XML and closed
     * \param rewrite
     */
    void close(const bool rewrite = false);

    void set_atswriter_section(const QMap<QString, QVariant> &atswriter_section);

    void set_survey_basedir(const QDir base);

    void set_messenger(std::shared_ptr<msg_logger> msg);


    void slot_apply_sys_calib(const uint on_1_off_0);


signals:

    void atsfile_closed(int why);
    void QDataStreamStatus(QDataStream::Status);    //!< should be 0, 1 is past the end, 2 corrupted, 3 write failed; see Qt doc
    void filename_changed(const QMap<QString,QVariant> &atswriter_section, const int &channel_id, const QString &measdir_wanted, const QString &measdoc_wanted);
    void signal_sync_info(const eQDateTime &timings);

    void tx_adu_msg(const int &system_serial_number,  const int &channel, const int &slot, const QString &message);




protected:

    /*!
       \brief clean_b_string cleans binary strings without NULL terminator, removes NULL terminator for QString because QString itself IS NULL terminated
       \param ins   char[nn]
       \param s_size size_of(char[nn])
       \return cleaned QString
     */



    void prepare_options();

    bool is_64bit_samples() const;


    QString clean_b_string(const char *ins, const int s_size, const bool remove_terminator = true) const;
    QString clean_b_stringUTF8(const char *ins, const int s_size, const bool remove_terminator = true) const;


    ATSHeader_80                     bin_atsheader;                   //!< binaray header for read/write
    ATSSliceHeader_1080              tslice;                          //!< slice for 1080 header
    std::vector<ATSSliceHeader_1080> tslices;                         //!< all slices for 1080 header
    size_t                           used_slices;                     //!< num slices


    QMap<QString, qint8>             LFFilters;
    QMap<QString, qint8>             HFFilters;

    QDataStream qds;
    QFile file;                                                       //!< internal file management
    QString suffix = "ats";                                           //!< default suffix
    double dipole_length = 0;                                         //!< helper
    qint64 headersize_buf = 0;                                        //!< helper

    bool can_read = false;                                            //!< stop if failure or not initialized
    bool can_write = false;                                           //!< stop if failure or not initialized


    qint64 filepos_offset = 0;                                        //!< in case of remote refence: this would be : 1024 * sizeof(qint32) * 60 if this file is 1 minute late @ 1kHz
    qint64 start_seconds_offset = 0;                                  //!< offset in seconds to RR site (can to be used for "sub seconds")

    bool is_scalable = false;                                         //!< will be set true if channel is E type and dipole length no zero

    eQDateTime edt_start;
    eQDateTime edt_stop;
    QMap<QString,QVariant> atswriter_section;                         //!< contains the data for the XML
    QString measdoc_name = "empty";

    bool change_measdir = true;                                       //!< when working with procmt we may need a new measdir after changing start time

    bool emit_extra = false;                                          //!< emit some extra signals; some require math and false avoids superfluous time
    bool is_remote = false;                                           //!< indicator for remote referencing


    prc_com options;                                                  //!< set options from / for processing

    std::shared_ptr<calibration> calib;                               //!< calibration inclding their vectors and resized data to window length
    std::shared_ptr<calibration> sys_calib;                           //!< calibration inclding their vectors and resized data to window length; the syscal is calculated!
    QFileInfo info_db;                                                //!< global database for all info, including sensor meta data
    QFileInfo master_cal_db;                                          //!< master calibration - e.g. when no calibratrion can be found
    QDir survey_basedir;                                              //!< directory where we find /ts/ /cal/ and so on

    std::shared_ptr<msg_logger> msg = nullptr;
    int slot = -1;                                                    //!< either slot of thread of row position in mc_c channel



};


/*!
   \brief The compsamples_ats_lhs_lt class compares  ats < other ats by samples; left hand file is smaller
 */
bool compsamples_ats_lhs_lt(const atsheader& lhs, const atsheader& rhs);
bool compsamples_ats_lhs_lt_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

/*!
\brief The compstartime_ats_lhs_lt class compares start time ats < other ats
*/

bool compstartime_ats_lhs_lt(const atsheader& lhs, const atsheader& rhs);
bool compstartime_ats_lhs_lt_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

/*!
\brief The compstoptime_ats_lhs_lt class compares start time ats < other ats
*/

bool compstoptime_ats_lhs_lt(const atsheader& lhs, const atsheader& rhs);
bool compstoptime_ats_lhs_lt_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

/*!
\brief delta_stop_start should 0 if can be concat, > 0 if needs fill, < 0 if overlapping and cant be concat
*/

int64_t delta_stop_start(const atsheader& lhs, const atsheader& rhs);
int64_t delta_stop_start_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);

/*!
\brief same_recording compares header items in order to find out if for example Hy and Ex belong to same record
*/

bool same_recording(const atsheader& lhs, const atsheader& rhs);
bool same_recording(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs);



#endif // ATSHEADER_H
