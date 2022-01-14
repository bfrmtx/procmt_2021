#ifndef PLOTDATA_H
#define PLOTDATA_H

#include "qcustomplot.h"

// stl-includes
#include <unordered_set>
#include <vector>
#include <complex>

// qt-includes
#include <QPointF>

typedef std::vector<std::vector<std::vector<size_t>>> grid_3d_vector;

struct AdditionalPlotData {
    static const bool s_mtdata_selected = false;
    static const bool s_mtdata_deselected = true;

    std::vector<double> data_x;
    std::vector<double> data_y;

    std::vector<bool> * selection_data = nullptr;

    grid_3d_vector selection_grid;
    QPointF selection_grid_step;
    QPointF selection_grid_min;
    QPoint selection_grid_size;

    QPointF plot_min_data;
    QPointF plot_max_data;

    QCustomPlot * plot = nullptr;

    bool x_one_dimensional = false;
    bool y_one_dimensional = false;

    bool deselect_datapoint(size_t index);
    bool select_datapoint(size_t index);
};

#endif // PLOTDATA_H
