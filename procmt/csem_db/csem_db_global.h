#ifndef CSEM_DB_GLOBAL_H
#define CSEM_DB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CSEM_DB_LIBRARY)
#  define CSEM_DB_EXPORT Q_DECL_EXPORT
#else
#  define CSEM_DB_EXPORT Q_DECL_IMPORT
#endif

#endif // CSEM_DB_GLOBAL_H
