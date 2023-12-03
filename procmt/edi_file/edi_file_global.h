#ifndef EDI_FILE_GLOBAL_H
#define EDI_FILE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EDI_FILE_LIBRARY)
#  define EDI_FILE_EXPORT Q_DECL_EXPORT
#else
#  define EDI_FILE_EXPORT Q_DECL_IMPORT
#endif

#endif // EDI_FILE_GLOBAL_H
