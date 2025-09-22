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

# 解决 Qt 6.8.2 noexcept 宏定义问题
DEFINES += QT_NO_EXCEPTIONS
DEFINES += QT_DISABLE_NOEXCEPT_CHECKS
QMAKE_CXXFLAGS += -fno-exceptions
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
QMAKE_CXXFLAGS += -Wno-error
QMAKE_CXXFLAGS += -DQT_DISABLE_NOEXCEPT_CHECKS
QMAKE_CXXFLAGS += -DQT_NO_EXCEPTIONS

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

# 添加MSYS2 MinGW64库路径
LIBS += -LC:/msys64/mingw64/lib

# 添加外部库链接 - 按依赖顺序排列
# 基础系统库
LIBS += -lwinmm
LIBS += -lws2_32
LIBS += -lole32
LIBS += -loleaut32
LIBS += -luuid
LIBS += -ladvapi32
LIBS += -lshell32
LIBS += -luser32
LIBS += -lkernel32

# GLib相关库
LIBS += -lglib-2.0
LIBS += -lgobject-2.0
LIBS += -lgio-2.0
LIBS += -lgmodule-2.0
LIBS += -lgthread-2.0
LIBS += -lffi

# 图像处理库
LIBS += -lfreetype
LIBS += -lharfbuzz
LIBS += -lcairo
LIBS += -lcairo-gobject
LIBS += -lpng16
LIBS += -ljpeg
LIBS += -lopenjp2

# 网络和加密库
LIBS += -lcurl
LIBS += -lssl
LIBS += -lcrypto

# NSS库 - 只使用MSYS2中实际存在的库
# NSPR库（NSS的依赖）
LIBS += -lnspr4
LIBS += -lplc4
LIBS += -lplds4
# Softoken库（NSS的加密模块）
LIBS += -lsoftokn3
LIBS += -lsoftokn
# NSS核心库
LIBS += -lnssutil3
LIBS += -lnss3
LIBS += -lnssb
LIBS += -lnssckfw
LIBS += -lcertdb
LIBS += -lcerthi
LIBS += -lpkixutil
# 添加更多NSS库
LIBS += -lfreebl
LIBS += -lsmime3

# GPGME库
LIBS += -lgpgme
LIBS += -lgpgmepp

# Poppler库 - 最后链接
LIBS += -lpoppler
LIBS += -lpoppler-cpp
LIBS += -lpoppler-qt6
LIBS += -lpoppler-glib

TARGET = ReportMason
TEMPLATE = app

# 包含路径 - 优先使用MinGW官方头文件
INCLUDEPATH += C:/msys64/mingw64/include
INCLUDEPATH += C:/msys64/mingw64/include/nss3
INCLUDEPATH += C:/msys64/mingw64/include/nspr
INCLUDEPATH += C:/msys64/mingw64/include/nspr4
INCLUDEPATH += C:/msys64/mingw64/include/freetype2
INCLUDEPATH += C:/msys64/mingw64/include/cairo
INCLUDEPATH += C:/msys64/mingw64/include/curl
INCLUDEPATH += C:/msys64/mingw64/include/gpgme++
INCLUDEPATH += C:/msys64/mingw64/include/libpng16
INCLUDEPATH += C:/msys64/mingw64/include/openjpeg-2.5
INCLUDEPATH += C:/msys64/mingw64/include/openssl
INCLUDEPATH += libs/karchive/src
INCLUDEPATH += tools/base
INCLUDEPATH += tools/docx
INCLUDEPATH += tools/pdf
INCLUDEPATH += tools/utils
INCLUDEPATH += tools/ai
INCLUDEPATH += src
INCLUDEPATH += src/widgetTest

# Poppler库配置（使用本地源码）
# 使用Poppler 24.08.0源码直接编译
# Poppler Qt6 配置
INCLUDEPATH += libs
INCLUDEPATH += libs/goo
INCLUDEPATH += libs/fofi
INCLUDEPATH += libs/splash
INCLUDEPATH += libs/poppler-core
INCLUDEPATH += libs/poppler-qt6
# Removed non-existent poppler-24.08.0 paths

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
    src/widgetTest/XmlTestWidget.cpp \
    src/widgetTest/TestWidget.cpp \
    src/widgetTest/DocxContentTestWidget.cpp \
    src/widgetTest/PdfContentTestWidget.cpp \
    src/widgetTest/AiTestWidget.cpp \
    src/widgetTest/DocxToXmlTestWidget.cpp \
    src/widgetTest/PdfToXmlTestWidget.cpp \
    src/widgetTest/LosslessConverterTestWidget.cpp \
    src/LosslessDocumentConverter.cpp \
    src/FileConverter.cpp \
    src/DocToXmlConverter.cpp \
    src/PdfToXmlConverter.cpp \
    src/TemplateManager.cpp \
    src/FieldExtractor.cpp \
    src/KZipUtils.cpp \
    src/PopplerCompat.cpp \
    libs/poppler-qt6/poppler-document.cc \
    libs/poppler-qt6/poppler-page.cc \
    libs/poppler-qt6/poppler-textbox.cc \
    libs/poppler-qt6/poppler-link.cc \
    libs/poppler-qt6/poppler-annotation.cc \
    libs/poppler-qt6/poppler-form.cc \
    libs/poppler-qt6/poppler-outline.cc \
    libs/poppler-qt6/poppler-embeddedfile.cc \
    libs/poppler-qt6/poppler-fontinfo.cc \
    libs/poppler-qt6/poppler-media.cc \
    libs/poppler-qt6/poppler-movie.cc \
    libs/poppler-qt6/poppler-sound.cc \
    libs/poppler-qt6/poppler-optcontent.cc \
    libs/poppler-qt6/poppler-page-transition.cc \
    libs/poppler-qt6/poppler-link-extractor.cc \
    libs/poppler-qt6/poppler-base-converter.cc \
    libs/poppler-qt6/poppler-pdf-converter.cc \
    libs/poppler-qt6/poppler-ps-converter.cc \
    libs/poppler-qt6/poppler-qiodeviceinstream.cc \
    libs/poppler-qt6/poppler-qiodeviceoutstream.cc \
    libs/poppler-qt6/poppler-private.cc \
    libs/poppler-qt6/poppler-version.cpp \
    libs/poppler-qt6/QPainterOutputDev.cc \
    libs/poppler-qt6/AsyncObject.cc \
    libs/poppler-qt6/OptContentModel.cc \
    libs/goo/gbase64.cc \
    libs/goo/gbasename.cc \
    libs/goo/gfile.cc \
    libs/goo/glibc_strtok_r.cc \
    libs/goo/glibc.cc \
    libs/goo/GooString.cc \
    libs/goo/GooTimer.cc \
    libs/goo/grandom.cc \
    libs/goo/gstrtod.cc \
    libs/goo/ft_utils.cc \
    libs/goo/ImgWriter.cc \
    libs/goo/NetPBMWriter.cc \
    libs/fofi/FoFiBase.cc \
    libs/fofi/FoFiEncodings.cc \
    libs/fofi/FoFiIdentifier.cc \
    libs/fofi/FoFiTrueType.cc \
    libs/fofi/FoFiType1.cc \
    libs/fofi/FoFiType1C.cc \
    libs/splash/Splash.cc \
    libs/splash/SplashBitmap.cc \
    libs/splash/SplashState.cc \
    libs/splash/SplashClip.cc \
    libs/splash/SplashXPath.cc \
    libs/splash/SplashXPathScanner.cc \
    libs/splash/SplashPath.cc \
    libs/splash/SplashFont.cc \
    libs/splash/SplashFontEngine.cc \
    libs/splash/SplashFontFile.cc \
    libs/splash/SplashFontFileID.cc \
    libs/splash/SplashScreen.cc \
    libs/splash/SplashPattern.cc \
    libs/splash/SplashSolidColor.cc \
    libs/splash/SplashGouraudColor.cc \
    libs/splash/JpegWriter.cc \
    libs/splash/PNGWriter.cc \
    libs/splash/TiffWriter.cc \
    libs/freetype/freetype_stub.c \
    libs/poppler-core/PDFDoc.cc \
    libs/poppler-core/Page.cc \
    libs/poppler-core/TextOutputDev.cc \
    libs/poppler-core/SplashOutputDev.cc \
    libs/poppler-core/Gfx.cc \
    libs/poppler-core/GfxState.cc \
    libs/poppler-core/GfxFont.cc \
    libs/poppler-core/OutputDev.cc \
    libs/poppler-core/Object.cc \
    libs/poppler-core/Dict.cc \
    libs/poppler-core/Array.cc \
    libs/poppler-core/Stream.cc \
    libs/poppler-core/Lexer.cc \
    libs/poppler-core/Parser.cc \
    libs/poppler-core/XRef.cc \
    libs/poppler-core/Catalog.cc \
    libs/poppler-core/GlobalParams.cc \
    libs/poppler-core/Error.cc \
    libs/poppler-core/Function.cc \
    libs/poppler-core/Annot.cc \
    libs/poppler-core/Link.cc \
    libs/poppler-core/Form.cc \
    libs/poppler-core/Outline.cc \
    libs/poppler-core/PageTransition.cc \
    libs/poppler-core/FontInfo.cc \
    libs/poppler-core/Movie.cc \
    libs/poppler-core/Sound.cc \
    libs/poppler-core/OptionalContent.cc \
    libs/poppler-core/StructElement.cc \
    libs/poppler-core/StructTreeRoot.cc \
    libs/poppler-core/CharCodeToUnicode.cc \
    libs/poppler-core/CMap.cc \
    libs/poppler-core/UnicodeMap.cc \
    libs/poppler-core/UnicodeMapFuncs.cc \
    libs/poppler-core/UnicodeTypeTable.cc \
    libs/poppler-core/UTF.cc \
    libs/poppler-core/PDFDocEncoding.cc \
    libs/poppler-core/FontEncodingTables.cc \
    libs/poppler-core/NameToCharCode.cc \
    libs/poppler-core/FlateStream.cc \
    libs/poppler-core/FlateEncoder.cc \
    libs/poppler-core/DCTStream.cc \
    libs/poppler-core/JPEG2000Stream.cc \
    libs/poppler-core/JPXStream.cc \
    libs/poppler-core/JBIG2Stream.cc \
    libs/poppler-core/JArithmeticDecoder.cc \
    libs/poppler-core/Decrypt.cc \
    libs/poppler-core/SecurityHandler.cc \
    libs/poppler-core/Linearization.cc \
    libs/poppler-core/Hints.cc \
    libs/poppler-core/ViewerPreferences.cc \
    libs/poppler-core/DateInfo.cc \
    libs/poppler-core/FileSpec.cc \
    libs/poppler-core/CertificateInfo.cc \
    libs/poppler-core/SignatureInfo.cc \
    libs/poppler-core/Rendition.cc \
    libs/poppler-core/MarkedContentOutputDev.cc \
    libs/poppler-core/PreScanOutputDev.cc \
    libs/poppler-core/BBoxOutputDev.cc \
    libs/poppler-core/PSOutputDev.cc \
    libs/poppler-core/PSTokenizer.cc \
    libs/poppler-core/ProfileData.cc \
    libs/poppler-core/ImageEmbeddingUtils.cc \
    libs/poppler-core/PageLabelInfo.cc \
    libs/poppler-core/JSInfo.cc \
    libs/poppler-core/CachedFile.cc \
    libs/poppler-core/CurlCachedFile.cc \
    libs/poppler-core/CurlPDFDocBuilder.cc \
    libs/poppler-core/FDPDFDocBuilder.cc \
    libs/poppler-core/FILECacheLoader.cc \
    libs/poppler-core/LocalPDFDocBuilder.cc \
    libs/poppler-core/PDFDocBuilder.cc \
    libs/poppler-core/PDFDocFactory.cc \
    libs/poppler-core/CryptoSignBackend.cc \
    libs/poppler-core/GPGMECryptoSignBackend.cc \
    libs/poppler-core/NSSCryptoSignBackend.cc \
    libs/poppler-core/CairoFontEngine.cc \
    libs/poppler-core/CairoOutputDev.cc \
    libs/poppler-core/CairoRescaleBox.cc \
    libs/poppler-core/AnnotStampImageHelper.cc \
    libs/poppler-core/GlobalParamsWin.cc \
    libs/poppler-core/CourierWidths.pregenerated.c \
    libs/poppler-core/CourierBoldWidths.pregenerated.c \
    libs/poppler-core/CourierBoldObliqueWidths.pregenerated.c \
    libs/poppler-core/CourierObliqueWidths.pregenerated.c \
    libs/poppler-core/HelveticaWidths.pregenerated.c \
    libs/poppler-core/HelveticaBoldWidths.pregenerated.c \
    libs/poppler-core/HelveticaBoldObliqueWidths.pregenerated.c \
    libs/poppler-core/HelveticaObliqueWidths.pregenerated.c \
    libs/poppler-core/SymbolWidths.pregenerated.c \
    libs/poppler-core/TimesBoldWidths.pregenerated.c \
    libs/poppler-core/TimesBoldItalicWidths.pregenerated.c \
    libs/poppler-core/TimesItalicWidths.pregenerated.c \
    libs/poppler-core/TimesRomanWidths.pregenerated.c \
    libs/poppler-core/ZapfDingbatsWidths.pregenerated.c \
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
    tools/base/XmlHelper.cpp \
    tools/utils/ContentUtils.cpp \
    tools/docx/DocxImageExtractor.cpp \
    tools/docx/DocxTableExtractor.cpp \
    tools/docx/DocxChartExtractor.cpp \
    tools/pdf/PdfImageExtractor.cpp \
    tools/pdf/PdfTableExtractor.cpp \
    tools/pdf/PdfChartExtractor.cpp \
    tools/ai/OpenAIBase.cpp \
    tools/ai/OpenAINonStreamClient.cpp \
    tools/ai/OpenAIStreamClient.cpp \
    tools/ai/AiManager.cpp

# 头文件
HEADERS += \
    src/LogSystem.h \
    src/widgetTest/XmlTestWidget.h \
    src/widgetTest/TestWidget.h \
    src/widgetTest/DocxContentTestWidget.h \
    src/widgetTest/PdfContentTestWidget.h \
    src/widgetTest/AiTestWidget.h \
    src/widgetTest/DocxToXmlTestWidget.h \
    src/widgetTest/PdfToXmlTestWidget.h \
    src/widgetTest/LosslessConverterTestWidget.h \
    src/LosslessDocumentConverter.h \
    src/FileConverter.h \
    src/DocToXmlConverter.h \
    src/PdfToXmlConverter.h \
    src/TemplateManager.h \
    src/KZipConfig.h \
    src/KZipUtils.h \
    src/FieldExtractor.h \
    src/QtCompat.h\
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
    tools/base/XmlHelper.h \
    tools/utils/ContentUtils.h \
    tools/docx/DocxImageExtractor.h \
    tools/docx/DocxTableExtractor.h \
    tools/docx/DocxChartExtractor.h \
    tools/pdf/PdfImageExtractor.h \
    tools/pdf/PdfTableExtractor.h \
    tools/pdf/PdfChartExtractor.h \
    tools/ai/LlmTypes.h \
    tools/ai/OpenAIBase.h \
    tools/ai/OpenAINonStreamClient.h \
    tools/ai/OpenAIStreamClient.h \
    tools/ai/AiManager.h \
    libs/fofi/FoFiEncodings.h \
    libs/fofi/FoFiBase.h \
    libs/fofi/FoFiIdentifier.h \
    libs/fofi/FoFiTrueType.h \
    libs/fofi/FoFiType1.h \
    libs/fofi/FoFiType1C.h \
    libs/splash/SplashErrorCodes.h \
    libs/splash/SplashTypes.h \
    libs/splash/SplashBitmap.h \
    libs/splash/SplashMath.h \
    libs/splash/SplashState.h \
    libs/splash/SplashGlyphBitmap.h \
    libs/splash/SplashPattern.h \
    libs/splash/SplashPath.h \
    libs/splash/SplashScreen.h \
    libs/splash/Splash.h \
    libs/splash/JpegWriter.h \
    libs/splash/PNGWriter.h \
    libs/splash/TiffWriter.h \
    libs/fofi/FoFiIdentifier.h \
    libs/splash/SplashXPath.h\
    libs/splash/SplashXPathScanner.h\
    libs/splash/SplashFontEngine.h\
    libs/splash/SplashClip.h\
    libs/span_compat.h\
    libs/poppler-qt6/poppler-qt6.h \
    libs/poppler-qt6/poppler-private.h \
    libs/poppler-qt6/poppler-form.h \
    libs/poppler-qt6/poppler-optcontent.h

# UI文件（已移除，使用代码创建界面）

# 默认规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
