#ifndef GFX_QT_MATH_SELECTION_H
#define GFX_QT_MATH_SELECTION_H

#include <QObject>
#include <unordered_set>
#include "gfx_qt_mathplot.h"
namespace ygfxqt {
    class YMathSelection : public QObject {
        Q_OBJECT

    private:
        std::vector<bool> selection;
        size_t selected_amount = 0;

    public:
        explicit YMathSelection(size_t elements, QObject * parent = nullptr);
        void addToSelection(std::vector<size_t> const & added);
        void removeFromSelection(std::vector<size_t> const & removed);
        std::vector<bool> const & getCurrentSelection() const { return selection; }
        void replaceSelection(std::vector<bool> const & new_selection);

    signals:
        void addedToSelection(std::vector<size_t> const & added);
        void removedFromSelection(std::vector<size_t> const & removed);
        void selectionReplaced(std::vector<bool> const & new_selection);
    };
}
#endif // GFX_QT_MATH_SELECTION_H
