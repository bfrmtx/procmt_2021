#ifndef QCUSTOMPLOT_GLOBAL_H
#define QCUSTOMPLOT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QCUSTOMPLOT_LIBRARY)
#  define QCUSTOMPLOT_EXPORT Q_DECL_EXPORT
#else
#  define QCUSTOMPLOT_EXPORT Q_DECL_IMPORT
#endif

#endif // QCUSTOMPLOT_GLOBAL_H
