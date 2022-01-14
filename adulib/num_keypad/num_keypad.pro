include (../../../../compile.pri)

QT += core gui widgets

TARGET = num_keypad
DEFINES += NUM_KEYPAD_LIBRARY


SOURCES += num_keypad.cpp

HEADERS += num_keypad.h

FORMS += \
    num_keypad.ui \


RESOURCES += \
    buttons.qrc

include(../../../../stat_libs.pri)
