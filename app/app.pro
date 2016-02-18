TEMPLATE = app

QT += qml quick widgets gui_private
CONFIG += c++11

# QMAKE_CXXFLAGS += -funwind-tables -Wl,--export-dynamic
# QMAKE_CXXFLAGS += -O3
TARGET = xochitl

SOURCES += main.cpp \
    drawingarea.cpp \
    collection.cpp \
    settings.cpp \
    systemmonitor.cpp \
    document.cpp \
    pdfdocument.cpp \
    imagedocument.cpp \
    nativedocument.cpp \
    messagehandler.cpp

RESOURCES += qml.qrc

LIBS += -L../pdfium/ -lpdfium
INCLUDEPATH += ../pdfium/pdfium/public

DEFINES += QT_MESSAGELOGCONTEXT

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    drawingarea.h \
    predictor.h \
    collection.h \
    settings.h \
    systemmonitor.h \
    document.h \
    line.h \
    pdfdocument.h \
    imagedocument.h \
    penpoint.h \
    debug.h \
    nativedocument.h \
    messagehandler.h

OTHER_FILES += \
    ArchiveBook.qml \
    ArchiveButton.qml \
    ArchiveMain.qml \
    ArchiveView.qml \
    ColorButton.qml \
    Dialog.qml \
    DocumentTab.qml \
    main.qml \
    MainScreenHeader.qml \
    MainScreen.qml \
    NoteTab.qml \
    TabBar.qml \
    ThumbnailGrid.qml \
    ToolBox.qml \
    ToolButton.qml

contains(QT_ARCH, arm) {
   CONFIG += android_app
   QMAKE_LFLAGS_APP -= -shared

   INCLUDEPATH += ../qsgepaper
   TARGETDEPS += ../qsgepaper/libqsgepaper.a
   LIBS += -L../qsgepaper -lqsgepaper

   SOURCES += digitizer.cpp
   HEADERS += digitizer.h
}
