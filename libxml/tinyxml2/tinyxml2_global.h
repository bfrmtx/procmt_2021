#ifndef TINYXML2_GLOBAL_H
#define TINYXML2_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TINYXML2_LIBRARY)
#  define TINYXML2_EXPORT Q_DECL_EXPORT
#else
#  define TINYXML2_EXPORT Q_DECL_IMPORT
#endif

#endif // TINYXML2_GLOBAL_H
