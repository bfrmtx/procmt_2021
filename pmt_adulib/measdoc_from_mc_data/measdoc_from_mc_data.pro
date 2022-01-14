include (../../../../compile.pri)

QT -= gui
QT += sql

DEFINES += PMEASDOC_FROM_MC_DATA_LIBRARY

SOURCES += \
    measdoc_from_mc_data.cpp

HEADERS += \
    measdoc_from_mc_data.h

CONFIG(release, release|debug) {
LIBS += -laduxml -lmeasdocxml -ltinyxml2 -ltinyxmlwriter -lgeocoordinates  -leQDateTime -lcalibration
}

CONFIG(debug, debug|release) {
LIBS += -laduxmld -lmeasdocxmld -ltinyxml2d -ltinyxmlwriterd -lgeocoordinatesd  -leQDateTimed -lcalibrationd
}



include(../../../../stat_libs.pri)
