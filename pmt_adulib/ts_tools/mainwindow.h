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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QList>
#include <QDrag>
#include <QDropEvent>
#include <QMainWindow>
#include <QFileDialog>
#include <QCheckBox>

#include <QUrl>
#include <QMimeType>
#include <QMimeData>
#include <QDir>
#include <QString>
#include <QMap>
#include <QMessageBox>
#include <QRadioButton>


#include <algorithm>    // std::sort

#include "adulib.h"
#include "adutable.h"
#include "adujoblist.h"

#include "atsfile.h"
#include "mc_data.h"
#include "measdocxml.h"
#include "group_atsfiles.h"

#include "gui_items.h"
#include "calibration.h"

#include "adu_json.h"

#include "qstring_utilities.h"
#include "lineedit_file.h"
#include "phoenix_json.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    QFileInfo qfi_base, qfi_target, qfi_source;

public slots:

    void slot_lines_processed(QString nlines_proc);


private slots:
    void on_pushButton_create_xml_clicked();

    void xml_changed(const QMap<QString,QVariant> &atswriter_section, const int &channel_id, const QString &measdir_wanted, const QString &measdoc_wanted);

    void on_pushButton_load_sensordb_clicked();

    void slot_qmap_created(const QMap<QString,QVariant> &data_map);

    void on_dirs_base_lineEdit_valid_dir(const bool &ok);

    void on_dirs_source_lineEdit_valid_dir(const bool &ok);

    void on_dirs_target_lineEdit_valid_dir(const bool &ok);


    void on_dirs_base_select_pushButton_clicked();

    void on_dirs_target_select_pushButton_clicked();

    void on_dirs_source_select_pushButton_clicked();

    void on_actionCreate_Survey_triggered();

    void slot_scan_all_measdocs();

    void set_all_channels_checked();

    void tsdata_subtraction(const size_t &slot);

    void rx_adu_msg(const int &system_serial_number,  const int &channel, const int &slot, const QString &message);



    void on_pushButton_tsdiff_clicked();

    void on_pushButton_run_ascii2ats_clicked();

    void on_pushButton_cancel_ascii2ats_clicked();

    void on_comboBox_sample_freq_currentIndexChanged(int index);

    void on_fcombo_tscat_template_currentIndexChanged(int index);

    void on_run_tscat_button_clicked();

    void on_cancel_tscat_button_clicked();

    void on_pushButton_create_survey_clicked();

    void on_pushButton_convert_json_clicked();

private:
    Ui::MainWindow *ui;

    void clear(const bool clear_mxcds = true);

    void prepare();

    void close_tscat_channels(bool iscancelled);

    void fetch_tsdiff_reader(const size_t slot);

    void sort_tscat_by_start_time();

    void scan_tscat_headers();

    void display_start_stop_tscat();

    void check_for_overlap_tscat();

    void regroup_tscat_atsfiles();


    void setup_group_boxes();


    //QList<atsfile> in_atsfiles;

    std::vector<std::unique_ptr<mc_data>> mxcds;

    std::unique_ptr<mc_data> mxcd;



    std::unique_ptr<adulib> adujob = nullptr;

    tinyxmlwriter tix;                                      //!< writes the job file
    tinyxmlwriter xcal;                                     //!< append the empty calibration section


    QDir basedir;
    QStringList allmeasdocs_source;

    ///////////////////////////////
    QString xml_cnf_basedir;
    QString HWConfig;
    QString HwDatabase;
    QString HwStatus;
    QString ChannelConfig;
    QString outputjobsdir;

    QFileInfo *qfiHWConfig = nullptr;
    QUrl *qrlHWConfig = nullptr;
    QByteArray *qbaHWConfig = nullptr;

    QFileInfo *qfiHwDatabase = nullptr;
    QUrl *qrlHwDatabase = nullptr;
    QByteArray *qbaHwDatabase = nullptr;


    QFileInfo *qfiChannelConfig = nullptr;
    QUrl *qrlChannelConfig = nullptr;
    QByteArray *qbaChannelConfig = nullptr;

    QFileInfo *qfiHwStatus;
    //////////////////////////////////////
    QString xml_filename;

    std::unique_ptr<calibration> calib;

    QFileInfo sensors_sql3;
    QFileInfo info_sql3;
    QFileInfo master_cal_db;


    std::shared_ptr<measdocxml> measdoc;

//    QMap<QString, QVariant> json_data_map;              //!< contains data from Phoenix ts.json file
//    QStringList Phoenix_tags;

    QList <QCheckBox*> chbtns;

    // ------------------- tsdiff --------------------------------------------------------
    std::vector<std::vector<double>>                            tsdiffs;
    size_t                                                      std_slice = 1048576;
    //size_t std_slice = 1024; // for debug

    std::mutex mtx;
    std::vector<std::thread>                                    writerthreads;
    std::vector<std::thread>                                    readerthreads;
    std::vector<std::thread>                                    fetcherthreads;

    std::vector<std::vector<double>>                            data;
    std::vector<std::shared_ptr<threadbuffer<double>>>          tsbuffers;
    std::vector<std::shared_ptr<threadbuffer<double>>>          tsdiffbuffers;


 //   std::vector<std::shared_ptr<threadbuffer<QString>>>         csv_strbuf;
   // std::vector<std::vector<QString>>                           lines;
    std::vector<std::shared_ptr<atsfile>>                       atsfiles;
    //std::vector<std::thread>                                    csvthreads;


    std::vector<int> buffer_status;
    std::vector<std::mutex *>                                   in_mutex;    /*! lock each fetch, wait until unlock e.g. from  tsdata_subtraction */
    std::vector<int>                                            fetchcounts; /*! count each channel input fetch */
    int                                                         can_run = 0; /*! can we run the diff */


    // ------------------- ascii2ats -------------------------------------------------------

    std::vector<std::unique_ptr<sensors_combo>> allsensors;
    std::unique_ptr<frequencies_combo> fcombo;
    std::unique_ptr<frequencies_combo> fcombo_tscat;
    QString NS_Indicatior = QString("X");
    QString EW_Indicatior = QString("X");
    QList<QFileInfo> inats2ascii_files;

    std::vector<std::shared_ptr<atsfile>> atsex;
    std::vector<std::shared_ptr<atsfile>> atsey;
    std::vector<std::shared_ptr<atsfile>> atshx;
    std::vector<std::shared_ptr<atsfile>> atshy;
    std::vector<std::shared_ptr<atsfile>> atshz;


    std::shared_ptr<atsfile> atsex_cat;
    std::shared_ptr<atsfile> atsey_cat;
    std::shared_ptr<atsfile> atshx_cat;
    std::shared_ptr<atsfile> atshy_cat;
    std::shared_ptr<atsfile> atshz_cat;
    std::shared_ptr<mc_data> mcdatas_cat;

    double tscat_freq = 0;
    QString ats_file_search_str;

    bool tscat_continue = false;

    std::unique_ptr<adu_json> adu;
    std::unique_ptr<Phoenix_json> phj_system_ts;
    std::unique_ptr<Phoenix_json> phj_system;

    std::vector<std::shared_ptr<Phoenix_json>> phj_coils;
    std::unique_ptr<eQDateTime> edt;
    QFileInfo phoenix_ts;
    QFileInfo phoenix_cal_coil;
    QFileInfo phoenix_cal_system;
    QMap<size_t, QString> chan_str_mtx;
    QMap<size_t, QString> chan_str_phoenix;

    QStringList rb_sensor_names;
    std::vector<std::vector<QRadioButton*>> v_rb_sensor_names;
    std::vector<QLabel*> v_rb_sensor_serials;

    radiobutton_list *rbl_0;
    radiobutton_list *rbl_1;
    radiobutton_list *rbl_2;

    size_t max_h_chan = 3;



};

#endif // MAINWINDOW_H
