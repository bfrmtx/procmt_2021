#ifndef ACSP_PRZ_GLOBAL_H
#define ACSP_PRZ_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ACSP_PRZ_LIBRARY)
#  define ACSP_PRZ_EXPORT Q_DECL_EXPORT
#else
#  define ACSP_PRZ_EXPORT Q_DECL_IMPORT
#endif

#endif // ACSP_PRZ_GLOBAL_H
