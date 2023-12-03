#ifndef ATMFILE_GLOBAL_H
#define ATMFILE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ATMFILE_LIBRARY)
#  define ATMFILE_EXPORT Q_DECL_EXPORT
#else
#  define ATMFILE_EXPORT Q_DECL_IMPORT
#endif

#endif // ATMFILE_GLOBAL_H
