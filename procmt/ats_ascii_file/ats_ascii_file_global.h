#ifndef ATS_ASCII_FILE_GLOBAL_H
#define ATS_ASCII_FILE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ATS_ASCII_FILE_LIBRARY)
#  define ATS_ASCII_FILE_EXPORT Q_DECL_EXPORT
#else
#  define ATS_ASCII_FILE_EXPORT Q_DECL_IMPORT
#endif

#endif // ATS_ASCII_FILE_GLOBAL_H
