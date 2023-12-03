#ifndef PROCMT_LIB_GLOBAL_H
#define PROCMT_LIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PROCMT_LIB_LIBRARY)
#  define PROCMT_LIB_EXPORT Q_DECL_EXPORT
#else
#  define PROCMT_LIB_EXPORT Q_DECL_IMPORT
#endif

#endif // PROCMT_LIB_GLOBAL_H
