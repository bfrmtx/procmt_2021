#ifndef PRC_COM_GLOBAL_H
#define PRC_COM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PRC_COM_LIBRARY)
#  define PRC_COM_EXPORT Q_DECL_EXPORT
#else
#  define PRC_COM_EXPORT Q_DECL_IMPORT
#endif

#endif // PRC_COM_GLOBAL_H
