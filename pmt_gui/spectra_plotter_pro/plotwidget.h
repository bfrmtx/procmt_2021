#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include "qcustomplot.h"
#include "plotdata.h"
#include "graph_selector.h"
#include "ygfxqt/gfx_qt_math_selector.h"

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

    ygfxqt::YMathSelectionMode * selector = nullptr;
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
    void set_selector(GraphWidgetSelector * selector) {
        // Qt6
        //this->selector = selector;
        selector_item->selector = selector;
    }

public slots:
    void add_to_selection(size_t graph_id, std::unordered_set<size_t> const & indices);
    void remove_from_selection(size_t graph_id, std::unordered_set<size_t> const & indices);
    void set_selection(size_t graph_id, QCPDataSelection const & selection);
    QCPDataSelection add_and_get_selection(size_t graph_id, const std::unordered_set<size_t> & indices);
    QCPDataSelection remove_and_get_selection(size_t graph_id, const std::unordered_set<size_t> & indices);

protected:
    void mouseMoveEvent(QMouseEvent * event) ;
    void mousePressEvent(QMouseEvent * event) ;
    void mouseReleaseEvent(QMouseEvent * event) ;
    void enterEvent(QEvent * event) ;
    void leaveEvent(QEvent * event) ;
    void keyPressEvent(QKeyEvent * event) ;
    void keyReleaseEvent(QKeyEvent * event) ;

signals:
    void statusMessage(QString const & message);
    void controlModifierChanged(bool value);
};

#endif // PLOTWIDGET_H
