#-------------------------------------------------
#
# Project created by QtCreator 2019-02-17T09:11:20
#
#-------------------------------------------------

include (../../../../compile.pri)
#include (../../../../bins.pri)

QT       += core gui printsupport sql


TARGET = spectra_plotter
DEFINES += SPECTRA_PLOTTER_LIBRARY

SOURCES += \
        main.cpp \
        spectra_plotter.cpp \
        plotwidget.cpp \
        plotdata.cpp \
        graph_selector.cpp \
        progressdialog.cpp \
        qcustomplot.cpp

HEADERS += \
        spectra_plotter.h \
        plotwidget.h \
        plotdata.h \
        graph_selector.h \
        progressdialog.h \
        qcustomplot.h

FORMS += \
        spectra_plotter.ui \
        progressdialog.ui

include (../../../../mc_data.pri)


CONFIG(release, release|debug) {
LIBS += -lacsp_prz \
        -lmt_site \
        -lmath_vector \
        -lmt_data \
        -ledi_file \
        -lgeocoordinates \
        -lqstring_utilities
}

CONFIG(debug, debug|release) {
LIBS += -lacsp_przd \
        -lmt_sited \
        -lmath_vectord \
        -lmt_datad \
        -ledi_filed \
        -lgeocoordinatesd \
        -lqstring_utilitiesd
}

include (../../../../stat_libs.pri)
