TEMPLATE = app

QT += qml quick widgets gui_private
CONFIG += c++11
#QMAKE_LFLAGS += -fsanitize=undefined
#QMAKE_CXXFLAGS += -fsanitize=undefined

# QMAKE_CXXFLAGS += -funwind-tables -Wl,--export-dynamic
# QMAKE_CXXFLAGS += -O3
TARGET = xochitl

SOURCES += main.cpp \
    drawingarea.cpp \
    collection.cpp \
    settings.cpp \
    systemmonitor.cpp \
    document.cpp \
    messagehandler.cpp \
    line.cpp \
    documentworker.cpp \
    pdfrenderer.cpp \
    pagepreview.cpp

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
    penpoint.h \
    debug.h \
    messagehandler.h \
    documentworker.h \
    pdfrenderer.h \
    pagepreview.h

OTHER_FILES += \
    ArchiveButton.qml \
    ArchiveMain.qml \
    ArchiveTab.qml \
    ColorButton.qml \
    Dialog.qml \
    DocumentPositionBar.qml \
    main.qml \
    MainScreen.qml \
    MainScreenPreviews.qml \
    NoteTab.qml \
    TabBar.qml \
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

DISTFILES += \
    BookThumbnail.qml \
    EditActions.qml \
    BookOverview.qml
