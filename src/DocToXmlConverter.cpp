/*
 * @Author: seelights
 * @Date: 2025-09-14 20:02:52
 * @LastEditTime: 2025-09-15 00:34:15
 * @LastEditors: seelights
 * @Description:
 * @FilePath: \ReportMason\src\DocToXmlConverter.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */
#include "QtCompat.h"
#include "DocToXmlConverter.h"
#include "kzip.h"
#include "karchivedirectory.h"
#include "karchivefile.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QTextDocument>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QBuffer>
#include <QDataStream>
#include "KZipUtils.h"
#include "tools/docx/DocxImageExtractor.h"
#include "tools/docx/DocxTableExtractor.h"
#include "tools/docx/DocxChartExtractor.h"

// 静态常量定义
const QString DocToXmlConverter::DOCX_DOCUMENT_PATH = QS("word/document.xml");
const QString DocToXmlConverter::DOCX_STYLES_PATH = QS("word/styles.xml");
const QStringList DocToXmlConverter::SUPPORTED_EXTENSIONS = {QS("docx")};

DocToXmlConverter::DocToXmlConverter(QObject* parent)
    : FileConverter(parent), m_imageExtractor(nullptr), m_tableExtractor(nullptr),
      m_chartExtractor(nullptr)
{
    // 初始化内容提取器
    m_imageExtractor = new DocxImageExtractor(this);
    m_tableExtractor = new DocxTableExtractor(this);
    m_chartExtractor = new DocxChartExtractor(this);
}

DocToXmlConverter::~DocToXmlConverter()
{
    // 析构函数中不需要手动删除，因为使用了parent-child关系
}

bool DocToXmlConverter::isSupported(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    return SUPPORTED_EXTENSIONS.contains(fileInfo.suffix().toLower());
}

FileConverter::ConvertStatus DocToXmlConverter::extractFields(const QString& filePath,
                                                              QMap<QString, FieldInfo>& fields)
{
    // 验证文件
    if (!validateFilePath(filePath)) {
        setLastError(QS("DOCX文件不存在或无法读取"));
        return ConvertStatus::FILE_NOT_FOUND;
    }

    if (!isSupported(filePath)) {
        setLastError(QS("不支持的文件格式，仅支持.docx文件"));
        return ConvertStatus::INVALID_FORMAT;
    }

    // 设置当前文件路径，供convertToXml使用
    m_currentFilePath = filePath;

    // 使用完整的内容提取功能
    ConvertStatus status = extractAllContent(filePath, fields);
    if (status != ConvertStatus::SUCCESS) {
        return status;
    }

    if (fields.isEmpty()) {
        setLastError(QS("无法从文档中提取到有效字段"));
        return ConvertStatus::PARSE_ERROR;
    }

    return ConvertStatus::SUCCESS;
}

FileConverter::ConvertStatus DocToXmlConverter::convertToXml(const QMap<QString, FieldInfo>& fields,
                                                             QByteArray& xmlOutput)
{
    QBuffer buffer(&xmlOutput);
    buffer.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&buffer);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML头部
    writer.writeStartDocument();
    writer.writeStartElement("ReportMasonDocument");
    writer.writeAttribute("version", "2.0");
    writer.writeAttribute("created", QDateTime::currentDateTime().toString(Qt::ISODate));
    writer.writeAttribute("type", "Word Document");
    writer.writeAttribute("source", "DocToXmlConverter");

    // 写入元数据
    writer.writeStartElement("metadata");
    writer.writeTextElement("title", "实验报告模板");
    writer.writeTextElement("format", "docx");
    writer.writeTextElement("converter", "DocToXmlConverter");
    writer.writeEndElement(); // metadata

    // 写入文档结构
    writer.writeStartElement("structure");
    
    // 收集所有元素并按位置排序
    QList<QPair<QRect, QByteArray>> allElements;
    
    // 1. 提取图片内容
    if (m_imageExtractor) {
        QList<ImageInfo> images;
        ExtractStatus status = m_imageExtractor->extractImages(m_currentFilePath, images);
        if (status == ExtractStatus::SUCCESS && !images.isEmpty()) {
            for (const ImageInfo& image : images) {
                QByteArray imageXml = m_imageExtractor->exportToXmlByteArray(image);
                allElements.append(qMakePair(image.position, imageXml));
            }
        }
    }

    // 2. 提取表格内容
    if (m_tableExtractor) {
        QList<TableInfo> tables;
        ExtractStatus status = m_tableExtractor->extractTables(m_currentFilePath, tables);
        if (status == ExtractStatus::SUCCESS && !tables.isEmpty()) {
            for (const TableInfo& table : tables) {
                QByteArray tableXml = m_tableExtractor->exportToXmlByteArray(table);
                allElements.append(qMakePair(table.position, tableXml));
            }
        }
    }

    // 3. 提取图表内容
    if (m_chartExtractor) {
        QList<ChartInfo> charts;
        ExtractStatus status = m_chartExtractor->extractCharts(m_currentFilePath, charts);
        if (status == ExtractStatus::SUCCESS && !charts.isEmpty()) {
            for (const ChartInfo& chart : charts) {
                QByteArray chartXml = m_chartExtractor->exportToXmlByteArray(chart);
                allElements.append(qMakePair(chart.position, chartXml));
            }
        }
    }
    
    // 按Y坐标排序（从上到下）
    std::sort(allElements.begin(), allElements.end(), 
        [](const QPair<QRect, QByteArray>& a, const QPair<QRect, QByteArray>& b) {
            return a.first.y() < b.first.y();
        });
    
    // 写入排序后的元素
    writer.writeStartElement("elements");
    writer.writeAttribute("count", QString::number(allElements.size()));
    
    for (const auto& element : allElements) {
        writer.writeCharacters("\n");
        writer.writeCDATA(element.second);
        writer.writeCharacters("\n");
    }
    
    writer.writeEndElement(); // elements

    // 4. 写入文本内容
    writer.writeStartElement("text");
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        const FieldInfo& field = it.value();
        if (field.keywords.contains(QS("文本")) || field.keywords.contains(QS("段落"))) {
            writer.writeStartElement("paragraph");
            writer.writeAttribute("name", field.name);
            writer.writeCDATA(field.content);
            writer.writeEndElement(); // paragraph
        }
    }
    writer.writeEndElement(); // text

    writer.writeEndElement(); // structure

    // 写入字段映射（用于快速编辑）
    writer.writeStartElement("fields");
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        const FieldInfo& field = it.value();

        writer.writeStartElement("field");
        writer.writeAttribute("name", field.name);
        writer.writeAttribute("required", field.required ? "true" : "false");

        if (!field.description.isEmpty()) {
            writer.writeTextElement("description", field.description);
        }

        writer.writeStartElement("content");
        writer.writeCDATA(field.content);
        writer.writeEndElement();

        if (!field.keywords.isEmpty()) {
            writer.writeStartElement("keywords");
            for (const QString& keyword : field.keywords) {
                writer.writeTextElement("keyword", keyword);
            }
            writer.writeEndElement();
        }

        writer.writeEndElement(); // field
    }
    writer.writeEndElement(); // fields

    writer.writeEndElement(); // ReportMasonDocument
    writer.writeEndDocument();

    return ConvertStatus::SUCCESS;
}

QStringList DocToXmlConverter::getSupportedFormats() const { return SUPPORTED_EXTENSIONS; }

FileConverter::ConvertStatus DocToXmlConverter::extractSdtFields(const QString& docxPath,
                                                                 QMap<QString, FieldInfo>& fields)
{
    try {
        // 读取document.xml
        QByteArray xmlContent = readXmlFromZip(docxPath, DOCX_DOCUMENT_PATH);
        if (xmlContent.isEmpty()) {
            setLastError(QS("无法读取DOCX文档内容"));
            return ConvertStatus::PARSE_ERROR;
        }

        // 解析XML内容
        if (!parseDocumentXml(xmlContent, fields)) {
            setLastError(QS("解析DOCX文档XML失败"));
            return ConvertStatus::PARSE_ERROR;
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QS("提取SDT字段时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus DocToXmlConverter::extractTextContent(const QString& docxPath,
                                                                   QString& textContent)
{
    try {
        // 读取document.xml
        QByteArray xmlContent = readXmlFromZip(docxPath, DOCX_DOCUMENT_PATH);
        if (xmlContent.isEmpty()) {
            setLastError(QS("无法读取DOCX文档内容"));
            qDebug() << "DocToXmlConverter: 无法从ZIP中读取document.xml";
            return ConvertStatus::PARSE_ERROR;
        }

        qDebug() << "DocToXmlConverter: 成功读取document.xml，大小:" << xmlContent.size() << "字节";
        qDebug() << "DocToXmlConverter: XML内容预览:" << QString::fromUtf8(xmlContent.left(500));

        // 解析XML并提取文本
        QXmlStreamReader reader(xmlContent);
        QStringList paragraphs;

        qDebug() << "DocToXmlConverter: 开始解析XML，寻找w:t元素";
        int elementCount = 0;
        int wTCount = 0;

        while (!reader.atEnd() && !reader.hasError()) {
            QXmlStreamReader::TokenType token = reader.readNext();

            if (token == QXmlStreamReader::StartElement) {
                elementCount++;
                QString elementName = reader.name().toString();
                qDebug() << "DocToXmlConverter: 找到元素:" << elementName;

                if (elementName == QS("t")) {
                    wTCount++;
                    QString text = reader.readElementText();
                    qDebug() << "DocToXmlConverter: t元素内容:" << text;
                    if (!text.isEmpty()) {
                        paragraphs.append(text);
                    }
                }
            }
        }

        qDebug() << "DocToXmlConverter: 总共找到" << elementCount << "个元素，其中" << wTCount
                 << "个w:t元素";

        if (reader.hasError()) {
            setLastError(QS("解析XML时出错: %1").arg(reader.errorString()));
            return ConvertStatus::PARSE_ERROR;
        }

        // 将段落用换行符连接，保持段落结构
        textContent = paragraphs.join(QS("\n"));
        qDebug() << "DocToXmlConverter: 提取到文本内容长度:" << textContent.length();
        qDebug() << "DocToXmlConverter: 提取到的段落数:" << paragraphs.size();
        if (!textContent.isEmpty()) {
            qDebug() << "DocToXmlConverter: 文本内容预览:" << textContent.left(200);
        }
        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QS("提取文本内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus DocToXmlConverter::createFilledDocx(
    const QString& templatePath, const QMap<QString, FieldInfo>& fields, const QString& outputPath)
{
    try {
        // 读取模板文档
        QByteArray xmlContent = readXmlFromZip(templatePath, DOCX_DOCUMENT_PATH);
        if (xmlContent.isEmpty()) {
            setLastError(QS("无法读取模板文档"));
            return ConvertStatus::PARSE_ERROR;
        }

        // 填充字段
        QByteArray modifiedXml = fillSdtFields(xmlContent, fields);
        if (modifiedXml.isEmpty()) {
            setLastError(QS("填充字段失败"));
            return ConvertStatus::WRITE_ERROR;
        }

        // 创建新的DOCX文件
        if (!createModifiedZip(templatePath, modifiedXml, outputPath)) {
            setLastError(QS("创建输出文档失败"));
            return ConvertStatus::WRITE_ERROR;
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QS("创建填充文档时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

QByteArray DocToXmlConverter::readXmlFromZip(const QString& zipPath, const QString& internalPath)
{
    QByteArray content;
    if (!KZipUtils::readFileFromZip(zipPath, internalPath, content)) {
        setLastError(QS("无法从ZIP中读取文件: ") + internalPath);
        return QByteArray();
    }
    return content;
}

bool DocToXmlConverter::parseDocumentXml(const QByteArray& xmlContent,
                                         QMap<QString, FieldInfo>& fields)
{
    QXmlStreamReader reader(xmlContent);

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement && reader.name() == QS("sdt")) {
            if (!parseSdtElement(reader, fields)) {
                return false;
            }
        }
    }

    return !reader.hasError();
}

bool DocToXmlConverter::parseSdtElement(QXmlStreamReader& reader, QMap<QString, FieldInfo>& fields)
{
    QString tagName;
    QString content;
    bool inSdtContent = false;
    int depth = 0;

    // 解析SDT属性，获取标签名
    while (reader.readNext() != QXmlStreamReader::EndElement || reader.name() != QS("sdt")) {
        if (reader.tokenType() == QXmlStreamReader::StartElement) {
            if (reader.name() == QS("tag")) {
                tagName = reader.attributes().value(QS("val")).toString();
            } else if (reader.name() == QS("sdtContent")) {
                inSdtContent = true;
                depth++; // 手动跟踪深度
            } else if (inSdtContent && reader.name() == QS("t")) {
                content += reader.readElementText();
            }
        } else if (reader.tokenType() == QXmlStreamReader::EndElement &&
                   reader.name() == QS("sdtContent")) {
            inSdtContent = false;
            depth--; // 手动跟踪深度
        }
    }

    if (!tagName.isEmpty()) {
        FieldInfo field(tagName, content.trimmed());
        fields[tagName] = field;
        return true;
    }

    return false;
}

QByteArray DocToXmlConverter::fillSdtFields(const QByteArray& xmlContent,
                                            const QMap<QString, FieldInfo>& fields)
{
    QByteArray result;
    QXmlStreamReader reader(xmlContent);
    QXmlStreamWriter writer(&result);

    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    bool inSdtContent = false;
    QString currentTag;
    int sdtDepth = 0;

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == QS("w:tag")) {
                currentTag = reader.attributes().value(QS("w:val")).toString();
                writer.writeStartElement(reader.namespaceUri().toString(),
                                         reader.name().toString());
                writer.writeAttributes(reader.attributes());
            } else if (reader.name() == QS("w:sdtContent")) {
                inSdtContent = true;
                sdtDepth++; // 手动跟踪深度
                writer.writeStartElement(reader.namespaceUri().toString(),
                                         reader.name().toString());
                writer.writeAttributes(reader.attributes());

                // 检查是否有对应的字段数据
                if (fields.contains(currentTag)) {
                    const FieldInfo& field = fields[currentTag];
                    writer.writeStartElement("w:p");
                    writer.writeStartElement("w:r");
                    writer.writeStartElement("w:t");
                    writer.writeAttribute("xml:space", "preserve");
                    writer.writeCharacters(field.content);
                    writer.writeEndElement(); // w:t
                    writer.writeEndElement(); // w:r
                    writer.writeEndElement(); // w:p
                }
            } else if (!inSdtContent) {
                writer.writeStartElement(reader.namespaceUri().toString(),
                                         reader.name().toString());
                writer.writeAttributes(reader.attributes());
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (reader.name() == QS("w:sdtContent")) {
                inSdtContent = false;
                sdtDepth--; // 手动跟踪深度
                writer.writeEndElement();
            } else if (!inSdtContent) {
                writer.writeEndElement();
            }
        } else if (token == QXmlStreamReader::Characters && !inSdtContent) {
            writer.writeCharacters(reader.text().toString());
        }
    }

    return result;
}

bool DocToXmlConverter::createModifiedZip(const QString& templatePath,
                                          const QByteArray& modifiedXml, const QString& outputPath)
{
    try {
        // 使用KZipUtils复制ZIP文件并替换document.xml
        QMap<QString, QByteArray> replacements;
        replacements[DOCX_DOCUMENT_PATH] = modifiedXml;

        if (!KZipUtils::copyZipWithReplacements(templatePath, outputPath, replacements)) {
            setLastError(QS("无法创建修改后的ZIP文件"));
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        setLastError(QS("创建ZIP文件时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return false;
    }
}

void DocToXmlConverter::extractFieldsFromParagraphs(const QString& textContent,
                                                    QMap<QString, FieldInfo>& fields)
{
    qDebug() << "DocToXmlConverter: 开始从段落提取字段，输入文本长度:" << textContent.length();
    QString cleanedText = processWordFormatting(textContent);
    qDebug() << "DocToXmlConverter: 清理后文本长度:" << cleanedText.length();

    // 使用基类的通用字段提取功能
    FileConverter::extractFieldsFromText(cleanedText, fields);
    qDebug() << "DocToXmlConverter: 基类提取到字段数:" << fields.size();

    // 如果基类没有提取到字段，尝试按段落分割
    if (fields.isEmpty()) {
        QStringList paragraphs = cleanedText.split(QS("\n"), Qt::SkipEmptyParts);
        for (int i = 0; i < paragraphs.size() && i < 20; ++i) { // 限制最多20个段落
            QString paragraph = paragraphs[i].trimmed();
            if (!paragraph.isEmpty() && paragraph.length() > 2) {
                QString fieldName = QStringLiteral("Paragraph_%1").arg(i + 1);
                fields[fieldName] = FieldInfo(fieldName, paragraph, false);
            }
        }
    }

    qDebug() << "DocToXmlConverter: 最终提取到字段数:" << fields.size();
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        qDebug() << "DocToXmlConverter: 字段" << it.key() << ":" << it.value().content;
    }
}

QString DocToXmlConverter::processWordFormatting(const QString& text) const
{
    QString processed = text;

    // 移除Word特有的控制字符
    processed.remove(QRegularExpression(QStringLiteral("[\u0001-\u0007]")));

    // 统一换行符
    processed.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    processed.replace(QStringLiteral("\r"), QStringLiteral("\n"));

    // 移除多余空白
    processed = processed.simplified();

    // 处理Word的特殊字符
    processed.replace(QStringLiteral("&amp;"), QStringLiteral("&"));
    processed.replace(QStringLiteral("&lt;"), QStringLiteral("<"));
    processed.replace(QStringLiteral("&gt;"), QStringLiteral(">"));
    processed.replace(QStringLiteral("&quot;"), QStringLiteral("\""));
    processed.replace(QStringLiteral("&#39;"), QStringLiteral("'"));

    return processed;
}

FileConverter::ConvertStatus DocToXmlConverter::extractAllContent(const QString& docxPath,
                                                                  QMap<QString, FieldInfo>& fields)
{
    try {
        qDebug() << "DocToXmlConverter: 开始提取DOCX完整内容:" << docxPath;

        // 1. 提取文本内容
        QString textContent;
        ConvertStatus status = extractTextContent(docxPath, textContent);
        if (status == ConvertStatus::SUCCESS && !textContent.isEmpty()) {
            // 使用基类的通用字段提取功能
            FileConverter::extractFieldsFromText(textContent, fields);

            // 如果基类没有提取到字段，尝试从段落提取
            if (fields.isEmpty()) {
                extractFieldsFromParagraphs(textContent, fields);
            }

            qDebug() << "DocToXmlConverter: 文本内容提取完成，字段数:" << fields.size();
        }

        // 2. 提取图片内容
        status = extractImageContent(docxPath, fields);
        if (status == ConvertStatus::SUCCESS) {
            qDebug() << "DocToXmlConverter: 图片内容提取完成";
        }

        // 3. 提取表格内容
        status = extractTableContent(docxPath, fields);
        if (status == ConvertStatus::SUCCESS) {
            qDebug() << "DocToXmlConverter: 表格内容提取完成";
        }

        // 4. 提取图表内容
        status = extractChartContent(docxPath, fields);
        if (status == ConvertStatus::SUCCESS) {
            qDebug() << "DocToXmlConverter: 图表内容提取完成";
        }

        qDebug() << "DocToXmlConverter: 完整内容提取完成，总字段数:" << fields.size();
        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取DOCX完整内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus DocToXmlConverter::extractImageContent(
    const QString& docxPath, QMap<QString, FieldInfo>& fields)
{
    try {
        if (!m_imageExtractor) {
            setLastError(QS("图片提取器未初始化"));
            return ConvertStatus::UNKNOWN_ERROR;
        }

        // 使用图片提取器提取图片信息
        QList<ImageInfo> images;
        ExtractStatus status = m_imageExtractor->extractImages(docxPath, images);

        if (status == ExtractStatus::SUCCESS) {
            for (int i = 0; i < images.size(); ++i) {
                const ImageInfo& image = images[i];
                QString fieldName = QString(QS("Image_%1")).arg(i + 1);

                FieldInfo field(fieldName, image.originalPath);
                field.description = QString(QS("图片: %1 (格式: %2, 大小: %3x%4)"))
                                   .arg(image.originalPath)
                                   .arg(image.format)
                                   .arg(image.size.width())
                                   .arg(image.size.height());
                field.keywords = QStringList() << QS("图片") << QS("图像") << image.format;

                fields[fieldName] = field;
            }

            qDebug() << "DocToXmlConverter: 提取到" << images.size() << "个图片";
        }

        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取图片内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus DocToXmlConverter::extractTableContent(
    const QString& docxPath, QMap<QString, FieldInfo>& fields)
{
    try {
        if (!m_tableExtractor) {
            setLastError(QS("表格提取器未初始化"));
            return ConvertStatus::UNKNOWN_ERROR;
        }

        // 使用表格提取器提取表格信息
        QList<TableInfo> tables;
        ExtractStatus status = m_tableExtractor->extractTables(docxPath, tables);

        if (status == ExtractStatus::SUCCESS) {
            for (int i = 0; i < tables.size(); ++i) {
                const TableInfo& table = tables[i];
                QString fieldName = QString(QS("Table_%1")).arg(i + 1);

                // 将表格数据转换为文本格式
                QString tableContent;
                for (int row = 0; row < table.rows; ++row) {
                    QStringList rowData;
                    for (int col = 0; col < table.columns; ++col) {
                        if (row < table.cells.size() && col < table.cells[row].size()) {
                            rowData << table.cells[row][col].content;
                        } else {
                            rowData << QS("");
                        }
                    }
                    tableContent += rowData.join(QS("\t")) + QS("\n");
                }

                FieldInfo field(fieldName, tableContent.trimmed());
                field.description = QString(QS("表格: %1x%2")).arg(table.rows).arg(table.columns);
                field.keywords = QStringList() << QS("表格") << QS("数据");

                fields[fieldName] = field;
            }

            qDebug() << "DocToXmlConverter: 提取到" << tables.size() << "个表格";
        }

        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取表格内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus DocToXmlConverter::extractChartContent(
    const QString& docxPath, QMap<QString, FieldInfo>& fields)
{
    try {
        if (!m_chartExtractor) {
            setLastError(QS("图表提取器未初始化"));
            return ConvertStatus::UNKNOWN_ERROR;
        }

        // 使用图表提取器提取图表信息
        QList<ChartInfo> charts;
        ExtractStatus status = m_chartExtractor->extractCharts(docxPath, charts);

        if (status == ExtractStatus::SUCCESS) {
            for (int i = 0; i < charts.size(); ++i) {
                const ChartInfo& chart = charts[i];
                QString fieldName = QString(QS("Chart_%1")).arg(i + 1);

                FieldInfo field(fieldName, chart.title);
                QString chartTypeStr = QString::number(static_cast<int>(chart.type));
                field.description =
                    QString(QS("图表: %1 (类型: %2)")).arg(chart.title).arg(chartTypeStr);
                field.keywords = QStringList() << QS("图表") << QS("图形") << chartTypeStr;

                fields[fieldName] = field;
            }

            qDebug() << "DocToXmlConverter: 提取到" << charts.size() << "个图表";
        }

        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取图表内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}
