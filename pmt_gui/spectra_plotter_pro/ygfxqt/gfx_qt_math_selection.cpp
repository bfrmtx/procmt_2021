#include "gfx_qt_math_selection.h"
#include <iostream>

ygfxqt::YMathSelection::YMathSelection(size_t elements, QObject * parent) : QObject(parent) {
    selection.resize(elements, false);
}

void ygfxqt::YMathSelection::addToSelection(std::vector<size_t> const & added) {
    STOPWATCH("time_add_to_selection: ");
    std::vector<size_t> added_indices; added_indices.reserve(added.size());
    for(size_t index : added) {
        if(!selection[index]) {
            added_indices.push_back(index);
            selection[index] = true;
        }
    }

    emit addedToSelection(added_indices);
}
void ygfxqt::YMathSelection::removeFromSelection(std::vector<size_t> const & removed) {

    STOPWATCH("time_remove_from_selection: ");
    std::vector<size_t> removed_indices; removed_indices.reserve(removed.size());
    for(size_t index : removed) {
        if(selection[index]) {
            removed_indices.push_back(index);
            selection[index] = false;
        }
    }

    emit removedFromSelection(removed_indices);
}

void ygfxqt::YMathSelection::replaceSelection(std::vector<bool> const & new_selection) {
    selection = new_selection;
    emit selectionReplaced(selection);
}
