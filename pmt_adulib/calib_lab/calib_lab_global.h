#ifndef CALIB_LAB_GLOBAL_H
#define CALIB_LAB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CALIB_LAB_LIBRARY)
#  define CALIB_LAB_EXPORT Q_DECL_EXPORT
#else
#  define CALIB_LAB_EXPORT Q_DECL_IMPORT
#endif

#endif // CALIB_LAB_GLOBAL_H
