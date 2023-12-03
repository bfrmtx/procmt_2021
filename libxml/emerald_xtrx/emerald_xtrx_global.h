#ifndef EMERALD_XTRX_GLOBAL_H
#define EMERALD_XTRX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EMERALD_XTRX_LIBRARY)
#  define EMERALD_XTRX_EXPORT Q_DECL_EXPORT
#else
#  define EMERALD_XTRX_EXPORT Q_DECL_IMPORT
#endif

#endif // EMERALD_XTRX_GLOBAL_H
