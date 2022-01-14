#-------------------------------------------------
#
# Project created by QtCreator 2017-08-30T14:20:50
#
#-------------------------------------------------
include (../../../../compile.pri)

QT       += sql
QT       -= gui

TARGET = mt_site
DEFINES += MT_SITE_LIBRARY


SOURCES += \
        mt_site.cpp

HEADERS += \
        mt_site.h \
        unused.h


CONFIG(release, release|debug) {
LIBS += -lprc_com  -lmt_data -lmath_vector


}
CONFIG(debug, debug|release) {
LIBS += -lprc_comd  -lmt_datad -lmath_vectord

}



include (../../../../stat_libs.pri)

buckets.path = $${PJH}/$${OSDIR}$${CPLDIR}/include
buckets.files = $${PJH}/src/procmt/Trunk/include/buckets.h
INSTALLS +=  buckets
