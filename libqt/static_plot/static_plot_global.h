#ifndef STATIC_PLOT_GLOBAL_H
#define STATIC_PLOT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STATIC_PLOT_LIBRARY)
#  define STATIC_PLOT_EXPORT Q_DECL_EXPORT
#else
#  define STATIC_PLOT_EXPORT Q_DECL_IMPORT
#endif

#endif // STATIC_PLOT_GLOBAL_H
