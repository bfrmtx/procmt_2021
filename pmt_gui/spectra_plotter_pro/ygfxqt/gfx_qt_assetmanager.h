#ifndef GFX_QT_ASSETMANAGER_H
#define GFX_QT_ASSETMANAGER_H

#include <QOpenGLTexture>
#include <QDebug>

#include <unordered_map>
#include <string>

#include "ygfx/gfxshader.h"

namespace ygfxqt {
    class YGFXQTAssetManager {
    public:
        static const std::string SHADER_PLOT_2D_POINTS;
        static const std::string SHADER_PLOT_2D_POINTS_PERF;
        static const std::string SHADER_PLOT_2D_LINES;
        static const std::string SHADER_PLOT_NAME_2D_POINTS_VERT;
        static const std::string SHADER_PLOT_NAME_2D_POINTS_FRAG;
        static const std::string SHADER_PLOT_NAME_2D_POINTS_PERF_VERT;
        static const std::string SHADER_PLOT_NAME_2D_POINTS_PERF_FRAG;
        static const std::string SHADER_PLOT_NAME_2D_POINTS_GEOM;
        static const std::string SHADER_PLOT_NAME_2D_LINES_FRAG;
        static const std::string SHADER_PLOT_NAME_2D_LINES_VERT;

        static const std::string TEXTURE_FILENAME_PLOT_2D_CROSS;
        static const std::string TEXTURE_FILENAME_PLOT_2D_DOTS;

    private:
        static std::unordered_map<std::string, ygfx::spShader> loaded_shaders;
        static std::unordered_map<std::string, QOpenGLTexture *> map_textures;

    public:
        static QOpenGLTexture * getTexture(std::string const & shader_name);
        static void addTexture(std::string const & texture_name, QOpenGLTexture * texture);
        static bool hasTexture(std::string const & texture_name);

        static ygfx::spShader getShader(std::string const & shader_name);
        static void addShader(std::string const & shader_name, ygfx::Shader shader);
        static bool hasShader(std::string const & shader_name);

        static void loadDefaultShader();
        static void loadDefaultTextures();
    };
}

#endif // GFX_QT_ASSETMANAGER_H
