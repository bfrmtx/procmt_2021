#ifndef STREAMHELPER_H
#define STREAMHELPER_H

#include <QPoint>
#include <QColor>

#include <glm/glm.hpp>
#include <fstream>
#include "ygfx/gfxconstexpr.h"

namespace helper {
    glm::vec4 qcolor_to_glm_vec4(QColor const & color);
    QColor glm_vec4_to_qcolor(glm::vec4 const & vec);
    QColor glm_vec3_to_qcolor(glm::vec3 const & vec);
    glm::dvec2 qpoint_to_glm_dvec2(QPoint const & p);
    void write_header(QDataStream & stream, int32_t type);
    bool check_header(QDataStream & stream, int32_t type);
}

#endif // STREAMHELPER_H
