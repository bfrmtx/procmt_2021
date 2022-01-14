#-------------------------------------------------
#
# Project created by QtCreator 2019-01-18T16:13:43
#
#-------------------------------------------------
include (../../../../compile.pri)

QT       += sql core
QT       -= gui

TARGET = calib_lab

SOURCES += \
        calib_lab.cpp

HEADERS +=  calib_lab.h \
    parallel_test_coherency.h


include (../../../../mc_data.pri)

CONFIG(release, release|debug) {
LIBS += -lmt_data \
        -lmt_site \
}


CONFIG(debug, debug|release) {
LIBS += -lmt_sited \
        -lmt_datad
}

include (../../../../stat_libs.pri)

