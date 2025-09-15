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

    // 使用正则表达式提取常见字段
    QMap<QString, QRegularExpression> patterns;
    patterns["Title"] = QRegularExpression(R"(题目[：:]\s*(.+?)(?:\n|$))");
    patterns["StudentName"] = QRegularExpression(R"(姓名[：:]\s*(.+?)(?:\n|$))");
    patterns["StudentID"] = QRegularExpression(R"(学号[：:]\s*(.+?)(?:\n|$))");
    patterns["Class"] = QRegularExpression(R"(班级[：:]\s*(.+?)(?:\n|$))");
    patterns["Abstract"] =
        QRegularExpression(R"(摘要[：:]\s*(.+?)(?=关键词|结论|$)|\s*(.+?)(?=关键词|结论|$))",
                           QRegularExpression::DotMatchesEverythingOption);
    patterns["Keywords"] = QRegularExpression(R"(关键词[：:]\s*(.+?)(?:\n|$))");
    patterns["Conclusion"] =
        QRegularExpression(R"(结论[：:]\s*(.+?)$)", QRegularExpression::DotMatchesEverythingOption);

    for (auto it = patterns.begin(); it != patterns.end(); ++it) {
        QRegularExpressionMatch match = it.value().match(cleanedText);
        if (match.hasMatch()) {
            QString content = match.captured(1).trimmed();
            if (!content.isEmpty()) {
                fields[it.key()] = FieldInfo(it.key(), content);
            }
        }
    }

    return !fields.isEmpty();
}

void FileConverter::setLastError(const QString& error)
{
    m_lastError = error;
    qDebug() << "FileConverter Error:" << error;
}

QString FileConverter::generateXmlHeader() const
{
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<ReportMasonTemplate version="1.0" created=")" +
           QDateTime::currentDateTime().toString(Qt::ISODate) + R"(">
)";
}

QString FileConverter::generateXmlFieldNode(const FieldInfo& field) const
{
    QString xml = QString(R"(    <field name="%1" required="%2">)")
                      .arg(field.name, field.required ? "true" : "false");

    if (!field.description.isEmpty()) {
        xml += QString(R"(<description>%1</description>)").arg(field.description);
    }

    xml += QString(R"(<content><![CDATA[%1]]></content>)").arg(field.content);

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
