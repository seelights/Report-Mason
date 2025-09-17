QT += core gui widgets sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 明确指定使用MinGW编译器
CONFIG += gcc

# 添加Qt 6.9.1兼容性标志
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += QT_DISABLE_NOEXCEPT_CHECKS

# 添加编译器特定标志来解决noexcept问题
QMAKE_CXXFLAGS += -Wno-error
QMAKE_CXXFLAGS += -fno-strict-aliasing
QMAKE_CXXFLAGS += -DQT_DISABLE_DEPRECATED_BEFORE=0x060000
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -DQT_DISABLE_NOEXCEPT_CHECKS
QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS += -DQT_NO_DEBUG_OUTPUT
QMAKE_CXXFLAGS += -DQT_DISABLE_DEBUG_OUTPUT

# Poppler Qt6 兼容性修复
QMAKE_CXXFLAGS += -DQT_STRINGVIEW_LEVEL=1
QMAKE_CXXFLAGS += -DQT_USE_QSTRINGBUILDER

# Qt版本兼容性修复 - 解决Qt 6.8.2与Poppler Qt6的ABI不兼容问题
QMAKE_CXXFLAGS += -DQT_CORE_LIB
QMAKE_CXXFLAGS += -DQT_NO_CAST_FROM_ASCII
QMAKE_CXXFLAGS += -DQT_NO_CAST_TO_ASCII
QMAKE_CXXFLAGS += -DQT_DISABLE_DEPRECATED_UP_TO=0x060800
QMAKE_CXXFLAGS += -DQT_STRICT_ITERATORS

# 强制使用兼容的Qt符号版本
QMAKE_CXXFLAGS += -DQT_DISABLE_DEPRECATED_BEFORE=0x060000

# 解决Qt 6.8.2字符串字面量问题
QMAKE_CXXFLAGS += -DQT_USE_QSTRINGBUILDER
QMAKE_CXXFLAGS += -DQT_STRINGVIEW_LEVEL=1

# 添加zlib支持
LIBS += -lz

TARGET = ReportMason
TEMPLATE = app

# 包含路径
INCLUDEPATH += libs/karchive/src
INCLUDEPATH += tools/base
INCLUDEPATH += tools/docx
INCLUDEPATH += tools/pdf
INCLUDEPATH += tools/utils
INCLUDEPATH += src

# Poppler库配置（使用本地源码）
# 使用Poppler 24.08.0源码直接编译
# Poppler Qt6 配置
INCLUDEPATH += libs/poppler-core
INCLUDEPATH += libs/poppler-qt6
INCLUDEPATH += poppler-24.08.0
INCLUDEPATH += poppler-24.08.0/poppler
INCLUDEPATH += poppler-24.08.0/qt6/src

# 添加符号版本控制来解决ABI兼容性问题
QMAKE_LFLAGS += -Wl,--allow-multiple-definition
QMAKE_LFLAGS += -Wl,--no-undefined
QMAKE_LFLAGS += -Wl,--as-needed

# 添加额外的链接选项
QMAKE_LFLAGS += -Wl,--allow-multiple-definition

# 注意：Poppler的DLL文件需要手动复制到生成目录（debug/或release/）
# 需要复制的DLL文件：
# - poppler.dll
# - poppler-cpp.dll
# - poppler-glib.dll
# 从：D:/Program Files (x86)/Release-24.08.0-0/poppler-24.08.0/Library/bin/
# 到：debug/ 或 release/ 目录

# 源文件
SOURCES += \
    main.cpp \
    src/LogSystem.cpp \
    src/XmlTestWidget.cpp \
    src/TestWidget.cpp \
    src/DocxContentTestWidget.cpp \
    src/PdfContentTestWidget.cpp \
    src/FileConverter.cpp \
    src/DocToXmlConverter.cpp \
    src/PdfToXmlConverter.cpp \
    src/TemplateManager.cpp \
    src/FieldExtractor.cpp \
    src/KZipUtils.cpp \
    src/PopplerCompat.cpp \
    libs/poppler-qt6/poppler-qt6-simple.cpp \
    libs/karchive/src/karchive.cpp \
    libs/karchive/src/kzip.cpp \
    libs/karchive/src/kcompressiondevice.cpp \
    libs/karchive/src/kfilterbase.cpp \
    libs/karchive/src/kgzipfilter.cpp \
    libs/karchive/src/klimitediodevice.cpp \
    libs/karchive/src/knonefilter.cpp \
    libs/karchive/src/ktar.cpp \
    libs/karchive/src/kar.cpp \
    libs/karchive/src/krcc.cpp \
    libs/karchive/src/loggingcategory.cpp \
    tools/base/ContentExtractor.cpp \
    tools/base/ImageExtractor.cpp \
    tools/base/TableExtractor.cpp \
    tools/base/ChartExtractor.cpp \
    tools/utils/ContentUtils.cpp \
    tools/docx/DocxImageExtractor.cpp \
    tools/docx/DocxTableExtractor.cpp \
    tools/docx/DocxChartExtractor.cpp \
    tools/pdf/PdfImageExtractor.cpp \
    tools/pdf/PdfTableExtractor.cpp \
    tools/pdf/PdfChartExtractor.cpp

# 头文件
HEADERS += \
    src/LogSystem.h \
    src/XmlTestWidget.h \
    src/TestWidget.h \
    src/DocxContentTestWidget.h \
    src/PdfContentTestWidget.h \
    src/FileConverter.h \
    src/DocToXmlConverter.h \
    src/PdfToXmlConverter.h \
    src/TemplateManager.h \
    src/KZipConfig.h \
    src/KZipUtils.h \
    src/FieldExtractor.h \
    libs/karchive/src/karchive.h \
    libs/karchive/src/karchive_export.h \
    libs/karchive/src/loggingcategory.h \
    libs/karchive/src/config-compression.h \
    libs/karchive/src/karchivedirectory.h \
    libs/karchive/src/karchiveentry.h \
    libs/karchive/src/karchivefile.h \
    libs/karchive/src/kzip.h \
    libs/karchive/src/kcompressiondevice.h \
    libs/karchive/src/kfilterbase.h \
    libs/karchive/src/kgzipfilter.h \
    libs/karchive/src/klimitediodevice_p.h \
    libs/karchive/src/kcompressiondevice_p.h \
    libs/karchive/src/karchive_p.h \
    libs/karchive/src/knonefilter.h \
    libs/karchive/src/ktar.h \
    libs/karchive/src/kar.h \
    libs/karchive/src/krcc.h \
    tools/base/ContentExtractor.h \
    tools/base/ImageExtractor.h \
    tools/base/TableExtractor.h \
    tools/base/ChartExtractor.h \
    tools/utils/ContentUtils.h \
    tools/docx/DocxImageExtractor.h \
    tools/docx/DocxTableExtractor.h \
    tools/docx/DocxChartExtractor.h \
    tools/pdf/PdfImageExtractor.h \
    tools/pdf/PdfTableExtractor.h \
    tools/pdf/PdfChartExtractor.h

# UI文件（已移除，使用代码创建界面）

# 默认规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target