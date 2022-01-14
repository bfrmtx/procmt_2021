include (../../../../compile.pri)
include (../../../../bins.pri)

QT += core sql
QT -= gui

TARGET = atstool
CONFIG += console


SOURCES += main.cpp

include (../../../../mc_data.pri)


CONFIG(release, release|debug) {
LIBS += -lacsp_prz \
        -ltinyxmlwriter \
        -lmt_site \
        -lmt_data \
        -ledi_file \
        -lgeocoordinates \
        -lqstring_utilities
}

CONFIG(debug, debug|release) {
LIBS += -lacsp_przd \
        -ltinyxmlwriterd \
        -lmt_sited \
        -lmt_datad \
        -ledi_filed \
        -lgeocoordinatesd \
        -lqstring_utilitiesd
}
