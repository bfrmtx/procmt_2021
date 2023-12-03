#ifndef MC_DATA_GLOBAL_H
#define MC_DATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MC_DATA_LIBRARY)
#  define MC_DATA_EXPORT Q_DECL_EXPORT
#else
#  define MC_DATA_EXPORT Q_DECL_IMPORT
#endif

#endif // MC_DATA_GLOBAL_H
