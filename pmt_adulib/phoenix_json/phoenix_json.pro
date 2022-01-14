include (../../../../compile.pri)


QT -= gui
QT += sql


SOURCES += \
    phoenix_json.cpp

HEADERS += \
    phoenix_json.h

CONFIG(release, release|debug) {
LIBS += -latsfile \
        -latsheader \
        -lmeasdocxml \
        -ltinyxml2 \
        -ltinyxmlwriter \
        -lsql_base_tools \
        -ldb_sensor_query \
        -lmath_vector

}

CONFIG(debug, debug|release) {
LIBS += -latsfiled \
        -latsheaderd \
        -lmeasdocxmld \
        -ltinyxml2d \
        -ltinyxmlwriterd \
        -lsql_base_toolsd \
        -ldb_sensor_queryd \
        -lmath_vectord




}



include(../../../../stat_libs.pri)
