#ifndef FFTREAL_VECTOR_GLOBAL_H
#define FFTREAL_VECTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FFTREAL_VECTOR_LIBRARY)
#  define FFTREAL_VECTOR_EXPORT Q_DECL_EXPORT
#else
#  define FFTREAL_VECTOR_EXPORT Q_DECL_IMPORT
#endif

#endif // FFTREAL_VECTOR_GLOBAL_H
