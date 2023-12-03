#ifndef ATSHEADER_GLOBAL_H
#define ATSHEADER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ATSHEADER_LIBRARY)
#  define ATSHEADER_EXPORT Q_DECL_EXPORT
#else
#  define ATSHEADER_EXPORT Q_DECL_IMPORT
#endif

#endif // ATSHEADER_GLOBAL_H
