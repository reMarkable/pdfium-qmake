#-------------------------------------------------
#
# Project created by QtCreator 2015-08-28T17:39:15
#
#-------------------------------------------------

QT = \
    core core_private \
    gui gui_private \
    quick quick_private

CONFIG += plugin static
CONFIG += c++11

TARGET = qsgepaper
TEMPLATE = lib

#DEFINES += TWO_PASS

SOURCES += \
    epcontext.cpp \
    epframebuffer.cpp \
    eprenderer.cpp \
    eprenderloop.cpp \
    eptexture.cpp \
    nodes/epglyphnode.cpp \
    nodes/epimagenode.cpp \
    nodes/epnode.cpp \
    nodes/eppainternode.cpp \
    nodes/eprectanglenode.cpp \
    qsgepaperplugin.cpp

HEADERS += \
    epcontext.h \
    epframebuffer.h \
    eprenderer.h \
    eprenderloop.h \
    eptexture.h \
    nodes/epglyphnode.h \
    nodes/epimagenode.h \
    nodes/epnode.h \
    nodes/eppainternode.h \
    nodes/eprectanglenode.h \
    qsgepaperplugin.h

DISTFILES += \
    qsgepaper.json
