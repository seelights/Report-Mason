#include "PdfToXmlConverter.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QRegularExpression>
#include <QTextDocument>
#include <QXmlStreamWriter>
#include <QBuffer>
#include <QTextStream>
#include <QDataStream>

// 静态常量定义
const QStringList PdfToXmlConverter::SUPPORTED_EXTENSIONS = {"pdf"};

const QRegularExpression PdfToXmlConverter::TEXT_PATTERN(R"((?:BT\s+)(?:[^E]*?)(?:ET))");
const QRegularExpression PdfToXmlConverter::FORM_FIELD_PATTERN(R"(\/T\s+\(([^)]+)\))");
const QRegularExpression PdfToXmlConverter::METADATA_PATTERN(R"(\/([A-Za-z]+)\s+\(([^)]+)\))");

const QMap<QString, QRegularExpression> PdfToXmlConverter::FIELD_PATTERNS = {
    {"Title", QRegularExpression(R"(题目[：:]\s*(.+?)(?:\n|$))")},
    {"StudentName", QRegularExpression(R"(姓名[：:]\s*(.+?)(?:\n|$))")},
    {"StudentID", QRegularExpression(R"(学号[：:]\s*(.+?)(?:\n|$))")},
    {"Class", QRegularExpression(R"(班级[：:]\s*(.+?)(?:\n|$))")},
    {"Abstract", QRegularExpression(R"(摘要[：:]\s*(.+?)(?=关键词|结论|$))",
                                    QRegularExpression::DotMatchesEverythingOption)},
    {"Keywords", QRegularExpression(R"(关键词[：:]\s*(.+?)(?:\n|$))")},
    {"ExperimentObjective", QRegularExpression(R"(实验目的[：:]\s*(.+?)(?=实验原理|实验步骤|$))",
                                               QRegularExpression::DotMatchesEverythingOption)},
    {"ExperimentPrinciple", QRegularExpression(R"(实验原理[：:]\s*(.+?)(?=实验步骤|实验内容|$))",
                                               QRegularExpression::DotMatchesEverythingOption)},
    {"ExperimentSteps", QRegularExpression(R"(实验步骤[：:]\s*(.+?)(?=实验结果|实验数据|$))",
                                           QRegularExpression::DotMatchesEverythingOption)},
    {"ExperimentResults", QRegularExpression(R"(实验结果[：:]\s*(.+?)(?=实验分析|结论|$))",
                                             QRegularExpression::DotMatchesEverythingOption)},
    {"ExperimentAnalysis", QRegularExpression(R"(实验分析[：:]\s*(.+?)(?=结论|$))",
                                              QRegularExpression::DotMatchesEverythingOption)},
    {"Conclusion",
     QRegularExpression(R"(结论[：:]\s*(.+?)$)", QRegularExpression::DotMatchesEverythingOption)}};

PdfToXmlConverter::PdfToXmlConverter(QObject* parent)
    : FileConverter(parent), m_preserveLayout(true), m_extractImages(false)
{
}

PdfToXmlConverter::~PdfToXmlConverter() {}

bool PdfToXmlConverter::isSupported(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    return SUPPORTED_EXTENSIONS.contains(fileInfo.suffix().toLower());
}

FileConverter::ConvertStatus PdfToXmlConverter::extractFields(const QString& filePath,
                                                              QMap<QString, FieldInfo>& fields)
{
    // 验证文件
    if (!validateFilePath(filePath)) {
        setLastError("PDF文件不存在或无法读取");
        return ConvertStatus::FILE_NOT_FOUND;
    }

    if (!isSupported(filePath)) {
        setLastError("不支持的文件格式，仅支持.pdf文件");
        return ConvertStatus::INVALID_FORMAT;
    }

    // 首先尝试提取表单字段
    ConvertStatus status = extractFormFields(filePath, fields);
    if (status == ConvertStatus::SUCCESS && !fields.isEmpty()) {
        return status;
    }

    // 如果表单字段为空，则提取文本内容并智能识别字段
    QString textContent;
    status = extractTextContent(filePath, textContent);
    if (status != ConvertStatus::SUCCESS) {
        return status;
    }

    // 从文本中智能提取字段
    extractFieldsFromPdfText(textContent, fields);

    if (fields.isEmpty()) {
        setLastError("无法从PDF文档中提取到有效字段");
        return ConvertStatus::PARSE_ERROR;
    }

    return ConvertStatus::SUCCESS;
}

FileConverter::ConvertStatus PdfToXmlConverter::convertToXml(const QMap<QString, FieldInfo>& fields,
                                                             QByteArray& xmlOutput)
{
    QBuffer buffer(&xmlOutput);
    buffer.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&buffer);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML头部
    writer.writeStartDocument();
    writer.writeStartElement("ReportMasonTemplate");
    writer.writeAttribute("version", "1.0");
    writer.writeAttribute("created", QDateTime::currentDateTime().toString(Qt::ISODate));
    writer.writeAttribute("type", "PDF Document");

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

    writer.writeEndElement(); // ReportMasonTemplate
    writer.writeEndDocument();

    return ConvertStatus::SUCCESS;
}

QStringList PdfToXmlConverter::getSupportedFormats() const { return SUPPORTED_EXTENSIONS; }

FileConverter::ConvertStatus PdfToXmlConverter::extractTextContent(const QString& pdfPath,
                                                                   QString& textContent)
{
    try {
        // 读取PDF文件
        QFile pdfFile(pdfPath);
        if (!pdfFile.open(QIODevice::ReadOnly)) {
            setLastError("无法打开PDF文件");
            return ConvertStatus::FILE_NOT_FOUND;
        }

        QByteArray pdfData = pdfFile.readAll();
        pdfFile.close();

        if (pdfData.isEmpty()) {
            setLastError("PDF文件为空");
            return ConvertStatus::PARSE_ERROR;
        }

        // 提取文本内容
        if (!extractTextFromPdfData(pdfData, textContent)) {
            setLastError("无法从PDF中提取文本内容");
            return ConvertStatus::PARSE_ERROR;
        }

        // 处理PDF文本格式
        textContent = processPdfText(textContent);

        if (textContent.isEmpty()) {
            setLastError("PDF文档中没有可提取的文本内容");
            return ConvertStatus::PARSE_ERROR;
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString("提取PDF文本时发生异常: %1").arg(e.what()));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus PdfToXmlConverter::extractFormFields(
    const QString& pdfPath, QMap<QString, FieldInfo>& formFields)
{
    try {
        // 读取PDF文件
        QFile pdfFile(pdfPath);
        if (!pdfFile.open(QIODevice::ReadOnly)) {
            setLastError("无法打开PDF文件");
            return ConvertStatus::FILE_NOT_FOUND;
        }

        QByteArray pdfData = pdfFile.readAll();
        pdfFile.close();

        // 简单的PDF表单字段提取（基于正则表达式）
        QString pdfContent = QString::fromUtf8(pdfData);
        QRegularExpressionMatchIterator matches = FORM_FIELD_PATTERN.globalMatch(pdfContent);

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString fieldName = match.captured(1);

            if (!fieldName.isEmpty()) {
                FieldInfo field(fieldName);
                field.description = QString("PDF表单字段: %1").arg(fieldName);
                formFields[fieldName] = field;
            }
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString("提取PDF表单字段时发生异常: %1").arg(e.what()));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus PdfToXmlConverter::extractMetadata(const QString& pdfPath,
                                                                QMap<QString, QString>& metadata)
{
    try {
        // 读取PDF文件
        QFile pdfFile(pdfPath);
        if (!pdfFile.open(QIODevice::ReadOnly)) {
            setLastError("无法打开PDF文件");
            return ConvertStatus::FILE_NOT_FOUND;
        }

        QByteArray pdfData = pdfFile.readAll();
        pdfFile.close();

        // 简单的PDF元数据提取
        QString pdfContent = QString::fromUtf8(pdfData);
        QRegularExpressionMatchIterator matches = METADATA_PATTERN.globalMatch(pdfContent);

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString key = match.captured(1);
            QString value = match.captured(2);

            if (!key.isEmpty() && !value.isEmpty()) {
                metadata[key] = value;
            }
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString("提取PDF元数据时发生异常: %1").arg(e.what()));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

void PdfToXmlConverter::setExtractionOptions(bool preserveLayout, bool extractImages)
{
    m_preserveLayout = preserveLayout;
    m_extractImages = extractImages;
}

bool PdfToXmlConverter::extractTextFromPdfData(const QByteArray& pdfData, QString& textContent)
{
    // 这是一个简化的PDF文本提取实现
    // 在实际项目中，建议使用专门的PDF处理库如Poppler-Qt或MuPDF

    QString pdfContent = QString::fromUtf8(pdfData);

    // 查找文本流对象
    QRegularExpressionMatchIterator matches = TEXT_PATTERN.globalMatch(pdfContent);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString streamContent = match.captured(0);

        // 简单的文本提取（实际PDF解析会更复杂）
        if (streamContent.contains("Tj") || streamContent.contains("TJ")) {
            // 提取文本内容
            QString text = streamContent;
            text.remove(QRegularExpression(R"(BT\s+)"));
            text.remove(QRegularExpression(R"(\s+ET)"));
            text.remove(QRegularExpression(R"([^a-zA-Z0-9\u4e00-\u9fff\s])"));
            textContent += text + " ";
        }
    }

    // 如果上面的方法没有提取到文本，尝试更简单的方法
    if (textContent.isEmpty()) {
        // 提取PDF中可见的文本（这是一个非常简化的实现）
        QRegularExpression simpleTextPattern(R"(\(([^)]+)\))");
        QRegularExpressionMatchIterator simpleMatches = simpleTextPattern.globalMatch(pdfContent);

        while (simpleMatches.hasNext()) {
            QRegularExpressionMatch match = simpleMatches.next();
            QString text = match.captured(1);

            // 过滤掉明显不是文本的内容
            if (text.length() > 1 && text.length() < 100 &&
                !text.contains(QRegularExpression(R"([0-9]+\.[0-9]+)"))) {
                textContent += text + " ";
            }
        }
    }

    return !textContent.isEmpty();
}

bool PdfToXmlConverter::parsePdfStream(const QByteArray& streamData, QString& textContent)
{
    // 简化的PDF流解析
    // 在实际应用中，这里需要完整的PDF解析逻辑

    QString stream = QString::fromUtf8(streamData);

    // 查找文本操作符
    if (stream.contains("Tj") || stream.contains("TJ")) {
        // 提取文本内容
        QRegularExpression textPattern(R"(\(([^)]+)\)\s*Tj)");
        QRegularExpressionMatchIterator matches = textPattern.globalMatch(stream);

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            textContent += match.captured(1) + " ";
        }

        return true;
    }

    return false;
}

void PdfToXmlConverter::extractFieldsFromPdfText(const QString& textContent,
                                                 QMap<QString, FieldInfo>& fields)
{
    QString cleanedText = processPdfText(textContent);

    // 使用基类的智能字段提取功能
    FileConverter::extractFieldsFromText(cleanedText, fields);

    // 添加PDF特有的字段识别
    for (auto it = FIELD_PATTERNS.begin(); it != FIELD_PATTERNS.end(); ++it) {
        QString fieldName = it.key();
        QRegularExpression pattern = it.value();

        QRegularExpressionMatch match = pattern.match(cleanedText);
        if (match.hasMatch()) {
            QString content = match.captured(1).trimmed();
            if (!content.isEmpty()) {
                FieldInfo field(fieldName, content);
                fields[fieldName] = field;
            }
        }
    }

    // 识别表格数据
    QMap<QString, QStringList> tables;
    if (extractTables(cleanedText, tables)) {
        for (auto it = tables.begin(); it != tables.end(); ++it) {
            QString tableName = it.key();
            QStringList tableData = it.value();
            QString tableContent = tableData.join("\n");

            FieldInfo field(tableName, tableContent);
            field.description = QString("表格数据: %1").arg(tableName);
            fields[tableName] = field;
        }
    }

    // 识别列表数据
    QMap<QString, QStringList> lists;
    if (extractLists(cleanedText, lists)) {
        for (auto it = lists.begin(); it != lists.end(); ++it) {
            QString listName = it.key();
            QStringList listData = it.value();
            QString listContent = listData.join("\n");

            FieldInfo field(listName, listContent);
            field.description = QString("列表数据: %1").arg(listName);
            fields[listName] = field;
        }
    }
}

QString PdfToXmlConverter::processPdfText(const QString& text) const
{
    QString processed = text;

    // 移除PDF特有的控制字符
    processed.remove(QRegularExpression(R"([\x00-\x08\x0B\x0C\x0E-\x1F])"));

    // 统一换行符
    processed.replace("\r\n", "\n");
    processed.replace("\r", "\n");

    // 移除多余空白
    processed = processed.simplified();

    // 处理PDF的特殊字符编码
    processed.replace("\\(", "(");
    processed.replace("\\)", ")");
    processed.replace("\\\\", "\\");
    processed.replace("\\n", "\n");
    processed.replace("\\r", "\r");
    processed.replace("\\t", "\t");

    // 移除PDF操作符
    processed.remove(QRegularExpression(R"(\s+(BT|ET|Tj|TJ|Tm|Td|TD|T*\w)\s+)"));

    // 清理多余的空白
    processed.replace(QRegularExpression(R"(\s+)"), " ");

    return processed.trimmed();
}

bool PdfToXmlConverter::extractTables(const QString& textContent,
                                      QMap<QString, QStringList>& tables)
{
    // 简单的表格识别（基于对齐的文本模式）
    QRegularExpression tablePattern(R"((?:数据|结果|表格)[：:]\s*([^\n]+(?:\n[^\n]+)*))",
                                    QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator matches = tablePattern.globalMatch(textContent);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString tableContent = match.captured(1);

        // 简单的表格行分割
        QStringList rows = tableContent.split('\n', Qt::SkipEmptyParts);
        if (rows.size() > 1) {
            tables["TableData"] = rows;
            return true;
        }
    }

    return false;
}

bool PdfToXmlConverter::extractLists(const QString& textContent, QMap<QString, QStringList>& lists)
{
    // 识别编号列表
    QRegularExpression numberedListPattern(
        R"((?:步骤|要点|项目)[：:]\s*((?:\d+[\.\)]\s*[^\n]+(?:\n|$))+))",
        QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator matches = numberedListPattern.globalMatch(textContent);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString listContent = match.captured(1);

        QStringList items = listContent.split('\n', Qt::SkipEmptyParts);
        if (items.size() > 1) {
            lists["NumberedList"] = items;
            return true;
        }
    }

    // 识别要点列表
    QRegularExpression bulletListPattern(R"((?:要点|注意)[：:]\s*((?:[•·▪▫]\s*[^\n]+(?:\n|$))+))",
                                         QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator bulletMatches = bulletListPattern.globalMatch(textContent);

    while (bulletMatches.hasNext()) {
        QRegularExpressionMatch match = bulletMatches.next();
        QString listContent = match.captured(1);

        QStringList items = listContent.split('\n', Qt::SkipEmptyParts);
        if (items.size() > 1) {
            lists["BulletList"] = items;
            return true;
        }
    }

    return false;
}
