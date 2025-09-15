QT       += core gui widgets sql network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = TestReportMason
TEMPLATE = app

# 源文件
SOURCES += \
    TestReportMason.cpp \
    ../src/FileConverter.cpp \
    ../src/DocToXmlConverter.cpp \
    ../src/PdfToXmlConverter.cpp \
    ../src/TemplateManager.cpp \
    ../src/FieldExtractor.cpp \
    ../src/KZipUtils.cpp

# 头文件
HEADERS += \
    ../src/FileConverter.h \
    ../src/DocToXmlConverter.h \
    ../src/PdfToXmlConverter.h \
    ../src/TemplateManager.h \
    ../src/KZipConfig.h \
    ../src/KZipUtils.h \
    ../src/FieldExtractor.h

# 默认规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target