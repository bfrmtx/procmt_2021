#-------------------------------------------------
#
# Project created by QtCreator 2016-12-07T10:24:43
#
#-------------------------------------------------
include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql widgets

TARGET = atsfilter


SOURCES += main.cpp\
        filter.cpp \
        low_high_band_passses.cpp

HEADERS  += filter.h \
         low_high_band_passses.h

FORMS    += filter.ui \
         low_high_band_passses.ui


CONFIG(release, release|debug) {
LIBS += -laduxml \
        -ltinyxmlwriter \
        -lgeocoordinates \
        -lgui_items \
        -lnum_keypad \
        -lselect_items_from_db_sensors
}

CONFIG(debug, debug|release) {
LIBS += -laduxmld \
        -ltinyxmlwriterd \
        -lgeocoordinatesd \
        -lgui_itemsd \
        -lnum_keypadd \
        -lselect_items_from_db_sensorsd
}

include (../../../../mc_data.pri)
