#
include (../../../../compile.pri)

QT += core gui network sql widgets

TARGET = aduxml
DEFINES += ADUXML_LIBRARY

SOURCES += \
    adulib.cpp \
    aduslot.cpp \
    channel.cpp \
    adbboard.cpp \
    cable.cpp \
    gps.cpp \
    sensor.cpp \
    adutable.cpp \
    adujoblist.cpp \
    stoptime_dialog.cpp \
    adunetwork.cpp \
    adurt.cpp \
    txm.cpp


HEADERS += \
    adulib.h \
    aduslot.h \
    channel.h \
    adbboard.h \
    cable.h \
    gps.h \
    sensor.h \
    adutable.h \
    adujoblist.h \
    stoptime_dialog.h \
    adunetwork.h \
    adurt.h \
    txm.h

CONFIG(release, release|debug) {
LIBS += -lmeasdocxml -ltinyxml2 -ltinyxmlwriter  -leQDateTime -lnum_keypad -lselect_items_from_db_sensors
}

CONFIG(debug, debug|release) {
LIBS += -lmeasdocxmld -ltinyxml2d -ltinyxmlwriterd  -leQDateTimed -lnum_keypadd -lselect_items_from_db_sensorsd
}



#install a database in the bin directory
#target.path =  $${PJH}/$${OSDIR}$${CPLDIR}/lib
sql.path = $${PJH}/$${OSDIR}$${CPLDIR}/bin
sql.files = $${PJH}/src/doc/Trunk/tables/info.sql3 $${PJH}/src/doc/Trunk/tables/master_calibration.sql3
INSTALLS +=  sql

CONFIG(debug, debug|release) {
# the make_proglibs.php should also install these for using inside qtcreator try here "double manner for debug"
# for relase we should better use the scripts
sqldebug.path = $${PJH}/$${OSDIR}$${CPLDIR}/adulib/debug/bin
sqldebug.files = $${PJH}/src/doc/Trunk/tables/info.sql3 $${PJH}/src/doc/Trunk/tables/master_calibration.sql3
INSTALLS +=  sqldebug
}

win32 {
xssl.path = $${PJH}/$${OSDIR}$${CPLDIR}/bin
xssl.files = $${PJH}/src/doc/Trunk/ssl/*
INSTALLS +=  xssl
}




FORMS += \
#    adugui_main.ui \
    stoptime_dialog.ui

include(../../../../stat_libs.pri)
