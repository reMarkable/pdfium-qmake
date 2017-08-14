#TEMPLATE = lib
#CONFIG += c++11 warn_off
#QT -= core gui

DEFINES += FT2_BUILD_LIBRARY

unix:!macx {
    DEFINES += _FX_OS_=_FX_LINUX_DESKTOP_
    DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_LINUX_
    QMAKE_CXXFLAGS += -Wno-unused-parameter
}
macx {
    DEFINES += _FX_OS_=_FX_MACOSX_
    DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_APPLE_
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    CONFIG -= pdfiumsources
    pdfium_framework.files = $$PWD/macosxprecompiled/Pdfium.framework
    pdfium_framework.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += pdfium_framework
    LIBS += -F$$PWD/macosxprecompiled -framework Pdfium
    INCLUDEPATH += $$PWD/pdfium/public
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}
win32 {
    DEFINES += _FX_OS_=_FX_WIN32_DESKTOP_
    DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_WINDOWS_
    DEFINES += FPDFSDK_EXPORTS
    CONFIG -= pdfiumsources
    LIBS += -L$$PWD/win32precompiled -lpdfium.dll
    INCLUDEPATH += $$PWD/pdfium/public
}
