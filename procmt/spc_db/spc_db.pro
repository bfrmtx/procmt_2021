include (../../../../compile.pri)

QT       += sql
QT       -= gui

TARGET = spc_db
DEFINES +=  SPC_DB_LIBRARY

SOURCES += \
    spc_db.cpp

HEADERS += \
    spc_db.h

CONFIG(release, release|debug) {
LIBS += -lprc_com  -lmt_data -lmath_vector


}
CONFIG(debug, debug|release) {
LIBS += -lprc_comd  -lmt_datad -lmath_vectord

}


include (../../../../stat_libs.pri)
