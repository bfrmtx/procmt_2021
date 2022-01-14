#ifndef GFX_QT_WIDGET_MATHS_SELECTOR_H
#define GFX_QT_WIDGET_MATHS_SELECTOR_H

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QColor>

#include <vector>
#include <complex>
#include <unordered_set>
#include <unordered_map>
#include <set>

#include "gfx_qt_mathplot.h"

#include "glm/glm.hpp"



namespace ygfxqt {
    enum class SelectionAction {
        REMOVED_SELECTION,
        ADDED_TO_SELECTION,
        REMOVED_FROM_SELECTION,
        SWAPPED_SELECTION
    };

    enum class YMathSelectorModifierMode {
        HOLD_KEY_TO_SET_MODE,
        PRESS_KEY_TO_TOGGLE_MODE,
        DEFAULT = HOLD_KEY_TO_SET_MODE
    };

    enum class YMathSelectionMode {
        NORMAL_MODE,
        UNION,
        DIFF,
        DEFAULT = NORMAL_MODE
    };

    class YMathWidgetSelector : public QObject {
        Q_OBJECT

    public:
        bool T_xaxis_reversed = false;
        bool T_yaxis_reversed = false;

    protected:
        YMathSelectorModifierMode               key_modifier_mode = YMathSelectorModifierMode::DEFAULT;
        YMathSelectionMode                      selection_mode = YMathSelectionMode::DEFAULT;

        Qt::Key key_selection_union             = Qt::Key_Shift;
        Qt::Key key_selection_intersection      = Qt::Key_End;
        Qt::Key key_selection_difference        = Qt::Key_Control;
        Qt::Key key_selection_swap              = Qt::Key_Delete;
        Qt::Key key_selection_cancel            = Qt::Key_Escape;
        Qt::Key key_selection_special_key       = Qt::Key_1;
        Qt::KeyboardModifier key_mousewheel     = Qt::KeyboardModifier::AltModifier;

        Qt::MouseButton mousebutton_use         = Qt::LeftButton;
        glm::vec3 color_selection_default       = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 color_selection_difference    = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 color_selection_union         = glm::vec3(1.0f, 0.0f, 0.0f);

        bool selection_mode_difference = false;
        bool selection_mode_union = false;
        bool mouse_usebutton_pressed = false;

        glm::dvec2 mouse_position_plot;
        glm::dvec2 mouse_position_plot_on_click;
        glm::dvec2 plot_bounds_min;
        glm::dvec2 plot_bounds_max;

        QPoint axes_size;
        QPoint widget_size;
        QPoint mouse_position_widget;
        QPoint mouse_position_on_click;

    protected:
        virtual glm::dvec2 selection_min() = 0;
        virtual glm::dvec2 selection_max() = 0;
        virtual void before_selection_calculation(std::vector<double> const * xdata,
                                                  std::vector<double> const * ydata) { Q_UNUSED(xdata); Q_UNUSED(ydata); }
        virtual void after_selection_calculation() {}
        virtual bool selection_function(std::vector<double> const * xdata,
                                        std::vector<double> const * ydata,
                                        size_t index) = 0;
        glm::vec3 get_current_selector_color();

    public:
        void select(std::vector<double> const * xdata, std::vector<double> const * ydata, YMathGfxPlot * plot);
        virtual void paint_selection(QPainter & painter) = 0;
        virtual void cancel_action() = 0;

        virtual ~YMathWidgetSelector() {}
        virtual void handle_mouse_press_event(QMouseEvent * event, glm::dvec2 plot_pos);
        virtual void handle_mouse_release_event(QMouseEvent * event, glm::dvec2 plot_pos);
        virtual void handle_mouse_move_event(QMouseEvent * event, glm::dvec2 plot_pos);
        virtual void handle_mouse_wheel_event(QWheelEvent * event);
        virtual void handle_key_press_event(QKeyEvent * event);
        virtual void handle_key_release_event(QKeyEvent * event);
        virtual void handle_dimension_change(glm::dvec2 bounds_min, glm::dvec2 bounds_max, QPoint widget_size, QPoint offset_size);

        virtual bool select_on_release() const = 0;
        virtual bool select_on_press() const = 0;
        virtual bool select_on_pressed_move() const = 0;
        virtual bool only_active_if_mouse_pressed() const = 0;

        Qt::KeyboardModifier get_mwheel_pass_key_modifier()     const { return key_mousewheel; }
        YMathSelectionMode get_selection_mode()                 const { return selection_mode; }
        Qt::MouseButton selectionButton()                       const { return mousebutton_use; }

        void set_color_defautl(glm::vec3 color)                             { color_selection_default = color; }
        void set_color_defautl(QColor color)                                { color_selection_default = glm::vec3(color.redF(), color.greenF(), color.blueF()); }
        void set_color_for_union(glm::vec3 color)                           { color_selection_union = color; }
        void set_color_for_union(QColor color)                              { color_selection_union = glm::vec3(color.redF(), color.greenF(), color.blueF()); }
        void set_color_for_difference(glm::vec3 color)                      { color_selection_difference = color; }
        void set_color_for_difference(QColor color)                         { color_selection_difference = glm::vec3(color.redF(), color.greenF(), color.blueF()); }
        void set_mwheel_pass_key_modifier(Qt::KeyboardModifier modifier)    { key_mousewheel = modifier; }
        void set_keyboard_modifier_mode(YMathSelectorModifierMode mode)     { key_modifier_mode = mode; }
        void set_selection_mode(YMathSelectionMode mode);
        bool isActive() const {return (!only_active_if_mouse_pressed() || (only_active_if_mouse_pressed() && mouse_usebutton_pressed)); }

    signals:
        void selectionModeChanged(YMathSelectionMode mode);
        void selectionOccured(std::vector<size_t> const & indices, SelectionAction action, YMathGfxPlot * plot);
    };


    class YMathWidgetSelectorEllipse : public YMathWidgetSelector {
    public:
        const int radius_max = 100;
        const int radius_min = 2;
        const int radius_default = 20;

    private:
        Qt::Key key_size_increase = Qt::Key_Plus;
        Qt::Key key_size_decrease = Qt::Key_Minus;
        int radius = radius_default;
        double cx = 0, cy = 0, rx = 0, ry = 0;

    public:
        virtual void handle_key_press_event(QKeyEvent * event);
        virtual void handle_mouse_wheel_event(QWheelEvent * event);

        void paint_selection(QPainter &painter);
        void cancel_action();

        bool select_on_release()            const { return false; }
        bool select_on_press()              const { return true; }
        bool select_on_pressed_move()       const { return true; }
        bool only_active_if_mouse_pressed() const { return false; }
        int  get_radius()                   const { return radius; }

        void set_radius(int new_radius) {
            new_radius = std::max(new_radius, radius_min);
            new_radius = std::min(new_radius, radius_max);
            radius = new_radius;
        }

    private:
        virtual bool selection_function(std::vector<double> const * xdata,
                                        std::vector<double> const * ydata,
                                        size_t index);
        virtual void before_selection_calculation(std::vector<double> const * xdata,
                                                  std::vector<double> const * ydata);

        virtual glm::dvec2 selection_min();
        virtual glm::dvec2 selection_max();
    };


    class YMathWidgetSelectorRectangle : public YMathWidgetSelector {
    public:
        void paint_selection(QPainter &painter);
        void cancel_action();

        virtual bool select_on_release()            const { return true; }
        virtual bool select_on_press()              const { return false; }
        virtual bool select_on_pressed_move()       const { return false; }
        virtual bool only_active_if_mouse_pressed() const { return true; }

    private:
        virtual bool selection_function(std::vector<double> const * xdata,
                                        std::vector<double> const * ydata,
                                        size_t index);
        virtual glm::dvec2 selection_min();
        virtual glm::dvec2 selection_max();
    };


    class YMathWidgetSelectorFreeHand : public YMathWidgetSelector {
    private:
        static constexpr double line_length_threshold = 9.5;
        std::vector<QPoint>     current_selection_points;
        std::vector<glm::dvec2> selection_world_coords;
        QPoint max_p;
        QPoint min_p;

    public:
        void paint_selection(QPainter &painter);
        void cancel_action();

        virtual bool select_on_release()            const { return true; }
        virtual bool select_on_press()              const { return false; }
        virtual bool select_on_pressed_move()       const { return false; }
        virtual bool only_active_if_mouse_pressed() const { return true; }

    private:
        virtual bool selection_function(std::vector<double> const * xdata,
                                        std::vector<double> const * ydata,
                                        size_t index);
        virtual void after_selection_calculation();
        virtual void before_selection_calculation(std::vector<double> const * xdata,
                                                  std::vector<double> const * ydata);
        virtual glm::dvec2 selection_min();
        virtual glm::dvec2 selection_max();

    public:
        void handle_mouse_press_event(QMouseEvent * event, glm::dvec2 plot_pos);
        void handle_mouse_move_event(QMouseEvent * event, glm::dvec2 plot_pos);
        void handle_mouse_release_event(QMouseEvent * event, glm::dvec2 plot_pos);
    };


    class YMathWidgetSelectorRange : public YMathWidgetSelector {
    private:
        glm::dvec2 selection_point_min;
        glm::dvec2 selection_point_max;

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
        void paint_selection(QPainter &painter);
        void cancel_action();


        virtual void before_selection_calculation(std::vector<double> const * xdata,
                                                  std::vector<double> const * ydata);

        virtual bool select_on_release()            const { return true; }
        virtual bool select_on_press()              const { return false; }
        virtual bool select_on_pressed_move()       const { return false; }
        virtual bool only_active_if_mouse_pressed() const { return false; }
        virtual void handle_key_release_event(QKeyEvent * event);

    protected:
        glm::dvec2 selection_min();
        glm::dvec2 selection_max();
        virtual bool selection_function(std::vector<double> const * xdata,
                                        std::vector<double> const * ydata, size_t index);
    };

}
#endif // GFX_QT_WIDGET_MATHS_SELECTOR_H
