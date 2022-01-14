#-------------------------------------------------
#
# Project created by QtCreator 2019-02-17T09:18:20
#
#-------------------------------------------------
include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql printsupport widgets

TARGET = ediplotter_new

SOURCES += \
        main.cpp \
        ediplotter.cpp \
    dualplot.cpp \
    ediplot.cpp \
    plotoptions.cpp

HEADERS += \
        ediplotter.h \
    dualplot.h \
    ediplot.h \
    plotoptions.h

FORMS += \
        ediplotter.ui \
    dualplot.ui \
    plotoptions.ui

CONFIG(release, release|debug) {
LIBS += -lmsg_logger \
    -ledi_file \
    -lprc_com \
    -lqcustomplot \
    -leQDateTime \
    -lmt_data \
    -lgeocoordinates
}

CONFIG(debug, debug|release) {
LIBS += -lmsg_loggerd \
    -ledi_filed \
    -lprc_comd  \
    -lqcustomplotd \
    -leQDateTimed \
    -lmt_datad \
    -lgeocoordinatesd\
}
