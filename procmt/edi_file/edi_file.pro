#-------------------------------------------------
#
# Project created by QtCreator 2018-05-04T11:15:46
#
#-------------------------------------------------
include (../../../../compile.pri)

QT       -= gui

TARGET = edi_file

DEFINES += EDI_FILE_LIBRARY


SOURCES += \
        edi_file.cpp

HEADERS += \
        edi_file.h

include (../../../../mc_data.pri)


CONFIG(release, release|debug) {
LIBS += -lprc_comd -leQDateTimed -lmt_data -lgeocoordinates
}

CONFIG(debug, debug|release) {
LIBS += -lprc_comd  -leQDateTimed -lmt_datad -lgeocoordinatesd
}

include (../../../../stat_libs.pri)

buckets.path = $${PJH}/$${OSDIR}$${CPLDIR}/include
buckets.files = $${PJH}/src/procmt/Trunk/include/buckets.h
INSTALLS +=  buckets
