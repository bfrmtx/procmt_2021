#-------------------------------------------------
#
# Project created by QtCreator 2019-05-25T07:00:10
#
#-------------------------------------------------

include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql widgets

TARGET = ats_multi_edit

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += ../include  \
            $${PJH}/src/include/Trunk


CONFIG(release, release|debug) {
LIBS += -lprc_com \
        -lprc_com_table \
        -latsheader \
        -latsfile \
        -lcalibration \
        -lmeasdocxml \
        -ltinyxml2 \
        -ltinyxmlwriter \
        -lsql_base_tools \
        -ldb_sensor_query \
        -lmath_vector \
        -leQDateTime \
        -lgeocoordinates \
        -lmsg_logger


}

CONFIG(debug, debug|release) {
LIBS += -lprc_comd \
        -lprc_com_tabled \
        -latsheaderd \
        -latsfiled \
        -lcalibrationd \
        -lmeasdocxmld \
        -ltinyxml2d \
        -ltinyxmlwriterd \
        -lsql_base_toolsd \
        -ldb_sensor_queryd \
        -lmath_vectord \
        -leQDateTimed \
        -lgeocoordinatesd \
        -lmsg_loggerd

}

