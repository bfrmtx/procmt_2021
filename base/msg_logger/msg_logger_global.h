#ifndef MSG_LOGGER_GLOBAL_H
#define MSG_LOGGER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MSG_LOGGER_LIBRARY)
#  define MSG_LOGGER_EXPORT Q_DECL_EXPORT
#else
#  define MSG_LOGGER_EXPORT Q_DECL_IMPORT
#endif

#endif // MSG_LOGGER_GLOBAL_H
