#include "gfxshader.h"
#include "gfxconstexpr.h"

#include <glm/gtc/type_ptr.hpp>
#include <regex>
#include <iostream>

#include <QDebug>
#include <QColor>
#include <QFile>


using namespace ygfx;

#define Y_GLVAROSTR(outp) case outp: {ostr << #outp; break; }

GLuint Shader::current_shader_id = 0;

static inline std::string replaceLiteralsWithConstants(std::string const & input) {
    auto output = std::regex_replace(input, std::regex(ygfx::loc_position), QString::number(ygfx::shaderlocation_position).toStdString());
    output = std::regex_replace(output, std::regex(ygfx::loc_position_x), QString::number(ygfx::shaderlocation_position_x).toStdString());
    output = std::regex_replace(output, std::regex(ygfx::loc_position_y), QString::number(ygfx::shaderlocation_position_y).toStdString());
    output = std::regex_replace(output, std::regex(ygfx::loc_color), QString::number(ygfx::shaderlocation_color).toStdString());
    output = std::regex_replace(output, std::regex(ygfx::loc_texture), QString::number(ygfx::shaderlocation_texture).toStdString());
    output = std::regex_replace(output, std::regex(ygfx::loc_normal), QString::number(ygfx::shaderlocation_normal).toStdString());
    output = std::regex_replace(output, std::regex(ygfx::loc_additional_1), QString::number(ygfx::shaderlocation_additional_1).toStdString());
    output = std::regex_replace(output, std::regex(ygfx::loc_additional_2), QString::number(ygfx::shaderlocation_additional_2).toStdString());
    return output;
}

static inline std::string glTypeString(GLenum type) {
    switch (type) {
    case GL_FRAGMENT_SHADER:    return "fragment shader";
    case GL_GEOMETRY_SHADER:    return "geometry shader";
    case GL_VERTEX_SHADER:      return "vertex shader";
    default:                    return "UNKNOWN";
    }
}

static inline GLuint createAttachAndCheckShader(const std::string& code, GLuint program, GLenum type, std::string & errorlog) {
    const char* cStringCode = code.c_str();
    GLuint ret = OGLFUNCS glCreateShader(type);
    OGLFUNCS glShaderSource(ret, 1, &cStringCode, nullptr);
    OGLFUNCS glCompileShader(ret);

    // check if shader was compiled correctly
    GLint compiledInt;
    OGLFUNCS glGetShaderiv(ret, GL_COMPILE_STATUS, &compiledInt);
    GLint blen = 0, slen = 0;
    if(compiledInt == GL_FALSE) {
        qDebug().noquote() << "compilation error in " << QString::fromStdString(glTypeString(type));
    }
    OGLFUNCS glGetShaderiv(ret, GL_INFO_LOG_LENGTH, &blen);
    if (blen > 1)
    {
        GLcharARB* compiler_log = static_cast<GLcharARB*>(malloc(size_t(blen)));
        OGLFUNCS glGetShaderInfoLog(ret, blen, &slen, compiler_log);
        qDebug().noquote() << "************** " << QString::fromStdString(glTypeString(type)) << " - compiler log **************\n" << compiler_log << "\n";
        errorlog += compiler_log;
        free (compiler_log);
    }

    // attach shader to program
    OGLFUNCS glAttachShader(program, ret);
    return ret;
}

void Shader::destroyShader() {
    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
            if (shader_id > 0) OGLFUNCS glDeleteProgram(shader_id);
        }
    }
}

std::shared_ptr<Shader> Shader::createShaderFromFileNames(const std::string & fragmentfile,
                                                          const std::string & vertexfile,
                                                          const std::string & geometryfile) {
    std::string vertexcode = "";
    std::string fragmentcode = "";
    std::string geomcode = "";

    if(!vertexfile.empty()) {
        QFile qVFile(QString::fromStdString(vertexfile));
        if(qVFile.open(QFile::ReadOnly)) {
            vertexcode = qVFile.readAll().toStdString();
        } else {
            qWarning().noquote() << QString("%1 - %2").arg(QString::fromStdString(vertexfile), qVFile.errorString());
        }
    }

    if(!fragmentfile.empty()) {
        QFile qFFile(QString::fromStdString(fragmentfile));
        if(qFFile.open(QFile::ReadOnly)) {
            fragmentcode = qFFile.readAll().toStdString();
        } else {
            qWarning().noquote() << QString("%1 - %2").arg(QString::fromStdString(fragmentfile), qFFile.errorString());
        }
    }

    if(!geometryfile.empty()) {
        QFile qGFile(QString::fromStdString(geometryfile));
        if(qGFile.open(QFile::ReadOnly)) {
            geomcode = qGFile.readAll().toStdString();
        } else {
            qWarning().noquote() << QString("%1 - %2").arg(QString::fromStdString(geometryfile), qGFile.errorString());
        }
    }

    return createShaderFromSourceCode(fragmentcode, vertexcode, geomcode);
}

std::shared_ptr<Shader> Shader::createShaderFromSourceCode(const std::string & fragmentcode,
                                                           const std::string & vertexcode,
                                                           const std::string & geometrycode) {
    auto new_shader = std::make_shared<Shader>();

    auto r_vertexcode = replaceLiteralsWithConstants(vertexcode);
    auto r_fragmentcode = replaceLiteralsWithConstants(fragmentcode);
    auto r_geomcode = replaceLiteralsWithConstants(geometrycode);

    std::string error_log_fragment;
    std::string error_log_vertex;
    std::string error_log_geometry;

    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
            GLuint pFragment = 0, pVertex = 0, pGeometry = 0;

            new_shader->errorstate = ShaderErrorState::NO_SHADER_ERROR;
            new_shader->shader_id = OGLFUNCS glCreateProgram();

            if (!r_fragmentcode.empty()) {
                pFragment = createAttachAndCheckShader(r_fragmentcode, new_shader->shader_id, GL_FRAGMENT_SHADER, error_log_fragment);
                if (pFragment == 0 || !error_log_fragment.empty()) {
                    new_shader->errorstate = ShaderErrorState::COMPILER_ERROR;
                    QString log = QString::fromStdString(error_log_fragment);

                    while(log.indexOf('(') >= 0 && log.indexOf(')') >= 0) {
                        auto i = log.indexOf('(') + 1;
                        auto j = log.indexOf(')');
                        auto k = log.indexOf('\n');
                        ShaderError new_error;
                        new_error.LineNumber = log.mid(i, j - i).toInt();
                        if(k >= 0) {
                            new_error.message = log.mid(j + 1, k - j - 1);
                        } else {
                            new_error.message = log.mid(j + 1);
                        }

                        new_shader->fragment_compile_errors.push_back(new_error);
                        log = log.mid(j + 1);
                    }
                }
            }

            if (!r_vertexcode.empty()) {
                pVertex = createAttachAndCheckShader(r_vertexcode, new_shader->shader_id, GL_VERTEX_SHADER, error_log_vertex);
                if (pVertex == 0 || error_log_vertex.size() > 0) {
                    new_shader->errorstate = ShaderErrorState::COMPILER_ERROR;
                    QString log = QString::fromStdString(error_log_vertex);

                    while(log.indexOf('(') >= 0 && log.indexOf(')') >= 0) {
                        auto i = log.indexOf('(') + 1;
                        auto j = log.indexOf(')');
                        auto k = log.indexOf('\n');
                        ShaderError new_error;
                        new_error.LineNumber = log.mid(i, j - i).toInt();
                        if(k >= 0) {
                            new_error.message = log.mid(j + 1, k - j - 1);
                        } else {
                            new_error.message = log.mid(j + 1);
                        }

                        new_shader->vertex_compile_errors.push_back(new_error);
                        log = log.mid(j + 1);
                    }
                }
            }
            if (!r_geomcode.empty()) {
                pGeometry = createAttachAndCheckShader(r_geomcode, new_shader->shader_id, GL_GEOMETRY_SHADER, error_log_geometry);
                if (pGeometry == 0 || error_log_vertex.size() > 0) {
                    new_shader->errorstate = ShaderErrorState::COMPILER_ERROR;
                    QString log = QString::fromStdString(error_log_geometry);

                    while(log.indexOf('(') >= 0 && log.indexOf(')') >= 0) {
                        auto i = log.indexOf('(') + 1;
                        auto j = log.indexOf(')');
                        auto k = log.indexOf('\n');
                        ShaderError new_error;
                        new_error.LineNumber = log.mid(i, j - i).toInt();
                        if(k >= 0) {
                            new_error.message = log.mid(j + 1, k - j - 1);
                        } else {
                            new_error.message = log.mid(j + 1);
                        }

                        new_shader->geometry_compile_errors.push_back(new_error);
                        log = log.mid(j + 1);
                    }
                }
            }
            OGLFUNCS glLinkProgram(new_shader->shader_id);

            if (!r_vertexcode.empty())    OGLFUNCS glDeleteShader(pVertex);
            if (!r_fragmentcode.empty())  OGLFUNCS glDeleteShader(pFragment);
            if (!r_geomcode.empty())      OGLFUNCS glDeleteShader(pGeometry);

        } else {
            qWarning().noquote() << "createShaderFromCode(...) failed - couldn't get QOpenGLExtraFunction from Context";
        }
    } else {
        qWarning().noquote() << "createShaderFromCode(...) failed - couldn't get current QOpenGLContext";
    }

    return new_shader;
}

Shader::Shader() :
    errorstate(ShaderErrorState::UNITIALIZED)
{}

Shader::~Shader() {
    destroyShader();
}

Shader::Shader(const Shader& from) {
    fragment_compile_errors = from.fragment_compile_errors;
    vertex_compile_errors   = from.vertex_compile_errors;
    geometry_compile_errors = from.geometry_compile_errors;
    shader_id       = from.shader_id;
    errorstate      = from.errorstate;
    code_vertex     = from.code_vertex;
    code_fragment   = from.code_fragment;
    code_geometry   = from.code_geometry;
}

Shader& Shader::operator= (const Shader& rhs) {
    fragment_compile_errors = rhs.fragment_compile_errors;
    vertex_compile_errors   = rhs.vertex_compile_errors;
    geometry_compile_errors = rhs.geometry_compile_errors;
    shader_id       = rhs.shader_id;
    errorstate      = rhs.errorstate;
    code_vertex     = rhs.code_vertex;
    code_fragment   = rhs.code_fragment;
    code_geometry   = rhs.code_geometry;

    return *this;
}

Shader& Shader::activate() {
    OGL_IF_FUNCS {
        OGL_IF_CONTEXT {
            if(shader_id != 0) {
                OGLFUNCS glUseProgram(shader_id);
                current_shader_id = shader_id;
            }
        }
    }
    return *this;
}

GLuint Shader::currentActivatedShaderID() {
    return current_shader_id;
}

#define uploadDataToGfxDevicePtr(loc, value, func) \
    if (loc >= 0) { \
        OGLFUNCS func(loc, 1, value); \
    } \
    return *this;

#define uploadDataToGfxDevice(loc, value, func) \
    if (loc >= 0) { \
        OGLFUNCS func(loc, value); \
    } \
    return *this;

#define uploadDataToGfxDeviceByName(loc_name, value, func) \
    OGL_IF_CONTEXT { \
        OGL_IF_FUNCS { \
            GLint uLoc = OGLFUNCS glGetUniformLocation(shader_id, location_name.c_str()); \
            uploadDataToGfxDevice(uLoc, value, func) \
        } \
    } \
    return *this;

#define uploadDataToGfxDevicePtrByName(loc_name, value, func) \
    OGL_IF_CONTEXT { \
        OGL_IF_FUNCS { \
            GLint uLoc = OGLFUNCS glGetUniformLocation(shader_id, location_name.c_str()); \
            uploadDataToGfxDevicePtr(uLoc, value, func) \
        } \
    } \
    return *this;

Shader& Shader::uploadUint(const std::string& location_name, const GLuint value) {
    uploadDataToGfxDeviceByName(location_name, value, glUniform1ui);
}

Shader& Shader::uploadInt(const std::string& location_name, const GLint value) {
    uploadDataToGfxDeviceByName(location_name, value, glUniform1i);
}

Shader &Shader::uploadVec2(const std::string & location_name, const glm::vec2 & value) {
    uploadDataToGfxDevicePtrByName(location_name, glm::value_ptr(value), glUniform2fv);
}

Shader &Shader::uploadVec3(const std::string & location_name, const glm::vec3 & value) {
    uploadDataToGfxDevicePtrByName(location_name, glm::value_ptr(value), glUniform3fv);
}

Shader& Shader::uploadColor3(const std::string& location_name, glm::vec3& color) {
    uploadDataToGfxDevicePtrByName(location_name, glm::value_ptr(color), glUniform3fv);
}

Shader &Shader::uploadFloat(const std::string & location_name, const GLfloat value) {
    uploadDataToGfxDeviceByName(location_name, value, glUniform1f);
}

Shader &Shader::uploadColor3(GLint location, glm::vec3 & color) {
    uploadDataToGfxDevicePtr(location, glm::value_ptr(color), glUniform3fv);
}

Shader& Shader::uploadColor3(const std::string& location_name, const QColor& color) {
    glm::vec3 glmcolor(color.redF(), color.greenF(), color.blueF());
    return uploadColor3(location_name, glmcolor);
}

Shader &Shader::uploadColor3(GLint location, const QColor & color) {
    glm::vec3 glmcolor(color.redF(), color.greenF(), color.blueF());
    return uploadColor3(location, glmcolor);
}

Shader &Shader::uploadColor4(const std::string& location_name, const QColor & color) {
    glm::vec4 glmcolor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    return uploadColor4(location_name, glmcolor);
}

Shader &Shader::uploadColor4(const std::string & location_name, glm::vec4 & color) {
    uploadDataToGfxDevicePtrByName(location_name, glm::value_ptr(color), glUniform4fv);
}

Shader &Shader::uploadColor4(GLint location, glm::vec4 & color) {
    uploadDataToGfxDevicePtr(location, glm::value_ptr(color), glUniform4fv);
}


GLint Shader::getUniformLocationForName(std::string & location_name) {
    GLint uLoc = -1;
    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
            uLoc = OGLFUNCS glGetUniformLocation(shader_id, location_name.c_str());
        }
    }
    return uLoc;
}

Shader& Shader::uploadMatrix44f(const std::string& location_name, const glm::mat4 & matrix) {
    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
            GLint uLoc = OGLFUNCS glGetUniformLocation(shader_id, location_name.c_str());
            if (uLoc >= 0) {
                OGLFUNCS glUniformMatrix4fv(uLoc, 1, GL_FALSE, &matrix[0][0]);
            }
        }
    }
    return *this;
}

Shader &Shader::uploadMatrix44f(GLint location, const glm::mat4 & matrix) {
    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
            OGLFUNCS glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
        }
    }
    return *this;
}



std::vector<ShaderVariableData> Shader::getUniformVariables() const {
    std::vector<ShaderVariableData> ret;

    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
            GLint count, pos;
            const GLsizei buffersize = 16;
            GLsizei size, length;
            GLenum type;
            GLchar name[buffersize];

            OGLFUNCS glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);
            for (GLint i = 0; i < count; ++i) {
                OGLFUNCS glGetActiveUniform(shader_id, static_cast<GLuint>(i), buffersize, &length, &size, &type, name);
                pos = OGLFUNCS glGetUniformLocation(shader_id, name);
                ret.push_back(ShaderVariableData(ShaderVariableData(static_cast<GLVariableType>(type), static_cast<GLuint>(pos), name)));
            }
        }
    }

    return ret;
}

std::vector<ShaderVariableData> Shader::getAttributeVariables() const {
    std::vector<ShaderVariableData> ret;

    OGL_IF_CONTEXT {
        OGL_IF_FUNCS {
            GLint count, pos;
            const GLsizei buffersize = 16;
            GLsizei size, length;
            GLenum type;
            GLchar name[buffersize];

            OGLFUNCS glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTES, &count);

            for (GLint i = 0; i < count; ++i)
            {
                OGLFUNCS glGetActiveAttrib(shader_id, static_cast<GLuint>(i), buffersize, &length, &size, &type, name);
                pos = OGLFUNCS glGetAttribLocation(shader_id, name);
                ret.push_back(ShaderVariableData(ShaderVariableData(static_cast<GLVariableType>(type), static_cast<GLuint>(pos), name)));
            }
        }
    }

    return ret;
}



namespace ygfx {
    std::ostream& operator<< (std::ostream& ostr, const Shader& shader) {
        ostr << "[Shader] - ID: " << shader.shaderID() << std::endl;
        ostr << "- Attributes -" << std::endl;
        const std::vector<ShaderVariableData>& attributes = shader.getAttributeVariables();
        for(size_t i = 0; i < attributes.size(); ++i) {
            ostr << attributes[i] << std::endl;
        }
        ostr << "-- Uniforms --" << std::endl;
        const std::vector<ShaderVariableData>& uniforms = shader.getUniformVariables();
        for(size_t i = 0; i < uniforms.size(); ++i) {
            ostr << uniforms[i] << std::endl;
        }
        return ostr;
    }

    std::ostream& operator<< (std::ostream& ostr, const ShaderVariableData& shaderdata) {
        ostr << "[Shaderdata][" << shaderdata.name << "][" << shaderdata.position << "][" << shaderdata.type << "]";
        return ostr;
    }

    std::ostream& operator<< (std::ostream& ostr, const GLVariableType vartype) {
        switch (vartype) {
        Y_GLVAROSTR(GLVariableType::BOOL)
        Y_GLVAROSTR(GLVariableType::VEC2B)
        Y_GLVAROSTR(GLVariableType::VEC3B)
        Y_GLVAROSTR(GLVariableType::VEC4B)
        Y_GLVAROSTR(GLVariableType::FLOAT)
        Y_GLVAROSTR(GLVariableType::VEC2F)
        Y_GLVAROSTR(GLVariableType::VEC3F)
        Y_GLVAROSTR(GLVariableType::VEC4F)
        Y_GLVAROSTR(GLVariableType::MAT2x2F)
        Y_GLVAROSTR(GLVariableType::MAT3x3F)
        Y_GLVAROSTR(GLVariableType::MAT4x4F)
        Y_GLVAROSTR(GLVariableType::MAT2x3F)
        Y_GLVAROSTR(GLVariableType::MAT2x4F)
        Y_GLVAROSTR(GLVariableType::MAT3x2F)
        Y_GLVAROSTR(GLVariableType::MAT3x4F)
        Y_GLVAROSTR(GLVariableType::MAT4x2F)
        Y_GLVAROSTR(GLVariableType::MAT4x3F)
        Y_GLVAROSTR(GLVariableType::DOUBLE)
        Y_GLVAROSTR(GLVariableType::VEC2D)
        Y_GLVAROSTR(GLVariableType::VEC3D)
        Y_GLVAROSTR(GLVariableType::VEC4D)
        Y_GLVAROSTR(GLVariableType::MAT2x2D)
        Y_GLVAROSTR(GLVariableType::MAT3x3D)
        Y_GLVAROSTR(GLVariableType::MAT4x4D)
        Y_GLVAROSTR(GLVariableType::MAT2x3D)
        Y_GLVAROSTR(GLVariableType::MAT2x4D)
        Y_GLVAROSTR(GLVariableType::MAT3x2D)
        Y_GLVAROSTR(GLVariableType::MAT3x4D)
        Y_GLVAROSTR(GLVariableType::MAT4x2D)
        Y_GLVAROSTR(GLVariableType::MAT4x3D)
        Y_GLVAROSTR(GLVariableType::INTEGER)
        Y_GLVAROSTR(GLVariableType::VEC2I)
        Y_GLVAROSTR(GLVariableType::VEC3I)
        Y_GLVAROSTR(GLVariableType::VEC4I)
        Y_GLVAROSTR(GLVariableType::UINTEGER)
        Y_GLVAROSTR(GLVariableType::VEC2UI)
        Y_GLVAROSTR(GLVariableType::VEC3UI)
        Y_GLVAROSTR(GLVariableType::VEC4UI)
        Y_GLVAROSTR(GLVariableType::SAMPLER_1D)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D)
        Y_GLVAROSTR(GLVariableType::SAMPLER_3D)
        Y_GLVAROSTR(GLVariableType::SAMPLER_CUBE)
        Y_GLVAROSTR(GLVariableType::SAMPLER_1D_SHADOW)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D_SHADOW)
        Y_GLVAROSTR(GLVariableType::SAMPLER_1D_ARRAY)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D_ARRAY)
        Y_GLVAROSTR(GLVariableType::SAMPLER_1D_ARRAY_SHADOW)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D_ARRAY_SHADOW)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D_MULTISAMPLE)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D_MULTISAMPLE_ARRAY)
        Y_GLVAROSTR(GLVariableType::SAMPLER_CUBE_SHADOW)
        Y_GLVAROSTR(GLVariableType::SAMPLER_BUFFER)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D_RECT)
        Y_GLVAROSTR(GLVariableType::SAMPLER_2D_RECT_SHADOW)
        default: { ostr << "UNKNOWN"; break; }
        }
        return ostr;
    }
#undef Y_GLVAROSTR

}
