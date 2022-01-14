include (../../../../compile.pri)
include (../../../../bins.pri)
QT       += core gui widgets network sql xml

SOURCES += \
    main.cpp \
    adu_gui.cpp

HEADERS += \
    adu_gui.h

FORMS += \
    adu_gui.ui

CONFIG(release, release|debug) {
LIBS += -ladu_json \
        -lrecording
}

CONFIG(debug, debug|release) {
LIBS += -ladu_jsond \
        -lrecordingd

}
