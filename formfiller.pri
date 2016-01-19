SOURCE_DIR = $$PWD/pdfium

HEADERS += \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_CBA_Fontmap.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_CheckBox.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_ComboBox.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_FormFiller.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_IFormFiller.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_ListBox.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_PushButton.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_RadioButton.h \
    $$SOURCE_DIR/fpdfsdk/include/formfiller/FFL_TextField.h \

SOURCES += \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_CBA_Fontmap.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_CheckBox.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_ComboBox.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_FormFiller.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_IFormFiller.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_ListBox.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_PushButton.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_RadioButton.cpp \
    $$SOURCE_DIR/fpdfsdk/src/formfiller/FFL_TextField.cpp \
