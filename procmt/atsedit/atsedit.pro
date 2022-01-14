#-------------------------------------------------
#
# Project created by QtCreator 2017-02-18T15:57:08
#
#-------------------------------------------------
include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql widgets

TARGET = atsedit

SOURCES += main.cpp\
        atsedit.cpp \
        overwrite_dialog.cpp

HEADERS  += atsedit.h \
    overwrite_dialog.h

FORMS    += atsedit.ui \
    overwrite_dialog.ui

INCLUDEPATH += ../include  \
            ../prc_com \
            ../atsheader \
            ../atsfile \
            ../ats_ascii_file \
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
        -lmsg_logger \
        -lgui_items


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
        -lmsg_loggerd \
        -lgui_itemsd

}


