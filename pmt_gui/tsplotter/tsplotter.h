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

#ifndef TSPLOTTER_H
#define TSPLOTTER_H

#include <QMainWindow>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>
#include <QTextEdit>
#include <QMessageBox>
#include <QProgressBar>

#include <memory>
#include <vector>

#include <atsfile.h>
#include <mc_data.h>

#include "gotosampledialog.h"
#include "plot.h"
#include "namespace.h"
#include "qthelper.h"
#include "qcustomplot.h"
#include "xmlcallib.h"
#include "qstring_utilities.h"
#include "geocoordinates.h"
#include "addressdialog.h"

#include "qwebdavdialog.h"

namespace Ui {
class tsplotter;
}

TS_NAMESPACE_BEGIN

struct MinMax {
    double min = 1e30;
    double max = -1e30;

    MinMax() : min(1e30), max(-1e30) {}
    MinMax(double minval, double maxval) : min(minval), max(maxval) {}
};

struct TSPlotInfo {
    TSPlotterPlot * plot_ts;
    TSPlotterPlot * plot_spc;
    int             plot_no;

    TSPlotInfo() : plot_ts(nullptr), plot_spc(nullptr), plot_no(0) {}
    TSPlotInfo(TSPlotterPlot * plot_, TSPlotterPlot * plot_spc_, int plot_no_)
        : plot_ts(plot_), plot_spc(plot_spc_), plot_no(plot_no_) {}
};

enum class TSPlotterPlotAddingOption {
    ADD,
    REPLACE
};

enum class TSPlotterPlotViewOption {
    SEPARATE,
    OVERLAY,
    OVERLAY_PARALLEL,
    OVERLAY_ALL
};

class tsplotter : public QMainWindow {
    Q_OBJECT

public:
    explicit tsplotter(QWidget *parent = Q_NULLPTR);
    ~tsplotter() override;
    void reload_current_sample_window(bool force_set_window_length = false);
    void set_sample_window(qulonglong position, qlonglong window_length,
                           bool force_set_window_length = false);
    void add_window_length(qlonglong window_length, QString caption = QString());
    void set_cursor_mode(CursorMode new_mode);
    void set_plot_view_mode(TSPlotterPlotViewOption new_mode);
    void set_plot_add_mode(TSPlotterPlotAddingOption new_mode);
    void update_scrollbar();
    void update_dialog();

    void open_file_list(const QList<QFileInfo> &files);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void closeEvent(QCloseEvent * event) override;

private:
    int visible_spectra_plots();
    int visible_timeseries_plots();
    void inc_cursor_mode();
    void set_yaxis_range_for_plot(TSPlotterPlot * plot);
    void set_yaxis_range_for_spcplot(TSPlotterPlot * plot);
    void initialize_menus();
    void refresh_axis_labels();
    void rescale_axes();
    void recreate_plots();
    void clear_structures();
    void create_buttons_for_all_ts_plots();
    void create_buttons_for_all_spc_plots();
    void reset_frequency_vector(std::shared_ptr<atsfile> ats_file);
    void load_selections();
    void save_selections();
    void quit_operation();
    bool spectra_is_visible();
    void enable_userinput(bool value);

    void open_files();
    void append_data_to_ats_file(QString const & file_name, std::vector<int32_t> const & data);

signals:
    void export_ascii(uint64_t window_length, uint64_t sample_position, bool iso_time, bool iscal);

public slots:
    void slot_general_msg(const QString &sender, const QString &component, const QString &message);

private slots:
    // **** scaling buttons **** //
    // ///////////////////////// //
    void on_button_scale_allH_clicked(bool checked);
    void on_button_scale_allHx_clicked(bool checked);
    void on_button_scale_allHxHy_clicked(bool checked);
    void on_button_scale_allHy_clicked(bool checked);
    void on_button_scale_allHz_clicked(bool checked);
    void on_button_scale_allEx_clicked(bool checked);
    void on_button_scale_allEy_clicked(bool checked);
    void on_button_scale_allE_clicked(bool checked);
    void on_button_scale_all_clicked(bool checked);

    void on_button_scale_allMan_clicked();

    // **** plot options **** //
    // ////////////////////// //
    void on_button_action_b_detrend_clicked(bool checked);
    void on_button_action_b_scale_clicked(bool checked);
    void on_button_action_b_calibrate_clicked(bool checked);
    void on_button_action_b_stackall_clicked();

    // **** buttons display plots **** //
    // /////////////////////////////// //
    void on_button_display_ts_clicked(bool checked);
    void on_button_display_spc_clicked(bool checked);

    // **** buttons cursor mode **** //
    // ///////////////////////////// //
    void on_button_group_adding_mode_buttonClicked(QAbstractButton * button);
    void on_button_group_plot_mode_buttonClicked(QAbstractButton * button);
    void on_button_group_cursor_mode_buttonClicked(QAbstractButton * button);

    // **** menu - file **** //
    // ///////////////////// //
    void on_menu_action_quit_triggered();
    void on_menu_action_selections_open_triggered();
    void on_menu_action_selections_save_triggered();
    void on_menu_action_export_print_triggered();
    void on_menu_action_export_ascii_triggered();
    void on_menu_action_export_isotime_ascii_triggered();

    // **** menu - utilities **** //
    // ////////////////////////// //
    void on_menu_action_goto_position_triggered();

    // **** menu - settings **** //
    // ///////////////////////// //
    void slot_on_actiongroup_mouse_triggered(QAction * action);
    void slot_on_actiongroup_selection_triggered(QAction * action);
    void slot_on_actiongroup_ticks_triggered(QAction * action);

    // **** scrollbar **** //
    // /////////////////// //
    void on_plot_scrollbar_valueChanged(int value);

    // **** slots statusbar **** //
    // ///////////////////////// //
    void slot_message_frequency(const double x, const double y, const FieldType field);
    void slot_message_coordinates(const double x, const double y, const FieldType field);
    void slot_message_interval(int x, int y);

    // **** other **** //
    // /////////////// //
    void slot_on_mcdata_files_opened(QStringList files);
    void slot_on_plot_interval_added(int a, int b);
    void slot_on_plot_interval_removed(int a, int b);

    // **** data from atsfiles **** //
    // //////////////////////////// //
    void slot_receive_ts_data(std::vector<double> const & new_tsdata);
    void slot_receive_spc_data(std::vector<double> const & new_spcdata);
    void slot_ts_button_checked(bool checked);
    void slot_spc_button_checked(bool checked);
    void on_button_debug_clicked();

    void on_slider_selection_step_valueChanged(int value);

    void on_button_deselect_all_clicked();

    // **** webdav based slots **** //
    // //////////////////////////// //
    void slot_connect_to_webdav(const QString &address);
    void slot_webdav_files_received(QStringList const & files, bool complete_files);
    void slot_webdav_timer_timeout();
    void slot_network_headers_received(QVector<QPair<QString, QByteArray>> const & headers);
    void slot_network_new_data_received(QVector<QPair<QString, std::vector<int32_t>>> const & data);

    // ******** for stack all progress //
    void slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading);
    void slot_update_progress_bar(const int counts);

    void on_actionShow_CAL_triggered();
    void on_actionConnect_triggered();
    void on_actionOpen_Files_triggered();

    void export_ascii_window(bool iso_time, bool iscal);

    void on_actionadd_sys_cal_triggered();
    void on_actionremove_System_Calibration_triggered();
    void on_pushButton_osm_clicked();
    void on_pushButton_gmaps_clicked();


  private:
    // ofc this has to be replaced later with better structure
    QStringList                         m_webdav_files;
    QMap<QString, quint64>              m_file_sizes;
    QMap<QString, quint64>              m_initial_file_sizes;
    QMap<QString, quint64>              m_last_file_size_received;
    QMap<QString, bool>                 m_files_ready;
    QMap<QString, QString>              m_local_files;
    bool                                m_complete_files = false;

    Ui::tsplotter * ui;
    GotoSampleDialog *                  m_gotodialog = nullptr;
    std::unique_ptr<mc_data>            m_mcdata = nullptr;
    qlonglong                           m_current_window_length = -1;
    qulonglong                          m_current_sample_position = 0;
    qulonglong                          m_max_samples = std::numeric_limits<qulonglong>::max();
    QSharedPointer<QCPAxisTickerText>   m_eqdatetime_ticks;
    QActionGroup *                      m_ui_actiongroup_windowlength = nullptr;
    QActionGroup *                      m_ui_actiongroup_mouse = nullptr;
    QActionGroup *                      m_ui_actiongroup_selection = nullptr;
    QActionGroup *                      m_ui_actiongroup_ticks = nullptr;
    eQDateTime                          m_datetime_minimum;
    eQDateTime                          m_datetime_maximum;
    double                              m_current_frequency = 0.0;
    QCPMarginGroup *                    m_ui_plotmargingroup = nullptr;
    std::map<TSPlotType, MinMax>        m_minmax_values;
    std::map<TSPlotType, MinMax>        m_minmax_spc_values;
    std::map<TSPlotterPlot*, MinMax>    m_minmax_p_values;
    std::map<TSPlotterPlot*, MinMax>    m_minmax_p_spc_values;

    QTimer *                            m_webdav_timer = nullptr;

    // dynamic gui elements //
    // //////////////////// //
    QHash<qlonglong, QAction *>         m_ui_actions_windowlength;
    QVector<TSPlotterPlot *>            m_ui_tsplots;
    QVector<TSPlotterPlot *>            m_ui_spcplots;
    QVector<QPushButton *>              m_ui_tsbuttons;
    QVector<QPushButton *>              m_ui_spcbuttons;
    QVector<QMetaObject::Connection>    m_atsfile_connections;

    TSPlotterPlotAddingOption           m_plot_adding_mode = TSPlotterPlotAddingOption::ADD;
    TSPlotterPlotViewOption             m_plot_viewing_mode = TSPlotterPlotViewOption::SEPARATE;
    CursorMode                          m_current_cursor_mode = CursorMode::DEFAULT;
    bool                                m_selections_changed = false;
    bool                                m_selections_common = true;
    bool                                m_option_scale = true;
    bool                                m_option_detrend = false;
    bool                                m_option_calibrate = false;
    int                                 m_option_system_calibrate = 0;
    bool                                m_show_time = true;
    bool                                m_show_all_ts = false;

    std::unordered_map<atsfile *, TSPlotInfo> m_plot_infos;
    std::unordered_map<atsfile *, std::vector<double>>   m_frequencies;
    std::shared_ptr<std::vector<double> > plotter_frequencies = nullptr;

    QProgressBar *qprgb;
    QFileInfo info_db;
    QFileInfo master_cal_db;
    std::unique_ptr<xmlcallib> xmlcal;

    QList<QFileInfo> fileinfos;                     //!< for opening files

    QWebDavDialog *                     m_webdav_dialog = nullptr;
    NetworkAddressDialog *              m_address_dialog = nullptr;

    /*!
     * \brief re_connect connect and re-connect the signal slots to mc_data
     */
    void re_connect();

    // QWidget interface
protected:
};

TS_NAMESPACE_END

#endif // TSPLOTTER_H
