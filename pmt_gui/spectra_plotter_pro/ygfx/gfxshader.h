#ifndef YGFX_GL_SHADER_H
#define YGFX_GL_SHADER_H

// std-includes
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

// gl/glm-includes
#include "gfxincludes.h"
#include <glm/glm.hpp>

namespace ygfx {
    class Shader;

    typedef std::shared_ptr<ygfx::Shader> spShader;

    enum class GLVariableType {
        BOOL = GL_BOOL,
        VEC2B = GL_BOOL_VEC2,
        VEC3B = GL_BOOL_VEC3,
        VEC4B = GL_BOOL_VEC4,
        FLOAT = GL_FLOAT,
        VEC2F = GL_FLOAT_VEC2,
        VEC3F = GL_FLOAT_VEC3,
        VEC4F = GL_FLOAT_VEC4,
        MAT2x2F = GL_FLOAT_MAT2,
        MAT3x3F = GL_FLOAT_MAT3,
        MAT4x4F = GL_FLOAT_MAT4,
        MAT2x3F = GL_FLOAT_MAT2x3,
        MAT2x4F = GL_FLOAT_MAT2x4,
        MAT3x2F = GL_FLOAT_MAT3x2,
        MAT3x4F = GL_FLOAT_MAT3x4,
        MAT4x2F = GL_FLOAT_MAT4x2,
        MAT4x3F = GL_FLOAT_MAT4x3,
        DOUBLE  = GL_DOUBLE,
        VEC2D = GL_DOUBLE_VEC2,
        VEC3D = GL_DOUBLE_VEC3,
        VEC4D = GL_DOUBLE_VEC4,
        MAT2x2D = GL_DOUBLE_MAT2,
        MAT3x3D = GL_DOUBLE_MAT3,
        MAT4x4D = GL_DOUBLE_MAT4,
        MAT2x3D = GL_DOUBLE_MAT2x3,
        MAT2x4D = GL_DOUBLE_MAT2x4,
        MAT3x2D = GL_DOUBLE_MAT3x2,
        MAT3x4D = GL_DOUBLE_MAT3x4,
        MAT4x2D = GL_DOUBLE_MAT4x2,
        MAT4x3D = GL_DOUBLE_MAT4x3,
        INTEGER = GL_INT,
        VEC2I = GL_INT_VEC2,
        VEC3I = GL_INT_VEC3,
        VEC4I = GL_INT_VEC4,
        UINTEGER = GL_UNSIGNED_INT,
        VEC2UI = GL_UNSIGNED_INT_VEC2,
        VEC3UI = GL_UNSIGNED_INT_VEC3,
        VEC4UI = GL_UNSIGNED_INT_VEC4,
        SAMPLER_1D = GL_SAMPLER_1D,
        SAMPLER_2D = GL_SAMPLER_2D,
        SAMPLER_3D = GL_SAMPLER_3D,
        SAMPLER_CUBE = GL_SAMPLER_CUBE,
        SAMPLER_1D_SHADOW = GL_SAMPLER_1D_SHADOW,
        SAMPLER_2D_SHADOW = GL_SAMPLER_2D_SHADOW,
        SAMPLER_1D_ARRAY = GL_SAMPLER_1D_ARRAY,
        SAMPLER_2D_ARRAY = GL_SAMPLER_2D_ARRAY,
        SAMPLER_1D_ARRAY_SHADOW = GL_SAMPLER_1D_ARRAY_SHADOW,
        SAMPLER_2D_ARRAY_SHADOW = GL_SAMPLER_2D_ARRAY_SHADOW,
        SAMPLER_2D_MULTISAMPLE = GL_SAMPLER_2D_MULTISAMPLE,
        SAMPLER_2D_MULTISAMPLE_ARRAY = GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
        SAMPLER_CUBE_SHADOW = GL_SAMPLER_CUBE_SHADOW,
        SAMPLER_BUFFER = GL_SAMPLER_BUFFER,
        SAMPLER_2D_RECT = GL_SAMPLER_2D_RECT,
        SAMPLER_2D_RECT_SHADOW = GL_SAMPLER_2D_RECT_SHADOW,
        UNKNOWN = 0,
    };

    struct ShaderVariableData {
        const GLVariableType type;
        const GLuint position;
        const std::string name;
        ShaderVariableData(GLVariableType vartype, GLuint pos, const std::string& varname) : type(vartype), position(pos), name(varname) {}
    };


    enum class ShaderErrorState {
        NO_SHADER_ERROR,
        COMPILER_ERROR,
        UNITIALIZED
    };

    struct ShaderError {
        int LineNumber = 0;
        QString message = "";
    };


    class Shader : public QObject {
    private:
        static GLuint current_shader_id;

        QList<ShaderError> fragment_compile_errors;
        QList<ShaderError> vertex_compile_errors;
        QList<ShaderError> geometry_compile_errors;
        QString code_vertex;
        QString code_fragment;
        QString code_geometry;

        GLuint shader_id;
        ShaderErrorState errorstate;

    private:
        void destroyShader();

    public:        
        static std::shared_ptr<Shader> createShaderFromFileNames(const std::string& fragmentfile,
            const std::string& vertexfile = "", const std::string& geometryfile = "");
        static std::shared_ptr<Shader> createShaderFromSourceCode(const std::string& fragmentcode,
            const std::string& vertexcode = "", const std::string& geometrycode = "");

        static GLuint currentActivatedShaderID();

    public:
        Shader(const Shader& from);
        Shader& operator=(const Shader& rhs);
        Shader();
        ~Shader();

        QList<ShaderError> const & vertexCompileErrors() const { return vertex_compile_errors; }
        QList<ShaderError> const & fragmentCompileErrors() const { return fragment_compile_errors; }
        QList<ShaderError> const & geometryCompileErrors() const { return geometry_compile_errors; }

        std::vector<ShaderVariableData> getUniformVariables() const;
        std::vector<ShaderVariableData> getAttributeVariables() const;
        Shader& uploadMatrix44f(const std::string& location_name, const glm::mat4& matrix);
        Shader& uploadMatrix44f(GLint location, const glm::mat4& matrix);

        Shader& uploadColor3(const std::string& location_name, glm::vec3& color);
        Shader& uploadColor3(const std::string& location_name, QColor const & color);
        Shader& uploadColor3(GLint location, glm::vec3 & color);
        Shader& uploadColor3(GLint location, QColor const & color);

        Shader& uploadColor4(const std::string& location_name, QColor const & color);
        Shader& uploadColor4(const std::string& location_name, glm::vec4 & color);
        Shader& uploadColor4(GLint location, glm::vec4 & color);

        Shader& uploadVec2(const std::string & location_name, const glm::vec2 & value);
        Shader& uploadVec3(const std::string & location_name, const glm::vec3 & value);
        Shader& uploadUint(const std::string& location_name, const GLuint value);
        Shader& uploadInt(const std::string& location_name, const GLint value);
        Shader& uploadFloat(const std::string& location_name, const GLfloat value);
        GLint getUniformLocationForName(std::string& location_name);

        Shader& activate();
        bool isValid() const { return shader_id > 0; }
        GLuint shaderID() const { return shader_id; }
    };


    std::ostream& operator<< (std::ostream& ostr, const Shader& shader);
    std::ostream& operator<< (std::ostream& ostr, const ShaderVariableData& shaderdata);
    std::ostream& operator<< (std::ostream& ostr, const GLVariableType vartype);
}


#endif // YOPENGL_ASSET_SHADER_H
