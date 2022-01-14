#ifndef SPECTRA_PLOTTER_H
#define SPECTRA_PLOTTER_H
#ifdef _msvc
#define _HAS_STD_BYTE 0
#endif
#include <QtOpenGL>

#include "progressdialog.h"

// mt-includes
#include "mt_data.h"
#include "mt_site.h"

// stl-includes
#include <memory>
#include <stack>

// qt-includes
#include <QMainWindow>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QFileDialog>

// y-includes
#include "ygfxqt/gfx_qt_widget_maths.h"
#include "ygfxqt/gfx_qt_math_selector.h"

namespace Ui {
class spectra_plotter;
}


class spectra_plotter : public QMainWindow {
    Q_OBJECT

    enum class SpecPlotOperation {
        ADDED_TO_SELECTION,
        REMOVED_FROM_SELECTION
    };

    struct UndoOperation {
        ygfxqt::YMathSelection * selection = nullptr;
        SpecPlotOperation operation;
        std::vector<size_t> indices;
    };

public:
    explicit spectra_plotter(QWidget *parent = nullptr);
    ~spectra_plotter();
    void set_cmd_line(std::shared_ptr<prc_com> cmdline);
    void set_mt_site(std::shared_ptr<mt_site> mt_site);
    void load_file(QString const & filename);
    void replot_tab(int id);
    void replot_current_tab();
    void reset_everything();

private:
    ProgressDialog * progress_dialog = nullptr;

    std::shared_ptr<mt_site> mtsite;
    std::shared_ptr<prc_com> cmdline;

    std::stack<UndoOperation *> undo_stack;
    std::stack<UndoOperation *> redo_stack;

    std::vector<QPushButton *> frequency_buttons;

    std::map<ygfxqt::YMathGfxPlot *, ygfxqt::YMathSelection *> selection_map;
    std::map<double, ygfxqt::YMathSelection *> selection_freq_map;
    std::vector<ygfxqt::YMathSelection *> selections;

    std::vector<gfxVector<GLfloat>*> gfx_vectors;
    std::vector<std::vector<double>*> ram_vectors;

    std::vector<std::vector<ygfxqt::YMathOpenGLWidget *>> plot_widgets_all;
    std::vector<ygfxqt::YMathOpenGLWidget *> plot_widgets_time_xx_yy;
    std::vector<ygfxqt::YMathOpenGLWidget *> plot_widgets_time_xy_yx;
    std::vector<ygfxqt::YMathOpenGLWidget *> plot_widgets_time_phi;
    std::vector<ygfxqt::YMathOpenGLWidget *> plot_widgets_time_rhoa;
    std::vector<ygfxqt::YMathOpenGLWidget *> plot_widgets_gauss;
    std::vector<ygfxqt::YMathOpenGLWidget *> plot_widgets_phi;
    std::vector<ygfxqt::YMathOpenGLWidget *> plot_widgets_rhoa;
    std::vector<ygfxqt::YMathOpenGLWidget *> plots_single_freq;
    std::vector<ygfxqt::YMathGfxPlotSet2D *> plots;

    ygfxqt::YMathWidgetSelector * current_selector = nullptr;
    ygfxqt::YMathWidgetSelectorRange * selector_range = nullptr;
    ygfxqt::YMathWidgetSelectorFreeHand * selector_free = nullptr;
    ygfxqt::YMathWidgetSelectorEllipse * selector_ellipse = nullptr;
    ygfxqt::YMathWidgetSelectorRectangle * selector_rectangle = nullptr;
    std::vector<ygfxqt::YMathWidgetSelector *> selectors;

    QButtonGroup * freq_button_group;

    QString slider_text_memory;
    bool opengl_init = false;
    bool block_stacks = false;
    bool time_plots_range_0_90 = false;

    Ui::spectra_plotter *ui;

private:
    void initialize_internal_plot_structs();
    void initialize_selectors();
    void connect_selector(ygfxqt::YMathWidgetSelector * selector);
    void add_frequency_data(double frequency, const mt_data_t<std::complex<double> > & data);

    void push_to_undo_stack(ygfxqt::YMathSelection * selection, SpecPlotOperation operation, std::vector<size_t> const & indices);
    void undo();
    void redo();

    void set_selection_from_mtsite_bool_vector(std::vector<std::vector<bool>> const & b, ygfxqt::YMathSelection * target);
    void reset_all_selections();
    void set_selection_mode_for_all_selectors(ygfxqt::YMathSelectionMode new_mode);
    void show_frequency(double frequency, QPushButton * freqButton = nullptr);
    void clear_history();
    void delete_all_plots();

    // slots activated by GUI-signals
private slots:
    void on_button_group_sel_modifier_buttonClicked(QAbstractButton * button);
    void on_button_group_visibility_buttonClicked(QAbstractButton * button);
    void on_button_group_sel_types_buttonClicked(QAbstractButton * button);
    void on_button_group_range_time_buttonClicked(QAbstractButton * button);
    void on_slider_frequency_valueChanged(int value);
    void on_tab_widget_currentChanged(int index);
    void on_button_undo_clicked();
    void on_button_redo_clicked();
    void on_button_selection_range_orientation_clicked();
    void on_button_selection_type_range_toggled(bool checked);
    void on_button_selection_type_brush_toggled(bool checked);
    void on_slider_circle_radius_valueChanged(int value);
    void on_button_process_clicked();
    void on_button_debug_reload_clicked();
    void on_button_debug_reset_clicked();
    void on_button_debug_load_data_clicked();
    void on_cb_performance_stateChanged(int val);

    // manual slots
public slots:
    void slot_replot();

private slots:
    void slot_selector_changed_mod(ygfxqt::YMathSelectionMode new_mode);
    void slot_frequency_button_clicked();

    void on_button_unlock_y_zoom_clicked(bool checked);

signals:
    void process_button_clicked();
    void opengl_init_finished();
};

#endif // SPECTRA_PLOTTER_H
