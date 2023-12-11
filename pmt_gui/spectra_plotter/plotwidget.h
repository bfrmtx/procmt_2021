#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include "qcustomplot.h"
#include "graph_selector.h"
#include "plotdata.h"

// stl-includes
#include <map>


enum class SelectionMode {
    NO_SELECTION,
    BRUSH_ELLIPSE,
    BRUSH_CUSTOM,
    RECTANGLE,
    FREE,
    RANGE,
    DEFAULT = NO_SELECTION
};

class PlotWidget : public QCustomPlot {
    Q_OBJECT

private:
    std::map<double, QCPGraph *> graph_map;
    std::map<double, AdditionalPlotData> graph_data;

    GraphWidgetSelector * selector = nullptr;
    QCPSelectorItem * selector_item = nullptr;

public:
    void rescale_axes_with_graph_data(double frequency);
    void rescale_axes_with_margin(double percentage, bool scale_x_axis = true, bool scale_y_axis = true);

    void set_adaptive_sampling(bool value);

    explicit PlotWidget(QWidget * parent = nullptr);

    void set_graph_data(double freq,
                        std::vector<double> const & keys,
                        std::vector<double> const & values, std::vector<bool> * selection,
                        bool visible = true,
                        bool adaptivesampling = true);

    void show_all_graphs();
    void show_single_graph(double freq);
    void clear_all_plots();
    void set_selector(GraphWidgetSelector * selector) { this->selector = selector; selector_item->selector = selector; }

public slots:
    void add_to_selection(size_t graph_id, std::unordered_set<size_t> const & indices);
    void remove_from_selection(size_t graph_id, std::unordered_set<size_t> const & indices);
    void set_selection(size_t graph_id, QCPDataSelection const & selection);
    QCPDataSelection add_and_get_selection(size_t graph_id, const std::unordered_set<size_t> & indices);
    QCPDataSelection remove_and_get_selection(size_t graph_id, const std::unordered_set<size_t> & indices);

protected:
    void mouseMoveEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

signals:
    void statusMessage(QString const & message);
    void controlModifierChanged(bool value);
};

#endif // PLOTWIDGET_H
