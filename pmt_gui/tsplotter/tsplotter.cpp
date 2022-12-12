#include "tsplotter.h"
#include "ui_tsplotter.h"
#include "xmlreader.h"
#include "qthelper.h"

#include <QAction>
#include <QVBoxLayout>

TS_USE_NAMESPACE

qlonglong const default_window_length = 1024;
qlonglong const minimal_window_length = 64;
qlonglong const window_count = 14;

int const window_length_magic_no = -1;

char const * warning_box_title             = "Warning!";
char const * warning_ts_sync_error         = "Selected time series don't overlap";
char const * warning_different_freqs       = "Can't show various data sets with different frequencies";

char const * warning_box_title_open        = "Open Files Error!";
char const * warning_different_freqs_overl = "Can't show various data sets with different frequencies / or nor overlapping";

char const * label_current_step_size       = "selection step: %1";
char const * message_position              = "%1:  %2mV";
char const * message_position_nT           = "%1:  %2nT";
char const * message_frequency             = "%1 Hz:  %2mV/sqrt(Hz)";
char const * message_interval              = "current interval: [%1 - %2]";
char const * selection_save_question       = "Save Selection before quit?";
char const * selection_info_changed        = "Selection has been changed";
char const * dialog_goto_title             = "goto sample position...";
char const * window_length_menu            = "Window Length (%1)";
char const * channel_type_str_hx           = "Hx";
char const * channel_type_str_hy           = "Hy";
char const * channel_type_str_hz           = "Hz";
char const * channel_type_str_ex           = "Ex";
char const * channel_type_str_ey           = "Ey";

char const * property_button_plot          = "prop_plot_no";

char const * filedialog_save_title         = "Save as...";
char const * filedialog_png_filter         = "Portable Network Graphics (*.png)";

int const WEBDAV_REQUEST_INTERVAL = 4000;

std::vector<QPen> const plot_qpens = {  QPen(Qt::blue, 0.5),
                                        QPen(Qt::darkRed, 0.5),
                                        QPen(Qt::darkGreen, 0.5),
                                        QPen(Qt::cyan, 0.5),
                                        QPen(Qt::yellow, 0.5),
                                        QPen(Qt::magenta, 0.5)};

double const spc_upper_cut_percentage   = 0.25;
double const spc_lower_cut_percentage   = 0.006;

FieldType get_fieldtype_for_plottype(TSPlotType type) {
    if(type == TSPlotType::EX || type == TSPlotType::EY) {
        return FieldType::E;
    } else {
        return FieldType::H;
    }
}

QString eqdatetime_to_hhmmss(eQDateTime const & time) {
    return QString("%1:%2:%3")
            .arg(time.time().hour(), 2, 10, QChar('0'))
            .arg(time.time().minute(), 2, 10, QChar('0'))
            .arg(time.time().second(), 2, 10, QChar('0'));
}

QString eqdatetime_to_ssms(eQDateTime const & time) {
    return QString("%1:%2")
            .arg(time.time().second(), 2, 10, QChar('0'))
            .arg(time.time().msec(), 3, 10, QChar('0'));
}

void add_timestamp_to_tickmap(QMap<double, QString> & tickmap, qulonglong x, eQDateTime const & time, double stepwidth, bool inc_one = false) {
    auto ctime = time.sample_time(x + (inc_one ? 1 : 0));
    if(stepwidth > 1.0) {
        tickmap[x] = eqdatetime_to_hhmmss(ctime);
    } else {
        tickmap[x] = eqdatetime_to_ssms(ctime);
    }
}

QString get_plot_name(atsfile * file) {
    return QString("%1 %2 #%3")
            .arg(file->channel_type())
            .arg(file->value("channel_number").toString())
            .arg(file->value("system_serial_number").toString());
}

QString get_string_for_window_length_menu(qlonglong length) {
    return QString("Window Length (%1)").arg(length < 0 ? "all" : QString::number(length));
}

TSPlotType get_plot_type_for_channel(QString const & channel_type_string) {
    if(channel_type_string == channel_type_str_ex)   return TSPlotType::EX;
    else if(channel_type_string == channel_type_str_ey)   return TSPlotType::EY;
    else if(channel_type_string == channel_type_str_hx)   return TSPlotType::HX;
    else if(channel_type_string == channel_type_str_hy)   return TSPlotType::HY;
    else if(channel_type_string == channel_type_str_hz)   return TSPlotType::HZ;
    return TSPlotType::UNKNOWN;
}

void set_button_checked_without_signals(QPushButton * target, bool value) {
    target->blockSignals(true);
    target->setChecked(value);
    target->blockSignals(false);
}

void set_action_checked_without_signals(QAction * target, bool value) {
    target->blockSignals(true);
    target->setChecked(value);
    target->blockSignals(false);
}

void visibility_button_checked(QPushButton * button,
                               QVector<TSPlotterPlot *> plots, QVector<QPushButton *> buttons,
                               QPushButton * cont_button, bool checked) {
    auto plot_no = button->property(property_button_plot).toInt();
    if(plots.size() > plot_no) {
        auto & plot = plots[plot_no];
        plot->setVisible(checked);
        bool checked = true;
        std::for_each(buttons.begin(), buttons.end(),
                      [&](auto & n) { checked &= n->isChecked(); });
        set_button_checked_without_signals(cont_button, checked);
    }
    for(auto & cplot : plots) cplot->update();
}

void create_legend_for_plot(TSPlotterPlot * plot) {
    QCPLayoutGrid *     subLayout = new QCPLayoutGrid;
    QCPLayoutElement *  dummyElement = new QCPLayoutElement;
    plot->plotLayout()->addElement(0, 1, subLayout);
    plot->plotLayout()->setColumnStretchFactor(1, 0.1);
    subLayout->addElement(0, 0, dummyElement);
    subLayout->addElement(1, 0, plot->legend);
    subLayout->addElement(2, 0, dummyElement);
    plot->legend->setVisible(true);
    plot->legend->setBorderPen(QPen(Qt::transparent));
}

QPushButton * create_plot_visibility_button(QVector<QPushButton *> & target_vector,
                                            QString const & name, int plot_number,
                                            QWidget * widget, bool checked) {
    auto new_button = new QPushButton(widget);
    widget->layout()->addWidget(new_button);
    target_vector.push_back(new_button);
    new_button->setText(name);
    new_button->setCheckable(true);
    new_button->setChecked(checked);
    new_button->setMinimumSize(60, 25);
    new_button->setProperty(property_button_plot, QVariant(plot_number));
    return new_button;
}





tsplotter::tsplotter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tsplotter),
    m_eqdatetime_ticks(new QCPAxisTickerText())
{
    ui->setupUi(this);
    this->setLocale(QLocale::c());
    ui->button_scale_menu->setVisible(false);

#ifndef QT_DEBUG
    ui->button_debug->setVisible(false);
#endif

    //setStyleSheet("QPushButton:checked { background-color: red; }");
    // this->ui->button_action_b_calibrate->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
    //   this->ui->button_action_b_calibrate->setStyleSheet(("QPushButton:checked { background-color: red; }"));

    m_gotodialog = new GotoSampleDialog(this);
    m_gotodialog->setWindowTitle(dialog_goto_title);

    this->re_connect();

    connect(m_gotodialog, &GotoSampleDialog::value_changed, this, [&](qlonglong value) {
        this->set_sample_window(static_cast<qulonglong>(value), m_current_window_length);
    });

    initialize_menus();
    m_current_window_length = default_window_length;

    this->qprgb = new QProgressBar(this);
    this->ui->statusBar->addPermanentWidget(this->qprgb);
    //QRect rec = QApplication::desktop()->screenGeometry();
    QRect rec = QGuiApplication::screens().at(0)->availableGeometry();
    this->setMinimumWidth(int(rec.width() * 0.7));
    this->setMinimumHeight(int(rec.height() * 0.7));


    info_db.setFile("info.sql3"); master_cal_db.setFile("master_calibration.sql3");
    this->xmlcal = std::make_unique<xmlcallib>(info_db, master_cal_db);

    if (m_option_calibrate) {
        this->ui->button_action_b_calibrate->setText("CAL is ON");
    } else {
        this->ui->button_action_b_calibrate->setText("CAL is OFF");
    }

    m_address_dialog = new NetworkAddressDialog(this);
    connect(m_address_dialog, &NetworkAddressDialog::addressSelected, this, &tsplotter::slot_connect_to_webdav);
}

tsplotter::~tsplotter() {
    delete ui;
}

void tsplotter::reload_current_sample_window(bool force_set_window_length) {

    if (m_option_calibrate) {
        this->m_mcdata->slot_apply_sys_calib(this->m_option_system_calibrate);
        if (this->m_option_system_calibrate) this->ui->button_action_b_calibrate->setText("CAL + SYS is ON");
        else this->ui->button_action_b_calibrate->setText("CAL is ON");

    }
    else {
        this->m_mcdata->slot_apply_sys_calib(this->m_option_system_calibrate);
        this->ui->button_action_b_calibrate->setText("CAL is OFF");
    }
    set_sample_window(m_current_sample_position, m_current_window_length, force_set_window_length);
}

void tsplotter::set_sample_window(qulonglong position, qlonglong window_length,
                                  bool force_set_window_length) {

    // set parameter for set_window_length
    bool change_window_length   = window_length != m_current_window_length;
    auto atsfileout_spectra     = atsfileout::atsfileout_void;
    auto atsfileout_timeseries  = atsfileout::atsfileout_unscaled_timeseries;

    if(spectra_is_visible())    atsfileout_spectra = atsfileout::atsfileout_spectra;
    if(m_option_calibrate)      atsfileout_spectra = atsfileout::atsfileout_calibrated_spectra;
    if(m_option_scale)          atsfileout_timeseries = atsfileout::atsfileout_scaled_timeseries;

    if(m_show_time) {
        double diff = std::pow(2.0, std::floor(std::log2(window_length)) - 3.0);
        QMap<double, QString> ticks;

        qlonglong pstart = qlonglong(position);
        qlonglong pend = static_cast<qlonglong>(position) + window_length;
        qlonglong step = static_cast<qlonglong>(diff);

        if(m_mcdata->in_atsfiles.size() > 0) {
            auto eqdatetime = m_datetime_minimum;
            double stepwidth = ((1.0 / m_current_frequency) * window_length) / 8.0;

            for(qlonglong p = pstart; p <= pend; p += step) {
                add_timestamp_to_tickmap(ticks, static_cast<qulonglong>(p), eqdatetime, stepwidth);
            }
            add_timestamp_to_tickmap(ticks, static_cast<qulonglong>(pend - 1), eqdatetime, stepwidth, true);
        }

        m_eqdatetime_ticks->setTicks(ticks);
    }

    if(window_length == window_length_magic_no) {
        position = 0;
        atsfileout_spectra = atsfileout::atsfileout_void;
    }

    refresh_axis_labels();
    if(position < m_max_samples) {
        m_current_sample_position = position;

        m_show_all_ts = (window_length == window_length_magic_no);
        if(m_show_all_ts) {
            window_length = static_cast<qlonglong>(m_max_samples);
        }

        // refresh window length menu
        if(m_ui_actions_windowlength.contains(window_length_magic_no))
            set_action_checked_without_signals(m_ui_actions_windowlength[window_length_magic_no], false);
        if(m_ui_actions_windowlength.contains(window_length))
            set_action_checked_without_signals(m_ui_actions_windowlength[window_length], true);
        if(m_ui_actions_windowlength.contains(m_current_window_length))
            set_action_checked_without_signals(m_ui_actions_windowlength[m_current_window_length], false);
        ui->menu_windowlength->setTitle(get_string_for_window_length_menu(window_length));

        if(!m_ui_tsplots.empty() && window_length > 0) {
            m_current_sample_position = std::min(m_current_sample_position, m_max_samples - qulonglong(window_length) - 1);
            m_minmax_values.clear();
            m_minmax_p_values.clear();

            m_minmax_spc_values.clear();
            m_minmax_p_spc_values.clear();

            if(change_window_length || force_set_window_length) {
                for(auto atsfile : m_mcdata->in_atsfiles) {
                    // ### TODO: fix this (bs) -> bfr
                    // if we want to see all ts-data...
                    atsfile->set_window_length(size_t(window_length), 0, 0, false,
                                               0, 0, atsfileout::atsfileout_spectra);
                    if(!m_show_all_ts) {
                        reset_frequency_vector(atsfile);
                    }
                }
                m_current_window_length = window_length;
            }

            for(auto atsfile : m_mcdata->in_atsfiles) {
                auto offset = atsfile->get_start_datetime().samples_to(m_datetime_minimum);
                atsfile->slot_slide_to_sample(m_current_sample_position + offset);
                atsfile->slot_ts_reader_mode(this->plotter_frequencies, atsfileout_timeseries, atsfileout_spectra, m_option_detrend);
                atsfile->read_tsplotter();
            }
        } else {
            m_current_window_length = window_length;
        }
    }

    update_dialog();
    update_scrollbar();
}

void tsplotter::add_window_length(qlonglong window_length, QString caption) {
    QAction * menu_window_entry = new QAction(caption.isEmpty() ? QString::number(window_length) : caption, this);
    menu_window_entry->setCheckable(true);
    ui->menu_windowlength->addAction(menu_window_entry);
    m_ui_actions_windowlength[window_length] = menu_window_entry;
    m_ui_actiongroup_windowlength->addAction(menu_window_entry);

    connect(menu_window_entry, &QAction::toggled, this, [this, window_length] (bool checked) {
        if(checked) {
            this->set_sample_window(m_current_sample_position, window_length, true);
        }
    });
}

void tsplotter::update_scrollbar() {
    int cwnd = int(m_current_window_length / 8) * 7;
    int maximum = int(qlonglong(m_max_samples) / cwnd - 1);
    int current = int(qlonglong(m_current_sample_position) / cwnd);
    ui->plot_scrollbar->blockSignals(true);
    ui->plot_scrollbar->setRange(0, maximum);
    ui->plot_scrollbar->setValue(current);
    ui->plot_scrollbar->blockSignals(false);
}

void tsplotter::update_dialog() {
    m_gotodialog->blockSignals(true);
    m_gotodialog->set_min_max(0, qlonglong(m_max_samples) -
                              qlonglong(m_current_window_length));
    m_gotodialog->set_value(qlonglong(m_current_sample_position));
    m_gotodialog->set_start_time(m_datetime_minimum);
    m_gotodialog->blockSignals(false);
}


void tsplotter::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

void tsplotter::dragMoveEvent(QDragMoveEvent * event) {
    event->acceptProposedAction();
}

void tsplotter::initialize_menus() {
    m_ui_actiongroup_windowlength = new QActionGroup(this);
    m_ui_actiongroup_windowlength->setExclusive(true);

    auto window_length = minimal_window_length;
    for(int i = 0; i < window_count; ++i) {
        add_window_length(window_length);
        window_length *= 2;
    }

    // --- 20.12.2019 - removed for future implementation --- //
    // add_window_length(window_length_magic_no, "all");

    m_ui_actions_windowlength[default_window_length]->setChecked(true);
    m_ui_actiongroup_mouse = new QActionGroup(this);
    m_ui_actiongroup_mouse->addAction(ui->menu_action_mouse_readout);
    m_ui_actiongroup_mouse->addAction(ui->menu_action_mouse_selects);
    m_ui_actiongroup_mouse->addAction(ui->menu_action_mouse_deselects);
    m_ui_actiongroup_mouse->setExclusive(true);

    m_ui_actiongroup_selection = new QActionGroup(this);
    m_ui_actiongroup_selection->addAction(ui->menu_action_sel_common);
    m_ui_actiongroup_selection->addAction(ui->menu_action_sel_separate);
    m_ui_actiongroup_selection->setExclusive(true);

    m_ui_actiongroup_ticks = new QActionGroup(this);
    m_ui_actiongroup_ticks->addAction(ui->menu_action_show_time);
    m_ui_actiongroup_ticks->addAction(ui->menu_action_show_sampleno);
    m_ui_actiongroup_ticks->setExclusive(true);

    connect(m_ui_actiongroup_mouse, &QActionGroup::triggered,
            this, &tsplotter::slot_on_actiongroup_mouse_triggered);
    connect(m_ui_actiongroup_selection, &QActionGroup::triggered,
            this, &tsplotter::slot_on_actiongroup_selection_triggered);
    connect(m_ui_actiongroup_ticks, &QActionGroup::triggered, this,
            &tsplotter::slot_on_actiongroup_ticks_triggered);

    ui->menu_action_sel_common->setChecked(true);
    ui->menu_action_show_time->setChecked(true);
    set_plot_view_mode(TSPlotterPlotViewOption::SEPARATE);
    set_plot_add_mode(TSPlotterPlotAddingOption::REPLACE);
    set_cursor_mode(CursorMode::DEFAULT);
    ui->slider_selection_step->setValue(0);
}

void tsplotter::refresh_axis_labels() {
    qApp->processEvents();
    bool T_test = true;
    for(auto pit = m_ui_tsplots.rbegin(); pit != m_ui_tsplots.rend(); ++pit) {
        auto plot = (*pit);
        if(m_show_time) {
            plot->xAxis->setTicker(m_eqdatetime_ticks);
        }
        if(plot->isVisible() && T_test) {
            if(!plot->xAxis->ticks()) {
                plot->xAxis->setTicks(true);
                plot->xAxis->setTickLabels(true);
                plot->replot();
            }
            T_test = false;
        } else {
            if(plot->xAxis->ticks()) {
                plot->xAxis->setTicks(false);
                plot->xAxis->setTickLabels(false);
                plot->replot();
            }
        }
    }
}

void tsplotter::rescale_axes() {
    for(auto & plot : m_ui_tsplots) {
        set_yaxis_range_for_plot(plot);
    }
    for(auto & plot : m_ui_spcplots) {
        set_yaxis_range_for_spcplot(plot);
    }
    for(auto & plot : m_ui_tsplots) {
        if(plot->isVisible()) plot->replot();
    }
    for(auto & plot : m_ui_spcplots) {
        if(plot->isVisible()) plot->replot();
    }
}

void tsplotter::slot_receive_ts_data(const std::vector<double> & new_tsdata) {
    auto ats_file = qobject_cast<atsfile *>(sender());
    auto iterator = m_plot_infos.find(ats_file);

    if(ats_file != nullptr && iterator != m_plot_infos.end()) {
        auto & plot_info = iterator->second;
        auto & plot = plot_info.plot_ts;
        auto & plot_no = plot_info.plot_no;
        auto graph = plot->graph(plot_no);

        graph->setData(QVector<double>(), QVector<double>());
        graph->setName(get_plot_name(ats_file));
        graph->setPen(plot_qpens[size_t(plot_no) % plot_qpens.size()]);

        double min_val =  1e30;
        double max_val = -1e30;
        qulonglong i = 0;

        for(auto & value : new_tsdata) {
            graph->addData(m_current_sample_position + i++, value);
            min_val = std::min(value, min_val);
            max_val = std::max(value, max_val);
        }

        if(m_minmax_p_values.find(plot) == m_minmax_p_values.end()) {
            m_minmax_p_values[plot].min = min_val;
            m_minmax_p_values[plot].max = max_val;
        } else {
            m_minmax_p_values[plot].min = std::min(min_val,  m_minmax_p_values[plot].min);
            m_minmax_p_values[plot].max = std::max(max_val,  m_minmax_p_values[plot].max);
        }

        if(m_minmax_values.find(plot->plot_type()) == m_minmax_values.end()) {
            m_minmax_values[plot->plot_type()].min = min_val;
            m_minmax_values[plot->plot_type()].max = max_val;
        } else {
            m_minmax_values[plot->plot_type()].min = std::min(m_minmax_values[plot->plot_type()].min, min_val);
            m_minmax_values[plot->plot_type()].max = std::max(m_minmax_values[plot->plot_type()].max, max_val);
        }

        plot->xAxis->setRange(m_current_sample_position, m_current_sample_position + new_tsdata.size() - 1);
        if(ats_file == m_mcdata->in_atsfiles.back().get()) {
            rescale_axes();
        }
    }
}

void tsplotter::slot_receive_spc_data(const std::vector<double> & new_spcdata) {
    auto ats_file = qobject_cast<atsfile *>(sender());
    auto iterator = m_plot_infos.find(ats_file);

    if(ats_file != nullptr && iterator != m_plot_infos.end()) {
        auto & plot_info = iterator->second;
        auto & plot = plot_info.plot_spc;
        auto & plot_no = plot_info.plot_no;
        auto graph = plot->graph(plot_no);

        auto freq_iterator = m_frequencies.find(ats_file);
        if(freq_iterator != m_frequencies.end()) {
            auto & keys = freq_iterator->second;
            if(!keys.empty() && keys.size() == new_spcdata.size()) {
                graph->setData(QVector<double>(), QVector<double>());
                graph->setName(get_plot_name(ats_file));
                graph->setPen(plot_qpens[size_t(plot_no) % plot_qpens.size()]);

                double min_val =  1e30;
                double max_val = -1e30;

                for(size_t i = 0; i < std::min(new_spcdata.size(), keys.size()); ++i) {
                    graph->addData(keys[i], new_spcdata[i]);
                    min_val = std::min(new_spcdata[i], min_val);
                    max_val = std::max(new_spcdata[i], max_val);
                }

                plot->xAxis->setRange(keys.front(), keys.back());
                plot->yAxis->setRange(std::pow(10, std::floor(std::log10(min_val))),
                                      std::pow(10, std::ceil(std::log10(max_val))));

                if(m_minmax_p_spc_values.find(plot) == m_minmax_p_spc_values.end()) {
                    m_minmax_p_spc_values[plot].min = min_val;
                    m_minmax_p_spc_values[plot].max = max_val;
                } else {
                    m_minmax_p_spc_values[plot].min = std::min(min_val,  m_minmax_p_spc_values[plot].min);
                    m_minmax_p_spc_values[plot].max = std::max(max_val,  m_minmax_p_spc_values[plot].max);
                }

                if(m_minmax_spc_values.find(plot->plot_type()) == m_minmax_spc_values.end()) {
                    m_minmax_spc_values[plot->plot_type()].min = min_val;
                    m_minmax_spc_values[plot->plot_type()].max = max_val;
                } else {
                    m_minmax_spc_values[plot->plot_type()].min = std::min(m_minmax_spc_values[plot->plot_type()].min, min_val);
                    m_minmax_spc_values[plot->plot_type()].max = std::max(m_minmax_spc_values[plot->plot_type()].max, max_val);
                }
                plot->replot();
            } else {
                qWarning() << "unexpected vector dimensions: " << new_spcdata.size() << "|" << keys.size();
            }
        }
    }
}

void tsplotter::slot_ts_button_checked(bool checked) {
    auto button = qobject_cast<QPushButton*>(sender());
    if(button != nullptr) {
        visibility_button_checked(button, m_ui_tsplots, m_ui_tsbuttons, ui->button_display_ts, checked);
        refresh_axis_labels();
    }
}

void tsplotter::slot_spc_button_checked(bool checked) {
    auto button = qobject_cast<QPushButton*>(sender());
    if(button != nullptr) {
        visibility_button_checked(button, m_ui_spcplots, m_ui_spcbuttons, ui->button_display_spc, checked);
        if(checked && visible_spectra_plots() == 1) {
            reload_current_sample_window();
        }
    }
}

void tsplotter::recreate_plots() {
    clear_structures();

    std::unordered_map<TSPlotType, TSPlotterPlot *> plot_type_ts_plots;
    std::unordered_map<TSPlotType, TSPlotterPlot *> plot_type_spc_plots;
    std::unordered_map<FieldType, TSPlotterPlot *> field_type_ts_plots;
    std::unordered_map<FieldType, TSPlotterPlot *> field_type_spc_plots;

    eQDateTime max_start_time;
    //eQDateTime min_stop_time(QDate(9999, 12, 12).startOfDay());
    eQDateTime min_stop_time(QDate(9999, 12, 12));

    // there is a atsfile->get_start_sample - method (just in case we need it)
    for(auto & atsfile : m_mcdata->in_atsfiles) {
        max_start_time = std::max(max_start_time, atsfile->get_start_datetime());
        min_stop_time = std::min(min_stop_time, atsfile->get_stop_datetime());

        auto atsfile_rawptr = atsfile.get();
        auto plot_type = get_plot_type_for_channel(atsfile->channel_type());
        auto field_type = get_fieldtype_for_plottype(plot_type);
        TSPlotterPlot * ts_plot = nullptr;
        TSPlotterPlot * spc_plot = nullptr;
        bool new_plot_created = true;

        if(m_plot_viewing_mode == TSPlotterPlotViewOption::SEPARATE) {
            ts_plot = new TSPlotterPlot(this);
            spc_plot = new TSPlotterPlot(this);
        } else if(m_plot_viewing_mode == TSPlotterPlotViewOption::OVERLAY) {
            if(plot_type_ts_plots.find(plot_type) == plot_type_ts_plots.end()) {
                plot_type_ts_plots[plot_type] = ts_plot = new TSPlotterPlot(this);
                plot_type_spc_plots[plot_type] = spc_plot = new TSPlotterPlot(this);
            } else {
                new_plot_created = false;
            }
            ts_plot = plot_type_ts_plots[plot_type];
            spc_plot = plot_type_spc_plots[plot_type];
        } else if(m_plot_viewing_mode == TSPlotterPlotViewOption::OVERLAY_PARALLEL) {
            if(field_type_ts_plots.find(field_type) == field_type_ts_plots.end()) {
                field_type_ts_plots[field_type] = ts_plot = new TSPlotterPlot(this);
                field_type_spc_plots[field_type] = spc_plot = new TSPlotterPlot(this);
            } else {
                new_plot_created = false;
            }
            ts_plot = field_type_ts_plots[field_type];
            spc_plot = field_type_spc_plots[field_type];
        } else if(m_plot_viewing_mode == TSPlotterPlotViewOption::OVERLAY_ALL) {
            if(field_type_ts_plots.find(field_type) == field_type_ts_plots.end()) {
                field_type_ts_plots[FieldType::E] = ts_plot = new TSPlotterPlot(this);
                field_type_spc_plots[FieldType::E] = spc_plot = new TSPlotterPlot(this);
                field_type_ts_plots[FieldType::H] = ts_plot = new TSPlotterPlot(this);
                field_type_spc_plots[FieldType::H] = spc_plot = new TSPlotterPlot(this);
            } else {
                new_plot_created = false;
            }
            ts_plot = field_type_ts_plots[FieldType::E];
            spc_plot = field_type_spc_plots[FieldType::E];

        }
        ts_plot->addGraph();
        spc_plot->addGraph();

        if(new_plot_created) {
            create_legend_for_plot(ts_plot);
            create_legend_for_plot(spc_plot);
            ts_plot->set_plot_type(plot_type);
            spc_plot->set_plot_type(plot_type);

            spc_plot->xAxis->setRange(0, m_current_window_length);
            spc_plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
            spc_plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
            spc_plot->setVisible(ui->button_display_spc->isChecked());

            QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
            spc_plot->yAxis->setTicker(logTicker);
            spc_plot->yAxis->setNumberFormat("eb");
            spc_plot->yAxis->setNumberPrecision(0);
        }

        connect(ts_plot,    &TSPlotterPlot::interval_added,                 this, &tsplotter::slot_on_plot_interval_added);
        connect(ts_plot,    &TSPlotterPlot::interval_removed,               this, &tsplotter::slot_on_plot_interval_removed);
        connect(ts_plot,    &TSPlotterPlot::plot_position_changed,          this, &tsplotter::slot_message_coordinates);
        connect(ts_plot,    &TSPlotterPlot::interval_selection_changed,     this, &tsplotter::slot_message_interval);
        connect(spc_plot,   &TSPlotterPlot::plot_position_changed,          this, &tsplotter::slot_message_frequency);

        ts_plot->setVisible(ui->button_display_ts->isChecked());

        if(new_plot_created) {
            m_ui_tsplots.push_back(ts_plot);
            m_ui_spcplots.push_back(spc_plot);
        }

        TSPlotInfo new_plot_info(ts_plot, spc_plot, ts_plot->graphCount() - 1);

        m_plot_infos[atsfile_rawptr] = new_plot_info;
        m_atsfile_connections.push_back(connect(atsfile_rawptr, &atsfile::signal_tsdata, this, &tsplotter::slot_receive_ts_data));
        m_atsfile_connections.push_back(connect(atsfile_rawptr, &atsfile::signal_spcdata, this, &tsplotter::slot_receive_spc_data));
    }


    for(auto & plot : m_ui_tsplots) {
        ui->widget_center->layout()->addWidget(plot);
    }
    for(auto & plot : m_ui_spcplots) {
        ui->widget_center->layout()->addWidget(plot);
    }

    create_buttons_for_all_ts_plots();
    create_buttons_for_all_spc_plots();

    if(!m_ui_tsplots.empty() && !m_ui_spcplots.empty()) {
        m_ui_plotmargingroup = new QCPMarginGroup(m_ui_tsplots[0]);
        for(auto i = 0; i < m_ui_tsplots.size(); ++i) {
            m_ui_tsplots[i]->axisRect()->setMarginGroup(QCP::msLeft, m_ui_plotmargingroup);
        }
        for(auto i = 0; i < m_ui_spcplots.size(); ++i) {
            m_ui_spcplots[i]->axisRect()->setMarginGroup(QCP::msLeft, m_ui_plotmargingroup);
        }
    }


    if(min_stop_time < max_start_time) {
        QMessageBox::warning(this, warning_box_title, warning_ts_sync_error);
        m_mcdata->close_infiles();
        m_mcdata->close_measdocs();
        m_mcdata->close_calibration();
        m_mcdata->close_outfiles();
        //m_frequency_string.clear();
        clear_structures();
        update();
    } else {
        m_datetime_maximum = min_stop_time;
        m_datetime_minimum = max_start_time;
        m_current_frequency = max_start_time.get_sample_freq();
        m_max_samples = max_start_time.samples_to(min_stop_time);

    }

    reload_current_sample_window(true);
    set_cursor_mode(m_current_cursor_mode);
}

void tsplotter::clear_structures() {
    for(auto & connection : m_atsfile_connections) {
        QObject::disconnect(connection);
    }

    for(auto & button : m_ui_tsbuttons)     delete button;
    for(auto & button : m_ui_spcbuttons)    delete button;
    for(auto & plot : m_ui_tsplots)         delete plot;
    for(auto & plot : m_ui_spcplots)        delete plot;
    m_atsfile_connections.clear();
    m_ui_tsplots.clear();
    m_ui_spcplots.clear();
    m_ui_tsbuttons.clear();
    m_ui_spcbuttons.clear();
    m_frequencies.clear();
    m_plot_infos.clear();
    m_minmax_values.clear();
    m_minmax_spc_values.clear();
}

void tsplotter::create_buttons_for_all_ts_plots() {
    for(int plot_no = 0; plot_no < m_ui_tsplots.size(); ++plot_no) {
        auto new_ts_button = create_plot_visibility_button(m_ui_tsbuttons, QString("TS %1").arg(plot_no), plot_no,
                                                           ui->widget_head, ui->button_display_ts->isChecked());
        connect(new_ts_button, &QPushButton::toggled, this, &tsplotter::slot_ts_button_checked);
    }
}

void tsplotter::create_buttons_for_all_spc_plots() {
    for(int plot_no = 0; plot_no < m_ui_spcplots.size(); ++plot_no) {
        auto new_spc_button = create_plot_visibility_button(m_ui_spcbuttons, QString("SPC %1").arg(plot_no), plot_no,
                                                            ui->widget_head, ui->button_display_spc->isChecked());
        connect(new_spc_button, &QPushButton::toggled, this, &tsplotter::slot_spc_button_checked);
    }
}

void tsplotter::reset_frequency_vector(std::shared_ptr<atsfile> ats_file) {
    std::vector<double> f(ats_file->get_frequencies());
    size_t start_idx = size_t ( double(f.size()) * spc_lower_cut_percentage);
    size_t stop_idx = size_t( double(f.size()) - ( double(f.size()) * spc_upper_cut_percentage)  ) + 1;
    if (stop_idx > f.size()) stop_idx = f.size() ;
    if (plotter_frequencies != nullptr) plotter_frequencies.reset();
    if (plotter_frequencies == nullptr) plotter_frequencies =
            std::make_shared<std::vector<double>>(f.begin() + static_cast<long long>(start_idx),
                                                  f.begin() + static_cast<long long>(stop_idx));
    m_frequencies[ats_file.get()] = {f.begin() + static_cast<long long>(start_idx),
            f.begin() + static_cast<long long>(stop_idx)};
}



void tsplotter::dropEvent(QDropEvent *event) {
    auto mime_data = event->mimeData();

    if(mime_data->hasUrls()) {

        this->fileinfos.clear();

        // ### TODO: check url-type (bs) ### //
        for(auto url : mime_data->urls()) {
            QFileInfo test(QFileInfo(url.toLocalFile()));
            if (test.isDir()) {
                QDir dir(test.absoluteFilePath());
                QStringList nameFilter;
                nameFilter << "*.XML";
                QFileInfoList lst = dir.entryInfoList(nameFilter, QDir::Files);
                for (const auto &qfi : lst) {
                    fileinfos.push_back(qfi);
                }
            }
            else {
                if ( test.absoluteFilePath().endsWith("xml", Qt::CaseInsensitive) ||
                     test.absoluteFilePath().endsWith("ats", Qt::CaseInsensitive) ) fileinfos.push_back(QFileInfo(url.toLocalFile()));
            }
        }

        this->open_files();
    }
}


void tsplotter::open_file_list(const QList<QFileInfo> &files)
{
    if (files.size()) this->fileinfos.clear();
    for(auto qfis : files) {

        if (qfis.isDir()) {
            QDir dir(qfis.absoluteFilePath());
            QStringList nameFilter;
            nameFilter << "*.XML";
            QFileInfoList lst = dir.entryInfoList(nameFilter, QDir::Files);
            for (const auto &qfi : lst) {
                fileinfos.push_back(qfi);
            }
        }
        else {
            if ( qfis.absoluteFilePath().endsWith("xml", Qt::CaseInsensitive) ||
                 qfis.absoluteFilePath().endsWith("ats", Qt::CaseInsensitive) ) fileinfos.push_back(qfis);
        }
    }

    this->open_files();

}


void tsplotter::open_files()
{
    if(fileinfos.empty()) return;

    // convert all to ats files in order beeing able to mix
    QList<QFileInfo> fileinfos_xml;
    QList<QFileInfo> fileinfos_ats;

    for (auto &qfi : fileinfos) {
        if (qfi.absoluteFilePath().endsWith("ats", Qt::CaseInsensitive)) fileinfos_ats.append(qfi);
        if (qfi.absoluteFilePath().endsWith("xml", Qt::CaseInsensitive)) fileinfos_xml.append(qfi);

    }



    fileinfos.clear();

    // check if we can open the XMLs together

    if (fileinfos_xml.size()) {
        QString title = "tsplotter: ";
        std::unique_ptr<mc_data> test_open = std::make_unique<mc_data>(this);
        connect(test_open.get(), &mc_data::signal_general_msg, this, &tsplotter::slot_general_msg);
        if (test_open->open_files(fileinfos_xml, true, mttype::nomt)) {
            for (auto & files : test_open->in_atsfiles) {
                fileinfos_ats.append(QFileInfo(files->absoluteFilePath()));


            }
            QMap <QString, QVariant> xmap = atsfilename_site_survey(fileinfos_xml.at(0).absoluteFilePath());
            if (xmap.contains("survey")) title.append(xmap.value("survey").toString() +  "  ");
            if (xmap.contains("site")) title.append(xmap.value("site").toString() +  "  ");
            for (auto & str : fileinfos_xml) title.append(str.baseName() + " ");
            this->setWindowTitle(title);
        }
        else {
            QMessageBox::warning(this, "xml file loaded, but no data inside", "QUIT");

            return;
        }
    }



    // all new files together now; make sure that we skip doubles in append
    fileinfos_xml.clear();



    switch(m_plot_adding_mode) {

    case TSPlotterPlotAddingOption::ADD: {

        // append the old files in this mode
        for (auto files : this->m_mcdata->in_atsfiles) {
            fileinfos.append(QFileInfo(files->absoluteFilePath()));
        }

        for (auto &newfile : fileinfos_ats) {
            if (!this->fileinfos.contains(newfile)) this->fileinfos.append(newfile);
        }

        std::unique_ptr<mc_data> test_open = std::make_unique<mc_data>(this);
        connect(test_open.get(), &mc_data::signal_general_msg, this, &tsplotter::slot_general_msg);

        if (test_open->open_files(fileinfos, true, mttype::nomt)) {
            test_open.reset();
            this->re_connect();
            m_mcdata->open_files(fileinfos, true);
        }
        break;
    }
    case TSPlotterPlotAddingOption::REPLACE: {

        this->fileinfos.swap(fileinfos_ats);

        std::unique_ptr<mc_data> test_open = std::make_unique<mc_data>(this);
        connect(test_open.get(), &mc_data::signal_general_msg, this, &tsplotter::slot_general_msg);

        if (test_open->open_files(fileinfos, true, mttype::nomt)) {
            test_open.reset();
            this->re_connect();
            m_mcdata->open_files(fileinfos, true, mttype::nomt);

        }
        break;
    }
    }

    if (!this->m_mcdata->in_atsfiles.size()) {
        QMessageBox::warning(this, "xml file loaded, but no data inside, file read error", "QUIT");

        return;
    }

    if (this->m_mcdata->in_atsfiles.at(0)->get_num_samples() < this->m_current_window_length) {
        //this->set_sample_window(0,this->m_mcdata->in_atsfiles.at(0)->get_num_samples(),false);
    }

    // reset t always usable state
    this->on_button_display_ts_clicked(true);
    this->on_button_display_spc_clicked(false);

}


void tsplotter::on_button_scale_allH_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allHx, false);
        set_button_checked_without_signals(ui->button_scale_allHy, false);
        set_button_checked_without_signals(ui->button_scale_allHz, false);
        set_button_checked_without_signals(ui->button_scale_allHxHy, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_allHx_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allH, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_allHxHy_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allH, false);
        set_button_checked_without_signals(ui->button_scale_allHx, false);
        set_button_checked_without_signals(ui->button_scale_allHy, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_allHy_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allH, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_allHz_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allH, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_allEx_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allE, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_allEy_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allE, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_allE_clicked(bool checked) {
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allEx, false);
        set_button_checked_without_signals(ui->button_scale_allEy, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);
    }
    rescale_axes();
}

void tsplotter::on_button_scale_all_clicked(bool checked)
{
    if(checked) {

        set_button_checked_without_signals(ui->button_scale_allEx, false);
        set_button_checked_without_signals(ui->button_scale_allEy, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);


        set_button_checked_without_signals(ui->button_scale_allHx, false);
        set_button_checked_without_signals(ui->button_scale_allHy, false);
        set_button_checked_without_signals(ui->button_scale_allHz, false);
        set_button_checked_without_signals(ui->button_scale_allHxHy, false);
        set_button_checked_without_signals(ui->button_scale_allMan, false);

    }
    rescale_axes();
}



void tsplotter::on_button_action_b_calibrate_clicked(bool checked) {
    m_option_calibrate = checked;
    if(checked) {
        ui->button_action_b_scale->setChecked(true);
        ui->button_action_b_detrend->setChecked(true);
        m_option_detrend = true;
        m_option_scale = true;
    }

    reload_current_sample_window(true);
}

void tsplotter::on_button_action_b_detrend_clicked(bool checked) {
    m_option_detrend = checked;
    if(!checked) {
        ui->button_action_b_calibrate->setChecked(false);
        m_option_calibrate = false;
    }
    reload_current_sample_window();
}

void tsplot::tsplotter::on_button_action_b_scale_clicked(bool checked) {
    m_option_scale = checked;
    if(!checked) {
        ui->button_action_b_calibrate->setChecked(false);
        m_option_calibrate = false;
    }
    reload_current_sample_window();
}

void tsplotter::on_menu_action_quit_triggered() {
    quit_operation();
}


void tsplotter::slot_on_mcdata_files_opened(QStringList files) {
    Q_UNUSED(files);
    recreate_plots();
}

void tsplotter::slot_on_actiongroup_selection_triggered(QAction * action) {
    m_selections_common = action == ui->menu_action_sel_common;
}

void tsplotter::slot_on_actiongroup_ticks_triggered(QAction * action) {
    m_show_time = (action == ui->menu_action_show_time);
    for(auto & ts_plot : m_ui_tsplots) {
        if(m_show_time) {
            ts_plot->xAxis->setTicker(m_eqdatetime_ticks);
        } else {
            ts_plot->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker()));
        }
    }
    reload_current_sample_window();
}


void tsplotter::inc_cursor_mode() {
    if(m_current_cursor_mode == CursorMode::DEFAULT)                set_cursor_mode(CursorMode::SELECTION_ADD);
    else if(m_current_cursor_mode == CursorMode::SELECTION_ADD)     set_cursor_mode(CursorMode::SELECTION_REMOVE);
    else if(m_current_cursor_mode == CursorMode::SELECTION_REMOVE)  set_cursor_mode(CursorMode::DEFAULT);
}

void tsplotter::on_button_group_adding_mode_buttonClicked(QAbstractButton * button) {
    if(button == ui->button_plotaddingmode_add)                     set_plot_add_mode(TSPlotterPlotAddingOption::ADD);
    else if(button == ui->button_plotaddingmode_replace)            set_plot_add_mode(TSPlotterPlotAddingOption::REPLACE);
}

void tsplotter::on_button_group_plot_mode_buttonClicked(QAbstractButton * button) {
    if(button == ui->button_plotmode_overlay)                       set_plot_view_mode(TSPlotterPlotViewOption::OVERLAY);
    else if(button == ui->button_plotmode_separate)                 set_plot_view_mode(TSPlotterPlotViewOption::SEPARATE);
    else if(button == ui->button_plotmode_ptest)                    set_plot_view_mode(TSPlotterPlotViewOption::OVERLAY_PARALLEL);
    else if(button == ui->button_plotmode_noise)                    set_plot_view_mode(TSPlotterPlotViewOption::OVERLAY_ALL);


}

void tsplotter::on_button_group_cursor_mode_buttonClicked(QAbstractButton * button) {
    if(button == ui->button_mode_default)                           set_cursor_mode(CursorMode::DEFAULT);
    else if(button == ui->button_mode_select_add)                   set_cursor_mode(CursorMode::SELECTION_ADD);
    else if(button == ui->button_mode_select_rem)                   set_cursor_mode(CursorMode::SELECTION_REMOVE);
}

void tsplotter::slot_on_actiongroup_mouse_triggered(QAction * action) {
    if(action->isChecked()) {
        if(action == ui->menu_action_mouse_readout)                 set_cursor_mode(CursorMode::READOUT);
        else if(action == ui->menu_action_mouse_selects)            set_cursor_mode(CursorMode::SELECTION_ADD);
        else if(action == ui->menu_action_mouse_deselects)          set_cursor_mode(CursorMode::SELECTION_REMOVE);
    }
}

void tsplot::tsplotter::on_button_display_ts_clicked(bool checked) {
    for(auto & ts_button : m_ui_tsbuttons) {
        ts_button->setChecked(checked);
    }
}

void tsplot::tsplotter::on_button_display_spc_clicked(bool checked) {
    for(auto & spc_button : m_ui_spcbuttons) {
        spc_button->setChecked(checked);
    }
}

void tsplot::tsplotter::on_plot_scrollbar_valueChanged(int value) {
    qulonglong new_sample_pos = qulonglong(value) * (qulonglong(m_current_window_length / 8) * 7);
    set_sample_window(new_sample_pos, m_current_window_length);
}

void tsplotter::on_menu_action_selections_open_triggered() {
    load_selections();
}

void tsplotter::on_menu_action_selections_save_triggered() {
    save_selections();
}

void tsplotter::slot_message_frequency(double x, double y) {
    QString str_y = QString::number(y, 'E');
    ui->statusBar->showMessage(QString(message_frequency).arg(x).arg(str_y));
}

void tsplotter::slot_message_coordinates(double x, double y) {
    QString str_x = QString::number(x);
    QString str_y = QString::number(y);
    if(m_show_time) {
        auto eqdatetime = m_mcdata->in_atsfiles[0]->get_start_datetime();
        str_x = eqdatetime.ISO_date_time_frac(qulonglong(x));
    }
    //! @todo YASC if Cal is down and plot is Hx, Hy, Hz THIS!!!! please ui->statusBar->showMessage(QString(message_position_nT).arg(str_x).arg(str_y));

    ui->statusBar->showMessage(QString(message_position).arg(str_x).arg(str_y));
}

void tsplotter::slot_message_interval(int min, int max) {
    ui->statusBar->showMessage(QString(message_interval).arg(min).arg(max));
}

void tsplotter::slot_on_plot_interval_added(int a, int b) {
    if(m_selections_common) {
        for(auto & plot : m_ui_tsplots) {
            plot->blockSignals(true);
            plot->add_selection_interval(a, b);
            plot->blockSignals(false);
        }
    }
    m_selections_changed = true;
}

void tsplotter::slot_on_plot_interval_removed(int a, int b) {
    if(m_selections_common) {
        for(auto & plot : m_ui_tsplots) {
            plot->blockSignals(true);
            plot->remove_selection_interval(a, b);
            plot->blockSignals(false);
        }
    }
    m_selections_changed = true;
}


void tsplotter::keyPressEvent(QKeyEvent * event) {
    if(event->key() == Qt::Key_Shift && m_current_cursor_mode != CursorMode::READOUT) {
        set_cursor_mode(CursorMode::SELECTION_REMOVE);
    }
}

void tsplotter::mousePressEvent(QMouseEvent * event) {
    if(event->button() == Qt::MiddleButton) {
        inc_cursor_mode();
    }
}

void tsplotter::closeEvent(QCloseEvent * event) {
    Q_UNUSED(event);
    quit_operation();
}

void tsplotter::keyReleaseEvent(QKeyEvent * event) {
    auto value = qthelper::get_numeric_value_for_number_key(event->key());
    if(value != -1) {
        value = value == 0 ? 9 : value - 1;
        if(value < m_ui_tsbuttons.size()) {
            m_ui_tsbuttons[value]->toggle();
        }
    }

    if(event->key() == Qt::Key_Shift && m_current_cursor_mode != CursorMode::READOUT) {
        set_cursor_mode(CursorMode::SELECTION_ADD);
    } else if(event->key() == Qt::Key::Key_Right) {
        on_plot_scrollbar_valueChanged(std::min(ui->plot_scrollbar->value() + 1,
                                                ui->plot_scrollbar->maximum()));
    } else if(event->key() == Qt::Key::Key_Left) {
        on_plot_scrollbar_valueChanged(std::max(ui->plot_scrollbar->value() - 1, 0));
    } else if(event->key() == Qt::Key::Key_Space) {
        for(auto & plot : m_ui_tsplots) {
            plot->blockSignals(true);
            auto i64samplepos = static_cast<int64_t>(m_current_sample_position);
            plot->add_selection_interval(i64samplepos,
                                         i64samplepos + m_current_window_length - 1);
            plot->blockSignals(false);
        }
    }
}

void tsplotter::set_cursor_mode(CursorMode new_mode) {
    for(auto plot : m_ui_tsplots) {
        plot->set_cursor_mode(new_mode);
    }
    ui->menu_action_mouse_readout->setChecked(false);
    ui->menu_action_mouse_selects->setChecked(false);
    ui->menu_action_mouse_deselects->setChecked(false);
    ui->button_mode_select_add->setChecked(false);
    ui->button_mode_select_add->setChecked(false);
    ui->button_mode_default->setChecked(false);

    switch(new_mode) {
    case CursorMode::READOUT: {
        set_action_checked_without_signals(ui->menu_action_mouse_readout, true);
        set_button_checked_without_signals(ui->button_mode_default, true);
        break;
    }
    case CursorMode::SELECTION_ADD: {
        set_action_checked_without_signals(ui->menu_action_mouse_selects, true);
        set_button_checked_without_signals(ui->button_mode_select_add, true);
        break;
    }
    case CursorMode::SELECTION_REMOVE: {
        set_action_checked_without_signals(ui->menu_action_mouse_deselects, true);
        set_button_checked_without_signals(ui->button_mode_select_rem, true);
        break;
    }
    }
    ui->slider_selection_step->setVisible(new_mode != CursorMode::READOUT);
    ui->label_selection_step->setVisible(new_mode != CursorMode::READOUT);
    m_current_cursor_mode = new_mode;
}

void tsplotter::set_plot_view_mode(TSPlotterPlotViewOption new_mode) {
    if(new_mode != m_plot_viewing_mode) {
        m_plot_viewing_mode = new_mode;
        recreate_plots();
    }
    switch(m_plot_viewing_mode) {
    case TSPlotterPlotViewOption::OVERLAY: {
        set_button_checked_without_signals(ui->button_plotmode_overlay, true);
        set_button_checked_without_signals(ui->button_plotmode_separate, false);
        set_button_checked_without_signals(ui->button_plotmode_ptest, false);
        set_button_checked_without_signals(ui->button_plotmode_noise, false);
        break;
    }
    case TSPlotterPlotViewOption::OVERLAY_PARALLEL: {
        set_button_checked_without_signals(ui->button_plotmode_overlay, false);
        set_button_checked_without_signals(ui->button_plotmode_separate, false);
        set_button_checked_without_signals(ui->button_plotmode_noise, false);
        set_button_checked_without_signals(ui->button_plotmode_ptest, true);
        break;
    }
    case TSPlotterPlotViewOption::OVERLAY_ALL: {
        set_button_checked_without_signals(ui->button_plotmode_overlay, false);
        set_button_checked_without_signals(ui->button_plotmode_separate, false);
        set_button_checked_without_signals(ui->button_plotmode_ptest, false);
        set_button_checked_without_signals(ui->button_plotmode_noise, true);

        break;
    }
    case TSPlotterPlotViewOption::SEPARATE: {
        set_button_checked_without_signals(ui->button_plotmode_overlay, false);
        set_button_checked_without_signals(ui->button_plotmode_separate, true);
        set_button_checked_without_signals(ui->button_plotmode_ptest, false);
        set_button_checked_without_signals(ui->button_plotmode_noise, false);

        break;
    }
    }
}

void tsplotter::set_plot_add_mode(TSPlotterPlotAddingOption new_mode) {
    m_plot_adding_mode = new_mode;
    switch(m_plot_adding_mode) {
    case TSPlotterPlotAddingOption::ADD: {
        set_button_checked_without_signals(ui->button_plotaddingmode_add, true);
        set_button_checked_without_signals(ui->button_plotaddingmode_replace, false);
        break;
    }
    case TSPlotterPlotAddingOption::REPLACE: {
        set_button_checked_without_signals(ui->button_plotaddingmode_add, false);
        set_button_checked_without_signals(ui->button_plotaddingmode_replace, true);
        break;
    }
    }
}

void tsplotter::set_yaxis_range_for_plot(TSPlotterPlot * plot) {
    MinMax & ex = m_minmax_values[TSPlotType::EX];
    MinMax & ey = m_minmax_values[TSPlotType::EY];
    MinMax & hx = m_minmax_values[TSPlotType::HX];
    MinMax & hy = m_minmax_values[TSPlotType::HY];
    MinMax & hz = m_minmax_values[TSPlotType::HZ];
    MinMax e(std::min(ex.min, ey.min),      std::max(ex.max, ey.max));
    MinMax hxhy(std::min(hx.min, hy.min),   std::max(hx.max, hy.max));
    MinMax h(std::min(hxhy.min, hz.min),    std::max(hxhy.max, hz.max));
    MinMax a(std::min(h.min, e.min),        std::max(h.max, e.max));


    bool set = false;
    if(plot->plot_type() == TSPlotType::EX) {
        if(ui->button_scale_allE->isChecked())          {plot->yAxis->setRange(e.min, e.max); set = true;}
        else if(ui->button_scale_allEx->isChecked())    {plot->yAxis->setRange(ex.min, ex.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}
    } else if(plot->plot_type() == TSPlotType::EY) {
        if(ui->button_scale_allE->isChecked())          {plot->yAxis->setRange(e.min, e.max); set = true;}
        else if(ui->button_scale_allEy->isChecked())    {plot->yAxis->setRange(ey.min, ey.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    } else if(plot->plot_type() == TSPlotType::HX) {
        if(ui->button_scale_allH->isChecked())          {plot->yAxis->setRange(h.min, h.max); set = true;}
        else if(ui->button_scale_allHx->isChecked())    {plot->yAxis->setRange(hx.min, hx.max); set = true;}
        else if(ui->button_scale_allHxHy->isChecked())  {plot->yAxis->setRange(hxhy.min, hxhy.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    } else if(plot->plot_type() == TSPlotType::HY) {
        if(ui->button_scale_allH->isChecked())          {plot->yAxis->setRange(h.min, h.max); set = true;}
        else if(ui->button_scale_allHy->isChecked())    {plot->yAxis->setRange(hy.min, hy.max); set = true;}
        else if(ui->button_scale_allHxHy->isChecked())  {plot->yAxis->setRange(hxhy.min, hxhy.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    } else if(plot->plot_type() == TSPlotType::HZ) {
        if(ui->button_scale_allH->isChecked())          {plot->yAxis->setRange(h.min, h.max); set = true;}
        else if(ui->button_scale_allHz->isChecked())    {plot->yAxis->setRange(hz.min, hz.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    }


    if(!set) {
        MinMax & minmax = m_minmax_p_values[plot];
        plot->yAxis->setRange(minmax.min, minmax.max);
    }
}

void tsplotter::set_yaxis_range_for_spcplot(TSPlotterPlot *plot)
{

    MinMax & ex = m_minmax_spc_values[TSPlotType::EX];
    MinMax & ey = m_minmax_spc_values[TSPlotType::EY];
    MinMax & hx = m_minmax_spc_values[TSPlotType::HX];
    MinMax & hy = m_minmax_spc_values[TSPlotType::HY];
    MinMax & hz = m_minmax_spc_values[TSPlotType::HZ];

    ex.min = std::pow(10, std::floor(std::log10(ex.min)));
    ex.max = std::pow(10, std::ceil(std::log10(ex.max)));
    ey.min = std::pow(10, std::floor(std::log10(ey.min)));
    ey.max = std::pow(10, std::ceil(std::log10(ey.max)));

    hx.min = std::pow(10, std::floor(std::log10(hx.min)));
    hx.max = std::pow(10, std::ceil(std::log10(hx.max)));
    hy.min = std::pow(10, std::floor(std::log10(hy.min)));
    hy.max = std::pow(10, std::ceil(std::log10(hy.max)));

    hz.min = std::pow(10, std::floor(std::log10(hz.min)));
    hz.max = std::pow(10, std::ceil(std::log10(hz.max)));

    MinMax e(std::min(ex.min, ey.min),      std::max(ex.max, ey.max));
    MinMax hxhy(std::min(hx.min, hy.min),   std::max(hx.max, hy.max));
    MinMax h(std::min(hxhy.min, hz.min),    std::max(hxhy.max, hz.max));
    MinMax a(std::min(h.min, e.min),        std::max(h.max, e.max));


    bool set = false;

    if (ui->button_scale_all->isChecked())      {
        plot->yAxis->setRange(a.min, a.max);
        std::cerr << a.min << "  " << a.max << std::endl;
        return;
    }

    else if(plot->plot_type() == TSPlotType::EX) {
        if(ui->button_scale_allE->isChecked())          {plot->yAxis->setRange(e.min, e.max); set = true;}
        else if(ui->button_scale_allEx->isChecked())    {plot->yAxis->setRange(ex.min, ex.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}
    } else if(plot->plot_type() == TSPlotType::EY) {
        if(ui->button_scale_allE->isChecked())          {plot->yAxis->setRange(e.min, e.max); set = true;}
        else if(ui->button_scale_allEy->isChecked())    {plot->yAxis->setRange(ey.min, ey.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    } else if(plot->plot_type() == TSPlotType::HX) {
        if(ui->button_scale_allH->isChecked())          {plot->yAxis->setRange(h.min, h.max); set = true;}
        else if(ui->button_scale_allHx->isChecked())    {plot->yAxis->setRange(hx.min, hx.max); set = true;}
        else if(ui->button_scale_allHxHy->isChecked())  {plot->yAxis->setRange(hxhy.min, hxhy.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    } else if(plot->plot_type() == TSPlotType::HY) {
        if(ui->button_scale_allH->isChecked())          {plot->yAxis->setRange(h.min, h.max); set = true;}
        else if(ui->button_scale_allHy->isChecked())    {plot->yAxis->setRange(hy.min, hy.max); set = true;}
        else if(ui->button_scale_allHxHy->isChecked())  {plot->yAxis->setRange(hxhy.min, hxhy.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    } else if(plot->plot_type() == TSPlotType::HZ) {
        if(ui->button_scale_allH->isChecked())          {plot->yAxis->setRange(h.min, h.max); set = true;}
        else if(ui->button_scale_allHz->isChecked())    {plot->yAxis->setRange(hz.min, hz.max); set = true;}
        else if(ui->button_scale_all->isChecked())      {plot->yAxis->setRange(a.min, a.max); set = true;}

    }




    if(!set) {
        MinMax & minmax = m_minmax_p_spc_values[plot];
        plot->yAxis->setRange(minmax.min, minmax.max);
    }
}


int tsplotter::visible_spectra_plots() {
    int ret = 0;
    std::for_each(m_ui_spcplots.begin(), m_ui_spcplots.end(), [&](auto & plot) { ret += plot->isVisible() ? 1 : 0; });
    return ret;
}

int tsplotter::visible_timeseries_plots() {
    int ret = 0;
    std::for_each(m_ui_tsplots.begin(), m_ui_tsplots.end(), [&](auto & plot) { ret += plot->isVisible() ? 1 : 0; });
    return ret;
}

bool tsplotter::spectra_is_visible() {
    return visible_spectra_plots() > 0;
}

void tsplotter::enable_userinput(bool value) {
    ui->frame_left->setEnabled(value);
    ui->frame_right->setEnabled(value);
    ui->menuBar->setEnabled(value);
    ui->widget_head->setEnabled(value);
}



void tsplotter::slot_general_msg(const QString &sender, const QString &component, const QString &message)
{
    QMessageBox::warning(this, sender + " " + component, message);
}


void tsplotter::on_button_scale_allMan_clicked() {
    bool checked = ui->button_scale_allMan->isChecked();
    ui->button_scale_menu->setVisible(checked);
    if(checked) {
        set_button_checked_without_signals(ui->button_scale_allE, false);
        set_button_checked_without_signals(ui->button_scale_allEx, false);
        set_button_checked_without_signals(ui->button_scale_allEy, false);
        set_button_checked_without_signals(ui->button_scale_allH, false);
        set_button_checked_without_signals(ui->button_scale_allHx, false);
        set_button_checked_without_signals(ui->button_scale_allHy, false);
        set_button_checked_without_signals(ui->button_scale_allHz, false);
        set_button_checked_without_signals(ui->button_scale_allHxHy, false);
    }
    for(auto plot : m_ui_tsplots) {
        plot->setInteraction(QCP::Interaction::iRangeZoom, checked);
        plot->setInteraction(QCP::Interaction::iRangeDrag, checked);
    }
    qDebug() << QString("on_button_scale_allMan_clicked() - not implemented yet");
}

void tsplotter::on_menu_action_export_ascii_triggered() {
    bool is_cal = false;
    if (ui->button_action_b_calibrate->isDown()) is_cal = true;

    this->export_ascii_window(false, is_cal);

    //    emit export_ascii(static_cast<uint64_t>(m_current_window_length),
    //                      m_current_sample_position,
    //                      false, is_cal);
}

void tsplotter::on_menu_action_export_isotime_ascii_triggered() {
    bool is_cal = false;
    if (ui->button_action_b_calibrate->isDown()) is_cal = true;

    this->export_ascii_window(true, is_cal);

    //    emit export_ascii(static_cast<uint64_t>(m_current_window_length),
    //                      m_current_sample_position,
    //                      true, is_cal);
}

void tsplotter::load_selections() {
    for(auto & atsfile : m_mcdata->in_atsfiles) {
        auto atsit = m_plot_infos.find(atsfile.get());
        if(atsit != m_plot_infos.end()) {
            atsfile->open_atm(1);
            auto & plotinfo = atsit->second;
            auto & b = atsfile->atm->b;

            bool in_interval = false;
            int rem = 0;

            for(int i = 0; i < b.size(); ++i) {
                if(b.at(i) && !in_interval) {
                    in_interval = true;
                    rem = i;
                } else if(in_interval && !b.at(i)) {
                    in_interval = false;
                    plotinfo.plot_ts->add_selection_interval(rem, i - 1);
                }
            }
        }
    }
}

void tsplotter::save_selections() {
    for(auto & atsfile : m_mcdata->in_atsfiles) {
        auto atsit = m_plot_infos.find(atsfile.get());
        if(atsit != m_plot_infos.end()) {
            auto offset = static_cast<int64_t>(atsfile->get_start_datetime().samples_to(m_datetime_minimum));
            atsfile->open_atm(1);
            auto & plotinfo = atsit->second;
            auto const & intervals = plotinfo.plot_ts->get_intervals();
            auto & atmfile = atsfile->atm;
            atmfile->b.fill(false);
            for(auto & iterator : intervals) {
                auto & interval = iterator.first;
                atmfile->b.fill(true,
                                static_cast<int>(interval.begin + offset),
                                static_cast<int>(interval.end + offset));
            }
            atmfile->write_all();
        }
    }
}

void tsplotter::quit_operation() {
    if(m_selections_changed) {
        auto answer = QMessageBox::question(nullptr, selection_info_changed, selection_save_question,
                                            QMessageBox::No | QMessageBox::Yes | QMessageBox::Cancel);
        if(answer == QMessageBox::Yes) {
            save_selections();
            exit(0);
        } else if(answer == QMessageBox::No) {
            exit(0);
        }
    } else {
        exit(0);
    }
}

void tsplotter::on_button_action_b_stackall_clicked() {
    m_minmax_values.clear();
    m_minmax_p_values.clear();

    m_minmax_spc_values.clear();
    m_minmax_p_spc_values.clear();

    enable_userinput(false);
    this->m_mcdata->stack_all_tsplotter_start();
}

void tsplotter::on_menu_action_export_print_triggered() {
    if(!m_ui_tsplots.empty()) {
        auto filename = QFileDialog::getSaveFileName(nullptr, filedialog_save_title,
                                                     QString(), filedialog_png_filter);
        if(!filename.isEmpty()) {
            int output_height = 0;

            for(auto plot : m_ui_tsplots) {
                if(plot->isVisible()) output_height += plot->height();
            }
            for(auto plot : m_ui_spcplots) {
                if(plot->isVisible()) output_height += plot->height();
            }

            QPixmap * output = new QPixmap(m_ui_tsplots[0]->width(),
                    output_height);
            QPoint offset;

            for(auto plot : m_ui_tsplots) {
                if(plot->isVisible()) {
                    plot->render(output, offset);
                    offset.ry() += plot->height();
                }
            }
            for(auto plot : m_ui_spcplots) {
                if(plot->isVisible()) {
                    plot->render(output, offset);
                    offset.ry() += plot->height();
                }
            }
            output->save(filename);
        }
    }
}

void tsplotter::on_menu_action_goto_position_triggered() {
    update_dialog();
    m_gotodialog->show();
}

void tsplot::tsplotter::on_button_debug_clicked() {
    auto i = 0;
    for(auto & iterator : m_minmax_values) {
        qDebug() << int(iterator.first) << " -> " << iterator.second.min << "|" << iterator.second.max;
    }
    for(auto & plot : m_ui_tsplots) {
        qDebug() << "<ts plot " << i++ << ">";
        qDebug() << "xAxis: " << plot->xAxis->range();
        qDebug() << "yAxis: " << plot->yAxis->range();
    }
    for(auto & plot : m_ui_tsplots) {
        plot->replot();
    }
    qDebug() << "<sample window info>";
    qDebug() << "current       : " << m_current_sample_position;
    qDebug() << "maximum       : " << m_max_samples;
    qDebug() << "window length : " << m_current_window_length;

    auto & ticks = m_eqdatetime_ticks->ticks();
    qDebug() << "<ticker>";
    for(auto it = ticks.begin(); it != ticks.end(); ++it) {
        qDebug() << QString("%1|%2").arg(it.key()).arg(it.value());
    }
}

void tsplot::tsplotter::on_slider_selection_step_valueChanged(int value) {
    int new_value = static_cast<int>(std::pow(2, value));
    ui->label_selection_step->setText(QString(label_current_step_size).arg(new_value));
    for(auto & plot : m_ui_tsplots) {
        plot->set_interval_step_size(new_value);
    }
}

void tsplot::tsplotter::on_button_deselect_all_clicked() {
    for(auto & plot : m_ui_tsplots) {
        plot->clear_selection();
        plot->replot();
    }
}

void tsplotter::slot_connect_to_webdav(QString const & address) {
    if(m_webdav_dialog != nullptr) {
        m_webdav_dialog->deleteLater();
    }
    if(m_webdav_timer != nullptr) {
        m_webdav_timer->deleteLater();
    }

    m_webdav_dialog = new QWebDavDialog(address, this);
    m_webdav_timer = new QTimer(this);

    connect(m_webdav_dialog, &QWebDavDialog::files_selected, this, &tsplotter::slot_webdav_files_received);
    connect(m_webdav_timer, &QTimer::timeout, this, &tsplotter::slot_webdav_timer_timeout);
    connect(m_webdav_dialog->networkManager(), &ATSNetworkManager::atsHeadersReceived, this, &tsplotter::slot_network_headers_received);
    connect(m_webdav_dialog->networkManager(), &ATSNetworkManager::atsDataReceived, this, &tsplotter::slot_network_new_data_received);

    m_webdav_timer->start(WEBDAV_REQUEST_INTERVAL);
    m_webdav_dialog->show();
}

void tsplotter::slot_webdav_files_received(QStringList const & files, bool complete_files) {
    std::cout << "dialog files received: " << std::endl;
    for(auto const & file : files) {
        std::cout << file.toStdString() << std::endl;
    }

    m_file_sizes.clear();
    m_files_ready.clear();
    m_initial_file_sizes.clear();
    m_webdav_files = files;
    m_complete_files = complete_files;
    m_local_files.clear();

    m_webdav_dialog->networkManager()->execGetATSHeadersRequest(files);
}

void tsplotter::slot_network_headers_received(QVector<QPair<QString, QByteArray>> const & headers) {
    std::cout << "network headers received" << std::endl;

    auto uuid = QUuid::createUuid().toString();
    for(auto header : headers) {
        auto * ptr = reinterpret_cast<ATSHeader_80 *>(header.second.data());

        if(ptr->uiHeaderLength == 1024) {
            uint64_t samples = ptr->uiSamples == UINT_MAX ? ptr->uiSamples64bit : ptr->uiSamples;

            // TODO: later
            if(ptr->rSampleFreq < samples) {
                ptr->uiSamples = (uint32_t(ptr->rSampleFreq));
            }
            ptr->uiSamples = 0;


            auto writable_dir = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppLocalDataLocation);
            auto target_dir = QString("%1/%2/%3/%4/meas_YYYY-MM-DD_hh-mm-ss/").arg(writable_dir, "streamed_data", uuid, "Site_1");

            QDir dir(target_dir);
            dir.mkpath(target_dir);

            // create an ats header (just for the filename)
            auto new_ats_header = new atsheader(QFileInfo(target_dir), QByteArray(header.second));

            // ignore everything and overwrite it with the actual header data after receiving it

            m_local_files[header.first] = new_ats_header->absoluteFilePath();

            {
                QFile f(new_ats_header->absoluteFilePath());
                f.open(QFile::WriteOnly);
                QDataStream ds(&f);
                ds.writeRawData(header.second.data(), 1024);
            }

            this->fileinfos.push_back(QFileInfo(new_ats_header->absoluteFilePath()));

            delete new_ats_header;
        }

    }
}

void tsplotter::slot_network_new_data_received(QVector<QPair<QString, std::vector<int32_t>>> const & data) {
    std::cout << "received new data from [" << data.size() << "] files" << std::endl;
    for(auto it = data.begin(); it != data.end(); ++it) {
        auto & uri = it->first;
        auto & vector = it->second;

        if(m_local_files.contains(uri)) {
            auto & local_file = m_local_files[uri];
            append_data_to_ats_file(local_file, vector);
        } else {
            std::cout << "that should not happen: " << uri.toStdString() << std::endl;
        }
    }

    this->open_files();
}

void tsplotter::append_data_to_ats_file(const QString &file_name, const std::vector<int32_t> &data) {
    // TODO: replace this with atsfile->append_int32 method
    QFile file(file_name);
    file.open(QFile::ReadWrite);
    QDataStream ds(&file);
    ds.device()->seek(4);
    uint32_t test;
    ds.readRawData((char*)&test, 4);
    ds.device()->seek(4);
    test += data.size();
    ds.writeRawData((char*)&test, 4);
    ds.device()->seek(file.size());
    for(auto & dat : data) {
        ds << dat;
    }

    file.close();
}

void tsplotter::slot_webdav_timer_timeout() {
    for(auto & file : m_webdav_files) {
        m_webdav_dialog->networkManager()->getContentLength(file, [file, this](quint64 length) {
            if((!m_file_sizes.contains(file) || m_file_sizes[file] != length)) {
                if(!m_initial_file_sizes.contains(file)) {
                    m_initial_file_sizes[file] = length;
                    m_file_sizes[file] = length;
                    m_files_ready[file] = false;
                } else {
                    m_files_ready[file] = true;
                }

                m_last_file_size_received[file] = m_file_sizes[file];
                m_file_sizes[file] = length;

                std::cout << QDateTime::currentDateTime().toString().toStdString() << "|" << file.toStdString() << "|" << length << std::endl;
            }
        });
    }

    if(!m_webdav_files.empty()) {
        bool ready = !m_files_ready.isEmpty() && std::find(m_files_ready.begin(), m_files_ready.end(), false) == m_files_ready.end();
        bool sync = std::adjacent_find(m_file_sizes.begin(), m_file_sizes.end(), std::not_equal_to<>()) == m_file_sizes.end();
        if(sync && ready) {
            std::fill(m_files_ready.begin(), m_files_ready.end(), false);

            int begin = m_last_file_size_received.first();
            int end = m_file_sizes.first() - 1;

            std::cout << "synced and ready -> sending requests " << "[" << begin << ";" << end << "]" << std::endl;

            m_webdav_dialog->networkManager()->execSecuredGetRequests(m_webdav_files, begin, end);
        }
    }
}

void tsplotter::slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading) {
    Q_UNUSED(last_reading);
    this->qprgb->setRange(0, int(max_counts));
    this->qprgb->setValue(0);
}

void tsplotter::slot_update_progress_bar(const int counts) {
    this->qprgb->setValue(counts);
    if(counts == this->qprgb->maximum()) {
        this->m_mcdata->stack_all_tsplotter_join();
        enable_userinput(true);
        this->qprgb->setValue(0);
    }
}

void tsplot::tsplotter::on_actionShow_CAL_triggered()
{
    qDebug() << "need a window";
    this->xmlcal->close();
    this->xmlcal->clear(true);

    int i = 0;
    for (auto &file : this->m_mcdata->in_atsfiles) {
        if (file->get_cal() != nullptr) {
            this->xmlcal->slot_set_cal(file->get_cal(), false);
            ++i;
        }
    }

    if (i) this->xmlcal->show();
}

void tsplotter::re_connect()
{
    if (this->m_mcdata != nullptr) this->m_mcdata.reset(nullptr);
    this->m_mcdata = std::make_unique<mc_data>(this);
    connect(m_mcdata.get(), &mc_data::files_open, this, &tsplotter::slot_on_mcdata_files_opened);
    connect(m_mcdata.get(), &mc_data::signal_get_max_counts_and_last, this, &tsplotter::slot_set_max_counts_and_last);
    connect(m_mcdata.get(), &mc_data::signal_counts_per, this, &tsplotter::slot_update_progress_bar);
    connect(m_mcdata.get(), &mc_data::signal_general_msg, this, &tsplotter::slot_general_msg);



}

void tsplot::tsplotter::on_actionConnect_triggered() {
    m_address_dialog->show();
}

void tsplot::tsplotter::on_actionOpen_Files_triggered() {
    QFileDialog dialog(this);
    QStringList filters;
    filters << "ATS Files (*.ats)" << "XML Files (*.xml)";

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setFilter(QDir::Files);
    dialog.setNameFilters(filters);

    dialog.setModal(false);
    dialog.show();

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles();
    if (fileNames.size()) {
        this->fileinfos.clear();
        for (auto &str : fileNames) {
            QFileInfo qfi(str);
            if(qfi.exists() && (qfi.suffix().toLower() == "xml" || qfi.suffix().toLower() == "ats")) {
                this->fileinfos.append(qfi);
            }
        }
    }

    if (fileinfos.size()) this->open_files();
}

void tsplotter::export_ascii_window(bool iso_time, bool iscal)
{
    if (this->m_mcdata == nullptr) return;
    if (!this->m_mcdata->size()) return;

    auto survey =  atsfilename_site_survey(this->m_mcdata->in_atsfiles.at(0)->absoluteFilePath());


    QDir dir;

    if (survey.contains("survey_path") && survey.contains("survey")) {
        dir.setPath(survey.value("survey_path").toString() + "/" + survey.value("survey").toString() + "/" + "dump");
        if (!dir.exists()) {
            dir.setPath(this->m_mcdata->in_atsfiles.at(0)->absolutePath());
        }
    }
    else dir.setPath(this->m_mcdata->in_atsfiles.at(0)->absolutePath());

    auto dialog = new QFileDialog(this, "save ascii", dir.absolutePath());
    //dialog->setFileMode(QFileDialog::DirectoryOnly);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);

    if (dialog->exec()) {
        dir.setPath(dialog->selectedFiles().at(0));
    }

    qDebug()  << dir.absolutePath();
    std::vector<std::future<quint64>> futures;
    for (auto &tsd : this->m_mcdata->in_atsfiles) {
        qDebug() << "saving ascii " << tsd->baseName();
        qDebug() << dir.absolutePath();
        qDebug() << m_datetime_minimum.ISO_date_time_frac(m_current_sample_position) << m_datetime_minimum.ISO_date_time_frac(m_current_sample_position + m_current_window_length);
        QString file_name(dir.absolutePath() + "/" + tsd->baseName() + "_" + m_datetime_minimum.ISO_date_time_frac(m_current_sample_position) + "_" + m_datetime_minimum.ISO_date_time_frac(m_current_sample_position + m_current_window_length) + ".tsdata");
        // for Windows we must remove ":"
        file_name.replace(QChar(':'), QChar('_'));
        if (iso_time) {
            futures.emplace_back(std::async(&atsfile::dump_tsbuffer, tsd, file_name, m_datetime_minimum, m_current_sample_position, true));
        }
        else {
            futures.emplace_back(std::async(&atsfile::dump_tsbuffer, tsd, file_name, m_datetime_minimum, m_current_sample_position, false));

        }
    }
    for(auto &f : futures) {
        f.get();
    }
    futures.clear();

    for (auto &tsd : this->m_mcdata->in_atsfiles) {
        qDebug() << m_datetime_minimum.ISO_date_time_frac(m_current_sample_position) << m_datetime_minimum.ISO_date_time_frac(m_current_sample_position + m_current_window_length);
        QString file_name(dir.absolutePath() + "/" + tsd->baseName() + "_" + m_datetime_minimum.ISO_date_time_frac(m_current_sample_position) + "_" + m_datetime_minimum.ISO_date_time_frac(m_current_sample_position + m_current_window_length) + ".spcdata");
        // for Windows we must remove ":"
        file_name.replace(QChar(':'), QChar('_'));
        futures.emplace_back(std::async(&atsfile::dump_spc_tsplotter, tsd, file_name));
        //tsd->dump_spc_tsplotter(file_name);
    }

    for(auto &f : futures) {
        f.get();
    }
    futures.clear();

    for (auto &tsd : this->m_mcdata->in_atsfiles) {
        qDebug() << m_datetime_minimum.ISO_date_time_frac(m_current_sample_position) << m_datetime_minimum.ISO_date_time_frac(m_current_sample_position + m_current_window_length);
        QString file_name(dir.absolutePath() + "/" + tsd->baseName() + "_" + m_datetime_minimum.ISO_date_time_frac(m_current_sample_position)+ ".stacked_spcdata");
        // for Windows we must remove ":"
        file_name.replace(QChar(':'), QChar('_'));
        futures.emplace_back(std::async(&atsfile::dump_spc_tsplotter_stacked, tsd, file_name));
        //tsd->dump_spc_tsplotter(file_name);
    }

    for(auto &f : futures) {
        f.get();
    }
    futures.clear();


}

void tsplot::tsplotter::on_actionadd_sys_cal_triggered() {
    this->m_option_system_calibrate = 1;
    reload_current_sample_window(true);
}

void tsplot::tsplotter::on_actionremove_System_Calibration_triggered() {
    this->m_option_system_calibrate = 0;
    reload_current_sample_window(true);
}


void tsplot::tsplotter::on_pushButton_osm_clicked()
{
    if (m_mcdata == nullptr) return;
    if (!m_mcdata->in_atsfiles.size()) return;
    geocoordinates coord;
    coord.set_lat_lon_msec(m_mcdata->in_atsfiles.at(0)->ivalue("latitude"), m_mcdata->in_atsfiles.at(0)->ivalue("longitude"), (long double)(m_mcdata->in_atsfiles.at(0)->dvalue("elevation")/100.));

    long double lat_rad, lon_rad, elev;
    coord.coordinates(lat_rad,lon_rad, elev);
    // site_name
    lat_rad = lat_rad / M_PI * 180.;
    lon_rad = lon_rad / M_PI * 180.;
    QUrl url;
    //https://www.j-berkemeier.de/osm_marker.html?lat=52.518493,lon=13.376026,info=Reichstagsgeb%C3%A4ude;lat=52.516195,lon=13.377442,info=Brandenburger%20Tor;lat=52.519981,lon=13.36916,info=Bundeskanzleramt
    url.setUrl("https://www.openstreetmap.org/?mlat=" + QString::number(lat_rad, 'g') + "&mlon=" + QString::number(lon_rad, 'g'));
    //url.setUrl("https://geo-metronix.de/osm_marker.html?lat=" + QString::number(lat_rad, 'g') + ",lon=" + QString::number(lon_rad, 'g') + ",info=" + this->atsh->svalue("site_name"));

    QDesktopServices::openUrl(url);
}

void tsplot::tsplotter::on_pushButton_gmaps_clicked() {
    if (m_mcdata == nullptr || !m_mcdata->in_atsfiles.size()) return;
    geocoordinates coord;
    coord.set_lat_lon_msec(m_mcdata->in_atsfiles.at(0)->ivalue("latitude"), m_mcdata->in_atsfiles.at(0)->ivalue("longitude"), (long double)(m_mcdata->in_atsfiles.at(0)->dvalue("elevation")/100.));


    long double lat_rad, lon_rad, elev;
    coord.coordinates(lat_rad, lon_rad, elev);
    lat_rad = lat_rad / M_PI * 180.;
    lon_rad = lon_rad / M_PI * 180.;
    QUrl url;
    // https://www.google.com/maps/search/?api=1&query=47.5951518,-122.3316393
    url.setUrl("https://www.google.com/maps/search/?api=1&query=" +  QString::number(lat_rad, 'g') + "," + QString::number(lon_rad, 'g'));
    QDesktopServices::openUrl(url);
}

