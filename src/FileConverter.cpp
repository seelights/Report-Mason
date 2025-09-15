#include "FileConverter.h"
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>
#include <QRegularExpression>
#include <QTextDocument>

FileConverter::FileConverter(QObject* parent) : QObject(parent)
{
    // 设置默认模板配置
    m_templateConfig = QJsonObject{{"version", "1.0"},
                                   {"name", "Default Template"},
                                   {"description", "Default template configuration"},
                                   {"fields", QJsonObject{}},
                                   {"rules", QJsonObject{}}};
}

FileConverter::~FileConverter() {}

FileConverter::ConvertStatus FileConverter::convertFileToXml(const QString& inputPath,
                                                             const QString& outputPath)
{
    // 验证输入文件
    if (!validateFilePath(inputPath)) {
        setLastError("输入文件路径无效或文件不存在");
        return ConvertStatus::FILE_NOT_FOUND;
    }

    // 检查是否支持该格式
    if (!isSupported(inputPath)) {
        setLastError("不支持的文件格式");
        return ConvertStatus::INVALID_FORMAT;
    }

    // 提取字段
    QMap<QString, FieldInfo> fields;
    ConvertStatus status = extractFields(inputPath, fields);
    if (status != ConvertStatus::SUCCESS) {
        return status;
    }

    // 验证字段
    QString validationError = validateFields(fields);
    if (!validationError.isEmpty()) {
        setLastError("字段验证失败: " + validationError);
        return ConvertStatus::PARSE_ERROR;
    }

    // 转换为XML
    QByteArray xmlOutput;
    status = convertToXml(fields, xmlOutput);
    if (status != ConvertStatus::SUCCESS) {
        return status;
    }

    // 写入输出文件
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError("无法创建输出文件: " + outputPath);
        return ConvertStatus::WRITE_ERROR;
    }

    outputFile.write(xmlOutput);
    outputFile.close();

    return ConvertStatus::SUCCESS;
}

QString FileConverter::getLastError() const { return m_lastError; }

void FileConverter::setTemplateConfig(const QJsonObject& config) { m_templateConfig = config; }

QJsonObject FileConverter::getTemplateConfig() const { return m_templateConfig; }

QString FileConverter::validateFields(const QMap<QString, FieldInfo>& fields) const
{
    // 检查必填字段
    QJsonObject fieldConfig = m_templateConfig["fields"].toObject();
    for (auto it = fieldConfig.begin(); it != fieldConfig.end(); ++it) {
        QString fieldName = it.key();
        QJsonObject fieldInfo = it.value().toObject();

        if (fieldInfo["required"].toBool() && !fields.contains(fieldName)) {
            return QString("缺少必填字段: %1").arg(fieldName);
        }

        if (fields.contains(fieldName)) {
            const FieldInfo& field = fields[fieldName];
            if (field.required && field.content.isEmpty()) {
                return QString("必填字段 %1 内容为空").arg(fieldName);
            }
        }
    }

    return QString(); // 验证通过
}

bool FileConverter::extractFieldsFromText(const QString& text,
                                          QMap<QString, FieldInfo>& fields) const
{
    QString cleanedText = cleanText(text);
    qDebug() << "FileConverter: 开始从文本提取字段，文本长度:" << cleanedText.length();

    // 使用正则表达式提取常见字段
    QMap<QString, QRegularExpression> patterns;
    patterns[QStringLiteral("Title")] = QRegularExpression(QStringLiteral("题目[：:]\\s*(.+?)(?:\\n|$)"));
    patterns[QStringLiteral("StudentName")] = QRegularExpression(QStringLiteral("姓名[：:]\\s*(.+?)(?:\\n|$)"));
    patterns[QStringLiteral("StudentID")] = QRegularExpression(QStringLiteral("学号[：:]\\s*(.+?)(?:\\n|$)"));
    patterns[QStringLiteral("Class")] = QRegularExpression(QStringLiteral("班级[：:]\\s*(.+?)(?:\\n|$)"));
    patterns[QStringLiteral("Abstract")] =
        QRegularExpression(QStringLiteral("摘要[：:]\\s*(.+?)(?=关键词|结论|$)|\\s*(.+?)(?=关键词|结论|$)"),
                           QRegularExpression::DotMatchesEverythingOption);
    patterns[QStringLiteral("Keywords")] = QRegularExpression(QStringLiteral("关键词[：:]\\s*(.+?)(?:\\n|$)"));
    patterns[QStringLiteral("Conclusion")] =
        QRegularExpression(QStringLiteral("结论[：:]\\s*(.+?)$"), QRegularExpression::DotMatchesEverythingOption);

    int matchedFields = 0;
    for (auto it = patterns.begin(); it != patterns.end(); ++it) {
        QRegularExpressionMatch match = it.value().match(cleanedText);
        if (match.hasMatch()) {
            QString content = match.captured(1).trimmed();
            if (!content.isEmpty()) {
                fields[it.key()] = FieldInfo(it.key(), content);
                matchedFields++;
                qDebug() << "FileConverter: 匹配到字段" << it.key() << ":" << content;
            }
        }
    }

    qDebug() << "FileConverter: 通过正则表达式匹配到" << matchedFields << "个字段";

    // 如果没有匹配到任何字段，尝试按行分割文本
    if (fields.isEmpty()) {
        QStringList lines = cleanedText.split('\n', Qt::SkipEmptyParts);
        qDebug() << "FileConverter: 文本按行分割后得到" << lines.size() << "行";
        
        for (int i = 0; i < lines.size() && i < 10; ++i) { // 限制最多10行
            QString line = lines[i].trimmed();
            if (!line.isEmpty() && line.length() > 2) {
                QString fieldName = QStringLiteral("Line_%1").arg(i + 1);
                fields[fieldName] = FieldInfo(fieldName, line, false);
                qDebug() << "FileConverter: 创建字段" << fieldName << ":" << line;
            }
        }
    }

    qDebug() << "FileConverter: 最终提取到" << fields.size() << "个字段";
    return !fields.isEmpty();
}

void FileConverter::setLastError(const QString& error)
{
    m_lastError = error;
    qDebug() << "FileConverter Error:" << error;
}

QString FileConverter::generateXmlHeader() const
{
    return QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                         "<ReportMasonTemplate version=\"1.0\" created=\"") +
           QDateTime::currentDateTime().toString(Qt::ISODate) + QStringLiteral("\">\n");
}

QString FileConverter::generateXmlFieldNode(const FieldInfo& field) const
{
    QString xml = QString(QStringLiteral("    <field name=\"%1\" required=\"%2\">"))
                      .arg(field.name, field.required ? QStringLiteral("true") : QStringLiteral("false"));

    if (!field.description.isEmpty()) {
        xml += QString(QStringLiteral("<description>%1</description>")).arg(field.description);
    }

    xml += QString(QStringLiteral("<content><![CDATA[%1]]></content>")).arg(field.content);

    if (!field.keywords.isEmpty()) {
        xml += "<keywords>";
        for (const QString& keyword : field.keywords) {
            xml += QString("<keyword>%1</keyword>").arg(keyword);
        }
        xml += "</keywords>";
    }

    xml += "</field>\n";
    return xml;
}

bool FileConverter::validateFilePath(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    return fileInfo.exists() && fileInfo.isFile() && fileInfo.isReadable();
}

QString FileConverter::cleanText(const QString& text) const
{
    // 统一换行符
    QString cleaned = text;
    cleaned.replace("\r\n", "\n");
    cleaned.replace("\r", "\n");

    // 移除多余空白
    cleaned = cleaned.simplified();

    // 统一标点符号（全角转半角）
    cleaned.replace("：", ":");
    cleaned.replace("，", ",");
    cleaned.replace("。", ".");
    cleaned.replace("！", "!");
    cleaned.replace("？", "?");

    return cleaned;
}

// 全局辅助函数实现
FileConverter::InputFormat getFileFormat(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == "docx") {
        return FileConverter::InputFormat::DOCX;
    } else if (suffix == "pdf") {
        return FileConverter::InputFormat::PDF;
    } else if (suffix == "pptx") {
        return FileConverter::InputFormat::PPTX;
    }

    return FileConverter::InputFormat::UNKNOWN;
}

QString getMimeType(FileConverter::InputFormat format)
{
    switch (format) {
    case FileConverter::InputFormat::DOCX:
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    case FileConverter::InputFormat::PDF:
        return "application/pdf";
    case FileConverter::InputFormat::PPTX:
        return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    default:
        return "application/octet-stream";
    }
}

QStringList getFileExtensions(FileConverter::InputFormat format)
{
    switch (format) {
    case FileConverter::InputFormat::DOCX:
        return {"docx"};
    case FileConverter::InputFormat::PDF:
        return {"pdf"};
    case FileConverter::InputFormat::PPTX:
        return {"pptx"};
    default:
        return {};
    }
}
