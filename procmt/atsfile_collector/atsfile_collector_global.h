#ifndef ATSFILE_COLLECTOR_GLOBAL_H
#define ATSFILE_COLLECTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ATSFILE_COLLECTOR_LIBRARY)
#  define ATSFILE_COLLECTOR_EXPORT Q_DECL_EXPORT
#else
#  define ATSFILE_COLLECTOR_EXPORT Q_DECL_IMPORT
#endif

#endif // ATSFILE_COLLECTOR_GLOBAL_H
