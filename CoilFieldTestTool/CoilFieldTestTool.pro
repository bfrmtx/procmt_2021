#-------------------------------------------------
#
# Project created by QtCreator 2015-04-14T11:06:01
#
#-------------------------------------------------

## add in Qt Creator under additional arguments for example:
# CONFIG+=gccstyle CONFIG+=procmtbfr

## sytle comes from qmake command line / batch or fro qtcreator

include (../../../../compile.pri)
include (../../../../bins.pri)
QT       += core gui widgets sql printsupport

TARGET = CoilFieldTestTool

INCLUDEPATH += ../prc_com  ../include $${PJH}/src/include/Trunk

SOURCES += main.cpp\
        mainwindow.cpp \
 #   utils.cpp \
    plot.cpp \
    GlobalIncludes.cpp \

HEADERS  += mainwindow.h \
#    utils.h \
    plot.h \
    GlobalIncludes.h \

FORMS    += mainwindow.ui \
    plot.ui


RESOURCES += \
    Pics.qrc


RC_FILE =    myapp.rc


# remove some libs please
CONFIG(release, release|debug) {
LIBS += -lcalib_lab \
        -lmt_site \
        -lmt_data \
        -lxmlcallib \
        -lplotlib \
        -lqcustomplot \
        -lcalibration  \
        -lmeasdocxml \
        -ltinyxml2 \
        -lmath_vector \
        -ldb_sensor_query \
        -lsql_base_tools \
        -lsql_vector

    gcc_win64_style {
    LIBS += -lEGL \
        -lGLESv2
    }

}

CONFIG(debug, debug|release) {
LIBS += -lcalib_labd \
        -lmt_sited \
        -lmt_datad \
        -lxmlcallibd \
        -lplotlibd \
        -lqcustomplotd \
        -lcalibrationd  \
        -lmeasdocxmld \
        -ltinyxml2d \
        -lmath_vectord \
        -ldb_sensor_queryd \
        -lsql_base_toolsd \
        -lsql_vectord
				
		gcc_win64_style {
		LIBS += -lEGLd \		
        -lGLESv2d
		}
}


include (../../../../mc_data.pri)

DISTFILES +=
