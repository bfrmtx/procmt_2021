#-------------------------------------------------
#
# Project created by QtCreator 2018-07-15T10:31:37
#
#-------------------------------------------------

include (../../../../compile.pri)
include (../../../../bins.pri)


INCLUDEPATH += ../spectra_plotter
INCLUDEPATH += ../spectra_plotter_pro
INCLUDEPATH += $${PJH}/src/pmt_gui/Trunk/spectra_plotter_pro/ygfx $${PJH}/src/pmt_gui/Trunk/spectra_plotter_pro/ygfxqt $${PJH}/src/pmt_gui/Trunk/spectra_plotter_pro

QT       += core gui sql widgets printsupport opengl

TARGET = procmt_mini

SOURCES += main.cpp \
        procmt_mini.cpp \
        dir_dialog.cpp

HEADERS += \
        procmt_mini.h \
        dir_dialog.h

FORMS += \
        procmt_mini.ui \
        dir_dialog.ui



CONFIG(release, release|debug) {
LIBS += -llineedit_file \
        -lprocmt_lib \
        -lmt_site \
        -lmt_data \
        -ledi_file \
        -lgeocoordinates \
        -lqstring_utilities \
        -lgui_items \
#        -lspectra_plotter \
        -lspectra_plotter_pro \
        -lstatic_plot \
        -lqcustomplot \
        -lspc_db \
        -lprc_com_xml

}

CONFIG(debug, debug|release) {

LIBS += -llineedit_filed \
        -lprocmt_libd \
        -lmt_sited \
        -lmt_datad \
        -ledi_filed \
        -lgeocoordinatesd \
        -lqstring_utilitiesd \
        -lgui_itemsd \
        -lstatic_plotd \
#        -lspectra_plotterd \
        -lspectra_plotter_prod \
        -lqcustomplotd \
        -lspc_dbd \
        -lprc_com_xmld
}

# make sure that OpenGL32 is loaded
win32 {
LIBS += -lOpenGL32
}


include (../../../../mc_data.pri)


