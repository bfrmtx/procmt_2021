include (../../../../compile.pri)

QT -= gui
QT += sql

TARGET = group_atsfiles

DEFINES += GROUP_ATSFILES_LIBRARY

SOURCES += \
    group_atsfiles.cpp

HEADERS += \
    group_atsfiles.h

CONFIG(release, release|debug) {
LIBS += -laduxml -lmeasdocxml -ltinyxml2 -ltinyxmlwriter -lgeocoordinates  -leQDateTime
}

CONFIG(debug, debug|release) {
LIBS += -laduxmld -lmeasdocxmld -ltinyxml2d -ltinyxmlwriterd -lgeocoordinatesd  -leQDateTimed
}



include(../../../../stat_libs.pri)
