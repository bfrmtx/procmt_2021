#ifndef YGFX_GL_INCLUDES_H
#define YGFX_GL_INCLUDES_H

//#define OGLDEBUGENABLED
//#define ENABLE_STOPWATCHES 1

#ifdef QT_CORE_LIB
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QDebug>
#define OGL_IF_CONTEXT if(QOpenGLContext::currentContext() != nullptr)
#define OGL_IF_FUNCS if(QOpenGLContext::currentContext()->extraFunctions() != nullptr)
#define OGLFUNCS QOpenGLContext::currentContext()->extraFunctions()->
#define OGLFUNCSP QOpenGLContext::currentContext()->functions()->
#define GFX_OSTREAM QDebug
#define GFX_OUT qDebug().noquote()
#define Y_ENDL ""
#define QCOLOR_TO_GLM(a) glm::vec4(a.redF(), a.greenF(), a.blueF(), a.alphaF())
#else
#include <iostream>
#include <sstream>
#include <gl/GL.h>
#define OGL_IF_CONTEXT if(true)
#define OGL_IF_FUNCS if(true)
#define OGLFUNCS
#define GFX_OSTREAM std::ostream
#define GFX_OUT std::cout
#define Y_ENDL std::endl
#endif

#endif // GFXINCLUDES_H
