#ifndef MATH_VECTOR_GLOBAL_H
#define MATH_VECTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MATH_VECTOR_LIBRARY)
#  define MATH_VECTOR_EXPORT Q_DECL_EXPORT
#else
#  define MATH_VECTOR_EXPORT Q_DECL_IMPORT
#endif

#endif // MATH_VECTOR_GLOBAL_H
