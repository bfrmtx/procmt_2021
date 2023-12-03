#ifndef GRAPH_SELECTOR_H
#define GRAPH_SELECTOR_H

#include "plotdata.h"
#include "qcustomplot.h"


// stl-includes
#include <complex>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>


// qt-includes
#include <QColor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>


enum class SelectionAction {
  REMOVED_SELECTION,
  ADDED_TO_SELECTION,
  REMOVED_FROM_SELECTION
};

enum class GraphWidgetSelectionModifierMode {
  HOLD_KEY_TO_SET_MODE,
  PRESS_KEY_TO_TOGGLE_MODE,
  DEFAULT = HOLD_KEY_TO_SET_MODE
};

enum class GraphWidgetSelectionMode {
  NORMAL_MODE,
  UNION,
  DIFF,
  DEFAULT = NORMAL_MODE
};

class GraphWidgetSelector : public QObject {
  Q_OBJECT

protected:
  GraphWidgetSelectionModifierMode key_modifier_mode = GraphWidgetSelectionModifierMode::DEFAULT;
  GraphWidgetSelectionMode selection_mode = GraphWidgetSelectionMode::DEFAULT;

  Qt::Key key_selection_union = Qt::Key_Shift;
  Qt::Key key_selection_difference = Qt::Key_Control;
  Qt::Key key_selection_cancel = Qt::Key_Escape;
  Qt::KeyboardModifier key_mousewheel = Qt::KeyboardModifier::AltModifier;

  Qt::MouseButton mousebutton_use = Qt::RightButton;

  bool selection_mode_union = false;
  bool selection_mode_difference = false;
  bool mouse_usebutton_pressed = false;

  std::set<size_t> short_time_memory;
  std::unordered_map<AdditionalPlotData *, std::unordered_set<size_t>> current_selected_indices;
  std::unordered_map<AdditionalPlotData *, std::unordered_set<size_t>> current_deselected_indices;

  QPointF mouse_position_plot;
  QPointF mouse_position_plot_on_click;
  QPointF plot_bounds_min;
  QPointF plot_bounds_max;

  QPoint axes_size;
  QPoint widget_size;
  QPoint mouse_position_widget;
  QPoint mouse_position_on_click;

  QColor color_selection_default = Qt::black;
  QColor color_selection_union = Qt::red;
  QColor color_selection_difference = Qt::green;

protected:
  virtual QPointF selection_min(QCustomPlot *plot) = 0;
  virtual QPointF selection_max(QCustomPlot *plot) = 0;
  virtual void before_selection_calculation(AdditionalPlotData *data) { Q_UNUSED(data) }
  virtual void after_selection_calculation() {}
  virtual bool selection_function(AdditionalPlotData *data, size_t index) = 0;
  QColor get_current_selector_color();

public:
  void select(size_t graph_id, AdditionalPlotData *plot);
  virtual void paint_selection(QCPPainter &painter) = 0;
  virtual void cancel_action() = 0;

  virtual ~GraphWidgetSelector() {}
  virtual void handle_mouse_press_event(QMouseEvent *event, QPointF plot_pos);
  virtual void handle_mouse_release_event(QMouseEvent *event, QPointF plot_pos);
  virtual void handle_mouse_move_event(QMouseEvent *event, QPointF plot_pos);
  virtual void handle_mouse_wheel_event(QWheelEvent *event);
  virtual void handle_key_press_event(QKeyEvent *event);
  virtual void handle_key_release_event(QKeyEvent *event);
  virtual void handle_dimension_change(QPointF bounds_min, QPointF bounds_max, QPoint widget_size, QPoint offset_size);

  virtual bool select_on_release() const = 0;
  virtual bool select_on_press() const = 0;
  virtual bool select_on_pressed_move() const = 0;
  virtual bool only_active_if_mouse_pressed() const = 0;

  Qt::KeyboardModifier get_mwheel_pass_key_modifier() const { return key_mousewheel; }
  GraphWidgetSelectionMode get_selection_mode() const { return selection_mode; }

  void set_color_default(QColor color) { color_selection_default = color; }
  void set_color_for_union(QColor color) { color_selection_union = color; }
  void set_color_for_difference(QColor color) { color_selection_difference = color; }
  void set_mwheel_pass_key_modifier(Qt::KeyboardModifier modifier) { key_mousewheel = modifier; }
  void set_keyboard_modifier_mode(GraphWidgetSelectionModifierMode mode) { key_modifier_mode = mode; }
  void set_selection_mode(GraphWidgetSelectionMode mode);
  bool isActive() const { return (!only_active_if_mouse_pressed() || (only_active_if_mouse_pressed() && mouse_usebutton_pressed)); }

signals:
  void selectionModeChanged(GraphWidgetSelectionMode mode);
  void indices_selected(size_t graph_id, std::unordered_set<size_t> const &selected);
  void indices_deselected(size_t graph_id, std::unordered_set<size_t> const &deselected);
};

class GraphWidgetSelectorBrushEllipse : public GraphWidgetSelector {

public:
  int const radius_max = 100;
  int const radius_min = 2;
  int const radius_default = 20;

  virtual void handle_key_press_event(QKeyEvent *event);
  virtual void handle_mouse_wheel_event(QWheelEvent *event);

  void set_radius(int new_radius) { radius = std::max(radius_min, std::min(radius_max, new_radius)); }
  void paint_selection(QCPPainter &painter);
  void cancel_action();

  bool select_on_release() const { return false; }
  bool select_on_press() const { return true; }
  bool select_on_pressed_move() const { return true; }
  bool only_active_if_mouse_pressed() const { return false; }

private:
  Qt::Key key_size_increase = Qt::Key_Plus;
  Qt::Key key_size_decrease = Qt::Key_Minus;
  int radius = this->radius_default;
  double cx = 0, cy = 0, rx = 0, ry = 0;

  virtual bool selection_function(AdditionalPlotData *data, size_t index);
  virtual void before_selection_calculation(AdditionalPlotData *data);
  virtual QPointF selection_min(QCustomPlot *plot);
  virtual QPointF selection_max(QCustomPlot *plot);
};

class GraphWidgetSelectorRectangle : public GraphWidgetSelector {
public:
  void paint_selection(QCPPainter &painter);
  void cancel_action();

  virtual bool select_on_release() const { return true; }
  virtual bool select_on_press() const { return false; }
  virtual bool select_on_pressed_move() const { return false; }
  virtual bool only_active_if_mouse_pressed() const { return true; }

private:
  virtual bool selection_function(AdditionalPlotData *data, size_t index);
  virtual QPointF selection_min(QCustomPlot *plot);
  virtual QPointF selection_max(QCustomPlot *plot);
};

class GraphWidgetSelectorFreeHand : public GraphWidgetSelector {
private:
  static constexpr double line_length_threshold = 9.5;
  std::vector<QPointF> current_selection_points;
  std::vector<QPointF> selection_world_coords;
  QPointF max_p;
  QPointF min_p;

public:
  void paint_selection(QCPPainter &painter);
  void cancel_action();

  virtual bool select_on_release() const { return true; }
  virtual bool select_on_press() const { return false; }
  virtual bool select_on_pressed_move() const { return false; }
  virtual bool only_active_if_mouse_pressed() const { return true; }

private:
  virtual bool selection_function(AdditionalPlotData *data, size_t index);
  virtual void after_selection_calculation();
  virtual void before_selection_calculation(AdditionalPlotData *data);
  virtual QPointF selection_min(QCustomPlot *plot);
  virtual QPointF selection_max(QCustomPlot *plot);

public:
  void handle_mouse_press_event(QMouseEvent *event, QPointF plot_pos);
  void handle_mouse_move_event(QMouseEvent *event, QPointF plot_pos);
  void handle_mouse_release_event(QMouseEvent *event, QPointF plot_pos);
};

class GraphWidgetSelectorRange : public GraphWidgetSelector {
private:
  QPointF selection_point_min;
  QPointF selection_point_max;

public:
  enum RangeSelectorAxis {
    X_AXIS,
    Y_AXIS
  };

private:
  RangeSelectorAxis current_axis = X_AXIS;

public:
  void set_range_axis(RangeSelectorAxis axis) { current_axis = axis; }
  void toggle_range_axis() { current_axis = current_axis == X_AXIS ? Y_AXIS : X_AXIS; }
  RangeSelectorAxis range_axis() const { return current_axis; }

  void paint_selection(QCPPainter &painter);
  void cancel_action();

  virtual void before_selection_calculation(AdditionalPlotData *data);

  virtual bool select_on_release() const { return true; }
  virtual bool select_on_press() const { return false; }
  virtual bool select_on_pressed_move() const { return false; }
  virtual bool only_active_if_mouse_pressed() const { return false; }

protected:
  QPointF selection_min(QCustomPlot *plot);
  QPointF selection_max(QCustomPlot *plot);
  bool selection_function(AdditionalPlotData *data, size_t index);
};

class QCPSelectorItem : public QCPAbstractItem {
  bool is_visible = false;

public:
  void setVisible(bool value) { is_visible = value; }
  QCPSelectorItem(QCustomPlot *parent) :
      QCPAbstractItem(parent) {}
  GraphWidgetSelector *selector = nullptr;
  double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const {
    Q_UNUSED(pos);
    Q_UNUSED(onlySelectable);
    Q_UNUSED(details);
    return 0;
  }
  void draw(QCPPainter *painter) {
    if (selector != nullptr && selector->isActive() && is_visible) {
      selector->paint_selection(*painter);
    }
  }
};

#endif
