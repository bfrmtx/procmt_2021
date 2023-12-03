#ifndef PLOTLIB_GLOBAL_H
#define PLOTLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLOTLIB_LIBRARY)
#  define PLOTLIB_EXPORT Q_DECL_EXPORT
#else
#  define PLOTLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // PLOTLIB_GLOBAL_H
