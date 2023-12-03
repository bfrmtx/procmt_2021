#ifndef CALSTAT_GLOBAL_H
#define CALSTAT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CALSTAT_LIBRARY)
#  define CALSTAT_EXPORT Q_DECL_EXPORT
#else
#  define CALSTAT_EXPORT Q_DECL_IMPORT
#endif

#endif // CALSTAT_GLOBAL_H
