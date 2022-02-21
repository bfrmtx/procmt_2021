#ifndef SPECTRA_PLOTTER_H
#define SPECTRA_PLOTTER_H

#include "graph_selector.h"
#include "qcustomplot.h"
#include "plotwidget.h"
#include "plotdata.h"
#include "progressdialog.h"

// mt-includes
#include "mt_data.h"
#include "mt_site.h"

// stl-includes
#include <memory>
#include <stack>

// qt-includes
#include <QMainWindow>

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
        size_t graph_id;
        SpecPlotOperation operation;
        std::unordered_set<size_t> indices;
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

    std::map<double, std::vector<std::vector<bool>>> selectiondata_copy;

    std::vector<std::vector<PlotWidget *>> plot_widgets_all;
    std::vector<PlotWidget*> plots_time_xx_yy;
    std::vector<PlotWidget*> plots_time_xy_yx;
    std::vector<PlotWidget*> plots_time_phi;
    std::vector<PlotWidget*> plots_time_rhoa;
    std::vector<PlotWidget*> plots_gauss;
    std::vector<PlotWidget*> plots_phi;
    std::vector<PlotWidget*> plots_rhoa;
    std::vector<PlotWidget*> plots_single_freq;

    GraphWidgetSelector * current_selector = nullptr;
    GraphWidgetSelectorRange * selector_range = nullptr;
    GraphWidgetSelectorFreeHand * selector_free = nullptr;
    GraphWidgetSelectorBrushEllipse * selector_ellipse = nullptr;
    GraphWidgetSelectorRectangle * selector_rectangle = nullptr;
    std::vector<GraphWidgetSelector*> selectors;

    QButtonGroup * freq_button_group;

    QString slider_text_memory;

    Ui::spectra_plotter *ui;

private:
    void initialize_internal_plot_structs();
    void initialize_selectors();
    void connect_selector(GraphWidgetSelector * selector);
    void add_frequency_data(double frequency, const mt_data_t<std::complex<double> > & data);
    void push_to_undo_stack(size_t graph_id, SpecPlotOperation operation, std::unordered_set<size_t> const & indices);
    void remove_selection(size_t graph_id, std::unordered_set<size_t> const & indices);
    void add_selection(size_t graph_id, std::unordered_set<size_t> const & indices);
    void remove_selection_ignore_history(size_t graph_id, std::unordered_set<size_t> const & indices);
    void add_selection_ignore_history(size_t graph_id, std::unordered_set<size_t> const & indices);
    void set_selection_from_copy_vector();
    void set_selection_mode_for_all_selectors(GraphWidgetSelectionMode new_mode);
    void show_frequency(double frequency, QPushButton * freqButton = nullptr);
    void clear_history();
    void undo();
    void redo();

    // slots activated by GUI-signals
private slots:
    void on_button_group_sel_modifier_buttonClicked(QAbstractButton * button);
    void on_button_group_sel_types_buttonClicked(QAbstractButton * button);
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

    // manual slots
public slots:
    void slot_replot();

private slots:
    void slot_selector_changed_mod(GraphWidgetSelectionMode new_mode);
    void slot_plot_range_changed_xy_yx(const QCPRange & newRange, const QCPRange & oldRange);
    void slot_plot_range_changed_xx_yy(const QCPRange & newRange, const QCPRange & oldRange);
    void slot_frequency_button_clicked();


signals:
    void process_button_clicked();
};

#endif // SPECTRA_PLOTTER_H
