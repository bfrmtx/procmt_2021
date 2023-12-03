#ifndef EMERALD_GLOBAL_H
#define EMERALD_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EMERALD_LIBRARY)
#  define EMERALD_EXPORT Q_DECL_EXPORT
#else
#  define EMERALD_EXPORT Q_DECL_IMPORT
#endif

#endif // EMERALD_GLOBAL_H
