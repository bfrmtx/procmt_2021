#-------------------------------------------------
#
# Project created by QtCreator 2017-04-26T14:39:10
#
#-------------------------------------------------

include (../../../../compile.pri)

QT       -= gui
QT       += sql

TARGET = emerald

DEFINES += EMERALD_LIBRARY


SOURCES += emerald.cpp

HEADERS += emerald.h\

INCLUDEPATH += ../include  \
            ../prc_com \
            ../atsheader \
            ../atsfile \
            $${PJH}/src/include/Trunk

CONFIG(release, release|debug) {
LIBS += -leQDateTime -lfftreal_vector -lprc_com -latsheader -latsfile \
        -lmsg_logger \
        -lcalibration  \
        -ldb_sensor_query \
        -lsql_base_tools
}

CONFIG(debug, debug|release) {
LIBS += -leQDateTimed -lfftreal_vectord  -lprc_comd -latsheaderd -latsfiled \
        -lmsg_loggerd \
        -lcalibrationd  \
        -ldb_sensor_queryd \
        -lsql_base_toolsd
}



include (../../../../stat_libs.pri)
