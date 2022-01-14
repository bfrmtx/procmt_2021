#include "spectra_plotter_pro.h"
#include "ui_spectra_plotter.h"

using namespace ygfxqt;
using namespace ygfx;

static const int ogl_initialization_delay = 1000;
static const char * prop_frequency = "hz";
static const char * prop_count = "counter";

spectra_plotter::spectra_plotter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::spectra_plotter)
{
    ui->setupUi(this);
    this->setLocale(QLocale::c());
    connect(ui->plot_rhoa_xx, &ygfxqt::YMathOpenGLWidget::initialized, this, [this] {
        QTimer::singleShot(ogl_initialization_delay, this, [this] {
            opengl_init = true;
            this->set_mt_site(mtsite);
            ui->cb_performance->setChecked(true);
        });
    });

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

    ui->tab_widget->setCurrentWidget(ui->main_page_rhoa);
    ui->stack_widget->setCurrentWidget(ui->page_first);
}

void spectra_plotter::set_selection_from_mtsite_bool_vector(std::vector<std::vector<bool>> const & b, YMathSelection * target) {
    std::vector<bool> selection;
    selection.reserve(b[0].size());
    for(size_t i = 0; i < b[0].size(); ++i) {
        bool not_to_be_processed = !b[0][i] || !b[1][i] || !b[2][i] || !b[3][i];
        selection.push_back(not_to_be_processed);
    }
    target->replaceSelection(selection);
}

void spectra_plotter::reset_all_selections() {
    if(this->mtsite != nullptr) {
        for(auto & z : this->mtsite->z) {
            if(selection_freq_map.find(z.f) != selection_freq_map.end()) {
                set_selection_from_mtsite_bool_vector(z.b, selection_freq_map[z.f]);
            }
        }
    }
}

void spectra_plotter::set_selection_mode_for_all_selectors(YMathSelectionMode new_mode) {
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

        for(auto plot : plots_single_freq) plot->hideAllPlotsExceptIndex(size_t(index));
        for(auto plot : plots_single_freq) plot->fitScaleToPlot(size_t(index));        
        if(time_plots_range_0_90) {
            for(auto plot_wgt : plot_widgets_time_phi) {
                plot_wgt->setNewYRange(-0.5, 90.5);
            }
        }

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

void spectra_plotter::delete_all_plots() {
    for(auto a : plot_widgets_all) for(auto plot_widget : a) {
        plot_widget->removeAllPlots();
    }
    for(auto plot : plots) {
        delete plot;
    }
    for(auto gfx_vector : gfx_vectors) {
        delete gfx_vector;
    }
    for(auto ram_vector : ram_vectors) {
        delete ram_vector;
    }
    for(auto selection : selections) {
        delete selection;
    }

    selection_map.clear();
    selection_freq_map.clear();
    selections.clear();
    gfx_vectors.clear();
    ram_vectors.clear();
    plots.clear();
}

void spectra_plotter::push_to_undo_stack(YMathSelection * selection,
                                         spectra_plotter::SpecPlotOperation operation,
                                         const std::vector<size_t> & indices) {
    if(!block_stacks) {
        undo_stack.push(new UndoOperation());
        undo_stack.top()->indices = indices;
        undo_stack.top()->operation = operation;
        undo_stack.top()->selection = selection;
        while(!redo_stack.empty()) {
            auto operation = redo_stack.top();
            redo_stack.pop();
            delete operation;
        }
        ui->button_redo->setEnabled(false);
        ui->button_undo->setEnabled(true);
    }
}

void spectra_plotter::undo() {
    if(!undo_stack.empty()) {        
        UndoOperation * operation = undo_stack.top();

        block_stacks = true;
        if(operation->operation == SpecPlotOperation::ADDED_TO_SELECTION) {
            operation->selection->removeFromSelection(operation->indices);
        } else if(operation->operation == SpecPlotOperation::REMOVED_FROM_SELECTION) {
            operation->selection->addToSelection(operation->indices);
        }
        block_stacks = false;

        undo_stack.pop();
        redo_stack.push(operation);
        if(undo_stack.empty()) ui->button_undo->setEnabled(false);
        ui->button_redo->setEnabled(true);

        replot_current_tab();
    }
}

void spectra_plotter::redo() {
    if(!redo_stack.empty()) {
        UndoOperation * operation = redo_stack.top();

        block_stacks = true;
        if(operation->operation == SpecPlotOperation::ADDED_TO_SELECTION) {
            operation->selection->addToSelection(operation->indices);
        } else if(operation->operation == SpecPlotOperation::REMOVED_FROM_SELECTION) {
            operation->selection->removeFromSelection(operation->indices);
        }
        block_stacks = false;

        redo_stack.pop();
        undo_stack.push(operation);
        if(redo_stack.empty()) ui->button_redo->setEnabled(false);
        ui->button_undo->setEnabled(true);

        replot_current_tab();
    }
}


spectra_plotter::~spectra_plotter() {
    qDebug() << "****************** spectra_plotter destructor called";
    delete_all_plots();
    delete ui;
}

void spectra_plotter::set_mt_site(std::shared_ptr<mt_site> mtsite) {
    this->mtsite = mtsite;

    if(mtsite != nullptr) {
        if(opengl_init) {
            progress_dialog->setWindowTitle("importing data...");
            progress_dialog->set_min_max(0, static_cast<int>(this->mtsite->z.size()));
            progress_dialog->set_progress(0);
            progress_dialog->show();

            for(auto a : plot_widgets_all) for(auto plot : a) {
                plot->blockSignals(true);
            }

            size_t counter = 0;
            for(auto & data : this->mtsite->z) {
                progress_dialog->set_progress_text(doublefreq2string_unit(data.f));
                progress_dialog->inc_progress();
                add_frequency_data(data.f, data);
                if(data.d.size() > 0)
                    counter += data.d[0].size();
                qDebug() << QString("%1 data points").arg(counter);
                qApp->processEvents();
            }

            progress_dialog->hide();

            for(auto a : plot_widgets_all) for(auto plot : a) {
                plot->blockSignals(false);
            }

            ui->tab_widget->setCurrentWidget(ui->main_page_xyyx);
            if(!this->mtsite->z.empty()) {
                show_frequency(this->mtsite->z.front().f);
            }

            for(auto a : plot_widgets_all) for(auto plot_widget : a) {
                plot_widget->fitScaleToAllVisiblePlots();
            }

            if(time_plots_range_0_90) {
                for(auto plot_wgt : plot_widgets_time_phi) {
                    plot_wgt->setNewYRange(-0.5, 90.5);
                }
            } else {
                for(auto plot_wgt : plot_widgets_time_phi) {
                    plot_wgt->fitScaleToAllVisiblePlots();
                }
            }

            replot_current_tab();
            clear_history();
        }
    }
}

void spectra_plotter::set_cmd_line(std::shared_ptr<prc_com> cmdline) {
    this->cmdline = cmdline;
}

void spectra_plotter::add_frequency_data(double frequency, const mt_data_t<std::complex<double>> & data) {
#ifdef QT_DEBUG
    std::cout << "adding frequency data for " << doublefreq2string_unit(frequency).toStdString() << " [" << frequency << " Hz]" << std::endl;
#endif

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

    ygfxqt::YGFXQTAssetManager::loadDefaultShader();
    ygfxqt::YGFXQTAssetManager::loadDefaultTextures();
    auto p_shader = ui->cb_performance->isChecked() ?
                        ygfxqt::YGFXQTAssetManager::getShader(ygfxqt::YGFXQTAssetManager::SHADER_PLOT_2D_POINTS_PERF) :
                        ygfxqt::YGFXQTAssetManager::getShader(ygfxqt::YGFXQTAssetManager::SHADER_PLOT_2D_POINTS);
    auto p_texture = ygfxqt::YGFXQTAssetManager::getTexture(ygfxqt::YGFXQTAssetManager::TEXTURE_FILENAME_PLOT_2D_CROSS);

    gfxVector<GLfloat> * fbuffer_freq = new gfxVector<GLfloat>();
    gfxVector<GLfloat> * fbuffer_selection = new gfxVector<GLfloat>();
    std::vector<double> * dbuffer_freq = new std::vector<double>();

    gfx_vectors.insert(gfx_vectors.end(), {fbuffer_freq, fbuffer_selection});
    ram_vectors.push_back(dbuffer_freq);

    auto glsize = static_cast<GLuint>(data.d[0].size());
    double lfreq = std::log10(frequency);
    fbuffer_selection->reserve(glsize);
    fbuffer_freq->reserve(glsize);


    YMathSelection * new_selection = new YMathSelection(data.d[0].size(), this);
    selections.push_back(new_selection);
    selection_freq_map[frequency] = new_selection;

    connect(new_selection, &YMathSelection::addedToSelection, this, [fbuffer_selection, this](std::vector<size_t> const & indices) {
        STOPWATCH("time_lambda_added_to_selection (vram): ");
        for(size_t index : indices) {
            fbuffer_selection->at(GLuint(index)) = 1.0f;
        }

        auto selection = static_cast<YMathSelection*>(sender());
        this->push_to_undo_stack(selection, SpecPlotOperation::ADDED_TO_SELECTION, indices);
    });

    connect(new_selection, &YMathSelection::removedFromSelection, this, [fbuffer_selection, this](std::vector<size_t> const & indices) {
        STOPWATCH("time_lambda_removed_from_selection (vram): ");
        for(size_t index : indices) {
            fbuffer_selection->at(GLuint(index)) = 0.0f;
        }

        auto selection = static_cast<YMathSelection*>(sender());
        this->push_to_undo_stack(selection, SpecPlotOperation::REMOVED_FROM_SELECTION, indices);
    });

    connect(new_selection, &YMathSelection::selectionReplaced, this, [fbuffer_selection](std::vector<bool> const & selection) {
        STOPWATCH("time_lambda_selection_replaced (vram): ");
        for(size_t index = 0; index < selection.size(); ++index) {
            fbuffer_selection->at(GLuint(index)) = selection[index] ? 1.0f : 0.0f;
        }
    });

#ifdef QT_DEBUG
    std::cout << "created selection buffer: " << fbuffer_selection << std::endl;
#endif

    set_selection_from_mtsite_bool_vector(data.b, new_selection);
    for(size_t i = 0; i < data.d[0].size(); ++i) {
        fbuffer_freq->push_back(static_cast<GLfloat>(lfreq));
        dbuffer_freq->push_back(lfreq);
    }

    for(size_t i = 0; i < 4; ++i) {
        gfxVector<GLfloat> * fbuffer_rhoa = new gfxVector<GLfloat>(); fbuffer_rhoa->reserve(glsize);
        gfxVector<GLfloat> * fbuffer_phi = new gfxVector<GLfloat>(); fbuffer_phi->reserve(glsize);
        gfxVector<GLfloat> * fbuffer_indices = new gfxVector<GLfloat>(); fbuffer_indices->reserve(glsize);
        gfxVector<GLfloat> * fbuffer_real = new gfxVector<GLfloat>(); fbuffer_real->reserve(glsize);
        gfxVector<GLfloat> * fbuffer_imag = new gfxVector<GLfloat>(); fbuffer_imag->reserve(glsize);
        std::vector<double> * dbuffer_rhoa = new std::vector<double>(); dbuffer_rhoa->reserve(glsize);
        std::vector<double> * dbuffer_phi = new std::vector<double>(); dbuffer_phi->reserve(glsize);
        std::vector<double> * dbuffer_indices = new std::vector<double>(); dbuffer_indices->reserve(glsize);
        std::vector<double> * dbuffer_real = new std::vector<double>(); dbuffer_real->reserve(glsize);
        std::vector<double> * dbuffer_imag = new std::vector<double>(); dbuffer_imag->reserve(glsize);
        gfx_vectors.insert(gfx_vectors.end(), {fbuffer_rhoa, fbuffer_phi, fbuffer_indices, fbuffer_real, fbuffer_imag});
        ram_vectors.insert(ram_vectors.end(), {dbuffer_rhoa, dbuffer_phi, dbuffer_indices, dbuffer_real, dbuffer_imag});

        double min_rhoa = 1e30;
        double max_rhoa = -1e30;
        double min_phi = 1e30;
        double max_phi = -1e30;
        double min_ind = 1e30;
        double max_ind = -1e30;
        double min_real = 1e30;
        double max_real = -1e30;
        double min_imag = 1e30;
        double max_imag = -1e30;

        for(size_t j = 0; j < data.d[i].size(); ++j) {
            double val_rhoa = std::log10(data.rho(i, j));
            double val_phi = data.phi_deg(i, j, true);
            double val_ind = double(j);
            double val_real = data.d[i][j].real();
            double val_imag = data.d[i][j].imag();

            fbuffer_rhoa->push_back(static_cast<GLfloat>(val_rhoa));
            fbuffer_phi->push_back(static_cast<GLfloat>(val_phi));
            fbuffer_indices->push_back(static_cast<GLfloat>(val_ind));
            fbuffer_real->push_back(static_cast<GLfloat>(val_real));
            fbuffer_imag->push_back(static_cast<GLfloat>(val_imag));

            dbuffer_rhoa->push_back(val_rhoa);
            dbuffer_phi->push_back(val_phi);
            dbuffer_indices->push_back(val_ind);
            dbuffer_real->push_back(val_real);
            dbuffer_imag->push_back(val_imag);

            min_rhoa = std::min(double(fbuffer_rhoa->at(fbuffer_rhoa->size() - 1)), min_rhoa);
            max_rhoa = std::max(double(fbuffer_rhoa->at(fbuffer_rhoa->size() - 1)), max_rhoa);
            min_phi = std::min(double(fbuffer_phi->at(fbuffer_phi->size() - 1)), min_phi);
            max_phi = std::max(double(fbuffer_phi->at(fbuffer_phi->size() - 1)), max_phi);
            min_real = std::min(double(fbuffer_real->at(fbuffer_real->size() - 1)), min_real);
            max_real = std::max(double(fbuffer_real->at(fbuffer_real->size() - 1)), max_real);
            min_imag = std::min(double(fbuffer_imag->at(fbuffer_imag->size() - 1)), min_imag);
            max_imag = std::max(double(fbuffer_imag->at(fbuffer_imag->size() - 1)), max_imag);
        }

        // shrink the buffers, so we don't waste main or video memory
        fbuffer_rhoa->shrink_to_fit();
        fbuffer_phi->shrink_to_fit();
        fbuffer_indices->shrink_to_fit();
        fbuffer_real->shrink_to_fit();
        fbuffer_imag->shrink_to_fit();
        dbuffer_rhoa->shrink_to_fit();
        dbuffer_phi->shrink_to_fit();
        dbuffer_indices->shrink_to_fit();
        dbuffer_real->shrink_to_fit();
        dbuffer_imag->shrink_to_fit();

        min_ind = 0;
        max_ind = data.d[i].size();

        auto range_imag = (max_imag - min_imag < std::numeric_limits<double>::epsilon() ? 10.0 : max_imag - min_imag);
        min_imag -= range_imag * 0.01;
        max_imag += range_imag * 0.01;
        auto range_real = (max_real - min_real < std::numeric_limits<double>::epsilon() ? 10.0 : max_real - min_real);
        min_real -= range_real * 0.01;
        max_real += range_real * 0.01;

        auto range_rhoa = max_rhoa - min_rhoa < std::numeric_limits<double>::epsilon() ? 10.0 : max_rhoa - min_rhoa;
        min_rhoa -= range_rhoa * 0.01; max_rhoa += range_rhoa * 0.01;
        auto range_phi = max_phi - min_phi < std::numeric_limits<double>::epsilon() ? 10.0 : max_phi - min_phi;
        min_phi -= range_phi * 0.01; max_phi += range_phi * 0.01;
        auto range_ind = max_ind - min_ind < std::numeric_limits<double>::epsilon() ? 10.0 : max_ind - min_ind;
        min_ind -= range_ind * 0.01; max_ind += range_ind * 0.01;

        auto plot_gauss = new ygfxqt::YMathGfxPlotSet2D(dbuffer_real, dbuffer_imag,
                              fbuffer_real, fbuffer_imag, fbuffer_selection,
                              p_shader, p_texture);
        plot_gauss->setMinX(min_real)->setMaxX(max_real)->setMinY(min_imag)->setMaxY(max_imag);
        selection_map[plot_gauss] = new_selection;
        plot_widgets_gauss[i]->addPlot(plot_gauss);
        plot_widgets_gauss[i]->update();
        plots.push_back(plot_gauss);


        auto plot_time_phi = new ygfxqt::YMathGfxPlotSet2D(dbuffer_indices, dbuffer_phi,
                                 fbuffer_indices, fbuffer_phi, fbuffer_selection,
                                 p_shader, p_texture);
        plot_time_phi->setMinX(min_ind)->setMaxX(max_ind)->setMinY(min_phi)->setMaxY(max_phi);
        selection_map[plot_time_phi] = new_selection;
        plot_widgets_time_phi[i]->addPlot(plot_time_phi);
        plot_widgets_time_phi[i]->update();
        plots.push_back(plot_time_phi);


        auto plot_time_rhoa = new ygfxqt::YMathGfxPlotSet2D(dbuffer_indices, dbuffer_rhoa,
                                  fbuffer_indices, fbuffer_rhoa, fbuffer_selection,
                                  p_shader, p_texture);
        plot_time_rhoa->setMinX(min_ind)->setMaxX(max_ind)->setMinY(min_rhoa)->setMaxY(max_rhoa);
        selection_map[plot_time_rhoa] = new_selection;
        plot_widgets_time_rhoa[i]->addPlot(plot_time_rhoa);
        plot_widgets_time_rhoa[i]->update();
        plots.push_back(plot_time_rhoa);


        auto plot_phi = new ygfxqt::YMathGfxPlotSet2D(dbuffer_freq, dbuffer_phi,
                            fbuffer_freq, fbuffer_phi, fbuffer_selection,
                            p_shader, p_texture);
        plot_phi->setMinX(lfreq)->setMaxX(lfreq)->setMinY(min_phi)->setMaxY(max_phi);
        selection_map[plot_phi] = new_selection;
        plot_widgets_phi[i]->addPlot(plot_phi);
        plot_widgets_phi[i]->update();
        plots.push_back(plot_phi);


        auto plot_rhoa = new ygfxqt::YMathGfxPlotSet2D(dbuffer_freq, dbuffer_rhoa,
                             fbuffer_freq, fbuffer_rhoa, fbuffer_selection,
                             p_shader, p_texture);
        plot_rhoa->setMinX(lfreq)->setMaxX(lfreq)->setMinY(min_rhoa)->setMaxY(max_rhoa);
        selection_map[plot_rhoa] = new_selection;
        plot_widgets_rhoa[i]->addPlot(plot_rhoa);
        plot_widgets_rhoa[i]->update();
        plots.push_back(plot_rhoa);

    }

    ui->slider_frequency->setRange(1, static_cast<int>(frequency_buttons.size()));
    if(!ui->slider_frequency->isEnabled()) {
        ui->slider_frequency->setEnabled(true);
        ui->slider_frequency->setValue(1);
    }
}

void spectra_plotter::initialize_internal_plot_structs() {
    plot_widgets_all.resize(4);

    plot_widgets_phi.insert(plot_widgets_phi.begin(), {ui->plot_phi_xx, ui->plot_phi_xy, ui->plot_phi_yx, ui->plot_phi_yy});
    plot_widgets_rhoa.insert(plot_widgets_rhoa.begin(), {ui->plot_rhoa_xx, ui->plot_rhoa_xy, ui->plot_rhoa_yx, ui->plot_rhoa_yy});
    plot_widgets_gauss.insert(plot_widgets_gauss.begin(), {ui->plot_fstack_xx, ui->plot_fstack_xy, ui->plot_fstack_yx, ui->plot_fstack_yy});
    plot_widgets_time_xx_yy.insert(plot_widgets_time_xx_yy.begin(), {ui->plot_t_xx_phi, ui->plot_t_xx_rhoa, ui->plot_t_yy_phi, ui->plot_t_yy_rhoa});
    plot_widgets_time_xy_yx.insert(plot_widgets_time_xy_yx.begin(), {ui->plot_t_xy_phi, ui->plot_t_xy_rhoa, ui->plot_t_yx_phi, ui->plot_t_yx_rhoa});
    plot_widgets_time_phi.insert(plot_widgets_time_phi.begin(), {ui->plot_t_xx_phi, ui->plot_t_xy_phi, ui->plot_t_yx_phi, ui->plot_t_yy_phi});
    plot_widgets_time_rhoa.insert(plot_widgets_time_rhoa.begin(), {ui->plot_t_xx_rhoa, ui->plot_t_xy_rhoa, ui->plot_t_yx_rhoa, ui->plot_t_yy_rhoa});
    plots_single_freq.insert(plots_single_freq.end(), plot_widgets_time_xx_yy.begin(), plot_widgets_time_xx_yy.end());
    plots_single_freq.insert(plots_single_freq.end(), plot_widgets_time_xy_yx.begin(), plot_widgets_time_xy_yx.end());
    plots_single_freq.insert(plots_single_freq.end(), plot_widgets_gauss.begin(), plot_widgets_gauss.end());

    plot_widgets_all[0].insert(plot_widgets_all[0].end(), plot_widgets_rhoa.begin(), plot_widgets_rhoa.end());
    plot_widgets_all[1].insert(plot_widgets_all[1].end(), plot_widgets_phi.begin(), plot_widgets_phi.end());
    plot_widgets_all[2].insert(plot_widgets_all[2].end(), plot_widgets_gauss.begin(), plot_widgets_gauss.end());
    plot_widgets_all[3].insert(plot_widgets_all[3].end(), {ui->plot_t_xx_phi, ui->plot_t_xx_rhoa,
                ui->plot_t_xy_phi, ui->plot_t_xy_rhoa,
                ui->plot_t_yx_phi, ui->plot_t_yx_rhoa,
                ui->plot_t_yy_phi, ui->plot_t_yy_rhoa});

    for(auto phi_plot : plot_widgets_phi) {
        phi_plot->setAxesScales(YMathOpenGLWidget::AxisScale::LOGARITHMIC, YMathOpenGLWidget::AxisScale::LINEAR);
    }
    for(auto rhoa_plot : plot_widgets_rhoa) {
        rhoa_plot->setAxesScales(YMathOpenGLWidget::AxisScale::LOGARITHMIC, YMathOpenGLWidget::AxisScale::LOGARITHMIC);
        rhoa_plot->setAxesOrders(YMathOpenGLWidget::AxisOrder::REVERSED, YMathOpenGLWidget::AxisOrder::FORWARD);
    }
    for(auto rhoa_t_plot : plot_widgets_time_rhoa) {
        rhoa_t_plot->setAxesScales(YMathOpenGLWidget::AxisScale::LINEAR, YMathOpenGLWidget::AxisScale::LOGARITHMIC);
    }
    for(auto a : plot_widgets_all) for(auto plot_wgt : a) {
        plot_wgt->setAxesOffsets(45, 45);
    }

    // allow only x-axis drag/zoom for time-plots
    for(auto time_plot_widget : plot_widgets_time_xx_yy) {
        time_plot_widget->setPlotOption(YMathOpenGLWidget::PlotOption::EnableYAxisMove, false);
        time_plot_widget->setPlotOption(YMathOpenGLWidget::PlotOption::EnableYAxisZoom, false);
    }
    for(auto time_plot_widget : plot_widgets_time_xy_yx) {
        time_plot_widget->setPlotOption(YMathOpenGLWidget::PlotOption::EnableYAxisMove, false);
        time_plot_widget->setPlotOption(YMathOpenGLWidget::PlotOption::EnableYAxisZoom, false);
    }

    // connect time plots axis behavior with each other
    for(auto plot_a : plot_widgets_time_xx_yy) {
        for(auto plot_b : plot_widgets_time_xx_yy) {
            if(plot_a != plot_b) {
                connect(plot_a, &YMathOpenGLWidget::xBoundingChanged, plot_b, &YMathOpenGLWidget::setNewXRange);
            }
        }
    }

    for(auto plot_a : plot_widgets_time_xy_yx) {
        for(auto plot_b : plot_widgets_time_xy_yx) {
            if(plot_a != plot_b) {
                connect(plot_a, &YMathOpenGLWidget::xBoundingChanged, plot_b, &YMathOpenGLWidget::setNewXRange);
            }
        }
    }
}

void spectra_plotter::connect_selector(YMathWidgetSelector * selector) {
    connect(selector, &YMathWidgetSelector::selectionOccured, this,
            [this](std::vector<size_t> const & indices, SelectionAction action, YMathGfxPlot * plot) {
        auto selection = selection_map.find(plot);
        if(selection != selection_map.end()) {
            STOPWATCH("time_selection_occured: ");
            if(action == SelectionAction::ADDED_TO_SELECTION) {
                (*selection).second->addToSelection(indices);
            } else if(action == SelectionAction::REMOVED_FROM_SELECTION) {
                (*selection).second->removeFromSelection(indices);
            }
        }
        this->replot_current_tab();
    });
}

void spectra_plotter::initialize_selectors() {
    connect_selector(selector_free = new YMathWidgetSelectorFreeHand());
    connect_selector(selector_range = new YMathWidgetSelectorRange());
    connect_selector(selector_ellipse = new YMathWidgetSelectorEllipse());
    connect_selector(selector_rectangle = new YMathWidgetSelectorRectangle());
    selectors.insert(selectors.begin(), {selector_free, selector_range, selector_ellipse, selector_rectangle});

    ui->button_selection_range_orientation->setVisible(false);
    ui->slider_circle_radius->setVisible(false);
    ui->slider_circle_radius->setRange(selector_ellipse->radius_min, selector_ellipse->radius_max);
    ui->slider_circle_radius->setValue(selector_ellipse->radius_default);
}

void spectra_plotter::on_button_group_sel_modifier_buttonClicked(QAbstractButton * button) {
    if(button == ui->button_selection_mod_add) {
        set_selection_mode_for_all_selectors(YMathSelectionMode::UNION);
    } else if(button == ui->button_selection_mod_remove) {
        set_selection_mode_for_all_selectors(YMathSelectionMode::DIFF);
    } else if(button == ui->button_selection_mod_none) {
        set_selection_mode_for_all_selectors(YMathSelectionMode::NORMAL_MODE);
    }
}

void spectra_plotter::on_button_group_visibility_buttonClicked(QAbstractButton * button) {
    if(button == ui->btn_show_all) {
        for(auto a : plot_widgets_all) for(auto plot : a) plot->setVisible(true);
    } else if(button == ui->btn_show_xx) {
        for(auto plot : plot_widgets_gauss) plot->setVisible(plot == ui->plot_fstack_xx);
        for(auto plot : plot_widgets_phi) plot->setVisible(plot == ui->plot_phi_xx);
        for(auto plot : plot_widgets_rhoa) plot->setVisible(plot == ui->plot_rhoa_xx);
    } else if(button == ui->btn_show_xy) {
        for(auto plot : plot_widgets_gauss) plot->setVisible(plot == ui->plot_fstack_xy);
        for(auto plot : plot_widgets_phi) plot->setVisible(plot == ui->plot_phi_xy);
        for(auto plot : plot_widgets_rhoa) plot->setVisible(plot == ui->plot_rhoa_xy);
    } else if(button == ui->btn_show_yx) {
        for(auto plot : plot_widgets_gauss) plot->setVisible(plot == ui->plot_fstack_yx);
        for(auto plot : plot_widgets_phi) plot->setVisible(plot == ui->plot_phi_yx);
        for(auto plot : plot_widgets_rhoa) plot->setVisible(plot == ui->plot_rhoa_yx);
    } else if(button == ui->btn_show_yy) {
        for(auto plot : plot_widgets_gauss) plot->setVisible(plot == ui->plot_fstack_yy);
        for(auto plot : plot_widgets_phi) plot->setVisible(plot == ui->plot_phi_yy);
        for(auto plot : plot_widgets_rhoa) plot->setVisible(plot == ui->plot_rhoa_yy);
    }
}

void spectra_plotter::on_button_group_sel_types_buttonClicked(QAbstractButton * button) {
    if(button == ui->button_selection_type_none) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->setSelectionTool(nullptr);
        current_selector = nullptr;
    } else if(button == ui->button_selection_type_brush) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->setSelectionTool(selector_ellipse);
        current_selector = selector_ellipse;
    } else if(button == ui->button_selection_type_rectangle) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->setSelectionTool(selector_rectangle);
        current_selector = selector_rectangle;
    } else if(button == ui->button_selection_type_freehand) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->setSelectionTool(selector_free);
        current_selector = selector_free;
    } else if(button == ui->button_selection_type_range) {
        for(auto const & a : plot_widgets_all) for(auto const & b : a) b->setSelectionTool(selector_range);
        current_selector = selector_range;
    }
}

void spectra_plotter::on_button_group_range_time_buttonClicked(QAbstractButton * button) {
    time_plots_range_0_90 = button == ui->button_range_9_90;
    if(time_plots_range_0_90) {
        for(auto plot_wgt : plot_widgets_time_phi) {
            plot_wgt->setNewYRange(-0.5, 90.5);
        }
    } else {
        for(auto plot_wgt : plot_widgets_time_phi) {
            plot_wgt->fitScaleToAllVisiblePlots();
        }
    }
}

void spectra_plotter::slot_selector_changed_mod(YMathSelectionMode new_mode) {
    if(new_mode == YMathSelectionMode::DIFF) {
        set_selection_mode_for_all_selectors(YMathSelectionMode::DIFF);
        ui->button_selection_mod_remove->setChecked(true);
    } else if(new_mode == YMathSelectionMode::UNION) {
        set_selection_mode_for_all_selectors(YMathSelectionMode::UNION);
        ui->button_selection_mod_add->setChecked(true);
    } else if(new_mode == YMathSelectionMode::NORMAL_MODE) {
        set_selection_mode_for_all_selectors(YMathSelectionMode::NORMAL_MODE);
        ui->button_selection_mod_none->setChecked(true);
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
    if (this->selection_freq_map.size() != this->mtsite->size()) {
        qWarning() << "selection frequency map and mtsite don't fit";
        return;
    }

    // re-read the booleans
    progress_dialog->setWindowTitle("updating data...");
    progress_dialog->set_min_max(0, static_cast<int>(this->mtsite->z.size()));
    progress_dialog->set_progress(0);
    progress_dialog->show();

    for (auto &data : this->mtsite->z) {
        progress_dialog->set_progress_text(QString("updating %1").arg(doublefreq2string_unit(data.f)));

        if(selection_freq_map.find(data.f) != selection_freq_map.end()) {
            set_selection_from_mtsite_bool_vector(data.b, selection_freq_map[data.f]);
        }

        progress_dialog->inc_progress();
        qApp->processEvents();
    }

    // update the plots
    progress_dialog->hide();
    replot_current_tab();
}


void spectra_plotter::replot_tab(int index) {
    if(index == 0) {
        for(auto plot : plot_widgets_rhoa) plot->update();
    } else if(index == 1) {
        for(auto plot : plot_widgets_phi) plot->update();
    } else if(index == 2) {
        for(auto plot : plot_widgets_gauss) plot->update();
    } else if(index == 3) {
        for(auto plot : plot_widgets_time_xy_yx) plot->update();
    } else if(index == 4) {
        for(auto plot : plot_widgets_time_xx_yy) plot->update();
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
    delete_all_plots();
    for(auto a : plot_widgets_all) for(auto plot_widget : a) {
        plot_widget->update();
    }
    replot_current_tab();
    if (this->mtsite != nullptr) this->mtsite.reset();

    qApp->processEvents();
}

void spectra_plotter::on_tab_widget_currentChanged(int index) {
    auto tab_wgt = qobject_cast<QTabWidget*>(sender());
    if(tab_wgt != nullptr) {
        auto cur = tab_wgt->currentWidget();

        if(cur == ui->main_page_phi || cur == ui->main_page_rhoa) {
            ui->scrollArea->setEnabled(false);
            ui->slider_frequency->setEnabled(false);
            slider_text_memory = ui->label_current_freq->text();
            ui->label_current_freq->setText("all");
        } else {
            ui->scrollArea->setEnabled(true);
            ui->slider_frequency->setEnabled(true);
            ui->label_current_freq->setText(slider_text_memory);
        }

        auto enable_btn_group = (cur == ui->main_page_phi ||
                                 cur == ui->main_page_rhoa ||
                                 cur == ui->main_page_fstack);

        if(cur == ui->main_page_phi || cur == ui->main_page_rhoa || cur == ui->main_page_fstack) {
            ui->stack_widget->setCurrentWidget(ui->page_first);
        } else {
            ui->stack_widget->setCurrentWidget(ui->page_second);
        }
        ui->btn_show_xx->setEnabled(enable_btn_group);
        ui->btn_show_xy->setEnabled(enable_btn_group);
        ui->btn_show_yx->setEnabled(enable_btn_group);
        ui->btn_show_yy->setEnabled(enable_btn_group);
        ui->btn_show_all->setEnabled(enable_btn_group);
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
    if(selector_range->range_axis() == YMathWidgetSelectorRange::X_AXIS) {
        ui->button_selection_range_orientation->setText("Range mode: vertical");
    } else if(selector_range->range_axis() == YMathWidgetSelectorRange::Y_AXIS) {
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
    reset_all_selections();
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
            if(selection_freq_map.find(data.f) != selection_freq_map.end()) {
                progress_dialog->set_progress_text(doublefreq2string_unit(data.f));
                progress_dialog->inc_progress();

                auto & selection = selection_freq_map[data.f]->getCurrentSelection();
#ifdef QT_DEBUG
                qDebug().noquote().nospace() << "updating bool-vectors for freq " << data.f;
#endif
                for(size_t i = 0; i < 4; ++i) {
                    for(size_t j = 0; j < data.b[i].size(); ++j) {
                        data.b[i][j] = !selection[j];
                    }
                }

                qApp->processEvents();
            }
        }

        progress_dialog->hide();
        emit process_button_clicked();
    }
}

void spectra_plotter::on_button_debug_load_data_clicked() {    
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
        reset_everything();

        load_file(fileNames.at(0));
    }
}

void spectra_plotter::on_cb_performance_stateChanged(int val) {
    auto p_shader = val > 0 ? ygfxqt::YGFXQTAssetManager::getShader(ygfxqt::YGFXQTAssetManager::SHADER_PLOT_2D_POINTS_PERF) :
                              ygfxqt::YGFXQTAssetManager::getShader(ygfxqt::YGFXQTAssetManager::SHADER_PLOT_2D_POINTS);
    for(auto plot : plots) plot->setCustomShader(p_shader);
    for(auto a : plot_widgets_all)
        for(auto plot : a) plot->update();
}

void spectra_plotter::on_button_unlock_y_zoom_clicked(bool checked) {
    for(auto plot_wgt : plot_widgets_time_phi) {
        plot_wgt->setPlotOption(YMathOpenGLWidget::PlotOption::EnableYAxisZoom, checked);
        plot_wgt->setPlotOption(YMathOpenGLWidget::PlotOption::EnableYAxisMove, checked);
    }
}
