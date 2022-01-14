#include "qthelper.h"

#include <QDebug>
#include "procmt_alldefines.h"

namespace qthelper {

int get_numeric_value_for_number_key(int qtkey) {
    switch(qtkey) {
        case Qt::Key_0: { return 0; }
        case Qt::Key_1: { return 1; }
        case Qt::Key_2: { return 2; }
        case Qt::Key_3: { return 3; }
        case Qt::Key_4: { return 4; }
        case Qt::Key_5: { return 5; }
        case Qt::Key_6: { return 6; }
        case Qt::Key_7: { return 7; }
        case Qt::Key_8: { return 8; }
        case Qt::Key_9: { return 9; }
        default: { return -1; }
    }
}

bool is_functional_key(int qtkey) {
    return qtkey == Qt::Key_Insert ||
           qtkey == Qt::Key_Cut ||
           qtkey == Qt::Key_Paste ||
           qtkey == Qt::Key_Backspace;
}

bool create_survey_subs(const QDir &base_dir) {
    base_dir.mkpath(base_dir.path());
    for (auto & str : pmt::survey_dirs) {
        if (!base_dir.mkpath(QString("%1/%2").arg(base_dir.path(), str))) return false;
    }
    return true;
}

}
