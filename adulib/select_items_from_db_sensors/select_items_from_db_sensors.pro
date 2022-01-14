include (../../../../compile.pri)

QT       += core gui sql widgets

TARGET = select_items_from_db_sensors
DEFINES += SELECT_ITEMS_FROM_DB_SENSORS_LIBRARY


SOURCES += select_items_from_db_sensors.cpp

HEADERS += select_items_from_db_sensors.h

FORMS += select_items_from_db_sensors.ui



CONFIG(release, release|debug) {
LIBS += -ldb_sensor_query
}

CONFIG(debug, debug|release) {
LIBS +=  -ldb_sensor_queryd
}


include(../../../../stat_libs.pri)
