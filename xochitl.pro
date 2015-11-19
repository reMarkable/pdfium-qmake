TEMPLATE = app

QT += qml quick widgets gui_private
CONFIG += c++11
CONFIG += android_app
QMAKE_LFLAGS_APP -= -shared

SOURCES += main.cpp \
    digitizer.cpp \
    drawingarea.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    digitizer.h \
    drawingarea.h

