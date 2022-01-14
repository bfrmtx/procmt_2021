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

#ifndef MEASDOCXML_H
#define MEASDOCXML_H

#include "tinyxml2.h"
#include "iostream"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QTextStream>
#include <QByteArray>
#include <QUrl>
#include <vector>
#include <string>
#include <mutex>



/*!
 * \brief The measdocxml class whereas the ADULIB reads all xml and modifies the class this small lib
 * is made for making changes directly to the XML itself.
 */
class measdocxml : public QObject
{
  Q_OBJECT
public:

    /*!
     * \brief measdocxml read - and this is together with re-read the only point where this->root can be moved a way from nullptr
     * \param qfi
     * \param qurl
     * \param qba
     */
    measdocxml(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr, QObject *parent = Q_NULLPTR);
    measdocxml(QObject *parent = Q_NULLPTR);

    bool open(const QFileInfo* qfi = nullptr, const QUrl* qurl = nullptr, const QByteArray *qba = nullptr);



    ~measdocxml();

    QByteArray* to_QByteArray();

    QString getXmlStr();

    bool save(const QFileInfo *qfi = nullptr);

    /*!
     * \brief close close the file and resets the pointers
     * \return
     */
    bool close();

    /*!
     * \brief set_date_time replaces all dates all occurences
     * \param start_datetime
     * \param stop_datetime
     * \return
     */
    int set_date_time(const QDateTime &start_datetime, const QDateTime &stop_datetime);

    /*!
     * \brief set_lat_lon_elev replaces all lat lon elev/height - use for change coordinates
     * \param msec_lat
     * \param msec_lon
     * \param cm_elev
     * \return
     */
    int set_lat_lon_elev(const int &msec_lat, const int &msec_lon, const int &cm_elev);

    //int set_atswriter_channels(const QList<QMap<QString,QVariant>> &atswriter_channels);

    //int set_ATSWriterSettings_ChannelConfig(const QList<QMap<QString,QVariant>> &atswriter_channels);

    /*!
     * \brief change_all_element_text
     * \param element_name
     * \param value
     * \return number of elements changed
     */
    int change_all_elements_text(const QString &element_name, const QString &value);

    /*!
     * \brief set_section set the XMLElement section to a section node at any depth; ex.: global_config,1 will set to  measurement->recording->input->Hardware->global_config
     * \param section_name top section
     * \param nth_section if you know that this section is repeated, you may want to take the second one
     * \return nth node used, if != input or 0, you have a fail
     */
    int goto_section(const QString &section_name, const int &nth_section = 1);

    /*!
     * \brief set_sub_section same as above BUT: having ATSWriter->configuration->channel AND calibration_channels->channel you want go to section ATSWriter first AND THEN to channel
     * \param section_name top section
     * \param subsection_name sub section
     * \param nth_section if you know that this section is repeated, you may want to take the second one, blongs to section, not sub section
     * \return nth node used, if != input or 0, you have a fail
     */
    int goto_sub_section(const QString &section_name, const QString &subsection_name, const int &nth_section = 1);


    bool is_ok() const;


    QFileInfo get_qfile_info() const;

    /*!
     * \brief get_siblings_data
     * \param data
     * \param insert_undefinded
     * \return
     */
    int get_siblings_data(tinyxml2::XMLElement *start_sibling, QMap<QString, QVariant> &data, const bool insert_undefinded = true);
    int get_siblings_data_txm(tinyxml2::XMLElement *start_sibling, QMap<QString, QVariant> &data, const QString &idstr,
                              const QString use_topnode = "", const QString use_node = "", const int position_id = -1);


    int set_siblings_data(tinyxml2::XMLElement *start_sibling, const QMap<QString, QVariant> &data);

    /*!
     * \brief get_atsfiles returns a list of filenames of ats
     * \param channel_nos contains the channel numbers
     * \return ats filenames
     */
    QStringList get_atsfiles(QList<int> &channel_nos);

    int get_calentries(const int channel, QMap<QString, QVariant> &calinfo, std::vector<double> &f_on, std::vector<double> &ampl_on, std::vector<double> &phase_grad_on,
                       std::vector<double> &f_off, std::vector<double> &ampl_off, std::vector<double> &phase_grad_off);

    /*!
     * \brief renumber_ids_calentries when changing Hx <-> Hy for example I must change the corresponding calibration entry
     * \param old_id_new_id
     * \return number of changed items
     */
    int renumber_ids_calentries(const QMap<int, int> old_id_new_id);

    /*!
     * \brief set_verbose generate qInfo Messages
     * \param true_false
     */
    void set_verbose(const bool true_false);

    void query_default_attributes(const tinyxml2::XMLElement* xmlelement);

    int set_channel_section(const QString &topnode, const QList<QMap<QString, QVariant>> &channel_maps, const int &channel_id, const bool main_time_sample_freq = true );

    int set_channel_section(const QString &topnode, const QMap<QString, QVariant> &channel_map, const int &channel_id, const bool main_time_sample_freq = true);

    /*!
     * \brief set_simple_section
     * \param topnode atswriter for example
     * \param subnode comments
     * \param section_data a map containig operator, area and so on
     * \return
     */
    int set_simple_section(const QString &topnode, const QString &subnode, const QMap<QString, QVariant> &section_data);

    int set_main_time_sample_freq(const QVariant &start_time, const QVariant &stop_time, const QVariant &start_date, const QVariant &stop_date, const QVariant &sample_freq);


    /*!
     * \brief get_channel_section parses all channels; want ATSWriter set topnode ATSWriter (even channels are deeper) ; important is the below ATSWriter is only one section with channels
     * \param topnode
     * \param channel_maps will contain the channel data
     * \param channel_id
     * \param insert_undefinded
     * \return
     */
    int get_channel_section(const QString &topnode, QList<QMap<QString, QVariant>> &channel_maps, const int &channel_id, const bool insert_undefinded = true);

    void get_atswriter_section();

    void get_adbboard_autgains();

    void get_txm(const bool emits);


    int get_recording_section(const QString &topnode, const QString &node, QMap<QString, QVariant> &data_map, const bool insert_undefinded = true);
    int get_section(const QString &topnode, const QString &node, QMap<QString, QVariant> &data_map, const bool insert_undefinded = true);
    int get_txm_section(const QString &topnode, const QString &node, QMap<QString, QVariant> &data_map,
                        const QString idstr = "", const QString use_topnode = "", const QString use_node = "", const int position_id = -1);


    int get_top_section(const QString &topnode, QMap<QString, QVariant> &data_map, const bool insert_undefinded = true);

    bool root_attributes(int &GMS, int &Type, QString &Version, QString &Name, QString &root_node_name);

    int get_adbboard(const QString &topnode, const bool insert_undefinded = true);


    int compare_gms_revmain_rev_sub_type(QMap<QString, QVariant> &data_map,const int &GMS, const int &RevMain, const int &RevSub, const int &Type);

    //bool database_keydata(int &GMS, int &Type, int &RevMain, int &RevSub, QString &serial, QString &root_node_name);

    void create_adbboard(const bool emits = true);
    void create_sensor(const bool emits = true);

    /*!
     * \brief create_atswriter create data for the atswritersettings atswriter section<br>
     * site_name should be a long arbitrary name and is part of the atsheader!, line_num and site_num are strings now and only part
     * of the XML file only
     * \param emits true emits the QMap objects
     */
    void create_atswriter(const bool emits = true);

    void create_txm(const bool emits = true);

    void create_gps_status(const bool emits = true);
    void create_system_status(const bool emits = true);
    void create_recording_status(const bool emits = true);
    void create_global_config(const bool emits = true);

    void create_adbboard_autogain(const bool emits = true);

    QString filename_from_atswriter_section(const QMap<QString, QVariant> &atswriter_section);

    QMap<QString, QVariant> get_qmap(const QString which);

public slots:

    void get_hw_status(const bool emits = true);

    void slot_update_atswriter(const QMap<QString, QVariant> &atswriter_section, const int &channel_id, const QString &measdir, const QString &measdoc);

signals:

    void signal_got_atswriter_channel(const QMap<QString, QVariant> &data_map, const int &id);
    void signal_got_adbboard_autogain_channel(const QMap<QString, QVariant> &data_map, const int &id);



    void signal_adbboard_data_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_sensor_data_qmap_created(const QMap<QString, QVariant> &data_map);

    void signal_cal_sensor_data_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_cal_sensor_header_qmap_created(const QMap<QString, QVariant> &data_map);

    void signal_atswriter_data_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_atswriter_configuration_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_atswriter_output_file_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_atswriter_comments_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_global_config_qmap_created(const QMap<QString, QVariant> &data_map);

    void signal_gps_status_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_system_status_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_recording_status_qmap_created(const QMap<QString, QVariant> &data_map);

    void signal_adbboard_autogain_qmap_created(const QMap<QString, QVariant> &data_map);

    void signal_txm_cycle_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_txm_sequence_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_txm_waveform_qmap_created(const QMap<QString, QVariant> &data_map);
    void signal_txm_epos_qmap_created(const QMap<QString, QVariant> &data_map);

    void xmlfilename_changed(const QString basefilename);


protected:

    QVariant get_native(const QVariant &in, const QVariant &native);

    void clear();



    /*!
     * \brief set_section set (a copy) of document root (for example) to atswriter; hence that a section ALWAYS has children!
     *  example this->set_section("ATSWriter", 1) <br>
     *  now we are in the first writer section <br>
     *  set_section("channel", 1, true)  <- see the true here and remind that this is the second call <br>
     *  now we are on the first channel node - and we have made sure that we are not in calibration->channel<br>
     *  tinyxml2::XMLElement *xmlelement = this->subsection;<br>
     *  now we want tp check siblings of channels <br>
     *    if (!xmlelement) return 0;<br>
     *       do {<br>
     *           xmlelement->QueryIntAttribute( "id", &id );<br>
     *           qDebug() << " : " << xmlelement->Value() << id;<br>
     *           xmlelement = xmlelement->NextSiblingElement();<br>
     *           id = -1;<br>
     *    } while (xmlelement);<br>
     *
     * \param section_name string like atswriter
     * \return section number or total amount of section which in cas can only by < nth_section
     */
    int pvt_set_section(const QString &section_name, const int &nth_section = 1, const bool second_call_for_subsection = false);


    QFileInfo qfi;

    bool verbose = false;

    int Type = 0;                       //!< database ID
    int GMS = 0;                        //!< database ID
    QString Name;                       //!< database ID - readable name
    QString Version;                    //!< Version of the metronix xml fileget_section
    int tmp_id;


    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError loaded_doc = tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED;
    tinyxml2::XMLElement *root = nullptr;

    tinyxml2::XMLElement *section = nullptr;
    tinyxml2::XMLElement *subsection = nullptr;
    tinyxml2::XMLNode    *node = nullptr;

    tinyxml2::XMLElement *attribute = nullptr;
    tinyxml2::XMLElement *element = nullptr;

    QMap<QString, QVariant> adbboard_data;
    QMap<QString, QVariant> sensor_data;

    QMap<QString, QVariant> cal_sensor_data;
    QMap<QString, QVariant> cal_sensor_header;

    QMap<QString, QVariant> atswriter_data;
    QMap<QString, QVariant> atswriter_configuration;
    QMap<QString, QVariant> atswriter_output_file;
    QMap<QString, QVariant> atswriter_comments;

    QMap<QString, QVariant> global_config;


    QMap<QString, QVariant> txm_cycle;
    QMap<QString, QVariant> txm_waveform;
    QMap<QString, QVariant> txm_sequence;
    QMap<QString, QVariant> txm_epos;



    QMap<QString, QVariant> gps_status;
    QMap<QString, QVariant> system_status;
    QMap<QString, QVariant> recording_status;

    QMap<QString, QVariant> adbboard_autogain;

    bool threaded = false;

protected:
    mutable std::recursive_mutex mutex;


};

#endif // MEASDOCXML_H
