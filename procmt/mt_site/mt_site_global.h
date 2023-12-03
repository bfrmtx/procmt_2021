#ifndef MT_SITE_GLOBAL_H
#define MT_SITE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MT_SITE_LIBRARY)
#  define MT_SITE_EXPORT Q_DECL_EXPORT
#else
#  define MT_SITE_EXPORT Q_DECL_IMPORT
#endif

#endif // MT_SITE_GLOBAL_H
