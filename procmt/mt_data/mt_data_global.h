#ifndef MT_DATA_GLOBAL_H
#define MT_DATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MT_DATA_LIBRARY)
#  define MT_DATA_EXPORT Q_DECL_EXPORT
#else
#  define MT_DATA_EXPORT Q_DECL_IMPORT
#endif

#endif // MT_DATA_GLOBAL_H
