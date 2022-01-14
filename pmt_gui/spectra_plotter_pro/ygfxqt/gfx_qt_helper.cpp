#include "gfx_qt_helper.h"
#include <QDataStream>
#include <iostream>

namespace helper {

bool check_header(QDataStream & stream, int32_t type) {
    int32_t first, second, third, read_type;
    stream >> first >> second >> third >> read_type;
    return (first == ygfx::file_header_first && second == ygfx::file_header_second && third == ygfx::file_header_third && type == read_type);
}

void write_header(QDataStream & stream, int32_t type) {
    stream << ygfx::file_header_first << ygfx::file_header_second << ygfx::file_header_third;
    stream << type;
}

QColor glm_vec4_to_qcolor(const glm::vec4 & vec) {
    return QColor::fromRgbF(double(vec.r), double(vec.g),
           double(vec.b), double(vec.a));
}

glm::vec4 qcolor_to_glm_vec4(const QColor & color) {
    return glm::vec4(color.redF(), color.greenF(),
                     color.blueF(), color.alphaF());
}

QColor glm_vec3_to_qcolor(const glm::vec3 & vec) {
    return QColor::fromRgbF(double(vec.r), double(vec.g),
                            double(vec.b));
}

glm::dvec2 qpoint_to_glm_dvec2(const QPoint & p) {
    return glm::dvec2(p.x(), p.y());
}

}
