SOURCE_DIR = $$PWD/pdfium

HEADERS += \
    $$SOURCE_DIR/fpdfsdk/include/fxedit/fx_edit.h \
    $$SOURCE_DIR/fpdfsdk/include/fxedit/fxet_edit.h \
    $$SOURCE_DIR/fpdfsdk/include/fxedit/fxet_list.h \

SOURCES += \
    $$SOURCE_DIR/fpdfsdk/src/fxedit/fxet_ap.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fxedit/fxet_edit.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fxedit/fxet_list.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fxedit/fxet_module.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fxedit/fxet_pageobjs.cpp \
