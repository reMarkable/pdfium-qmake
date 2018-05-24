#TEMPLATE = lib
#CONFIG += c++11 warn_off
#QT -= core gui

DEFINES += FT2_BUILD_LIBRARY

unix:!macx {
    DEFINES += _FX_OS_=_FX_LINUX_DESKTOP_
    DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_LINUX_
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    LIBS += -L$$PWD/linuxprecompiled -lpdfium
    INCLUDEPATH += $$PWD/prebuilt-headers-linux/
}
macx {
    DEFINES += _FX_OS_=_FX_MACOSX_
    DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_APPLE_
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    CONFIG -= pdfiumsources
    LIBS += -L$$PWD/macosxprecompiled -lpdfium -lfdrm -lformfiller -lfpdfapi -lfpdfdoc -lfpdftext -lfxcodec -lfxcrt -lfxge -lfxjs -lpwl
    INCLUDEPATH += $$PWD/prebuilt-headers/
}
win32 {
    DEFINES += _FX_OS_=_FX_WIN32_DESKTOP_
    DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_WINDOWS_
    DEFINES += FPDFSDK_EXPORTS
    CONFIG -= pdfiumsources
    LIBS += -L$$PWD/win32precompiled -lpdfium
    INCLUDEPATH += $$PWD/pdfium/public
}
