DEFINES += FT2_BUILD_LIBRARY

win32 {
    DEFINES += _FX_OS_=_FX_WIN32_DESKTOP_
    DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_WINDOWS_
    DEFINES += FPDFSDK_EXPORTS
    LIBS += -L$$PWD/win32precompiled/ -lpdfium
    INCLUDEPATH += $$PWD/prebuilt-headers/
} else {
    macx {
        DEFINES += _FX_OS_=_FX_MACOSX_
        DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_APPLE_
        LIBS += -L$$PWD/macosxprecompiled -lpdfium
        INCLUDEPATH += $$PWD/prebuilt-headers/
    } else {
        DEFINES += _FX_OS_=_FX_LINUX_DESKTOP_
        DEFINES += _FXM_PLATFORM_=_FXM_PLATFORM_LINUX_
        LIBS += -L$$PWD/linuxprecompiled -lpdfium
        INCLUDEPATH += $$PWD/prebuilt-headers-linux/
    }
}
