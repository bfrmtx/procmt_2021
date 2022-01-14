include (../../../../compile.pri)

QT       += sql
QT       -= gui

TARGET = csem_db
DEFINES += CSEM_DB_LIBRARY

SOURCES += \
    csem_db.cpp

HEADERS += \
    csem_db.h

include (../../../../stat_libs.pri)
