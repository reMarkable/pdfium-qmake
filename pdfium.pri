SOURCE_DIR = $$PWD/pdfium

HEADERS += \
    $$SOURCE_DIR/fpdfsdk/include/fsdk_actionhandler.h \
    $$SOURCE_DIR/fpdfsdk/include/fsdk_annothandler.h \
    $$SOURCE_DIR/fpdfsdk/include/fsdk_baseannot.h \
    $$SOURCE_DIR/fpdfsdk/include/fsdk_baseform.h \
    $$SOURCE_DIR/public/fpdf_dataavail.h \
    $$SOURCE_DIR/public/fpdf_doc.h \
    $$SOURCE_DIR/public/fpdf_edit.h \
    $$SOURCE_DIR/public/fpdf_ext.h \
    $$SOURCE_DIR/public/fpdf_flatten.h \
    $$SOURCE_DIR/public/fpdf_formfill.h \
    $$SOURCE_DIR/public/fpdf_fwlevent.h \
    $$SOURCE_DIR/public/fpdf_ppo.h \
    $$SOURCE_DIR/public/fpdf_progressive.h \
    $$SOURCE_DIR/public/fpdf_save.h \
    $$SOURCE_DIR/public/fpdf_searchex.h \
    $$SOURCE_DIR/public/fpdf_sysfontinfo.h \
    $$SOURCE_DIR/public/fpdf_text.h \
    $$SOURCE_DIR/public/fpdf_transformpage.h \
    $$SOURCE_DIR/public/fpdfview.h \

SOURCES += \
    $$SOURCE_DIR/fpdfsdk/src/fpdfdoc.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdfeditimg.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdfeditpage.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdfformfill.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdfppo.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdfsave.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdftext.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdfview.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdf_dataavail.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdf_ext.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdf_flatten.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdf_progressive.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdf_searchex.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdf_sysfontinfo.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fpdf_transformpage.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fsdk_actionhandler.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fsdk_annothandler.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fsdk_baseannot.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fsdk_baseform.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fsdk_mgr.cpp \
    $$SOURCE_DIR/fpdfsdk/src/fsdk_rendercontext.cpp \
