#ifndef MESTPLOTLIB_GLOBAL_H
#define MESTPLOTLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MESTPLOTLIB_LIBRARY)
#  define MESTPLOTLIB_EXPORT Q_DECL_EXPORT
#else
#  define MESTPLOTLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // MESTPLOTLIB_GLOBAL_H
