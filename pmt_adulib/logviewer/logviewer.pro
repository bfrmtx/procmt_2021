include (../../../../compile.pri)
include (../../../../bins.pri)

QT       += core gui widgets sql printsupport

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

CONFIG(release, release|debug) {
LIBS += -lqcustomplot

}

CONFIG(debug, debug|release) {
LIBS += -lqcustomplotd
}


scripts.path = $${PJH}/$${OSDIR}$${CPLDIR}/bin/scripts
scripts.files = $${PJH}/src/doc/Trunk/scripts/mysql2sqlite $${PJH}/src/doc/Trunk/scripts/README_mysql2sqlite.md
INSTALLS +=  scripts
