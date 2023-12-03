#ifndef SPC_DB_GLOBAL_H
#define SPC_DB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SPC_DB_LIBRARY)
#  define SPC_DB_EXPORT Q_DECL_EXPORT
#else
#  define SPC_DB_EXPORT Q_DECL_IMPORT
#endif

#endif // SPC_DB_GLOBAL_H
