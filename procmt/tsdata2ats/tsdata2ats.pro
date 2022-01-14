#-------------------------------------------------
#
# Project created by QtCreator 2016-11-26T10:27:29
#
#-------------------------------------------------

include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql widgets


TARGET = tsdata2ats


SOURCES += main.cpp\
        mainwindow.cpp \
    tsdata_header.cpp

HEADERS  += mainwindow.h \
    tsdata_header.h

FORMS    += mainwindow.ui

include (../../../../mc_data.pri)


