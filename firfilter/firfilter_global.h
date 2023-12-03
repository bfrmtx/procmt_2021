#ifndef FIRFILTER_GLOBAL_H
#define FIRFILTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FIRFILTER_LIBRARY)
#  define FIRFILTER_EXPORT Q_DECL_EXPORT
#else
#  define FIRFILTER_EXPORT Q_DECL_IMPORT
#endif

#endif // FIRFILTER_GLOBAL_H
