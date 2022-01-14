
include (../../../../compile.pri)
include (../../../../bins.pri)

QT += core sql
QT -= gui

TARGET = procmt
CONFIG += console

SOURCES += main.cpp

include (../../../../mc_data.pri)


CONFIG(release, release|debug) {
LIBS += -lmt_site \
        -lmt_data \
        -ledi_file \
        -lgeocoordinates \
        -lqstring_utilities


}
CONFIG(debug, debug|release) {
LIBS += -lmt_sited \
        -lmt_datad \
        -ledi_filed \
        -lgeocoordinatesd \
        -lqstring_utilitiesd
}


sql.path = $${PJH}/$${OSDIR}$${CPLDIR}/bin
sql.files = $${PJH}/src/doc/Trunk/tables/info.sql3 ${PJH}/src/doc/Trunk/tables/master_calibration.sql3
INSTALLS +=  sql

