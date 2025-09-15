/*
 * @Author: seelights
 * @Date: 2025-09-14 20:02:52
 * @LastEditTime: 2025-09-15 00:34:15
 * @LastEditors: seelights
 * @Description:
 * @FilePath: \ReportMason\src\DocToXmlConverter.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */
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

// 静态常量定义
const QString DocToXmlConverter::DOCX_DOCUMENT_PATH = "word/document.xml";
const QString DocToXmlConverter::DOCX_STYLES_PATH = "word/styles.xml";
const QStringList DocToXmlConverter::SUPPORTED_EXTENSIONS = {"docx"};

DocToXmlConverter::DocToXmlConverter(QObject* parent) : FileConverter(parent) {}

DocToXmlConverter::~DocToXmlConverter() {}

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
        setLastError("DOCX文件不存在或无法读取");
        return ConvertStatus::FILE_NOT_FOUND;
    }

    if (!isSupported(filePath)) {
        setLastError("不支持的文件格式，仅支持.docx文件");
        return ConvertStatus::INVALID_FORMAT;
    }

    // 提取文档的文本内容
    QString textContent;
    ConvertStatus status = extractTextContent(filePath, textContent);
    if (status != ConvertStatus::SUCCESS) {
        return status;
    }

    // 使用基类的通用字段提取功能
    FileConverter::extractFieldsFromText(textContent, fields);

    // 如果基类没有提取到字段，尝试从段落提取
    if (fields.isEmpty()) {
        extractFieldsFromParagraphs(textContent, fields);
    }

    if (fields.isEmpty()) {
        setLastError("无法从文档中提取到有效字段");
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
    writer.writeStartElement("Document");
    writer.writeAttribute("version", "1.0");
    writer.writeAttribute("created", QDateTime::currentDateTime().toString(Qt::ISODate));
    writer.writeAttribute("type", "Word Document");
    writer.writeAttribute("source", "DocToXmlConverter");

    // 写入字段信息
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

    writer.writeEndElement(); // Document
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
            setLastError("无法读取DOCX文档内容");
            return ConvertStatus::PARSE_ERROR;
        }

        // 解析XML内容
        if (!parseDocumentXml(xmlContent, fields)) {
            setLastError("解析DOCX文档XML失败");
            return ConvertStatus::PARSE_ERROR;
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString("提取SDT字段时发生异常: %1").arg(e.what()));
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
            setLastError("无法读取DOCX文档内容");
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
                
                if (elementName == "t") {
                    wTCount++;
                    QString text = reader.readElementText();
                    qDebug() << "DocToXmlConverter: t元素内容:" << text;
                    if (!text.isEmpty()) {
                        paragraphs.append(text);
                    }
                }
            }
        }
        
        qDebug() << "DocToXmlConverter: 总共找到" << elementCount << "个元素，其中" << wTCount << "个w:t元素";

        if (reader.hasError()) {
            setLastError(QString("解析XML时出错: %1").arg(reader.errorString()));
            return ConvertStatus::PARSE_ERROR;
        }

        // 将段落用换行符连接，保持段落结构
        textContent = paragraphs.join("\n");
        qDebug() << "DocToXmlConverter: 提取到文本内容长度:" << textContent.length();
        qDebug() << "DocToXmlConverter: 提取到的段落数:" << paragraphs.size();
        if (!textContent.isEmpty()) {
            qDebug() << "DocToXmlConverter: 文本内容预览:" << textContent.left(200);
        }
        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString("提取文本内容时发生异常: %1").arg(e.what()));
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
            setLastError("无法读取模板文档");
            return ConvertStatus::PARSE_ERROR;
        }

        // 填充字段
        QByteArray modifiedXml = fillSdtFields(xmlContent, fields);
        if (modifiedXml.isEmpty()) {
            setLastError("填充字段失败");
            return ConvertStatus::WRITE_ERROR;
        }

        // 创建新的DOCX文件
        if (!createModifiedZip(templatePath, modifiedXml, outputPath)) {
            setLastError("创建输出文档失败");
            return ConvertStatus::WRITE_ERROR;
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString("创建填充文档时发生异常: %1").arg(e.what()));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

QByteArray DocToXmlConverter::readXmlFromZip(const QString& zipPath, const QString& internalPath)
{
    QByteArray content;
    if (!KZipUtils::readFileFromZip(zipPath, internalPath, content)) {
        setLastError("无法从ZIP中读取文件: " + internalPath);
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

        if (token == QXmlStreamReader::StartElement && reader.name() == "sdt") {
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
    while (reader.readNext() != QXmlStreamReader::EndElement || reader.name() != "sdt") {
        if (reader.tokenType() == QXmlStreamReader::StartElement) {
            if (reader.name() == "tag") {
                tagName = reader.attributes().value("val").toString();
            } else if (reader.name() == "sdtContent") {
                inSdtContent = true;
                depth++; // 手动跟踪深度
            } else if (inSdtContent && reader.name() == "t") {
                content += reader.readElementText();
            }
        } else if (reader.tokenType() == QXmlStreamReader::EndElement &&
                   reader.name() == "sdtContent") {
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
            if (reader.name() == "w:tag") {
                currentTag = reader.attributes().value("w:val").toString();
                writer.writeStartElement(reader.namespaceUri().toString(),
                                         reader.name().toString());
                writer.writeAttributes(reader.attributes());
            } else if (reader.name() == "w:sdtContent") {
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
            if (reader.name() == "w:sdtContent") {
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
            setLastError("无法创建修改后的ZIP文件");
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        setLastError(QString("创建ZIP文件时发生异常: %1").arg(e.what()));
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
        QStringList paragraphs = cleanedText.split('\n', Qt::SkipEmptyParts);
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
    processed.replace("\r\n", "\n");
    processed.replace("\r", "\n");

    // 移除多余空白
    processed = processed.simplified();

    // 处理Word的特殊字符
    processed.replace("&amp;", "&");
    processed.replace("&lt;", "<");
    processed.replace("&gt;", ">");
    processed.replace("&quot;", "\"");
    processed.replace("&#39;", "'");

    return processed;
}
