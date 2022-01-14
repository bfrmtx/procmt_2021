#-------------------------------------------------
#
# Project created by QtCreator 2019-02-17T09:11:20
#
#-------------------------------------------------

include (../../../../compile.pri)

QT       += core gui printsupport sql opengl


TARGET   = spectra_plotter_pro
DEFINES  += SPECTRA_PLOTTER_PRO_LIBRARY

SOURCES += \
        main.cpp \
        progressdialog.cpp \
        ygfx/gfxarraybasecontainer.cpp \
        ygfx/gfxfloatbufferset.cpp \
        ygfx/gfxshader.cpp \
        ygfx/gfxuniforms.cpp \
        ygfxqt/gfx_qt_math_selection.cpp \
        ygfxqt/gfx_qt_math_selector.cpp \
        ygfxqt/gfx_qt_mathplot.cpp \
        ygfxqt/gfx_qt_mathplot_set.cpp \
        ygfxqt/gfx_qt_widget_maths.cpp \
        ygfxqt/gfx_qt_helper.cpp \
        ygfxqt/gfx_qt_assetmanager.cpp \
        spectra_plotter_pro.cpp

HEADERS += \
        progressdialog.h \
        ygfx.h \
        ygfx/gfxarray.h \
        ygfx/gfxarraybasecontainer.h \
        ygfx/gfxconstexpr.h \
        ygfx/gfxfloatbufferset.h \
        ygfx/gfxincludes.h \
        ygfx/gfxshader.h \
        ygfx/gfxuniforms.h \
        ygfx/gfxvector.h \
        ygfx/gfxvertex.h \
        ygfxqt/gfx_qt_math_selection.h \
        ygfxqt/gfx_qt_math_selector.h \
        ygfxqt/gfx_qt_mathplot.h \
        ygfxqt/gfx_qt_mathplot_set.h \
        ygfxqt/gfx_qt_widget_maths.h \
        ygfxqt/gfx_qt_helper.h \
        ygfxqt/gfx_qt_assetmanager.h \
        spectra_plotter_pro.h

FORMS += \
        spectra_plotter.ui \
        progressdialog.ui

INCLUDEPATH += ygfx
INCLUDEPATH += ygfxqt

include (../../../../mc_data.pri)

# make sure that OpenGL32 is loaded
win32 {
LIBS += -lOpenGL32
}


CONFIG(release, release|debug) {
LIBS += -lacsp_prz \
        -lmt_site \
        -lmath_vector \
        -lmt_data \
        -ledi_file \
        -lgeocoordinates \
        -lqstring_utilities
}

CONFIG(debug, debug|release) {
LIBS += -lacsp_przd \
        -lmt_sited \
        -lmath_vectord \
        -lmt_datad \
        -ledi_filed \
        -lgeocoordinatesd \
        -lqstring_utilitiesd
}


RESOURCES += \
    default_resources.qrc

include (../../../../stat_libs.pri)
