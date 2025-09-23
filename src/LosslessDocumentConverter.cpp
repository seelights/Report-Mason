/*
 * @Author: seelights
 * @Date: 2025-01-27 22:00:00
 * @LastEditTime: 2025-09-22 15:03:31
 * @LastEditors: seelights
 * @Description: 无损文档转换器实现
 * @FilePath: \ReportMason\src\LosslessDocumentConverter.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "LosslessDocumentConverter.h"
#include "QtCompat.h"
#include "KZipUtils.h"
#include "PopplerCompat.h"
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDateTime>
#include <QCryptographicHash>

// Poppler头文件
#include <poppler-qt6.h>
#include <poppler-form.h>
#include <QRegularExpression>
#include <QDebug>
#include <QBuffer>
#include <QQueue>
#include <QPoint>
#include <QColor>
#include <QImage>

// 包含Poppler头文件
#include "poppler-qt6.h"

LosslessDocumentConverter::LosslessDocumentConverter(QObject *parent)
    : QObject(parent), m_elementCounter(0)
{
    // 初始化支持的格式
    m_supportedFormats[QS("docx")] = InputFormat::DOCX;
    m_supportedFormats[QS("pdf")] = InputFormat::PDF;
}

LosslessDocumentConverter::~LosslessDocumentConverter()
{
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::convertToLosslessXml(const QString &filePath, const QString &outputPath)
{
    emit conversionProgress(0, QS("开始转换文档..."));
    
    if (!isSupported(filePath)) {
        emit conversionFinished(ConvertStatus::INVALID_FORMAT, QS("不支持的文件格式"));
        return ConvertStatus::INVALID_FORMAT;
    }
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        emit conversionFinished(ConvertStatus::FILE_NOT_FOUND, QS("文件不存在"));
        return ConvertStatus::FILE_NOT_FOUND;
    }
    
    emit conversionProgress(10, QS("解析文档结构..."));
    
    QList<DocumentElement> elements;
    ConvertStatus status;
    
    QString extension = fileInfo.suffix().toLower();
    if (extension == QS("docx")) {
        status = parseDocxDocument(filePath, elements);
    } else if (extension == QS("pdf")) {
        status = parsePdfDocument(filePath, elements);
    } else {
        status = ConvertStatus::INVALID_FORMAT;
    }
    
    if (status != ConvertStatus::SUCCESS) {
        emit conversionFinished(status, QS("文档解析失败"));
        return status;
    }
    
    emit conversionProgress(50, QS("建立元素关系..."));
    establishElementRelationships(elements);
    
    emit conversionProgress(70, QS("生成XML文件..."));
    
    // 创建输出目录
    QDir outputDir = QFileInfo(outputPath).absoluteDir();
    if (!outputDir.exists()) {
        outputDir.mkpath(QS("."));
    }
    
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit conversionFinished(ConvertStatus::WRITE_ERROR, QS("无法创建输出文件"));
        return ConvertStatus::WRITE_ERROR;
    }
    
    QXmlStreamWriter writer(&outputFile);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);
    
    status = writeElementsToXml(elements, writer);
    outputFile.close();
    
    if (status != ConvertStatus::SUCCESS) {
        emit conversionFinished(status, QS("XML写入失败"));
        return status;
    }
    
    emit conversionProgress(90, QS("验证转换完整性..."));
    
    bool isComplete = validateConversionIntegrity(filePath, outputPath);
    if (!isComplete) {
        emit conversionFinished(ConvertStatus::PARSE_ERROR, QS("转换完整性验证失败"));
        return ConvertStatus::PARSE_ERROR;
    }
    
    emit conversionProgress(100, QS("转换完成"));
    emit conversionFinished(ConvertStatus::SUCCESS, QS("无损转换成功"));
    
    return ConvertStatus::SUCCESS;
}

QByteArray LosslessDocumentConverter::convertToLosslessXmlByteArray(const QString &filePath)
{
    QByteArray result;
    
    if (!isSupported(filePath)) {
        return result;
    }
    
    QList<DocumentElement> elements;
    ConvertStatus status;
    
    QString extension = QFileInfo(filePath).suffix().toLower();
    if (extension == QS("docx")) {
        status = parseDocxDocument(filePath, elements);
    } else if (extension == QS("pdf")) {
        status = parsePdfDocument(filePath, elements);
    } else {
        return result;
    }
    
    if (status != ConvertStatus::SUCCESS) {
        return result;
    }
    
    establishElementRelationships(elements);
    
    QBuffer buffer(&result);
    buffer.open(QIODevice::WriteOnly);
    
    QXmlStreamWriter writer(&buffer);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);
    
    writeElementsToXml(elements, writer);
    
    return result;
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::restoreFromLosslessXml(const QString &xmlPath, const QString &outputPath, InputFormat targetFormat)
{
    // TODO: 实现从XML还原为原格式的功能
    // 这需要根据目标格式生成相应的文档结构
    Q_UNUSED(xmlPath)
    Q_UNUSED(outputPath)
    Q_UNUSED(targetFormat)
    
    return ConvertStatus::UNKNOWN_ERROR;
}

bool LosslessDocumentConverter::isSupported(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    return m_supportedFormats.contains(extension);
}

QStringList LosslessDocumentConverter::getSupportedFormats() const
{
    return m_supportedFormats.keys();
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::parseDocxDocument(const QString &filePath, QList<DocumentElement> &elements)
{
    elements.clear();
    m_elementCounter = 0;
    
    try {
        // 读取document.xml
        QByteArray documentXml;
        if (!KZipUtils::readFileFromZip(filePath, QS("word/document.xml"), documentXml)) {
            return ConvertStatus::PARSE_ERROR;
        }
        
        // 读取样式信息
        QByteArray stylesXml;
        KZipUtils::readFileFromZip(filePath, QS("word/styles.xml"), stylesXml);
        
        // 读取关系文件
        QByteArray relationshipsXml;
        KZipUtils::readFileFromZip(filePath, QS("word/_rels/document.xml.rels"), relationshipsXml);
        
        // 解析主文档
        QXmlStreamReader reader(documentXml);
        DocumentElement currentElement;
        bool inParagraph = false;
        FormatInfo currentFormat;
        
        while (!reader.atEnd()) {
            reader.readNext();
            
            if (reader.isStartElement()) {
                QString elementName = reader.name().toString();
                QString namespaceUri = reader.namespaceUri().toString();
                
                if (elementName == QS("p") && namespaceUri.contains(QS("w"))) {
                    // 段落开始
                    inParagraph = true;
                    currentElement = DocumentElement();
                    currentElement.type = DocumentElementType::PARAGRAPH;
                    currentElement.id = generateElementId(DocumentElementType::PARAGRAPH, m_elementCounter++);
                    
                    // 解析段落格式
                    parseParagraphFormat(reader, currentElement.format);
                    
                } else if (elementName == QS("r") && namespaceUri.contains(QS("w"))) {
                    // 文本运行开始
                    if (!inParagraph) {
                        // 如果没有段落，创建一个
                        currentElement = DocumentElement();
                        currentElement.type = DocumentElementType::TEXT;
                        currentElement.id = generateElementId(DocumentElementType::TEXT, m_elementCounter++);
                        inParagraph = true;
                    }
                    
                    // 解析运行格式
                    parseRunFormat(reader, currentFormat);
                    
                } else if (elementName == QS("t") && namespaceUri.contains(QS("w"))) {
                    // 文本内容
                    QString text = reader.readElementText();
                    if (!text.isEmpty()) {
                        if (inParagraph) {
                            currentElement.content += text;
                            // 合并格式信息 - FormatInfo是结构体，不是QMap
                            // 这里暂时不处理格式合并，因为FormatInfo是结构体
                        } else {
                            // 如果没有段落，创建一个文本元素
                            DocumentElement textElement;
                            textElement.type = DocumentElementType::TEXT;
                            textElement.id = generateElementId(DocumentElementType::TEXT, m_elementCounter++);
                            textElement.content = text;
                            textElement.format = currentFormat;
                            elements.append(textElement);
                        }
                    }
                    
                } else if (elementName == QS("drawing") && namespaceUri.contains(QS("w"))) {
                    // 图片或形状
                    DocumentElement imageElement;
                    imageElement.type = DocumentElementType::IMAGE;
                    imageElement.id = generateElementId(DocumentElementType::IMAGE, m_elementCounter++);
                    
                    parseDrawingElement(reader, imageElement, filePath);
                    elements.append(imageElement);
                    
                } else if (elementName == QS("tbl") && namespaceUri.contains(QS("w"))) {
                    // 表格
                    DocumentElement tableElement;
                    tableElement.type = DocumentElementType::TABLE;
                    tableElement.id = generateElementId(DocumentElementType::TABLE, m_elementCounter++);
                    
                    parseTableElement(reader, tableElement);
                    elements.append(tableElement);
                }
                
            } else if (reader.isEndElement()) {
                QString elementName = reader.name().toString();
                QString namespaceUri = reader.namespaceUri().toString();
                
                if (elementName == QS("p") && namespaceUri.contains(QS("w"))) {
                    // 段落结束
                    if (inParagraph && !currentElement.content.isEmpty()) {
                        elements.append(currentElement);
                    }
                    inParagraph = false;
                    
                } else if (elementName == QS("r") && namespaceUri.contains(QS("w"))) {
                    // 运行结束
                    // 不需要特殊处理
                }
            }
        }
        
        if (reader.hasError()) {
            qDebug() << QS("XML解析错误:") << reader.errorString();
            return ConvertStatus::PARSE_ERROR;
        }
        
        return ConvertStatus::SUCCESS;
        
    } catch (const std::exception &e) {
        qDebug() << QS("DOCX解析异常:") << e.what();
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::parsePdfDocument(const QString &filePath, QList<DocumentElement> &elements)
{
    elements.clear();
    m_elementCounter = 0;
    
    try {
        // 1. 加载PDF文档
        auto document = loadPdfDocument(filePath);
        if (!document) {
            return ConvertStatus::PARSE_ERROR;
        }
        
        // 2. 检查数字签名
        bool hasSignatures = checkDigitalSignatures(document.get());
        
        // 3. 处理所有页面
        ConvertStatus pageStatus = processAllPages(document.get(), elements);
        if (pageStatus != ConvertStatus::SUCCESS) {
            return pageStatus;
        }
        
        // 4. 添加签名信息（如果有）
        if (hasSignatures) {
            addSignatureElements(elements);
        }
        
        qDebug() << "PDF parsing completed, extracted" << elements.size() << "elements";
        return ConvertStatus::SUCCESS;
        
    } catch (const std::exception &e) {
        qDebug() << QS("PDF解析异常:") << e.what();
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

std::unique_ptr<Poppler::Document> LosslessDocumentConverter::loadPdfDocument(const QString &filePath)
{
    try {
        std::unique_ptr<Poppler::Document> document = Poppler::Document::load(filePath);
        if (!document) {
            qDebug() << "Failed to load PDF document:" << filePath;
            return nullptr;
        }
        
        if (document->isLocked()) {
            qDebug() << "PDF document is locked:" << filePath;
            return nullptr;
        }
        
        int pageCount = document->numPages();
        if (pageCount <= 0) {
            qDebug() << "PDF document has no pages:" << filePath;
            return nullptr;
        }
        
        qDebug() << "Successfully loaded PDF with" << pageCount << "pages";
        return document;
        
    } catch (const std::exception &e) {
        qDebug() << "Error loading PDF document:" << e.what();
        return nullptr;
    }
}

bool LosslessDocumentConverter::checkDigitalSignatures(Poppler::Document *document)
{
    try {
        auto signatures = document->signatures();
        bool hasSignatures = !signatures.empty();
        if (hasSignatures) {
            qDebug() << "PDF document has" << signatures.size() << "digital signatures";
        }
        return hasSignatures;
    } catch (const std::exception &e) {
        qDebug() << "Error checking signatures:" << e.what();
        return false;
    }
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::processAllPages(Poppler::Document *document, QList<DocumentElement> &elements)
{
    int pageCount = document->numPages();
    
    for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
        try {
            std::unique_ptr<Poppler::Page> page = document->page(pageIndex);
            if (!page) {
                qDebug() << "Failed to load page" << pageIndex;
                continue;
            }
            
            // 处理单个页面
            ConvertStatus pageStatus = processSinglePage(page.get(), pageIndex, elements);
            if (pageStatus != ConvertStatus::SUCCESS) {
                qDebug() << "Error processing page" << pageIndex;
                continue; // 继续处理下一页
            }
            
        } catch (const std::exception &e) {
            qDebug() << "Error processing page" << pageIndex << ":" << e.what();
            continue; // 继续处理下一页
        }
    }
    
    return ConvertStatus::SUCCESS;
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::processSinglePage(Poppler::Page *page, int pageIndex, QList<DocumentElement> &elements)
{
    try {
        // 获取页面尺寸
        QSizeF pageSize = page->pageSizeF();
        qDebug() << "Processing page" << pageIndex << "size:" << pageSize;
        
        // 1. 提取文本元素（最安全，不会崩溃）
        extractTextElements(page, pageIndex, elements);
        
        // 2. 尝试提取图片元素（可能崩溃，添加保护）
        try {
            extractImageElements(page, pageIndex, elements);
        } catch (const std::exception &e) {
            qDebug() << "Image extraction failed for page" << pageIndex << ":" << e.what();
        } catch (...) {
            qDebug() << "Image extraction failed for page" << pageIndex << "with unknown error";
        }
        
        // 3. 提取表格元素（相对安全）
        try {
            extractTableElements(page, pageIndex, elements);
        } catch (const std::exception &e) {
            qDebug() << "Table extraction failed for page" << pageIndex << ":" << e.what();
        } catch (...) {
            qDebug() << "Table extraction failed for page" << pageIndex << "with unknown error";
        }
        
        // 4. 尝试提取图表元素（可能崩溃，添加保护）
        try {
            extractChartElements(page, pageIndex, elements);
        } catch (const std::exception &e) {
            qDebug() << "Chart extraction failed for page" << pageIndex << ":" << e.what();
        } catch (...) {
            qDebug() << "Chart extraction failed for page" << pageIndex << "with unknown error";
        }
        
        return ConvertStatus::SUCCESS;
        
    } catch (const std::exception &e) {
        qDebug() << "Error processing page" << pageIndex << ":" << e.what();
        return ConvertStatus::UNKNOWN_ERROR;
    } catch (...) {
        qDebug() << "Unknown error processing page" << pageIndex;
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

void LosslessDocumentConverter::extractTextElements(Poppler::Page *page, int pageIndex, QList<DocumentElement> &elements)
{
    try {
        // 使用textList()方法提取精确的文本位置信息
        auto textBoxes = page->textList();
        for (const auto &textBox : textBoxes) {
            if (!textBox) continue;
                
            DocumentElement textElement;
            textElement.type = DocumentElementType::TEXT;
            textElement.id = generateElementId(DocumentElementType::TEXT, m_elementCounter++);
            textElement.content = textBox->text();
            textElement.position.pageNumber = pageIndex + 1;
            
            // 使用TextBox的精确边界框
            QRectF bbox = textBox->boundingBox();
            textElement.position.boundingBox = QRect(
                static_cast<int>(bbox.x()),
                static_cast<int>(bbox.y()),
                static_cast<int>(bbox.width()),
                static_cast<int>(bbox.height())
            );
            
            // 设置格式信息
            extractTextBoxFormatInfo(textBox.get(), textElement);
            
            elements.append(textElement);
        }
    } catch (const std::exception &e) {
        qDebug() << "Error extracting text elements:" << e.what();
    }
}

void LosslessDocumentConverter::extractImageElements(Poppler::Page *page, int pageIndex, QList<DocumentElement> &elements)
{
    try {
        // 添加页面有效性检查
        if (!page) {
            qDebug() << "Page is null, skipping image extraction";
            return;
        }
        
        // 获取页面尺寸进行验证
        QSizeF pageSize = page->pageSizeF();
        if (pageSize.width() <= 0 || pageSize.height() <= 0) {
            qDebug() << "Invalid page size, skipping image extraction";
            return;
        }
        
        // 渲染页面为图像进行分析，添加错误处理
        QImage pageImage;
        try {
            // 使用较低的分辨率来避免内存问题
            pageImage = page->renderToImage(72.0, 72.0); // 降低DPI从150到72
        } catch (const std::exception &e) {
            qDebug() << "Error rendering page to image:" << e.what();
            return;
        } catch (...) {
            qDebug() << "Unknown error rendering page to image";
            return;
        }
        
        if (pageImage.isNull()) {
            qDebug() << "Failed to render page to image";
            return;
        }
        
        // 检测图片区域（基于颜色和形状分析）
        QList<QRect> imageRegions = detectImageRegions(pageImage);
        
        for (int i = 0; i < imageRegions.size(); ++i) {
            const QRect &region = imageRegions[i];
            
            // 验证区域有效性
            if (region.width() <= 0 || region.height() <= 0) {
                continue;
            }
            
            DocumentElement imageElement;
            imageElement.type = DocumentElementType::IMAGE;
            imageElement.id = generateElementId(DocumentElementType::IMAGE, m_elementCounter++);
            imageElement.content = QS("图片区域_%1_%2x%3").arg(i + 1).arg(region.width()).arg(region.height());
            imageElement.position.pageNumber = pageIndex + 1;
            imageElement.position.boundingBox = region;
            imageElement.mimeType = QS("image/png");
            
            // 裁剪并保存图片数据，添加错误处理
            try {
                QImage croppedImage = pageImage.copy(region);
                if (!croppedImage.isNull()) {
                    QBuffer buffer(&imageElement.binaryData);
                    buffer.open(QIODevice::WriteOnly);
                    croppedImage.save(&buffer, "PNG");
                }
            } catch (const std::exception &e) {
                qDebug() << "Error processing image region:" << e.what();
                continue;
            }
            
            elements.append(imageElement);
        }
    } catch (const std::exception &e) {
        qDebug() << "Error extracting image elements:" << e.what();
    } catch (...) {
        qDebug() << "Unknown error in extractImageElements";
    }
}

void LosslessDocumentConverter::extractTableElements(Poppler::Page *page, int pageIndex, QList<DocumentElement> &elements)
{
    try {
        // 使用Poppler的表格检测功能
        auto textBoxes = page->textList();
        
        // 直接使用std::vector，避免QList的unique_ptr问题
        QList<QRectF> tableRegions = detectTableRegionsFromVector(textBoxes);
        
        for (const QRectF &region : tableRegions) {
            DocumentElement tableElement;
            tableElement.type = DocumentElementType::TABLE;
            tableElement.id = generateElementId(DocumentElementType::TABLE, m_elementCounter++);
            tableElement.content = QS("表格");
            tableElement.position.pageNumber = pageIndex + 1;
            tableElement.position.boundingBox = QRect(
                static_cast<int>(region.x()),
                static_cast<int>(region.y()),
                static_cast<int>(region.width()),
                static_cast<int>(region.height())
            );
            
            // 提取表格内容
            QString tableContent = extractTableContentFromVector(textBoxes, region);
            tableElement.content = tableContent;
            
            elements.append(tableElement);
        }
    } catch (const std::exception &e) {
        qDebug() << "Error extracting table elements:" << e.what();
    }
}

void LosslessDocumentConverter::extractChartElements(Poppler::Page *page, int pageIndex, QList<DocumentElement> &elements)
{
    try {
        // 添加页面有效性检查
        if (!page) {
            qDebug() << "Page is null, skipping chart extraction";
            return;
        }
        
        // 渲染页面为图像进行分析，添加错误处理
        QImage pageImage;
        try {
            // 使用较低的分辨率来避免内存问题
            pageImage = page->renderToImage(72.0, 72.0); // 降低DPI从150到72
        } catch (const std::exception &e) {
            qDebug() << "Error rendering page to image for chart detection:" << e.what();
            return;
        } catch (...) {
            qDebug() << "Unknown error rendering page to image for chart detection";
            return;
        }
        
        if (pageImage.isNull()) {
            qDebug() << "Failed to render page to image for chart detection";
            return;
        }
        
        // 检测图表区域（基于颜色和形状分析）
        QList<QRectF> chartRegions = detectChartRegions(pageImage);
        
        for (const QRectF &region : chartRegions) {
            // 验证区域有效性
            if (region.width() <= 0 || region.height() <= 0) {
                continue;
            }
            
            DocumentElement chartElement;
            chartElement.type = DocumentElementType::CHART;
            chartElement.id = generateElementId(DocumentElementType::CHART, m_elementCounter++);
            chartElement.content = QS("图表");
            chartElement.position.pageNumber = pageIndex + 1;
            chartElement.position.boundingBox = QRect(
                static_cast<int>(region.x()),
                static_cast<int>(region.y()),
                static_cast<int>(region.width()),
                static_cast<int>(region.height())
            );
            
            chartElement.mimeType = QS("image/chart");
            elements.append(chartElement);
        }
    } catch (const std::exception &e) {
        qDebug() << "Error extracting chart elements:" << e.what();
    } catch (...) {
        qDebug() << "Unknown error in extractChartElements";
    }
}

void LosslessDocumentConverter::addSignatureElements(QList<DocumentElement> &elements)
{
    try {
        DocumentElement signatureElement;
        signatureElement.type = DocumentElementType::SIGNATURE;
        signatureElement.id = generateElementId(DocumentElementType::SIGNATURE, m_elementCounter++);
        signatureElement.content = QS("数字签名信息");
        signatureElement.position.pageNumber = 0; // 签名通常是文档级别的
        signatureElement.mimeType = QS("application/pdf-signature");
        
        elements.append(signatureElement);
        qDebug() << "Added signature element to document";
    } catch (const std::exception &e) {
        qDebug() << "Error processing signatures:" << e.what();
    }
}

QList<QRect> LosslessDocumentConverter::detectImageRegions(const QImage &pageImage)
{
    QList<QRect> imageRegions;
    
    // 简单的图片检测：寻找非白色/非黑色的区域
    for (int y = 0; y < pageImage.height(); y += 20) {
        for (int x = 0; x < pageImage.width(); x += 20) {
            QColor color = pageImage.pixelColor(x, y);
            
            // 检测非白色/非黑色的区域
            if (color.red() != color.green() || color.green() != color.blue()) {
                // 可能是图片区域
                QRect region(x, y, 200, 200); // 假设图片大小
                imageRegions.append(region);
            }
        }
    }
    
    return imageRegions;
}

QList<QRectF> LosslessDocumentConverter::detectTableRegionsFromVector(const std::vector<std::unique_ptr<Poppler::TextBox>> &textBoxes)
{
    QList<QRectF> tableRegions;
    
    // 简单的表格检测算法：寻找对齐的文本块
    QList<QRectF> textRects;
    for (const auto &textBox : textBoxes) {
        if (textBox) {
            textRects.append(textBox->boundingBox());
        }
    }
    
    // 按Y坐标分组，检测列对齐
    QMap<int, QList<QRectF>> rows;
    for (const QRectF &rect : textRects) {
        int y = static_cast<int>(rect.y());
        rows[y].append(rect);
    }
    
    // 检测表格模式
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        if (it.value().size() >= 2) { // 至少2列
            QRectF tableRect;
            for (const QRectF &rect : it.value()) {
                if (tableRect.isNull()) {
                    tableRect = rect;
                } else {
                    tableRect = tableRect.united(rect);
                }
            }
            tableRegions.append(tableRect);
        }
    }
    
    return tableRegions;
}

QString LosslessDocumentConverter::extractTableContentFromVector(const std::vector<std::unique_ptr<Poppler::TextBox>> &textBoxes, const QRectF &region)
{
    QString content;
    QList<QString> cells;
    
    for (const auto &textBox : textBoxes) {
        if (textBox && region.contains(textBox->boundingBox())) {
            cells.append(textBox->text());
        }
    }
    
    // 简单的表格格式化
    content = cells.join(QS(" | "));
    return content;
}

QList<QRectF> LosslessDocumentConverter::detectChartRegions(const QImage &pageImage)
{
    QList<QRectF> chartRegions;
    
    // 简单的图表检测：寻找彩色区域
    for (int y = 0; y < pageImage.height(); y += 10) {
        for (int x = 0; x < pageImage.width(); x += 10) {
            QColor color = pageImage.pixelColor(x, y);
            
            // 检测非白色/非黑色的区域
            if (color.red() != color.green() || color.green() != color.blue()) {
                // 可能是图表区域
                QRectF region(x, y, 100, 100); // 假设图表大小
                chartRegions.append(region);
            }
        }
    }
    
    return chartRegions;
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::writeElementsToXml(const QList<DocumentElement> &elements, QXmlStreamWriter &writer)
{
    writer.writeStartDocument(QS("1.0"), true);
    writer.writeStartElement(QS("LosslessDocument"));
    writer.writeAttribute(QS("version"), QS("1.0"));
    writer.writeAttribute(QS("created"), QDateTime::currentDateTime().toString(Qt::ISODate));
    writer.writeAttribute(QS("elementCount"), QString::number(elements.size()));
    
    // 按位置排序元素
    QList<DocumentElement> sortedElements = elements;
    std::sort(sortedElements.begin(), sortedElements.end(), DocumentElementComparator());
    
    for (const DocumentElement &element : sortedElements) {
        writeElementToXml(element, writer);
    }
    
    writer.writeEndElement(); // LosslessDocument
    writer.writeEndDocument();
    
    return ConvertStatus::SUCCESS;
}

LosslessDocumentConverter::ConvertStatus LosslessDocumentConverter::readElementsFromXml(QXmlStreamReader &reader, QList<DocumentElement> &elements)
{
    // TODO: 实现从XML读取元素的功能
    Q_UNUSED(reader)
    Q_UNUSED(elements)
    
    return ConvertStatus::UNKNOWN_ERROR;
}

QString LosslessDocumentConverter::generateElementId(DocumentElementType type, int index)
{
    QString typeStr;
    switch (type) {
        case DocumentElementType::TEXT: typeStr = QS("text"); break;
        case DocumentElementType::IMAGE: typeStr = QS("img"); break;
        case DocumentElementType::TABLE: typeStr = QS("table"); break;
        case DocumentElementType::CHART: typeStr = QS("chart"); break;
        case DocumentElementType::SHAPE: typeStr = QS("shape"); break;
        case DocumentElementType::HYPERLINK: typeStr = QS("link"); break;
        case DocumentElementType::FOOTNOTE: typeStr = QS("footnote"); break;
        case DocumentElementType::HEADER: typeStr = QS("header"); break;
        case DocumentElementType::FOOTER: typeStr = QS("footer"); break;
        case DocumentElementType::PAGE_BREAK: typeStr = QS("pagebreak"); break;
        case DocumentElementType::LINE_BREAK: typeStr = QS("linebreak"); break;
        case DocumentElementType::PARAGRAPH: typeStr = QS("para"); break;
        case DocumentElementType::SIGNATURE: typeStr = QS("signature"); break;
    }
    
    return QString(QS("%1_%2_%3"))
           .arg(typeStr)
           .arg(QDateTime::currentMSecsSinceEpoch())
           .arg(index);
}

void LosslessDocumentConverter::establishElementRelationships(QList<DocumentElement> &elements)
{
    // 建立元素间的父子关系和引用关系
    for (int i = 0; i < elements.size(); ++i) {
        DocumentElement &element = elements[i];
        
        // 查找相关的元素（位置重叠或相邻）
        for (int j = 0; j < elements.size(); ++j) {
            if (i == j) continue;
            
            const DocumentElement &otherElement = elements[j];
            
            // 检查位置关系
            if (element.position.boundingBox.intersects(otherElement.position.boundingBox)) {
                element.position.relatedIds.append(otherElement.id);
            }
        }
    }
}

bool LosslessDocumentConverter::validateConversionIntegrity(const QString &originalPath, const QString &xmlPath)
{
    // TODO: 实现转换完整性验证
    // 检查原始文档和XML文档的元素数量、内容等是否一致
    Q_UNUSED(originalPath)
    Q_UNUSED(xmlPath)
    
    return true;
}

// 辅助方法实现
void LosslessDocumentConverter::parseParagraphFormat(QXmlStreamReader &reader, FormatInfo &format)
{
    // 解析段落格式信息
    QXmlStreamAttributes attributes = reader.attributes();
    for (const QXmlStreamAttribute &attr : attributes) {
        QString attrName = attr.name().toString();
        QString attrValue = attr.value().toString();
        
        if (attrName == QS("w:val")) {
            // 解析对齐方式
            if (attrValue == QS("center")) {
                format.alignment = Qt::AlignCenter;
            } else if (attrValue == QS("right")) {
                format.alignment = Qt::AlignRight;
            } else {
                format.alignment = Qt::AlignLeft;
            }
        } else if (attrName == QS("w:sz")) {
            format.fontSize = attrValue.toInt();
        } else if (attrName == QS("w:color")) {
            // 颜色信息暂时不处理，因为FormatInfo没有color成员
        }
    }
}

void LosslessDocumentConverter::parseRunFormat(QXmlStreamReader &reader, FormatInfo &format)
{
    // 解析文本运行格式信息
    QXmlStreamAttributes attributes = reader.attributes();
    for (const QXmlStreamAttribute &attr : attributes) {
        QString attrName = attr.name().toString();
        QString attrValue = attr.value().toString();
        
        if (attrName == QS("w:sz")) {
            format.fontSize = attrValue.toInt();
        } else if (attrName == QS("w:b")) {
            format.bold = (attrValue == QS("1") || attrValue.toLower() == QS("true"));
        } else if (attrName == QS("w:i")) {
            format.italic = (attrValue == QS("1") || attrValue.toLower() == QS("true"));
        } else if (attrName == QS("w:u")) {
            format.underline = (attrValue == QS("1") || attrValue.toLower() == QS("true"));
        } else if (attrName == QS("w:strike")) {
            format.strikethrough = (attrValue == QS("1") || attrValue.toLower() == QS("true"));
        }
    }
}

void LosslessDocumentConverter::parseDrawingElement(QXmlStreamReader &reader, DocumentElement &element, const QString &filePath)
{
    // TODO: 解析绘图元素（图片、形状等）
    Q_UNUSED(reader)
    Q_UNUSED(element)
    Q_UNUSED(filePath)
}

void LosslessDocumentConverter::parseTableElement(QXmlStreamReader &reader, DocumentElement &element)
{
    // TODO: 解析表格元素
    Q_UNUSED(reader)
    Q_UNUSED(element)
}

void LosslessDocumentConverter::parsePdfTextFormat(void *textBox, FormatInfo &format)
{
    // TODO: 解析PDF文本格式信息
    Q_UNUSED(textBox)
    Q_UNUSED(format)
}

void LosslessDocumentConverter::writeElementToXml(const DocumentElement &element, QXmlStreamWriter &writer)
{
    QString elementTypeName;
    switch (element.type) {
        case DocumentElementType::TEXT:
            elementTypeName = QS("Text");
            break;
        case DocumentElementType::PARAGRAPH:
            elementTypeName = QS("Paragraph");
            break;
        case DocumentElementType::IMAGE:
            elementTypeName = QS("Image");
            break;
        case DocumentElementType::TABLE:
            elementTypeName = QS("Table");
            break;
        case DocumentElementType::CHART:
            elementTypeName = QS("Chart");
            break;
        case DocumentElementType::SHAPE:
            elementTypeName = QS("Shape");
            break;
        default:
            elementTypeName = QS("Unknown");
            break;
    }
    
    writer.writeStartElement(elementTypeName);
    
    // 写入基本属性
    writer.writeAttribute(QS("id"), element.id);
    writer.writeAttribute(QS("type"), QString::number(static_cast<int>(element.type)));
    
    // 写入位置信息
    if (!element.position.boundingBox.isNull()) {
        writer.writeAttribute(QS("x"), QString::number(element.position.boundingBox.x()));
        writer.writeAttribute(QS("y"), QString::number(element.position.boundingBox.y()));
        writer.writeAttribute(QS("width"), QString::number(element.position.boundingBox.width()));
        writer.writeAttribute(QS("height"), QString::number(element.position.boundingBox.height()));
    }
    
    if (element.position.pageNumber > 0) {
        writer.writeAttribute(QS("page"), QString::number(element.position.pageNumber));
    }
    
    // 写入内容
    if (!element.content.isEmpty()) {
        writer.writeTextElement(QS("Content"), element.content);
    }
    
    // 写入格式信息
    writer.writeStartElement(QS("Format"));
    writer.writeAttribute(QS("bold"), element.format.bold ? QS("true") : QS("false"));
    writer.writeAttribute(QS("italic"), element.format.italic ? QS("true") : QS("false"));
    writer.writeAttribute(QS("underline"), element.format.underline ? QS("true") : QS("false"));
    writer.writeAttribute(QS("strikethrough"), element.format.strikethrough ? QS("true") : QS("false"));
    writer.writeAttribute(QS("fontSize"), QString::number(element.format.fontSize));
    writer.writeAttribute(QS("fontFamily"), element.format.fontFamily);
    writer.writeAttribute(QS("alignment"), QString::number(static_cast<int>(element.format.alignment)));
    writer.writeAttribute(QS("lineSpacing"), QString::number(element.format.lineSpacing));
    writer.writeAttribute(QS("paragraphSpacing"), QString::number(element.format.paragraphSpacing));
    writer.writeAttribute(QS("leftIndent"), QString::number(element.format.leftIndent));
    writer.writeAttribute(QS("rightIndent"), QString::number(element.format.rightIndent));
    writer.writeAttribute(QS("firstLineIndent"), QString::number(element.format.firstLineIndent));
    writer.writeEndElement(); // Format
    
    // 写入额外属性
    if (!element.attributes.isEmpty()) {
        writer.writeStartElement(QS("Attributes"));
        for (auto it = element.attributes.begin(); it != element.attributes.end(); ++it) {
            writer.writeAttribute(it.key(), it.value());
        }
        writer.writeEndElement(); // Attributes
    }
    
    // 写入相关元素ID
    if (!element.position.relatedIds.isEmpty()) {
        writer.writeStartElement(QS("RelatedElements"));
        for (const QString &relatedId : element.position.relatedIds) {
            writer.writeTextElement(QS("RelatedId"), relatedId);
        }
        writer.writeEndElement(); // RelatedElements
    }
    
    writer.writeEndElement(); // elementTypeName
}

void LosslessDocumentConverter::extractTextBoxFormatInfo(void *textBox, DocumentElement &element)
{
    if (!textBox) {
        // 设置默认格式信息
        element.format.bold = false;
        element.format.italic = false;
        element.format.underline = false;
        element.format.strikethrough = false;
        element.format.fontSize = 12;
        element.format.fontFamily = QS("Arial");
        element.format.alignment = Qt::AlignLeft;
        element.format.lineSpacing = 1.0;
        element.format.paragraphSpacing = 0.0;
        element.format.leftIndent = 0;
        element.format.rightIndent = 0;
        element.format.firstLineIndent = 0;
        
        // 设置属性
        element.attributes[QS("extraction_method")] = QS("textList");
        element.attributes[QS("source")] = QS("PDF_TextBox");
        return;
    }
    
    // 使用真正的TextBox提取格式信息
    Poppler::TextBox *tb = static_cast<Poppler::TextBox*>(textBox);
    
    // 设置默认格式信息
    element.format.bold = false;
    element.format.italic = false;
    element.format.underline = false;
    element.format.strikethrough = false;
    element.format.fontSize = 12;
    element.format.fontFamily = QS("Arial");
    element.format.alignment = Qt::AlignLeft;
    element.format.lineSpacing = 1.0;
    element.format.paragraphSpacing = 0.0;
    element.format.leftIndent = 0;
    element.format.rightIndent = 0;
    element.format.firstLineIndent = 0;
    
    // 设置属性
    element.attributes[QS("extraction_method")] = QS("textList");
    element.attributes[QS("source")] = QS("PDF_TextBox");
    element.attributes[QS("text_content")] = tb->text();
    
    // 获取边界框信息
    QRectF bbox = tb->boundingBox();
    element.attributes[QS("bbox_x")] = QString::number(bbox.x());
    element.attributes[QS("bbox_y")] = QString::number(bbox.y());
    element.attributes[QS("bbox_width")] = QString::number(bbox.width());
    element.attributes[QS("bbox_height")] = QString::number(bbox.height());
}

QRect LosslessDocumentConverter::floodFillRegion(const QImage &image, int startX, int startY, QVector<QVector<bool>> &visited)
{
    int width = image.width();
    int height = image.height();
    
    int minX = startX, maxX = startX;
    int minY = startY, maxY = startY;
    
    QQueue<QPoint> queue;
    queue.enqueue(QPoint(startX, startY));
    visited[startX][startY] = true;
    
    QRgb targetColor = image.pixel(startX, startY);
    QColor target(targetColor);
    int targetLightness = target.lightness();
    
    while (!queue.isEmpty()) {
        QPoint current = queue.dequeue();
        int x = current.x();
        int y = current.y();
        
        minX = qMin(minX, x);
        maxX = qMax(maxX, x);
        minY = qMin(minY, y);
        maxY = qMax(maxY, y);
        
        // 检查四个方向
        QList<QPoint> directions = {{x+1, y}, {x-1, y}, {x, y+1}, {x, y-1}};
        for (const QPoint &dir : directions) {
            int nx = dir.x();
            int ny = dir.y();
            
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[nx][ny]) {
                QRgb pixel = image.pixel(nx, ny);
                QColor color(pixel);
                
                // 如果颜色相似，加入区域
                if (qAbs(color.lightness() - targetLightness) < 30) {
                    visited[nx][ny] = true;
                    queue.enqueue(QPoint(nx, ny));
                }
            }
        }
    }
    
    return QRect(minX, minY, maxX - minX + 1, maxY - minY + 1);
}

QList<QRect> LosslessDocumentConverter::detectTableRegions(const QString &pageText, void *page)
{
    QList<QRect> regions;
    
    // 简单的表格检测：寻找包含制表符或对齐文本的区域
    QStringList lines = pageText.split(QS("\n"), Qt::SkipEmptyParts);
    
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        
        // 检测表格特征：包含制表符、多个空格分隔、或重复的字符模式
        if (line.contains(QS("\t")) || 
            line.count(QS("  ")) > 2 || 
            QRegularExpression(QS("\\s{3,}")).match(line).hasMatch()) {
            
            // 使用search方法获取这一行的位置
            Poppler::Page *popplerPage = static_cast<Poppler::Page*>(page);
            QList<QRectF> positions = popplerPage->search(line);
            if (!positions.isEmpty()) {
                QRectF bbox = positions.first();
                regions.append(QRect(
                    static_cast<int>(bbox.x()),
                    static_cast<int>(bbox.y()),
                    static_cast<int>(bbox.width()),
                    static_cast<int>(bbox.height())
                ));
            }
        }
    }
    
    return regions;
}

QList<QRect> LosslessDocumentConverter::detectChartRegions(void *page)
{
    QList<QRect> regions;
    
    // 简单的图表检测：通过搜索常见的图表关键词
    QStringList chartKeywords = {
        QS("图表"), QS("图"), QS("Chart"), QS("Figure"), QS("Fig"),
        QS("柱状图"), QS("饼图"), QS("折线图"), QS("散点图"),
        QS("Bar"), QS("Pie"), QS("Line"), QS("Scatter")
    };
    
    Poppler::Page *popplerPage = static_cast<Poppler::Page*>(page);
    for (const QString &keyword : chartKeywords) {
        QList<QRectF> positions = popplerPage->search(keyword);
        for (const QRectF &bbox : positions) {
            // 扩展区域以包含可能的图表
            QRect expandedRegion(
                static_cast<int>(bbox.x() - 50),
                static_cast<int>(bbox.y() - 50),
                static_cast<int>(bbox.width() + 100),
                static_cast<int>(bbox.height() + 100)
            );
            regions.append(expandedRegion);
        }
    }
    
    return regions;
}

