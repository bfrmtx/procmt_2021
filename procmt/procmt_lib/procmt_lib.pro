#-------------------------------------------------
#
# Project created by QtCreator 2018-07-15T09:49:42
#
#-------------------------------------------------
include (../../../../compile.pri)

QT       += sql

QT       -= gui

TARGET = procmt_lib

DEFINES += PROCMT_LIB_LIBRARY


SOURCES += \
        procmt_lib.cpp

HEADERS += \
        procmt_lib.h

include (../../../../mc_data.pri)


CONFIG(release, release|debug) {
LIBS += -ledi_file \
        -lqstring_utilities
}

CONFIG(debug, debug|release) {
LIBS += -ledi_filed \
        -lqstring_utilitiesd
}


include (../../../../stat_libs.pri)
