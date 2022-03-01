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

#ifndef PROCMT_MINI_H
#define PROCMT_MINI_H

#include <QMainWindow>
#include <QDebug>
#include <memory>
#include <thread>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QDialog>
#include <QLabel>
#include <QVariant>
#include "gui_items.h"
#include "qstring_utilities.h"
#include "dir_dialog.h"
#include "procmt_alldefines.h"
#include "qtx_templates.h"
#include "prc_com.h"
#include "msg_logger.h"
#include "mt_site.h"
#include "edi_file.h"
#include "procmt_lib.h"
#include <queue>
#include <QProgressBar>
#include <QTableView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableWidgetItem>
#include <QProcess>
#include <QVector>
#include <QMap>
#include <QStandardPaths>

#include "qcustomplot.h"

#ifdef USE_SPECTRAL_PLOTTER_PRO
 #include "spectra_plotter_pro.h"
#else
 #include "spectra_plotter.h"
#endif
#include "lineedit_file.h"

#include "static_plot.h"
#include "spc_db.h"
#include "prc_com_xml.h"

#include <QMessageBox>

namespace Ui {
class procmt_mini;
}

class procmt_mini : public QMainWindow
{
    Q_OBJECT

public:
    explicit procmt_mini(std::shared_ptr<prc_com> cmdline, std::shared_ptr<msg_logger> msg, QWidget *parent = Q_NULLPTR);
    ~procmt_mini();

    //void get_mt_data(std::shared_ptr<mt_site> mtsite, std::vector<std::shared_ptr<edi_file> > &edifiles);

    QFileInfo qfi_base, qfi_center, qfi_emap, qfi_rr;

    std::vector<std::shared_ptr<edi_file> > edifiles;
    std::shared_ptr<mt_site> mtsite;

    void keyPressEvent(QKeyEvent *keyevent);

signals:

    //    void run_procmt_lib(const QStringList &allmeasdocs_center, const QStringList &allmeasdocs_emap, const QStringList &allmeasdocs_rr,
    //                        const mttype &myttype, const proc_type &my_proc_type);

public slots:
    void dir_selected(const QString &absolute_path, const int &target);

    /*!
     * \brief slot_mtdata_finished that is the FIRST run including FFT
     * \param message
     */
    void slot_mtdata_finished(const QString &message);

    void slot_file_progess(const QString &message);

    void slot_change_prc_com_variant(const QString &key, const QVariant &value);

    void slot_set_survey_base_dir(const QString &survey_base_dir);

    /*!
     * \brief slot_mt_site_finished for re-run etc
     */
    void slot_mt_site_finished();




private slots:

    // void on_dirs_base_lineEdit_textChanged(const QString &arg1);

    // void on_dirs_center_lineEdit_editingFinished();

    //  void on_dirs_center_lineEdit_textChanged(const QString &arg1);

    void on_dirs_center_select_pushButton_clicked();


    void on_dirs_rr_select_pushButton_clicked();

    void on_dirs_emap_select_pushButton_clicked();

    void slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);

    void slot_update_progress_bar(const int counts);


    void on_run_pushButton_clicked();

    void on_pushButton_to_settings_clicked();

    void slot_plot();

    void slot_tsplotter(const QString &xmlfile);

    void slot_recalculate();

    void on_pushButton_recaluculate_clicked();

    void slot_actual_edifile_name(const QString edifile_name);

    void slot_set_bw_prz(const QString& str_smooth_medium_sharp);

    void on_pushButton_save_spectra_clicked();

    void on_pushButton_load_spectra_clicked();

    void on_dirs_base_select_pushButton_clicked();

    void on_pushButton_spectral_plotter_clicked();

    void on_pushButton_reset_tensor_clicked();

    void on_pushButton_clear_log_clicked();

    void on_pushButton_edi_plotter_clicked();

    void on_pushButton_clear_edi_clicked();

    //  void on_dirs_emap_lineEdit_textChanged(const QString &arg1);

    //   void on_dirs_rr_lineEdit_textChanged(const QString &arg1);

    void on_dirs_base_lineEdit_valid_dir(const bool &ok);

    void on_dirs_emap_lineEdit_valid_dir(const bool &ok);

    void on_dirs_center_lineEdit_valid_dir(const bool &ok);

    void on_dirs_rr_lineEdit_valid_dir(const bool &ok);


    void slot_add_f_button(const double &frequency);   // add plots
    void slot_frequency_button_clicked();

    void slot_add_tsplotter_center_button();   // add tsplotter plots
    void slot_add_tsplotter_center_button_clicked();

    void slot_add_tsplotter_rr_button();   // add tsplotter plots
    void slot_add_tsplotter_rr_button_clicked();

    void slot_add_tsplotter_emap_button();   // add tsplotter plots
    void slot_add_tsplotter_emap_button_clicked();


    void on_pushButton_export_ascii_clicked();


    void on_pushButton_clear_rr_lineEdit_clicked();

    void on_pushButton_clear_base_lineEdit_clicked();

    void on_pushButton_clear_emap_lineEdit_clicked();

    void on_pushButton_clear_center_lineEdit_clicked();

    void on_skip_marked_parts_clicked();

    void on_pushButton_deselect_all_frequencies_clicked();

    void on_pushButton_select_all_frequecies_clicked();

    void re_check_allmeasdocs(const std::vector<double> &freqs);


    void on_actionHelp_triggered();

    void on_actionCreate_Survey_triggered();

    void on_pushButton_close_all_plots_clicked();

    void on_pushButton_dump_atze_clicked();


    void on_pushButton_save_processing_clicked();

    void on_pushButton_load_processing_clicked();

    void on_OSM_push_button_clicked();

    void on_GMaps_push_button_clicked();

private:

    void fill_prc_com_with_base_values();

    //void check_dirlines();

    std::unique_ptr<atsheader> get_atsheader_first_xml_first_ats();


    Ui::procmt_mini *ui;

    QFileInfo check_url(const QString &instring, bool set_title = false);

    dir_dialog *dirdialog = Q_NULLPTR;

    QDir basedir;
    mutable std::recursive_mutex mutex;

    void insert_combos();

    QString dbname;
    //QComboBox *fbox;
    // std::unique_ptr<frequencies_combo>  selectfreq = nullptr;

    //QComboBox *wbox;
    // std::unique_ptr<windowlength_combo>  select_wl = nullptr;


     std::unique_ptr<spectra_plotter>  spc_plotter = nullptr;

    int all_ckecks = 0;
    QVector<single_column_combo *> scc;
    QMap<QString, int> mapscc;

    QVector<prc_com_check_box *> cbp;
    QMap<QString, int> mapscbp;

    QProgressBar *qprgb;

    QFileInfo last_spectral_dump_qfi;



    void evaluate_mt_type();

    mttype my_mttype = mttype::nomt;
    proc_type my_proc_type = proc_type::no_mtproc;



    QStringList allmeasdocs_center;                 //!< all local station, single site
    QStringList allmeasdocs_center_orig;            //!< all local station, single site

    QStringList allmeasdocs_emap;                   //!< all emap sites, E only taken
    QStringList allmeasdocs_rr;                     //!< all RR sites

    std::shared_ptr<msg_logger> msg = nullptr;
    std::shared_ptr<prc_com> cmdline = nullptr;


    QString edifile_name;                          //!< can change from site_name to site_name_1 and so on


    //std::shared_ptr<procmt_lib> pmt_lib = nullptr;
    std::thread pmt_thread;

    std::queue<std::shared_ptr<procmt_lib>> processing_queue;

    QString message_queue_size;
    QString message_act_queue;
    QString messages_running;

    QStandardItemModel *msg_model;

    checkbox_list *mt_types;

    checkbox_list *smooth_medium_sharp;
    QCheckBox *skip_marked_parts;

    size_t num_threads = 0;

    QStringList site_names;

    QProcess *procediplotter = Q_NULLPTR;
    double old_parzen_radius;


    QButtonGroup *freq_button_group;
    std::vector<QPushButton *> frequency_buttons;

    std::vector<QPushButton *> tsplotter_center_buttons;
    std::vector<QPushButton *> tsplotter_rr_buttons;
    std::vector<QPushButton *> tsplotter_emap_buttons;


    multi_static_plots plots_im, plots_re, plots;


    QMap<double, QString> map_sel_freqs;

    check_boxes_scoll_area* check_freq;

    pmt_open_help pmt_help;

    QComboBox *ftable_combo;


};

#endif // PROCMT_MINI_H
