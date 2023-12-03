#ifndef SQL_VECTOR_GLOBAL_H
#define SQL_VECTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SQL_VECTOR_LIBRARY)
#  define SQL_VECTOR_EXPORT Q_DECL_EXPORT
#else
#  define SQL_VECTOR_EXPORT Q_DECL_IMPORT
#endif

#endif // SQL_VECTOR_GLOBAL_H
