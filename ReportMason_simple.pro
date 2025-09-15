QT += core gui widgets

CONFIG += c++17
CONFIG += gcc

# 添加Qt 6.9.1兼容性标志
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += QT_NO_DEBUG_OUTPUT

# 添加编译器特定标志来解决noexcept问题
QMAKE_CXXFLAGS += -Wno-error
QMAKE_CXXFLAGS += -fno-strict-aliasing

TARGET = ReportMason
TEMPLATE = app

# 源文件 - 只包含核心文件
SOURCES += \
    main.cpp \
    Widget.cpp \
    src/FileConverter.cpp \
    src/DocToXmlConverter.cpp \
    src/PdfToXmlConverter.cpp \
    src/TemplateManager.cpp \
    src/FieldExtractor.cpp \
    src/KZipUtils.cpp

# 头文件
HEADERS += \
    Widget.h \
    src/FileConverter.h \
    src/DocToXmlConverter.h \
    src/PdfToXmlConverter.h \
    src/TemplateManager.h \
    src/KZipConfig.h \
    src/KZipUtils.h \
    src/FieldExtractor.h

# UI文件
FORMS += \
    Widget.ui

