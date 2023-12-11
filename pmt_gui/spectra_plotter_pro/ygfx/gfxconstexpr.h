#ifndef YGFX_GL_CONSTEXPR_H
#define YGFX_GL_CONSTEXPR_H

#include <string>

#ifdef QT_CORE_LIB
#include <QString>
#endif

namespace ygfx {
    const std::string loc_position = "\\$loc_position\\$";
    const std::string loc_color = "\\$loc_color\\$";
    const std::string loc_texture = "\\$loc_texture\\$";
    const std::string loc_normal = "\\$loc_normal\\$";
    const std::string loc_position_x = "\\$loc_position_x\\$";
    const std::string loc_position_y = "\\$loc_position_y\\$";
    const std::string loc_additional_1 = "\\$loc_add_1\\$";
    const std::string loc_additional_2 = "\\$loc_add_2\\$";

    constexpr int shaderlocation_position = 0;
    constexpr int shaderlocation_color = 1;
    constexpr int shaderlocation_texture = 2;
    constexpr int shaderlocation_normal = 3;
    constexpr int shaderlocation_position_x = 4;
    constexpr int shaderlocation_position_y = 5;
    constexpr int shaderlocation_additional_1 = 2;
    constexpr int shaderlocation_additional_2 = 3;

    constexpr int32_t file_header_first = 2;
    constexpr int32_t file_header_second = 5;
    constexpr int32_t file_header_third = 14;
    constexpr int32_t file_header_ext_atlasdata = 1;
    constexpr int32_t file_header_ext_atlas = 2;
}

#ifdef QT_CORE_LIB
namespace ygfxqt {
    const QString mime_prefab_id = "mime_prefab_id";
    const QString mime_sender = "mime_sender";
    const QString mime_type_image = "mime_image";
    const QString mime_type_shader = "mime_shader";
    const QString mime_type_shader_vertex = "mime_shader_vert";
    const QString mime_type_shader_fragment = "mime_shader_frag";
    const QString mime_type_shader_geometry = "mime_shader_geom";
    const QString mime_type_textureatlas = "mime_texatlas";
    const QByteArray mime_placeholder = "";
}
#endif

#endif // GFXCONSTEXPR_H
