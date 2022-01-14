#include "plotdata.h"

bool AdditionalPlotData::deselect_datapoint(size_t index) {
    bool ret = true;

    if(index < selection_data->size()) {
        if(selection_data->at(index) == s_mtdata_deselected) ret = false;
        selection_data->at(index) = s_mtdata_deselected;
    }

    return ret;
}

bool AdditionalPlotData::select_datapoint(size_t index) {
    bool ret = true;
    if(index < selection_data->size()) {
        if(selection_data->at(index) == s_mtdata_selected) ret = false;
        selection_data->at(index) = s_mtdata_selected;
    }

    return ret;
}
