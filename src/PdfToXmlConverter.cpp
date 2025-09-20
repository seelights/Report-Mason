#include "QtCompat.h"
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
#include "tools/pdf/PdfImageExtractor.h"
#include "tools/pdf/PdfTableExtractor.h"
#include "tools/pdf/PdfChartExtractor.h"

// 静态常量定义
const QStringList PdfToXmlConverter::SUPPORTED_EXTENSIONS = {QS("pdf")};

const QRegularExpression PdfToXmlConverter::TEXT_PATTERN(QS(R"((?:BT\s+)(?:[^E]*?)(?:ET))"));
const QRegularExpression PdfToXmlConverter::FORM_FIELD_PATTERN(QS(R"(\/T\s+\(([^)]+)\))"));
const QRegularExpression PdfToXmlConverter::METADATA_PATTERN(QS(R"(\/([A-Za-z]+)\s+\(([^)]+)\))"));

// 注意：PdfToXmlConverter专注于PDF到XML的无损转换
// 实验报告字段识别功能应该在其他专门的类中实现

PdfToXmlConverter::PdfToXmlConverter(QObject* parent)
    : FileConverter(parent), m_preserveLayout(true), m_extractImages(false),
      m_imageExtractor(nullptr), m_tableExtractor(nullptr), m_chartExtractor(nullptr)
{
    // 初始化内容提取器
    m_imageExtractor = new PdfImageExtractor(this);
    m_tableExtractor = new PdfTableExtractor(this);
    m_chartExtractor = new PdfChartExtractor(this);
}

PdfToXmlConverter::~PdfToXmlConverter()
{
    // 析构函数中不需要手动删除，因为使用了parent-child关系
}

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
        setLastError(QS("PDF文件不存在或无法读取"));
        return ConvertStatus::FILE_NOT_FOUND;
    }

    if (!isSupported(filePath)) {
        setLastError(QS("不支持的文件格式，仅支持.pdf文件"));
        return ConvertStatus::INVALID_FORMAT;
    }

    // 使用完整的内容提取功能
    ConvertStatus status = extractAllContent(filePath, fields);
    if (status != ConvertStatus::SUCCESS) {
        return status;
    }

    if (fields.isEmpty()) {
        setLastError(QS("无法从PDF文档中提取到有效字段"));
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
    writer.writeStartElement("ReportMasonDocument");
    writer.writeAttribute("version", "2.0");
    writer.writeAttribute("created", QDateTime::currentDateTime().toString(Qt::ISODate));
    writer.writeAttribute("type", "PDF Document");
    writer.writeAttribute("source", "PdfToXmlConverter");

    // 写入元数据
    writer.writeStartElement("metadata");
    writer.writeTextElement("title", "实验报告模板");
    writer.writeTextElement("format", "pdf");
    writer.writeTextElement("converter", "PdfToXmlConverter");
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

QStringList PdfToXmlConverter::getSupportedFormats() const { return SUPPORTED_EXTENSIONS; }

FileConverter::ConvertStatus PdfToXmlConverter::extractTextContent(const QString& pdfPath,
                                                                   QString& textContent)
{
    try {
        // 读取PDF文件
        QFile pdfFile(pdfPath);
        if (!pdfFile.open(QIODevice::ReadOnly)) {
            setLastError(QS("无法打开PDF文件"));
            return ConvertStatus::FILE_NOT_FOUND;
        }

        QByteArray pdfData = pdfFile.readAll();
        pdfFile.close();

        if (pdfData.isEmpty()) {
            setLastError(QS("PDF文件为空"));
            return ConvertStatus::PARSE_ERROR;
        }

        // 提取文本内容
        if (!extractTextFromPdfData(pdfData, textContent)) {
            setLastError(QS("无法从PDF中提取文本内容"));
            return ConvertStatus::PARSE_ERROR;
        }

        // 处理PDF文本格式
        textContent = processPdfText(textContent);

        if (textContent.isEmpty()) {
            setLastError(QS("PDF文档中没有可提取的文本内容"));
            return ConvertStatus::PARSE_ERROR;
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString(QS("提取PDF文本时发生异常: %1")).arg(QString::fromUtf8(e.what())));
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
            setLastError(QS("无法打开PDF文件"));
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
                field.description = QString(QS("PDF表单字段: %1")).arg(fieldName);
                formFields[fieldName] = field;
            }
        }

        return ConvertStatus::SUCCESS;
    } catch (const std::exception& e) {
        setLastError(QString(QS("提取PDF表单字段时发生异常: %1")).arg(QString::fromUtf8(e.what())));
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
            setLastError(QS("无法打开PDF文件"));
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
        setLastError(QString(QS("提取PDF元数据时发生异常: %1")).arg(QString::fromUtf8(e.what())));
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
        if (streamContent.contains(QS("Tj")) || streamContent.contains(QS("TJ"))) {
            // 提取文本内容
            QString text = streamContent;
            text.remove(QRegularExpression(QS(R"(BT\s+)")));
            text.remove(QRegularExpression(QS(R"(\s+ET)")));
            text.remove(QRegularExpression(QS("[^a-zA-Z0-9\\u4e00-\\u9fff\\s]")));
            textContent += text + QS(" ");
        }
    }

    // 如果上面的方法没有提取到文本，尝试更简单的方法
    if (textContent.isEmpty()) {
        // 提取PDF中可见的文本（这是一个非常简化的实现）
        QRegularExpression simpleTextPattern(QS(R"(\(([^)]+)\))"));
        QRegularExpressionMatchIterator simpleMatches = simpleTextPattern.globalMatch(pdfContent);

        while (simpleMatches.hasNext()) {
            QRegularExpressionMatch match = simpleMatches.next();
            QString text = match.captured(1);

            // 过滤掉明显不是文本的内容
            if (text.length() > 1 && text.length() < 100 &&
                !text.contains(QRegularExpression(QS(R"([0-9]+\.[0-9]+)")))) {
                textContent += text + QS(" ");
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
    if (stream.contains(QS("Tj")) || stream.contains(QS("TJ"))) {
        // 提取文本内容
        QRegularExpression textPattern(QS(R"(\(([^)]+)\)\s*Tj)"));
        QRegularExpressionMatchIterator matches = textPattern.globalMatch(stream);

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            textContent += match.captured(1) + QS(" ");
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

    // 注意：PDF特有的实验报告字段识别功能已移除
    // 专注于PDF到XML的无损转换，字段识别功能应该在专门的类中实现

    // 识别表格数据
    QMap<QString, QStringList> tables;
    if (extractTables(cleanedText, tables)) {
        for (auto it = tables.begin(); it != tables.end(); ++it) {
            QString tableName = it.key();
            QStringList tableData = it.value();
            QString tableContent = tableData.join(QS("\n"));

            FieldInfo field(tableName, tableContent);
            field.description = QString(QS("表格数据: %1")).arg(tableName);
            fields[tableName] = field;
        }
    }

    // 识别列表数据
    QMap<QString, QStringList> lists;
    if (extractLists(cleanedText, lists)) {
        for (auto it = lists.begin(); it != lists.end(); ++it) {
            QString listName = it.key();
            QStringList listData = it.value();
            QString listContent = listData.join(QS("\n"));

            FieldInfo field(listName, listContent);
            field.description = QString(QS("列表数据: %1")).arg(listName);
            fields[listName] = field;
        }
    }
}

QString PdfToXmlConverter::processPdfText(const QString& text) const
{
    QString processed = text;

    // 移除PDF特有的控制字符
    processed.remove(QRegularExpression(QS(R"([\x00-\x08\x0B\x0C\x0E-\x1F])")));

    // 统一换行符
    processed.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    processed.replace(QStringLiteral("\r"), QStringLiteral("\n"));

    // 移除多余空白
    processed = processed.simplified();

    // 处理PDF的特殊字符编码
    processed.replace(QStringLiteral("\\("), QStringLiteral("("));
    processed.replace(QStringLiteral("\\)"), QStringLiteral(")"));
    processed.replace(QStringLiteral("\\\\"), QStringLiteral("\\"));
    processed.replace(QStringLiteral("\\n"), QStringLiteral("\n"));
    processed.replace(QStringLiteral("\\r"), QStringLiteral("\r"));
    processed.replace(QStringLiteral("\\t"), QStringLiteral("\t"));

    // 移除PDF操作符
    processed.remove(QRegularExpression(QS(R"(\s+(BT|ET|Tj|TJ|Tm|Td|TD|T*\w)\s+)")));

    // 清理多余的空白
    processed.replace(QRegularExpression(QS(R"(\s+)")), QS(" "));

    return processed.trimmed();
}

bool PdfToXmlConverter::extractTables(const QString& textContent,
                                      QMap<QString, QStringList>& tables)
{
    // 简单的表格识别（基于对齐的文本模式）
    QRegularExpression tablePattern(QS(R"((?:数据|结果|表格)[：:]\s*([^\n]+(?:\n[^\n]+)*))"),
                                    QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator matches = tablePattern.globalMatch(textContent);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString tableContent = match.captured(1);

        // 简单的表格行分割
        QStringList rows = tableContent.split(QS("\n"), Qt::SkipEmptyParts);
        if (rows.size() > 1) {
            tables[QS("TableData")] = rows;
            return true;
        }
    }

    return false;
}

bool PdfToXmlConverter::extractLists(const QString& textContent, QMap<QString, QStringList>& lists)
{
    // 识别编号列表
    QRegularExpression numberedListPattern(
        QS(R"((?:步骤|要点|项目)[：:]\s*((?:\d+[\.\)]\s*[^\n]+(?:\n|$))+))"),
        QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator matches = numberedListPattern.globalMatch(textContent);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString listContent = match.captured(1);

        QStringList items = listContent.split(QS("\n"), Qt::SkipEmptyParts);
        if (items.size() > 1) {
            lists[QS("NumberedList")] = items;
            return true;
        }
    }

    // 识别要点列表
    QRegularExpression bulletListPattern(
        QS(R"((?:要点|注意)[：:]\s*((?:[•·▪▫]\s*[^\n]+(?:\n|$))+))"),
        QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator bulletMatches = bulletListPattern.globalMatch(textContent);

    while (bulletMatches.hasNext()) {
        QRegularExpressionMatch match = bulletMatches.next();
        QString listContent = match.captured(1);

        QStringList items = listContent.split(QS("\n"), Qt::SkipEmptyParts);
        if (items.size() > 1) {
            lists[QS("BulletList")] = items;
            return true;
        }
    }

    return false;
}

FileConverter::ConvertStatus PdfToXmlConverter::extractAllContent(const QString& pdfPath,
                                                                  QMap<QString, FieldInfo>& fields)
{
    try {
        qDebug() << "PdfToXmlConverter: 开始提取PDF完整内容:" << pdfPath;

        // 1. 首先尝试提取表单字段
        ConvertStatus status = extractFormFields(pdfPath, fields);
        if (status == ConvertStatus::SUCCESS && !fields.isEmpty()) {
            qDebug() << "PdfToXmlConverter: 表单字段提取完成，字段数:" << fields.size();
        }

        // 2. 提取文本内容
        QString textContent;
        status = extractTextContent(pdfPath, textContent);
        if (status == ConvertStatus::SUCCESS && !textContent.isEmpty()) {
            // 从文本中智能提取字段
            extractFieldsFromPdfText(textContent, fields);
            qDebug() << "PdfToXmlConverter: 文本内容提取完成，字段数:" << fields.size();
        }

        // 3. 提取图片内容
        status = extractImageContent(pdfPath, fields);
        if (status == ConvertStatus::SUCCESS) {
            qDebug() << "PdfToXmlConverter: 图片内容提取完成";
        }

        // 4. 提取表格内容
        status = extractTableContent(pdfPath, fields);
        if (status == ConvertStatus::SUCCESS) {
            qDebug() << "PdfToXmlConverter: 表格内容提取完成";
        }

        // 5. 提取图表内容
        status = extractChartContent(pdfPath, fields);
        if (status == ConvertStatus::SUCCESS) {
            qDebug() << "PdfToXmlConverter: 图表内容提取完成";
        }

        qDebug() << "PdfToXmlConverter: 完整内容提取完成，总字段数:" << fields.size();
        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取PDF完整内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus PdfToXmlConverter::extractImageContent(
    const QString& pdfPath, QMap<QString, FieldInfo>& fields)
{
    try {
        if (!m_imageExtractor) {
            setLastError(QS("图片提取器未初始化"));
            return ConvertStatus::UNKNOWN_ERROR;
        }

        // 使用图片提取器提取图片信息
        QList<ImageInfo> images;
        ExtractStatus status = m_imageExtractor->extractImages(pdfPath, images);

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

            qDebug() << "PdfToXmlConverter: 提取到" << images.size() << "个图片";
        }

        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取图片内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus PdfToXmlConverter::extractTableContent(
    const QString& pdfPath, QMap<QString, FieldInfo>& fields)
{
    try {
        if (!m_tableExtractor) {
            setLastError(QS("表格提取器未初始化"));
            return ConvertStatus::UNKNOWN_ERROR;
        }

        // 使用表格提取器提取表格信息
        QList<TableInfo> tables;
        ExtractStatus status = m_tableExtractor->extractTables(pdfPath, tables);

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

            qDebug() << "PdfToXmlConverter: 提取到" << tables.size() << "个表格";
        }

        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取表格内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}

FileConverter::ConvertStatus PdfToXmlConverter::extractChartContent(
    const QString& pdfPath, QMap<QString, FieldInfo>& fields)
{
    try {
        if (!m_chartExtractor) {
            setLastError(QS("图表提取器未初始化"));
            return ConvertStatus::UNKNOWN_ERROR;
        }

        // 使用图表提取器提取图表信息
        QList<ChartInfo> charts;
        ExtractStatus status = m_chartExtractor->extractCharts(pdfPath, charts);

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

            qDebug() << "PdfToXmlConverter: 提取到" << charts.size() << "个图表";
        }

        return ConvertStatus::SUCCESS;

    } catch (const std::exception& e) {
        setLastError(QS("提取图表内容时发生异常: %1").arg(QString::fromUtf8(e.what())));
        return ConvertStatus::UNKNOWN_ERROR;
    }
}
