/***************************************************************************
**                                                                        **
**  ProcMT and modules                                                    **
**  Copyright (C) 2017-2021 metronix GmbH                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
** Parts of the software are linked against the Open Source Version of Qt **
** Qt source code copy is available under https://www.qt.io               **
** Qt source code modifications are non-proprietary,no modifications made **
** https://www.qt.io/licensing/open-source-lgpl-obligations               **
****************************************************************************
**           Author: metronix geophysics                                  **
**  Website/Contact: www.metronix.de                                      **
**                                                                        **
**                                                                        **
****************************************************************************/

#ifndef PLOT_H
#define PLOT_H

#include "qcustomplot.h"
#include <set>

#include "namespace.h"



TS_NAMESPACE_BEGIN

enum class FieldType {
    H, E,
};

enum class TSPlotType {
    UNKNOWN, HX, HY, HZ, EX, EY, EZ,
};

enum class CursorMode {
    READOUT,
    SELECTION_ADD,
    SELECTION_REMOVE,
    DEFAULT = READOUT
};

struct SelectionInterval {
    int64_t begin   = 0;
    int64_t end     = 0;
    explicit SelectionInterval(int64_t b, int64_t e) : begin(std::min(b, e)), end(std::max(b, e)) {}

    bool isValid() {
        return end > begin;
    }

    bool contains(SelectionInterval const & other) const {
        return begin < other.begin && end > other.end;
    }

    static bool overlap(SelectionInterval const & a, SelectionInterval const & b) {
        return a.begin <= b.end && b.begin <= a.end;
    }

    friend bool operator<(const SelectionInterval & lhs, const SelectionInterval& rhs) {
        return lhs.begin == rhs.begin ? lhs.end < rhs.end : lhs.begin < rhs.begin;
    }

    SelectionInterval & operator += (const SelectionInterval & rhs){
        this->begin = std::min(this->begin, rhs.begin);
        this->end = std::max(this->end, rhs.end);
        return *this;
    }

    static std::vector<SelectionInterval> difference(SelectionInterval const & lhs, SelectionInterval const & rhs) {
        std::vector<SelectionInterval> ret;
        if(lhs.contains(rhs)) {
            ret.push_back(SelectionInterval(lhs.begin, rhs.begin));
            ret.push_back(SelectionInterval(lhs.end, rhs.end));
        } else if(!rhs.contains(lhs)){
            ret.push_back(rhs.begin > lhs.begin ? SelectionInterval(lhs.begin, rhs.begin) :
                                                  SelectionInterval(rhs.end, lhs.end));
        }
        return ret;
    }

    operator QString() const { return QString("(%1, %2)").arg(begin).arg(end); }

};

typedef std::pair<SelectionInterval, QCPItemRect *> interval_pair;

struct ipair_compare {
    bool operator() (const interval_pair& lhs, const interval_pair& rhs) const {
        return lhs.second < rhs.second;
    }
};

class TSPlotterPlot : public QCustomPlot {
    Q_OBJECT

public:
    explicit TSPlotterPlot(QWidget * parent = nullptr);
    void set_cursor_mode(CursorMode new_mode);
    void set_plot_type(TSPlotType new_type);
    void add_selection_interval(int64_t a, int64_t b);
    void remove_selection_interval(int64_t a, int64_t b);    
    void clear_selection();
    TSPlotType plot_type() const { return m_plot_type; }
    std::set<interval_pair, ipair_compare> const & get_intervals() const { return m_intervals; }
    void set_interval_step_size(int new_step_size) {m_interval_cursor_step = new_step_size; }

protected:
    void mousePressEvent(QMouseEvent * event) ;
    void mouseReleaseEvent(QMouseEvent * event) ;
    void mouseMoveEvent(QMouseEvent * event) ;
    void wheelEvent(QWheelEvent * event) ;
    void enterEvent(QEnterEvent * event) ;
    void leaveEvent(QEvent * event) ;

private:
    void set_selection_coords(double min, double max);
    void set_selection_color(QColor const & color);
    void set_cursor_coords(double x, double y);
    void set_cursor_coords(double x, const std::vector<double> & values);
    void handle_current_interval_moved(int new_x, bool pressed);
    void handle_current_interval_dropped(double a, double b);
    void handle_readout(const int x, const int y);

signals:
    void plot_position_changed(const double key, const double value, const FieldType field);
    void interval_selection_changed(int64_t begin, int64_t end);
    void interval_added(int64_t a, int64_t b);
    void interval_removed(int64_t a, int64_t b);

private:
    TSPlotType                              m_plot_type = TSPlotType::UNKNOWN;
    CursorMode                              m_cursor_mode = CursorMode::DEFAULT;
    int                                     m_interval_cursor_step = 1;
    std::set<interval_pair, ipair_compare>  m_intervals;
    double                                  m_pressed_pos_x = 0.0;
    QCPItemRect *                           m_mouse_rect = nullptr;
    QCPItemLine *                           m_vertical_line = nullptr;
    std::vector<QCPItemLine *>              m_horizontal_lines;
};

TS_NAMESPACE_END



#endif // PLOT_H
