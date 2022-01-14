#-------------------------------------------------
#
# Project created by QtCreator 2018-06-22T08:21:05
#
#-------------------------------------------------
include (../../../../compile.pri)
QT       -= gui

TARGET = mt_data


SOURCES += \
        mt_data.cpp

HEADERS += \
        mt_data.h


include (../../../../stat_libs.pri)

buckets.path = $${PJH}/$${OSDIR}$${CPLDIR}/include
buckets.files = $${PJH}/src/procmt/Trunk/include/buckets.h
INSTALLS +=  buckets
