#include "spectra_plotter.h"
#include "ui_spectra_plotter.h"

static const char * prop_frequency = "hz";
static const char * prop_count = "counter";
static constexpr double plotMarginPercentage = 0.00125;

spectra_plotter::spectra_plotter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::spectra_plotter)
{
    ui->setupUi(this);

    freq_button_group = new QButtonGroup(this);

#ifndef QT_DEBUG
    ui->button_debug_reload->hide();
    ui->button_debug_reset->hide();
    ui->button_debug_load_data->hide();
#endif

    initialize_internal_plot_structs();
    initialize_selectors();

    progress_dialog = new ProgressDialog(this, "importing data...");
    progress_dialog->hide();
}



void spectra_plotter::remove_selection(size_t graph_id, const std::unordered_set<size_t> & indices) {
    if(indices.size() > 0) {
        remove_selection_ignore_history(graph_id, indices);
        push_to_undo_stack(graph_id, SpecPlotOperation::REMOVED_FROM_SELECTION, indices);
    }
}

void spectra_plotter::add_selection(size_t graph_id, const std::unordered_set<size_t> & indices) {
    if(indices.size() > 0) {
        add_selection_ignore_history(graph_id, indices);
        push_to_undo_stack(graph_id, SpecPlotOperation::ADDED_TO_SELECTION, indices);
    }
}

void spectra_plotter::remove_selection_ignore_history(size_t graph_id, const std::unordered_set<size_t> & indices) {
    auto iterator = selectiondata_copy.begin();
    for(size_t i = 0; i < graph_id; ++i) ++iterator;
    std::vector<std::vector<bool>> & target = iterator->second;

    if(indices.size() > 0) {
        auto sel = ui->plot_xx_rhoa->remove_and_get_selection(graph_id, indices);

        for(size_t i = 0; i < plot_widgets_all.size(); ++i) {
            for(size_t j = 0; j < plot_widgets_all[i].size(); ++j) {
                if(i > 0 || j > 0) {
                    plot_widgets_all[i][j]->set_selection(graph_id, sel);
                }
            }
        }

        for(size_t i = 0; i < target.size(); ++i) {
            for(size_t index : indices) {
                target[i][index] = AdditionalPlotData::s_mtdata_deselected;
            }
        }

        replot_current_tab();
    }
}

void spectra_plotter::add_selection_ignore_history(size_t graph_id, const std::unordered_set<size_t> & indices) {
    auto iterator = selectiondata_copy.begin();
    for(size_t i = 0; i < graph_id; ++i) ++iterator;
    std::vector<std::vector<bool>> & target = iterator->second;
    if(indices.size() > 0) {
        auto sel = ui->plot_xx_rhoa->add_and_get_selection(graph_id, indices);

        for(size_t i = 0; i < plot_widgets_all.size(); ++i) {
            for(size_t j = 0; j < plot_widgets_all[i].size(); ++j) {
                if(i > 0 || j > 0) {
                    plot_widgets_all[i][j]->set_selection(graph_id, sel);
                }
            }
        }

        for(size_t i = 0; i < target.size(); ++i) {
            for(size_t index : indices) {
                target[i][index] = AdditionalPlotData::s_mtdata_selected;
            }
        }

        replot_current_tab();
    }
}

void spectra_plotter::set_selection_from_copy_vector() {
    size_t current_graph_id = 0;

    progress_dialog->setWindowTitle("importing data...");
    progress_dialog->set_min_max(0, static_cast<int>(this->mtsite->z.size()));
    progress_dialog->set_progress(0);
    progress_dialog->show();

    for(auto & iterator : frequency_buttons) {
        auto const frequency = iterator->property(prop_frequency).toDouble();
        std::cout << "updating frequency " << doublefreq2string_unit(frequency).toStdString() << " [" << frequency << " Hz]" << std::endl;
        progress_dialog->set_progress_text(QString("updating %1").arg(doublefreq2string_unit(frequency)));

        QCPDataSelection dataselection;
        for(size_t i = 0; i < selectiondata_copy[frequency].size(); ++i) {
            auto const & selection = selectiondata_copy[frequency][i];
            for(size_t j = 0; j < selection.size(); ++j) {
                if(selection[j] == AdditionalPlotData::s_mtdata_selected) {
                    auto index = static_cast<int>(j);
                    dataselection.addDataRange(QCPDataRange(index, index + 1), false);
                }
            }
        }
        dataselection.simplify();

        for(auto a : plot_widgets_all) {
            for(auto plot : a) {
                plot->set_selection(current_graph_id, dataselection);
            }
        }

        progress_dialog->inc_progress();
        qApp->processEvents();

        ++current_graph_id;
    }

    progress_dialog->hide();
    clear_history();
}

void spectra_plotter::set_selection_mode_for_all_selectors(GraphWidgetSelectionMode new_mode) {
    for(auto selector : selectors) {
        selector->blockSignals(true);
        selector->set_selection_mode(new_mode);
        selector->blockSignals(false);
    }
}

void spectra_plotter::show_frequency(double frequency, QPushButton * freqButton) {
    if(freqButton == nullptr) {
        for(auto button : frequency_buttons) {
            if(button->property(prop_frequency).toDouble() - frequency <= std::numeric_limits<double>::epsilon()) {
                freqButton = button;
                break;
            }
        }
    }
    if(freqButton != nullptr) {
        int index = freqButton->property(prop_count).toInt();

        freqButton->blockSignals(true);
        freqButton->setChecked(true);
        freqButton->blockSignals(false);

        for(auto plot : plots_single_freq) plot->show_single_graph(frequency);
        ui->slider_frequency->blockSignals(true);
        ui->slider_frequency->setValue(index);
        ui->slider_frequency->blockSignals(false);
        ui->label_current_freq->setText(doublefreq2string_unit(frequency));
    }
}

void spectra_plotter::clear_history() {
    while(!redo_stack.empty()) {
        auto operation = redo_stack.top();
        redo_stack.pop();
        delete operation;
    }
    while(!undo_stack.empty()) {
        auto operation = undo_stack.top();
        undo_stack.pop();
        delete operation;
    }
    ui->button_undo->setEnabled(false);
    ui->button_redo->setEnabled(false);
}

void spectra_plotter::push_to_undo_stack(size_t graph_id,
                                         spectra_plotter::SpecPlotOperation operation,
                                         const std::unordered_set<size_t> & indices) {
    undo_stack.push(new UndoOperation());
    undo_stack.top()->indices = indices;
    undo_stack.top()->operation = operation;
    undo_stack.top()->graph_id = graph_id;
    while(!redo_stack.empty()) {
        auto operation = redo_stack.top();
        redo_stack.pop();
        delete operation;
    }
    ui->button_redo->setEnabled(false);
    ui->button_undo->setEnabled(true);
}

void spectra_plotter::undo() {
    if(!undo_stack.empty()) {
        UndoOperation * operation = undo_stack.top();
        if(operation->operation == SpecPlotOperation::ADDED_TO_SELECTION) {
            remove_selection_ignore_history(operation->graph_id, operation->indices);
        } else if(operation->operation == SpecPlotOperation::REMOVED_FROM_SELECTION) {
            add_selection_ignore_history(operation->graph_id, operation->indices);
        }
        undo_stack.pop();
        redo_stack.push(operation);
        if(undo_stack.empty()) ui->button_undo->setEnabled(false);
        ui->button_redo->setEnabled(true);
    }
}

void spectra_plotter::redo() {
    if(!redo_stack.empty()) {
        UndoOperation * operation = redo_stack.top();
        if(operation->operation == SpecPlotOperation::ADDED_TO_SELECTION) {
            add_selection_ignore_history(operation->graph_id, operation->indices);
        } else if(operation->operation == SpecPlotOperation::REMOVED_FROM_SELECTION) {
            remove_selection_ignore_history(operation->graph_id, operation->indices);
        }
        redo_stack.pop();
        undo_stack.push(operation);
        if(redo_stack.empty()) ui->button_redo->setEnabled(false);
        ui->button_undo->setEnabled(true);
    }
}


spectra_plotter::~spectra_plotter() {
    // for(auto pv : plot_widgets_all) for(auto pw : pv) pw->setParent(nullptr);
    delete ui;
    // if (this->mtsite != nullptr) this->mtsite.reset();
}

void spectra_plotter::set_mt_site(std::shared_ptr<mt_site> mtsite) {
    this->mtsite = mtsite;

    progress_dialog->setWindowTitle("importing data...");
    progress_dialog->set_min_max(0, static_cast<int>(this->mtsite->z.size()));
    progress_dialog->set_progress(0);
    progress_dialog->show();

    for(auto a : plot_widgets_all) for(auto plot : a) {
        plot->blockSignals(true);
    }

    for(auto & data : this->mtsite->z) {
        progress_dialog->set_progress_text(doublefreq2string_unit(data.f));
        progress_dialog->inc_progress();
        add_frequency_data(data.f, data);
        qApp->processEvents();
    }

    set_selection_from_copy_vector();

    progress_dialog->hide();

    for(auto a : plot_widgets_all) for(auto plot : a) {
        plot->set_adaptive_sampling(false);
        plot->blockSignals(false);
    }

    ui->tab_widget->setCurrentWidget(ui->main_page_xyyx);
    if(!this->mtsite->z.empty()) {
        show_frequency(this->mtsite->z.front().f);
    }
    replot_current_tab();
    clear_history();
}

void spectra_plotter::set_cmd_line(std::shared_ptr<prc_com> cmdline) {
    this->cmdline = cmdline;
}

void spectra_plotter::add_frequency_data(double frequency, const mt_data_t<std::complex<double>> & data) {
    std::cout << "adding frequency data for " << doublefreq2string_unit(frequency).toStdString() << " [" << frequency << " Hz]" << std::endl;

    QPushButton * new_menu_button = new QPushButton(ui->usermenu_left);
    new_menu_button->setProperty(prop_frequency, QVariant(frequency));
    new_menu_button->setProperty(prop_count, QVariant(uint(frequency_buttons.size())));
    new_menu_button->setCheckable(true);

    frequency_buttons.push_back(new_menu_button);
    freq_button_group->addButton(new_menu_button);
#ifdef QT_DEBUG
    new_menu_button->setText(QString("%1/%2").arg(doublefreq2string_unit(frequency)).arg(frequency));
#else
    new_menu_button->setText(doublefreq2string_unit(frequency));
#endif
    connect(new_menu_button, &QPushButton::clicked, this, &spectra_plotter::slot_frequency_button_clicked);

    ui->usermenu_left->layout()->removeItem(ui->spacer_left);
    ui->usermenu_left->layout()->addWidget(new_menu_button);
    ui->usermenu_left->layout()->addItem(ui->spacer_left);
    ui->usermenu_left_top->setMaximumWidth(ui->usermenu_left->width() + 64);

    selectiondata_copy[frequency] = data.b;

    for(size_t i = 0; i < 4; ++i) {
        std::vector<double> data_rhoa(data.size());
        std::vector<double> data_phi(data.size());
        std::vector<double> data_real(data.size());
        std::vector<double> data_imag(data.size());
        std::vector<double> data_indices(data.size());
        std::vector<double> data_freq(data.size(), data.f);

        for(size_t j = 0; j < data.d[i].size(); ++j) {
            data_rhoa[j] = data.rho(i, j);
            data_phi[j] = data.phi_deg(i, j, true);
            data_indices[j] = static_cast<double>(j);
            data_real[j] = data.d[i][j].real();
            data_imag[j] = data.d[i][j].imag();
        }

        plot_widgets_all[0][i]->set_graph_data(data.f, data_freq, data_rhoa, &selectiondata_copy[frequency][i], true, true);
        plot_widgets_all[1][i]->set_graph_data(data.f, data_freq, data_phi, &selectiondata_copy[frequency][i], true, true);
        plot_widgets_all[2][i]->set_graph_data(data.f, data_real, data_imag, &selectiondata_copy[frequency][i], false, true);

        plot_widgets_all[0][i]->rescale_axes_with_margin(plotMarginPercentage);
        plot_widgets_all[1][i]->rescale_axes_with_margin(plotMarginPercentage);

        plot_widgets_all[3][2 * i]->set_graph_data(data.f, data_indices, data_phi, &selectiondata_copy[frequency][i], false, true);
        plot_widgets_all[3][2 * i + 1]->set_graph_data(data.f, data_indices, data_rhoa, &selectiondata_copy[frequency][i], false, true);
    }

    ui->slider_frequency->setRange(1, static_cast<int>(frequency_buttons.size()));
    if(!ui->slider_frequency->isEnabled()) {
        ui->slider_frequency->setEnabled(true);
        ui->slider_frequency->setValue(1);
    }
}

void spectra_plotter::initialize_internal_plot_structs() {
    plot_widgets_all.resize(4);

    plots_phi.insert(plots_phi.begin(), {ui->plot_xx_phi, ui->plot_xy_phi, ui->plot_yx_phi, ui->plot_yy_phi});
    plots_rhoa.insert(plots_rhoa.begin(), {ui->plot_xx_rhoa, ui->plot_xy_rhoa, ui->plot_yx_rhoa, ui->plot_yy_rhoa});
    plots_gauss.insert(plots_gauss.begin(), {ui->plot_xx_gauss, ui->plot_xy_gauss, ui->plot_yx_gauss, ui->plot_yy_gauss});
    plots_time_xx_yy.insert(plots_time_xx_yy.begin(), {ui->plot_time_xx_phi, ui->plot_time_xx_rhoa, ui->plot_time_yy_phi, ui->plot_time_yy_rhoa});
    plots_time_xy_yx.insert(plots_time_xy_yx.begin(), {ui->plot_time_xy_phi, ui->plot_time_xy_rhoa, ui->plot_time_yx_phi, ui->plot_time_yx_rhoa});
    plots_time_phi.insert(plots_time_phi.begin(), {ui->plot_time_xx_phi, ui->plot_time_xy_phi, ui->plot_time_yy_phi, ui->plot_time_yx_phi});
    plots_time_rhoa.insert(plots_time_rhoa.begin(), {ui->plot_time_xx_rhoa, ui->plot_time_xy_rhoa, ui->plot_time_yy_rhoa, ui->plot_time_yx_rhoa});
    plots_single_freq.insert(plots_single_freq.end(), plots_time_xx_yy.begin(), plots_time_xx_yy.end());
    plots_single_freq.insert(plots_single_freq.end(), plots_time_xy_yx.begin(), plots_time_xy_yx.end());
    plots_single_freq.insert(plots_single_freq.end(), plots_gauss.begin(), plots_gauss.end());

    plot_widgets_all[0].insert(plot_widgets_all[0].end(), plots_rhoa.begin(), plots_rhoa.end());
    plot_widgets_all[1].insert(plot_widgets_all[1].end(), plots_phi.begin(), plots_phi.end());
    plot_widgets_all[2].insert(plot_widgets_all[2].end(), plots_gauss.begin(), plots_gauss.end());
    plot_widgets_all[3].insert(plot_widgets_all[3].end(), {ui->plot_time_xx_phi, ui->plot_time_xx_rhoa,
                ui->plot_time_xy_phi, ui->plot_time_xy_rhoa,
                ui->plot_time_yx_phi, ui->plot_time_yx_rhoa,
                ui->plot_time_yy_phi, ui->plot_time_yy_rhoa});

    for(auto phi_plot : plots_phi) {
        QSharedPointer<QCPAxisTickerLog> log_ticker(new QCPAxisTickerLog);
        phi_plot->xAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
        phi_plot->xAxis->setTicker(log_ticker);
        phi_plot->xAxis->setRangeReversed(true);
    }
    for(auto rhoa_plot : plots_rhoa) {
        QSharedPointer<QCPAxisTickerLog> log_ticker(new QCPAxisTickerLog);
        rhoa_plot->xAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
        rhoa_plot->xAxis->setTicker(log_ticker);
        rhoa_plot->yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
        rhoa_plot->yAxis->setTicker(log_ticker);
        rhoa_plot->xAxis->setRangeReversed(true);
    }

    for(auto time_plot_rhoa : plots_time_rhoa) {
        QSharedPointer<QCPAxisTickerLog> log_ticker(new QCPAxisTickerLog);
        time_plot_rhoa->yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
        time_plot_rhoa->yAxis->setTicker(log_ticker);
    }

    // make a margin-group for every tab to align plot axes to each other
    for(size_t i = 0; i < 4; ++i) {
        QCPMarginGroup* group = new QCPMarginGroup(plot_widgets_all[i][0]);
        for(size_t j = 0; j < (i < 3 ? 4 : 8); ++j) {
            plot_widgets_all[i][j]->axisRect(0)->setMarginGroup(QCP::MarginSide::msAll, group);
        }
    }

    // allow only x-axis drag/zoom for time-plots
    for(auto time_plot : plot_widgets_all[3]) {
        time_plot->axisRect(0)->setRangeDrag(Qt::Orientation::Horizontal | Qt::Orientation::Vertical);
        time_plot->axisRect(0)->setRangeZoom(Qt::Orientation::Horizontal | Qt::Orientation::Vertical);
    }

    // connect plot status messages to window status bar //TODO: implement status message
    for(auto a : plot_widgets_all) for(auto plot : a) {
        connect(plot, SIGNAL(statusMessage(QString)), this->statusBar(), SLOT(showMessage(QString)));
    }

    // connect time plots axis behavior with each other
    for(auto plot_xx_yy : plots_time_xx_yy) {
        connect(plot_xx_yy->xAxis, SIGNAL(rangeChanged(QCPRange, QCPRange)),
                this, SLOT(slot_plot_range_changed_xx_yy(QCPRange, QCPRange)));
    }

    for(auto plot_xy_yx : plots_time_xy_yx) {
        connect(plot_xy_yx->xAxis, SIGNAL(rangeChanged(QCPRange, QCPRange)),
                this, SLOT(slot_plot_range_changed_xy_yx(QCPRange, QCPRange)));
    }

}

void spectra_plotter::connect_selector(GraphWidgetSelector * selector) {
    connect(selector, &GraphWidgetSelector::indices_selected, this, &spectra_plotter::add_selection);
    connect(selector, &GraphWidgetSelector::indices_deselected, this, &spectra_plotter::remove_selection);
    connect(selector, &GraphWidgetSelector::selectionModeChanged, this, &spectra_plotter::slot_selector_changed_mod);
}

void spectra_plotter::initialize_selectors() {
    connect_selector(selector_free = new GraphWidgetSelectorFreeHand());
    connect_selector(selector_range = new GraphWidgetSelectorRange());
    connect_selector(selector_ellipse = new GraphWidgetSelectorBrushEllipse());
    connect_selector(selector_rectangle = new GraphWidgetSelectorRectangle());
    selectors.insert(selectors.begin(), {selector_free, selector_range, selector_ellipse, selector_rectangle});

    ui->button_selection_range_orientation->setVisible(false);
    ui->slider_circle_radius->setVisible(false);
    ui->slider_circle_radius->setRange(selector_ellipse->radius_min, selector_ellipse->radius_max);
    ui->slider_circle_radius->setValue(selector_ellipse->radius_default);
}

void spectra_plotter::on_button_group_sel_modifier_buttonClicked(QAbstractButton * button) {
    if(current_selector != nullptr) {
        if(button == ui->button_selection_mod_add) {
            set_selection_mode_for_all_selectors(GraphWidgetSelectionMode::UNION);
        } else if(button == ui->button_selection_mod_remove) {
            set_selection_mode_for_all_selectors(GraphWidgetSelectionMode::DIFF);
        } else if(button == ui->button_selection_mod_none) {
            set_selection_mode_for_all_selectors(GraphWidgetSelectionMode::NORMAL_MODE);
        }
    }
}

void spectra_plotter::on_button_group_sel_types_buttonClicked(QAbstractButton * button) {
    if(button == ui->button_selection_type_none) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->set_selector(nullptr);
        current_selector = nullptr;
    } else if(button == ui->button_selection_type_brush) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->set_selector(selector_ellipse);
        current_selector = selector_ellipse;
    } else if(button == ui->button_selection_type_rectangle) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->set_selector(selector_rectangle);
        current_selector = selector_rectangle;
    } else if(button == ui->button_selection_type_freehand) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->set_selector(selector_free);
        current_selector = selector_free;
    } else if(button == ui->button_selection_type_range) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->set_selector(selector_range);
        current_selector = selector_range;
    }
}

void spectra_plotter::slot_selector_changed_mod(GraphWidgetSelectionMode new_mode) {
    if(new_mode == GraphWidgetSelectionMode::DIFF) {
        set_selection_mode_for_all_selectors(GraphWidgetSelectionMode::DIFF);
        ui->button_selection_mod_remove->setChecked(true);
    } else if(new_mode == GraphWidgetSelectionMode::UNION) {
        set_selection_mode_for_all_selectors(GraphWidgetSelectionMode::UNION);
        ui->button_selection_mod_add->setChecked(true);
    } else if(new_mode == GraphWidgetSelectionMode::NORMAL_MODE) {
        set_selection_mode_for_all_selectors(GraphWidgetSelectionMode::NORMAL_MODE);
        ui->button_selection_mod_none->setChecked(true);
    }
}

static void set_plot_x_axis_range_without_emitting(PlotWidget * plot, const QCPRange & range) {
    plot->blockSignals(true);
    plot->xAxis->setRange(range);
    plot->replot();
    plot->blockSignals(false);
}

void spectra_plotter::slot_plot_range_changed_xy_yx(const QCPRange & newRange, const QCPRange & oldRange) {
    Q_UNUSED(oldRange);
    for(auto plot : plots_time_xy_yx) {
        set_plot_x_axis_range_without_emitting(plot, newRange);
    }
}

void spectra_plotter::slot_plot_range_changed_xx_yy(const QCPRange & newRange, const QCPRange & oldRange) {
    Q_UNUSED(oldRange);
    for(auto plot : plots_time_xx_yy) {
        set_plot_x_axis_range_without_emitting(plot, newRange);
    }
}

void spectra_plotter::slot_frequency_button_clicked() {
    auto * freqButton = reinterpret_cast<QPushButton*>(sender());
    double frequency = freqButton->property(prop_frequency).toDouble();
    show_frequency(frequency, freqButton);
}

void spectra_plotter::on_slider_frequency_valueChanged(int value) {
    auto uValue = static_cast<size_t>(value);
    if(uValue < frequency_buttons.size()) {
        frequency_buttons[uValue]->click();
    }
}

void spectra_plotter::load_file(const QString & filename) {
    QFileInfo qfi(filename);
    if(qfi.exists()) {
        this->mtsite = std::make_shared<mt_site>(4, mttype::mt, this);
        this->mtsite->read_bin_spectra(qfi);
        this->mtsite->calc_z();
        set_mt_site(this->mtsite);
    }
}

void spectra_plotter::slot_replot() {

    // data size must be the same
    if (this->selectiondata_copy.size() != this->mtsite->size()) return;

    // no changes in order - so I can rebuild ?
    size_t graphid = 0;

    // re-read the booleans
    for (auto &data : this->mtsite->z) {
        this->selectiondata_copy[data.f] = data.b;
    }

    // overwrite booleans from this->mtsite->z.b and clear undo-redo stack
    set_selection_from_copy_vector();

    // update the plots
    replot_current_tab();
}


void spectra_plotter::replot_tab(int index) {
    if(index == 0) {
        ui->plot_xx_rhoa->replot();
        ui->plot_xy_rhoa->replot();
        ui->plot_yx_rhoa->replot();
        ui->plot_yy_rhoa->replot();
    } else if(index == 1) {
        ui->plot_xx_phi->replot();
        ui->plot_xy_phi->replot();
        ui->plot_yx_phi->replot();
        ui->plot_yy_phi->replot();
    } else if(index == 2) {
        ui->plot_xx_gauss->replot();
        ui->plot_xy_gauss->replot();
        ui->plot_yx_gauss->replot();
        ui->plot_yy_gauss->replot();
    } else if(index == 3) {
        ui->plot_time_xy_phi->replot();
        ui->plot_time_yx_phi->replot();
        ui->plot_time_xy_rhoa->replot();
        ui->plot_time_yx_rhoa->replot();
    } else if(index == 4) {
        ui->plot_time_xx_phi->replot();
        ui->plot_time_yy_phi->replot();
        ui->plot_time_yy_rhoa->replot();
        ui->plot_time_xx_rhoa->replot();
    }
}

void spectra_plotter::replot_current_tab() {
    replot_tab(ui->tab_widget->currentIndex());
}

void spectra_plotter::reset_everything() {
    clear_history();

    ui->slider_frequency->setEnabled(false);
    ui->slider_frequency->setValue(0);
    ui->slider_frequency->setRange(0, 0);
    ui->label_current_freq->clear();
    for(auto button : frequency_buttons) {
        button->deleteLater();
    }
    frequency_buttons.clear();
    selectiondata_copy.clear();
    for(auto a : plot_widgets_all) for(auto plot_widget : a) {
        plot_widget->clear_all_plots();
        plot_widget->replot();
    }
    mtsite.reset();
}

void spectra_plotter::on_tab_widget_currentChanged(int index) {
    auto tab_wgt = qobject_cast<QTabWidget*>(sender());
    if(tab_wgt != nullptr) {
        if(tab_wgt->currentWidget() == ui->main_page_phi || tab_wgt->currentWidget() == ui->main_page_rhoa) {
            ui->scrollArea->setEnabled(false);
            ui->slider_frequency->setEnabled(false);
            slider_text_memory = ui->label_current_freq->text();
            ui->label_current_freq->setText("all");
        } else {
            ui->scrollArea->setEnabled(true);
            ui->slider_frequency->setEnabled(true);
            ui->label_current_freq->setText(slider_text_memory);
        }
    }
    replot_tab(index);
}

void spectra_plotter::on_button_undo_clicked() {
    undo();
}

void spectra_plotter::on_button_redo_clicked() {
    redo();
}

void spectra_plotter::on_button_selection_range_orientation_clicked() {
    selector_range->toggle_range_axis();
    if(selector_range->range_axis() == GraphWidgetSelectorRange::X_AXIS) {
        ui->button_selection_range_orientation->setText("Range mode: vertical");
    } else if(selector_range->range_axis() == GraphWidgetSelectorRange::Y_AXIS) {
        ui->button_selection_range_orientation->setText("Range mode: horizontal");
    }
}

void spectra_plotter::on_button_selection_type_range_toggled(bool checked) {
    ui->button_selection_range_orientation->setVisible(checked);
}

void spectra_plotter::on_button_selection_type_brush_toggled(bool checked) {
    ui->slider_circle_radius->setVisible(checked);
}

void spectra_plotter::on_slider_circle_radius_valueChanged(int value) {
    selector_ellipse->set_radius(value);
}

void spectra_plotter::on_button_debug_reload_clicked() {
    set_selection_from_copy_vector();
}

void spectra_plotter::on_button_debug_reset_clicked() {
    reset_everything();
}

void spectra_plotter::on_button_process_clicked() {
    if(mtsite != nullptr) {
        progress_dialog->setWindowTitle("processing data...");
        progress_dialog->set_min_max(0, static_cast<int>(this->mtsite->z.size()));
        progress_dialog->set_progress(0);
        progress_dialog->show();

        for(auto & data : mtsite->z) {
            if(selectiondata_copy.find(data.f) != selectiondata_copy.end()) {
                progress_dialog->set_progress_text(doublefreq2string_unit(data.f));
                progress_dialog->inc_progress();

                data.b = selectiondata_copy[data.f];
                qApp->processEvents();
            }
        }

        progress_dialog->hide();
        emit process_button_clicked();
    }
}

void spectra_plotter::on_button_debug_load_data_clicked() {
    if (this->mtsite != nullptr) this->mtsite.reset();
    auto dialog = new QFileDialog(this);
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setModal(false);
    if (this->cmdline != nullptr && cmdline->svalue("survey_basedir") != "empty") dialog->setDirectory(cmdline->svalue("survey_basedir"));

#ifdef yasc
    dialog->setDirectory("D:/projects/Qt/metronix/procmt/ac_spectra_test");
#endif

    dialog->setNameFilter(tr("binary spectra (*.binspectra*)"));
    dialog->show();
    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();
    if (fileNames.size() > 0) {
        load_file(fileNames.at(0));
    }
}
