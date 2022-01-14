#-------------------------------------------------
#
# Project created by QtCreator 2015-11-29T17:40:51
#
#-------------------------------------------------
include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui sql widgets sql printsupport network xml


TARGET = tsplotter


SOURCES += main.cpp\
    addressdialog.cpp \
    qextnetworkaccessmanager.cpp \
    qwebdav.cpp \
    qwebdavdialog.cpp \
    tsnetworkhandler.cpp \
    tsplotter.cpp \
    xmlreader.cpp \
    plot.cpp \
    qthelper.cpp \
    gotosampledialog.cpp \
    manualscalingdialog.cpp \
    #        qcustomplot.cpp \


HEADERS  += tsplotter.h \
    addressdialog.h \
            namespace.h \
    qextnetworkaccessmanager.h \
    qwebdav.h \
    qwebdavdialog.h \
    tsnetworkhandler.h \
    xmlreader.h \
    plot.h \
    qthelper.h \
    gotosampledialog.h \
    manualscalingdialog.h \
    #            qcustomplot.h \


FORMS    += tsplotter.ui \
    addressdialog.ui \
    gotosampledialog.ui \
    manualscalingdialog.ui \
    qwebdavdialog.ui

CONFIG(release, release|debug) {
  LIBS += -lqcustomplot

}

CONFIG(debug, debug|release) {
  LIBS +=  lqcustomplotd
}

CONFIG(local) {
    message("success")
    INCLUDEPATH += E:/projekte/arbeit/metronix/win10_64_vs2019_msvc/include
    DEPENDPATH += E:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib
}

CONFIG(release, release|debug) {
LIBS += -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lxmlcallib \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lgui_items \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lplotlib \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lqcustomplot \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lcalibration  \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lmeasdocxml \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -ltinyxml2 \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -ltinyxmlwriter \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lmath_vector \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -ldb_sensor_query \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lsql_base_tools \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lsql_vector \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lgeocoordinates \
        -LE:/projekte/arbeit/metronix/win10_64_vs2019_msvc/lib -lqstring_utilities
}

CONFIG(debug, debug|release) {
LIBS += -lxmlcallibd \
        -lgui_itemsd \
        -lplotlibd \
        -lqcustomplotd \
        -lcalibrationd  \
        -lmeasdocxmld \
        -ltinyxml2d \
        -ltinyxmlwriterd \
        -lmath_vectord \
        -ldb_sensor_queryd \
        -lsql_base_toolsd \
        -lsql_vectord \
        -lgeocoordinatesd \
        -lqstring_utilitiesd
}


include (../../../../mc_data.pri)

message($$OSDIR$$)
message($$PJH$$)
message($$CPLDIR$$)

RESOURCES += \
    resources.qrc
