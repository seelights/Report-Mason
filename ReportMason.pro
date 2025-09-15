QT += core gui widgets sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 明确指定使用MinGW编译器
CONFIG += gcc

# 添加Qt 6.9.1兼容性标志
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# 添加编译器特定标志来解决noexcept问题
QMAKE_CXXFLAGS += -Wno-error
QMAKE_CXXFLAGS += -fno-strict-aliasing
QMAKE_CXXFLAGS += -DQT_NO_EXCEPTIONS

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

# 源文件
SOURCES += \
    main.cpp \
    src/LogSystem.cpp \
    src/XmlTestWidget.cpp \
    src/TestWidget.cpp \
    src/FileConverter.cpp \
    src/DocToXmlConverter.cpp \
    src/PdfToXmlConverter.cpp \
    src/TemplateManager.cpp \
    src/FieldExtractor.cpp \
    src/KZipUtils.cpp \
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
    src/FileConverter.h \
    src/DocToXmlConverter.h \
    src/PdfToXmlConverter.h \
    src/TemplateManager.h \
    src/KZipConfig.h \
    src/KZipUtils.h \
    src/FieldExtractor.h \
    libs/karchive/src/karchive.h \
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