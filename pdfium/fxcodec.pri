SOURCE_DIR = $$PWD/pdfium

HEADERS += \
    $$SOURCE_DIR/core/include/fxcodec/fx_codec.h \
    $$SOURCE_DIR/core/include/fxcodec/fx_codec_def.h \
    $$SOURCE_DIR/core/include/fxcodec/fx_codec_flate.h \
    $$SOURCE_DIR/core/src/fxcodec/codec/codec_int.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_ArithDecoder.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_ArithIntDecoder.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_BitStream.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Context.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Define.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_GrdProc.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_GrrdProc.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_GsidProc.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_HtrdProc.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_HuffmanDecoder.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_HuffmanTable.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_HuffmanTable_Standard.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Image.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_List.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Page.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_PatternDict.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_PddProc.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_SddProc.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Segment.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_SymbolDict.h \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_TrdProc.h \

SOURCES += \
    $$SOURCE_DIR/core/src/fxcodec/codec/fx_codec.cpp \
    $$SOURCE_DIR/core/src/fxcodec/codec/fx_codec_fax.cpp \
    $$SOURCE_DIR/core/src/fxcodec/codec/fx_codec_flate.cpp \
    $$SOURCE_DIR/core/src/fxcodec/codec/fx_codec_icc.cpp \
    $$SOURCE_DIR/core/src/fxcodec/codec/fx_codec_jbig.cpp \
    $$SOURCE_DIR/core/src/fxcodec/codec/fx_codec_jpeg.cpp \
    $$SOURCE_DIR/core/src/fxcodec/codec/fx_codec_jpx_opj.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_ArithDecoder.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_ArithIntDecoder.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_BitStream.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Context.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_GrdProc.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_GrrdProc.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_GsidProc.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_HtrdProc.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_HuffmanDecoder.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_HuffmanTable.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Image.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_PatternDict.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_PddProc.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_SddProc.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_Segment.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_SymbolDict.cpp \
    $$SOURCE_DIR/core/src/fxcodec/jbig2/JBig2_TrdProc.cpp \
