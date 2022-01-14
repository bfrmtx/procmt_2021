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

#ifndef FILTER_H
#define FILTER_H

#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include <QRadioButton>
#include <QList>
#include <QUrl>
#include <QFileInfo>
#include <QFileDialog>
#include <QDrag>
#include <QDir>
#include <QDirIterator>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QProgressBar>
#include <QButtonGroup>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QKeyEvent>

#include <memory>

#include "mc_data.h"
#include "atsfile.h"
#include "atsheader.h"
#include "ats_ascii_file.h"
#include "prc_com.h"
#include "eqdatetime.h"
#include "adulib.h"
#include "atsfile_collector.h"

#include "low_high_band_passses.h"

namespace Ui {
class filter;
}


/*!
 * \brief The filter class
 * \details works on start and use samples.
 */
class filter : public QMainWindow
{
    Q_OBJECT

public:
    explicit filter(QWidget *parent = Q_NULLPTR);
    ~filter();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void keyPressEvent(QKeyEvent *keyevent);

public slots:

    void set_xmlfiles(const  QList<QFileInfo> &qxml);

    void set_decimation_filter(const QString &str_x);
    void set_ascii_output(const QString &str_asc);
    void set_notch_filter(const QString &str_x);


    void on_runbuton_clicked();

    /*!
     * \brief on_check60_clicked set / unset non fitting other check boxes when active
     * \param checked
     */
    void on_check60_clicked(bool checked);

    /*!
     * \brief on_check150_clicked  set / unset non fitting other check boxes when active
     * \param checked
     */
    void on_check150_clicked(bool checked);


    /*!
     * \brief on_check50_clicked  set / unset non fitting other check boxes when active
     * \param checked
     */
    void on_check50_clicked(bool checked);

    /*!
     * \brief on_check180_clicked  set / unset non fitting other check boxes when active
     * \param checked
     */
    void on_check180_clicked(bool checked);


    void on_rb_notch_clicked(bool checked);

private slots:

    /*!
     * \brief slot_set_filter_state unset some check boxes when notch is not actice
     */
    void slot_set_filter_state();


    void slot_gui_fir_filter();

    void slot_single_sub_fir_notch_filter();

    void slot_single_sub_fir_filter();

    void slot_to_ascii_ts();

    void slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);

    void slot_update_progress_bar(const int counts);


    void uncheck_btns();


    void on_rb_1x_clicked(bool checked);

    void on_spinBox_use_samples_valueChanged(int arg1);

    void on_spinBox_start_sample_valueChanged(int arg1);

    void on_spinBox_stop_sample_valueChanged(int arg1);

    void on_TimeStampspinBox_start_valueChanged(int arg1);

    void on_TimeStampspinBox_stop_valueChanged(int arg1);

    void on_dateTimeEdit_start_dateTimeChanged(const QDateTime &dateTime);

    void on_dateTimeEdit_stop_dateTimeChanged(const QDateTime &dateTime);

    void on_check_fullseconds_clicked(bool checked);


    void on_rb_cut_clicked();

    void on_checkBox_copy_old_files_clicked();

    void build_remapper();


    void on_rb_remap_clicked();

    void on_rb_low_pass_clicked();

    void on_rb_high_pass_clicked();

    void on_rb_band_pass_clicked();

    void on_actionHelp_triggered();

    void on_actionopenFolder_triggered();

    void on_actionopenXML_triggered();

    void on_actionopenATS_triggered();

signals:

    void signal_start_filter();

    void signal_start_ascii_output();

    void signal_start_sub_notch_filter();
    void signal_start_sub_fir_filter();

private:

    std::shared_ptr<prc_com> cmdline;

    bool check_cut(const quint64 starts, const quint64 uses, const bool force_min_change = false);

    void count_files();

    void clear();

    void open();

    void set_waiting_for_files(const bool wait);

    int remaining_cascades;

    void delete_files();

    Ui::filter *ui;
    QList<QRadioButton *> allButtons;

    QList<QMap<QString,QVariant>> notch_filter_settings;
    QList<QMap<QString,QVariant>> notch_filter_settings_remember;


    QList<QFileInfo> qfisxmlats_sub;
    QList<QFileInfo> qfisats_sub;

    QList<QFileInfo> delete_atsfiles;
    QList<QFileInfo> delete_xmlfiles;

    QButtonGroup *qbg;


    QList<QFileInfo> qfis;
    QList<QFileInfo> qxml;
    QList<QFileInfo> qdirs;
    QList<QFileInfo> qats;

    std::unique_ptr<eQDateTime>    edate_start;
    std::unique_ptr<eQDateTime>    tmp_edate_start;
    std::unique_ptr<eQDateTime>    tmp_edate_stop;



    bool has_remapper_built = false;
    QMap<QString, QCheckBox*>       in_channels_labels;     //!< names on left side
    QMap<QString, QWidget*>         out_channels_widgets;   //!< for the radio groups
    QMap<QString, QHBoxLayout*>     out_channels_layouts;   //!< for the radio groups layouts
    QMap<QString, QButtonGroup*>     out_channels_groups;   //!< for the radio groups layouts
    QStringList remap_btn_names;
    QMap<QString, int> button_id_map;

    QMap<QString, int> chmap_low;
    QMap<QString, int> chmap_high;



    prc_com first_header;

    quint64       use_samples;               //!< selected samples;

    bool is_gui_update = true;

    bool join_startime_samples;

    void create_xml_from_ukn(std::vector<std::shared_ptr<atsfile>> out_atsfiles);

    low_high_band_passses *qdi = nullptr;

    std::shared_ptr<QMap<QString, QVariant>> extrafilters_status;

    pmt_open_help pmt_help;

    bool slide_to_samepos_after_open = false;       //!< if we cut we don't want to do that because the samples we be re-estimated by time



};

#endif // FILTER_H
