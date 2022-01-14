#-------------------------------------------------
#
# Project created by QtCreator 2018-12-02T11:43:12
#
#-------------------------------------------------
include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql widgets

TARGET = ts_tools

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui



CONFIG(release, release|debug) {
LIBS += -latsfile \
        -latsheader \
        -latmfile \
        -leQDateTime \
        -lprc_com \
        -lcalibration \
        -lgroup_atsfiles \
        -llineedit_file \
        -lprc_com_table \
        -laduxml \
        -lqstring_utilities \
        -lgeocoordinates \
        -ltinyxmlwriter \
        -lgui_items \
        -lgeocoordinates \
        -lphoenix_json \
        -ladu_json
}

CONFIG(debug, debug|release) {
LIBS += -latsfiled \
        -latsheaderd \
        -latmfiled \
        -leQDateTimed \
        -lprc_comd \
        -lcalibrationd \
        -lgroup_atsfilesd \
        -llineedit_filed \
        -lprc_com_tabled \
        -laduxmld \
        -lqstring_utilitiesd \
        -lgeocoordinatesd \
        -ltinyxmlwriterd \
        -lgui_itemsd \
        -lgeocoordinatesd \
        -lphoenix_jsond\
        -ladu_jsond



}

include (../../../../mc_data.pri)

