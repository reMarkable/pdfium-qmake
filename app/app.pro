TEMPLATE = app

QT += qml quick widgets gui_private
CONFIG += c++11

TARGET = xochitl

SOURCES += main.cpp \
    drawingarea.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    drawingarea.h \
    predictor.h

contains(QT_ARCH, arm) {
   CONFIG += android_app
   QMAKE_LFLAGS_APP -= -shared

   INCLUDEPATH += ../qsgepaper
   TARGETDEPS += ../qsgepaper/libqsgepaper.a
   LIBS += -L../qsgepaper -lqsgepaper

   SOURCES += digitizer.cpp
   HEADERS += digitizer.h
}
