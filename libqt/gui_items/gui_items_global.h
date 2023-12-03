#ifndef GUI_ITEMS_GLOBAL_H
#define GUI_ITEMS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GUI_ITEMS_LIBRARY)
#  define GUI_ITEMS_EXPORT Q_DECL_EXPORT
#else
#  define GUI_ITEMS_EXPORT Q_DECL_IMPORT
#endif

#endif // GUI_ITEMS_GLOBAL_H
