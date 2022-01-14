#-------------------------------------------------
#
# Project created by QtCreator 2018-02-07T11:06:24
#
#-------------------------------------------------
include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql widgets

TARGET = mt_gensign


SOURCES += \
        main.cpp \
        mt_gensign.cpp

HEADERS += \
        mt_gensign.h

FORMS += \
        mt_gensign.ui

include (../../../../mc_data.pri)


CONFIG(release, release|debug) {
LIBS += -laduxml \
        -lgui_items \
        -ltinyxmlwriter \
        -lgeocoordinates \
        -lcalibration \
        -lsql_base_tools \
        -ldb_sensor_query \
        -lmath_vector \
        -lmeasdocxml \
        -ltinyxml2
}

CONFIG(debug, debug|release) {
LIBS += -laduxmld \
        -lgui_itemsd \
        -ltinyxmlwriterd \
        -lgeocoordinatesd \
        -lcalibrationd \
        -lsql_base_toolsd \
        -ldb_sensor_queryd \
        -lmath_vectord \
        -lmeasdocxmld \
        -ltinyxml2d
}





