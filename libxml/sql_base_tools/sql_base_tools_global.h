#ifndef SQL_BASE_TOOLS_GLOBAL_H
#define SQL_BASE_TOOLS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SQL_BASE_TOOLS_LIBRARY)
#  define SQL_BASE_TOOLS_EXPORT Q_DECL_EXPORT
#else
#  define SQL_BASE_TOOLS_EXPORT Q_DECL_IMPORT
#endif

#endif // SQL_BASE_TOOLS_GLOBAL_H