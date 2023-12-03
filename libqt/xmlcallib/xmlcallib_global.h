#ifndef XMLCALLIB_GLOBAL_H
#define XMLCALLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XMLCALLIB_LIBRARY)
#  define XMLCALLIB_EXPORT Q_DECL_EXPORT
#else
#  define XMLCALLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // XMLCALLIB_GLOBAL_H
