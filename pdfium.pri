PWD/pdfium/ = $$PWD/pdfium//pdfium

include(config.pri)

include(fdrm.pri)
include(fpdfdoc.pri)
include(fpdfapi.pri)
include(fpdftext.pri)
include(fxcodec.pri)
include(fxcrt.pri)
include(fxge.pri)
include(fxedit.pri)
include(pdfwindow.pri)
include(javascript.pri)
include(formfiller.pri)

include(bigint.pri)
include(fx_freetype.pri)
include(fx_agg.pri)
include(fx_lcms2.pri)
include(libjpeg.pri)
include(fx_libopenjpeg.pri)
include(fx_zlib.pri)
include(pdfium_base.pri)

HEADERS += \
    $$PWD/pdfium//fpdfsdk/include/fsdk_actionhandler.h \
    $$PWD/pdfium//fpdfsdk/include/fsdk_annothandler.h \
    $$PWD/pdfium//fpdfsdk/include/fsdk_baseannot.h \
    $$PWD/pdfium//fpdfsdk/include/fsdk_baseform.h \
    $$PWD/pdfium//public/fpdf_dataavail.h \
    $$PWD/pdfium//public/fpdf_doc.h \
    $$PWD/pdfium//public/fpdf_edit.h \
    $$PWD/pdfium//public/fpdf_ext.h \
    $$PWD/pdfium//public/fpdf_flatten.h \
    $$PWD/pdfium//public/fpdf_formfill.h \
    $$PWD/pdfium//public/fpdf_fwlevent.h \
    $$PWD/pdfium//public/fpdf_ppo.h \
    $$PWD/pdfium//public/fpdf_progressive.h \
    $$PWD/pdfium//public/fpdf_save.h \
    $$PWD/pdfium//public/fpdf_searchex.h \
    $$PWD/pdfium//public/fpdf_sysfontinfo.h \
    $$PWD/pdfium//public/fpdf_text.h \
    $$PWD/pdfium//public/fpdf_transformpage.h \
    $$PWD/pdfium//public/fpdfview.h \

SOURCES += \
    $$PWD/pdfium//fpdfsdk/src/fpdfdoc.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdfeditimg.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdfeditpage.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdfformfill.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdfppo.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdfsave.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdftext.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdfview.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdf_dataavail.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdf_ext.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdf_flatten.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdf_progressive.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdf_searchex.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdf_sysfontinfo.cpp \
    $$PWD/pdfium//fpdfsdk/src/fpdf_transformpage.cpp \
    $$PWD/pdfium//fpdfsdk/src/fsdk_actionhandler.cpp \
    $$PWD/pdfium//fpdfsdk/src/fsdk_annothandler.cpp \
    $$PWD/pdfium//fpdfsdk/src/fsdk_baseannot.cpp \
    $$PWD/pdfium//fpdfsdk/src/fsdk_baseform.cpp \
    $$PWD/pdfium//fpdfsdk/src/fsdk_mgr.cpp \
    $$PWD/pdfium//fpdfsdk/src/fsdk_rendercontext.cpp \

INCLUDEPATH += \
    $$PWD/pdfium/ \
    $$PWD/pdfium/third_party/freetype/include \
    $$PWD/pdfium/third_party/freetype/include/freetype \
