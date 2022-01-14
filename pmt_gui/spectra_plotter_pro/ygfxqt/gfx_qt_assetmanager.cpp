#include "gfx_qt_assetmanager.h"

//using namespace std;
using namespace ygfx;
using namespace ygfxqt;

std::string const YGFXQTAssetManager::SHADER_PLOT_2D_LINES                   = "plot_2d_lines";
std::string const YGFXQTAssetManager::SHADER_PLOT_2D_POINTS                  = "plot_2d_points";
std::string const YGFXQTAssetManager::SHADER_PLOT_2D_POINTS_PERF             = "plot_2d_points_performance";
std::string const YGFXQTAssetManager::SHADER_PLOT_NAME_2D_POINTS_VERT        = ":/plot/tex_vert.glsl";
std::string const YGFXQTAssetManager::SHADER_PLOT_NAME_2D_POINTS_FRAG        = ":/plot/tex_frag.glsl";
std::string const YGFXQTAssetManager::SHADER_PLOT_NAME_2D_POINTS_GEOM        = ":/plot/tex_geom.glsl";
std::string const YGFXQTAssetManager::SHADER_PLOT_NAME_2D_POINTS_PERF_VERT   = ":/plot/performance_vert.glsl";
std::string const YGFXQTAssetManager::SHADER_PLOT_NAME_2D_POINTS_PERF_FRAG   = ":/plot/performance_frag.glsl";
std::string const YGFXQTAssetManager::SHADER_PLOT_NAME_2D_LINES_FRAG         = ":/plot/line_frag.glsl";
std::string const YGFXQTAssetManager::SHADER_PLOT_NAME_2D_LINES_VERT         = ":/plot/line_vert.glsl";

std::string const YGFXQTAssetManager::TEXTURE_FILENAME_PLOT_2D_CROSS         = ":/tex_cross_small.png";
std::string const YGFXQTAssetManager::TEXTURE_FILENAME_PLOT_2D_DOTS          = ":/tex_dot_small.png";

std::string const dbg_output_msg_shader = "ASSET-MGR: DEFAULT SHADER %1 LOADED";
std::string const dbg_output_msg_texture = "ASSET-MGR: DEFAULT TEXTURE %1 LOADED";

std::unordered_map<std::string, spShader> YGFXQTAssetManager::loaded_shaders;
std::unordered_map<std::string, QOpenGLTexture *> YGFXQTAssetManager::map_textures;

QOpenGLTexture * YGFXQTAssetManager::getTexture(const std::string & shader_name) {
    auto iterator = map_textures.find(shader_name);
    return iterator == map_textures.end() ? nullptr : iterator->second;
}

void YGFXQTAssetManager::addTexture(const std::string & texture_name, QOpenGLTexture * texture) {
    if(map_textures.find(texture_name) == map_textures.end()) {
        map_textures[texture_name] = texture;
    }
}

bool YGFXQTAssetManager::hasTexture(const std::string & texture_name) {
    return map_textures.find(texture_name) != map_textures.end();
}

spShader YGFXQTAssetManager::getShader(const std::string & shader_name) {
    auto iterator = loaded_shaders.find(shader_name);
    return iterator == loaded_shaders.end() ? nullptr : iterator->second;
}

void YGFXQTAssetManager::addShader(const std::string & shader_name, Shader shader) {
    if(loaded_shaders.find(shader_name) == loaded_shaders.end()) {
        loaded_shaders[shader_name] = spShader(new Shader(shader));
    }
}

bool YGFXQTAssetManager::hasShader(const std::string & shader_name) {
    return loaded_shaders.find(shader_name) != loaded_shaders.end();
}

void YGFXQTAssetManager::loadDefaultShader() {
    if(!hasShader(SHADER_PLOT_2D_POINTS)) {
        loaded_shaders[SHADER_PLOT_2D_POINTS] = Shader::createShaderFromFileNames(SHADER_PLOT_NAME_2D_POINTS_FRAG,
                                                                                  SHADER_PLOT_NAME_2D_POINTS_VERT,
                                                                                  SHADER_PLOT_NAME_2D_POINTS_GEOM);
        qDebug() << QString(dbg_output_msg_shader.c_str()).arg(SHADER_PLOT_2D_POINTS.c_str());;
    }

    if(!hasShader(SHADER_PLOT_2D_LINES)) {
        loaded_shaders[SHADER_PLOT_2D_LINES] = Shader::createShaderFromFileNames(SHADER_PLOT_NAME_2D_LINES_FRAG,
                                                                                 SHADER_PLOT_NAME_2D_LINES_VERT);
        qDebug() << QString(dbg_output_msg_shader.c_str()).arg(SHADER_PLOT_2D_LINES.c_str());;
    }

    if(!hasShader(SHADER_PLOT_2D_POINTS_PERF)) {
        loaded_shaders[SHADER_PLOT_2D_POINTS_PERF] = Shader::createShaderFromFileNames(SHADER_PLOT_NAME_2D_POINTS_PERF_FRAG,
                                                                                       SHADER_PLOT_NAME_2D_POINTS_PERF_VERT);
        qDebug() << QString(dbg_output_msg_shader.c_str()).arg(SHADER_PLOT_2D_POINTS_PERF.c_str());;
    }
}

void YGFXQTAssetManager::loadDefaultTextures() {
    if(!hasTexture(TEXTURE_FILENAME_PLOT_2D_CROSS)) {
        QOpenGLTexture * texture = new QOpenGLTexture(QImage(QString::fromStdString(TEXTURE_FILENAME_PLOT_2D_CROSS)));
        addTexture(TEXTURE_FILENAME_PLOT_2D_CROSS, texture);
        qDebug() << QString(dbg_output_msg_texture.c_str()).arg(TEXTURE_FILENAME_PLOT_2D_CROSS.c_str());;
    }
    if(!hasTexture(TEXTURE_FILENAME_PLOT_2D_DOTS)) {
        QOpenGLTexture * texture = new QOpenGLTexture(QImage(QString::fromStdString(TEXTURE_FILENAME_PLOT_2D_DOTS)));
        addTexture(TEXTURE_FILENAME_PLOT_2D_DOTS, texture);
        qDebug() << QString(dbg_output_msg_texture.c_str()).arg(TEXTURE_FILENAME_PLOT_2D_DOTS.c_str());;
    }
}

