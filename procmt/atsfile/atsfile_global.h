#ifndef ATSFILE_GLOBAL_H
#define ATSFILE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ATSFILE_LIBRARY)
#  define ATSFILE_EXPORT Q_DECL_EXPORT
#else
#  define ATSFILE_EXPORT Q_DECL_IMPORT
#endif

#endif // ATSFILE_GLOBAL_H
