#include "DocToXmlConverter.h"
#include "PdfToXmlConverter.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSqlRecord>
#include <QSqlError>
#include <QCoreApplication>
#include <QDateTime>

#include "TemplateManager.h"

TemplateManager::TemplateManager(QObject* parent) : QObject(parent), m_initialized(false)
{
    // 设置默认存储根目录
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_storageRoot = QDir(appDataPath).absoluteFilePath("templates");
}

TemplateManager::~TemplateManager() { shutdown(); }

bool TemplateManager::initialize(const QString& dbPath)
{
    if (m_initialized) {
        return true;
    }

    // 设置数据库路径
    if (dbPath.isEmpty()) {
        QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(appDataPath);
        m_databasePath = QDir(appDataPath).absoluteFilePath("templates.db");
    } else {
        m_databasePath = dbPath;
    }

    // 初始化数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", "TemplateManager");
    m_database.setDatabaseName(m_databasePath);

    if (!m_database.open()) {
        qDebug() << "无法打开数据库:" << m_database.lastError().text();
        return false;
    }

    // 创建表结构
    if (!createTables()) {
        qDebug() << "创建数据库表失败";
        return false;
    }

    // 初始化文件转换器
    initializeConverters();

    // 创建模板存储目录
    QDir().mkpath(m_storageRoot);

    m_initialized = true;
    qDebug() << "模板管理器初始化成功";
    return true;
}

void TemplateManager::shutdown()
{
    if (m_initialized) {
        m_database.close();
        m_initialized = false;
    }
}

int TemplateManager::importTemplate(const QString& filePath, const QString& templateName,
                                    const QString& description, const QStringList& tags)
{
    if (!m_initialized) {
        return -1;
    }

    // 验证模板文件
    auto validation = validateTemplate(filePath);
    if (!validation.first) {
        emit templateImported(-1, false, validation.second);
        return -1;
    }

    // 获取文件信息
    QFileInfo fileInfo(filePath);
    QString fileName = templateName.isEmpty() ? fileInfo.baseName() : templateName;
    QString fileHash = calculateFileHash(filePath);
    FileConverter::InputFormat format = getFileFormat(filePath);

    // 检查是否已存在相同哈希的模板
    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM templates WHERE file_hash = ?");
    query.addBindValue(fileHash);
    if (query.exec() && query.next()) {
        int existingId = query.value(0).toInt();
        emit templateImported(existingId, false, "相同内容的模板已存在");
        return existingId;
    }

    // 插入模板记录
    query.prepare(R"(
        INSERT INTO templates (name, description, file_path, file_hash, format, 
                              field_mapping, extraction_rules, created_at, updated_at, 
                              is_active, tags)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(fileName);
    query.addBindValue(description);
    query.addBindValue(filePath);
    query.addBindValue(fileHash);
    query.addBindValue(QString::fromStdString(std::to_string(static_cast<int>(format))));

    // 默认字段映射和抽取规则
    QJsonObject defaultFieldMapping;
    QJsonObject defaultExtractionRules;

    query.addBindValue(QJsonDocument(defaultFieldMapping).toJson(QJsonDocument::Compact));
    query.addBindValue(QJsonDocument(defaultExtractionRules).toJson(QJsonDocument::Compact));
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(true);
    query.addBindValue(tags.join(","));

    if (!query.exec()) {
        qDebug() << "插入模板记录失败:" << query.lastError().text();
        emit templateImported(-1, false, "数据库操作失败");
        return -1;
    }

    int templateId = query.lastInsertId().toInt();

    // 复制模板文件到存储目录
    QString storedFilePath = copyTemplateFile(filePath, templateId);
    if (storedFilePath.isEmpty()) {
        // 如果复制失败，删除数据库记录
        deleteTemplate(templateId);
        emit templateImported(-1, false, "复制模板文件失败");
        return -1;
    }

    // 更新数据库中的文件路径
    query.prepare("UPDATE templates SET file_path = ? WHERE id = ?");
    query.addBindValue(storedFilePath);
    query.addBindValue(templateId);
    query.exec();

    emit templateImported(templateId, true, "模板导入成功");
    return templateId;
}

bool TemplateManager::deleteTemplate(int templateId)
{
    if (!m_initialized || templateId < 0) {
        return false;
    }

    // 获取模板信息
    TemplateInfo info = getTemplate(templateId);
    if (info.id < 0) {
        return false;
    }

    // 删除模板文件
    if (!info.filePath.isEmpty() && QFile::exists(info.filePath)) {
        QFile::remove(info.filePath);
    }

    // 删除模板目录
    QString templateDir = createTemplateDirectory(templateId);
    if (QDir(templateDir).exists()) {
        QDir(templateDir).removeRecursively();
    }

    // 删除数据库记录
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM templates WHERE id = ?");
    query.addBindValue(templateId);

    bool success = query.exec();
    if (success) {
        emit templateDeleted(templateId);
    }

    return success;
}

bool TemplateManager::updateTemplate(const TemplateInfo& templateInfo)
{
    if (!m_initialized || templateInfo.id < 0) {
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE templates SET 
            name = ?, description = ?, field_mapping = ?, extraction_rules = ?,
            updated_at = ?, is_active = ?, tags = ?
        WHERE id = ?
    )");

    query.addBindValue(templateInfo.name);
    query.addBindValue(templateInfo.description);
    query.addBindValue(QJsonDocument(templateInfo.fieldMapping).toJson(QJsonDocument::Compact));
    query.addBindValue(QJsonDocument(templateInfo.extractionRules).toJson(QJsonDocument::Compact));
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(templateInfo.isActive);
    query.addBindValue(templateInfo.tags);
    query.addBindValue(templateInfo.id);

    bool success = query.exec();
    if (success) {
        emit templateUpdated(templateInfo.id);
    }

    return success;
}

TemplateInfo TemplateManager::getTemplate(int templateId) const
{
    TemplateInfo info;

    if (!m_initialized || templateId < 0) {
        return info;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM templates WHERE id = ?");
    query.addBindValue(templateId);

    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();

        info.id = record.value("id").toInt();
        info.name = record.value("name").toString();
        info.description = record.value("description").toString();
        info.filePath = record.value("file_path").toString();
        info.fileHash = record.value("file_hash").toString();
        info.format = record.value("format").toString();
        info.createdAt = record.value("created_at").toDateTime();
        info.updatedAt = record.value("updated_at").toDateTime();
        info.isActive = record.value("is_active").toBool();
        info.tags = record.value("tags").toString();

        // 解析JSON字段
        QJsonParseError error;
        QJsonDocument doc;

        doc = QJsonDocument::fromJson(record.value("field_mapping").toByteArray(), &error);
        if (error.error == QJsonParseError::NoError) {
            info.fieldMapping = doc.object();
        }

        doc = QJsonDocument::fromJson(record.value("extraction_rules").toByteArray(), &error);
        if (error.error == QJsonParseError::NoError) {
            info.extractionRules = doc.object();
        }
    }

    return info;
}

QList<TemplateInfo> TemplateManager::getAllTemplates(bool activeOnly) const
{
    QList<TemplateInfo> templates;

    if (!m_initialized) {
        return templates;
    }

    QString sql = "SELECT * FROM templates";
    if (activeOnly) {
        sql += " WHERE is_active = 1";
    }
    sql += " ORDER BY created_at DESC";

    QSqlQuery query(m_database);
    query.exec(sql);

    while (query.next()) {
        QSqlRecord record = query.record();
        TemplateInfo info;

        info.id = record.value("id").toInt();
        info.name = record.value("name").toString();
        info.description = record.value("description").toString();
        info.filePath = record.value("file_path").toString();
        info.fileHash = record.value("file_hash").toString();
        info.format = record.value("format").toString();
        info.createdAt = record.value("created_at").toDateTime();
        info.updatedAt = record.value("updated_at").toDateTime();
        info.isActive = record.value("is_active").toBool();
        info.tags = record.value("tags").toString();

        // 解析JSON字段
        QJsonParseError error;
        QJsonDocument doc;

        doc = QJsonDocument::fromJson(record.value("field_mapping").toByteArray(), &error);
        if (error.error == QJsonParseError::NoError) {
            info.fieldMapping = doc.object();
        }

        doc = QJsonDocument::fromJson(record.value("extraction_rules").toByteArray(), &error);
        if (error.error == QJsonParseError::NoError) {
            info.extractionRules = doc.object();
        }

        templates.append(info);
    }

    return templates;
}

QList<TemplateInfo> TemplateManager::searchTemplates(const QString& name) const
{
    QList<TemplateInfo> templates;

    if (!m_initialized || name.isEmpty()) {
        return templates;
    }

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT * FROM templates WHERE name LIKE ? AND is_active = 1 ORDER BY created_at DESC");
    query.addBindValue(QString("%%1%").arg(name));

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord record = query.record();
            TemplateInfo info;

            info.id = record.value("id").toInt();
            info.name = record.value("name").toString();
            info.description = record.value("description").toString();
            info.filePath = record.value("file_path").toString();
            info.fileHash = record.value("file_hash").toString();
            info.format = record.value("format").toString();
            info.createdAt = record.value("created_at").toDateTime();
            info.updatedAt = record.value("updated_at").toDateTime();
            info.isActive = record.value("is_active").toBool();
            info.tags = record.value("tags").toString();

            templates.append(info);
        }
    }

    return templates;
}

QList<TemplateInfo> TemplateManager::searchTemplatesByTag(const QString& tag) const
{
    QList<TemplateInfo> templates;

    if (!m_initialized || tag.isEmpty()) {
        return templates;
    }

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT * FROM templates WHERE tags LIKE ? AND is_active = 1 ORDER BY created_at DESC");
    query.addBindValue(QString("%%1%").arg(tag));

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord record = query.record();
            TemplateInfo info;

            info.id = record.value("id").toInt();
            info.name = record.value("name").toString();
            info.description = record.value("description").toString();
            info.filePath = record.value("file_path").toString();
            info.fileHash = record.value("file_hash").toString();
            info.format = record.value("format").toString();
            info.createdAt = record.value("created_at").toDateTime();
            info.updatedAt = record.value("updated_at").toDateTime();
            info.isActive = record.value("is_active").toBool();
            info.tags = record.value("tags").toString();

            templates.append(info);
        }
    }

    return templates;
}

bool TemplateManager::setTemplateFieldMapping(int templateId, const QJsonObject& fieldMapping)
{
    if (!m_initialized || templateId < 0 || !validateFieldMapping(fieldMapping)) {
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare("UPDATE templates SET field_mapping = ?, updated_at = ? WHERE id = ?");
    query.addBindValue(QJsonDocument(fieldMapping).toJson(QJsonDocument::Compact));
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(templateId);

    return query.exec();
}

QJsonObject TemplateManager::getTemplateFieldMapping(int templateId) const
{
    QJsonObject fieldMapping;

    if (!m_initialized || templateId < 0) {
        return fieldMapping;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT field_mapping FROM templates WHERE id = ?");
    query.addBindValue(templateId);

    if (query.exec() && query.next()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(query.value(0).toByteArray(), &error);
        if (error.error == QJsonParseError::NoError) {
            fieldMapping = doc.object();
        }
    }

    return fieldMapping;
}

bool TemplateManager::setTemplateExtractionRules(int templateId, const QJsonObject& extractionRules)
{
    if (!m_initialized || templateId < 0 || !validateExtractionRules(extractionRules)) {
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare("UPDATE templates SET extraction_rules = ?, updated_at = ? WHERE id = ?");
    query.addBindValue(QJsonDocument(extractionRules).toJson(QJsonDocument::Compact));
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(templateId);

    return query.exec();
}

QJsonObject TemplateManager::getTemplateExtractionRules(int templateId) const
{
    QJsonObject extractionRules;

    if (!m_initialized || templateId < 0) {
        return extractionRules;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT extraction_rules FROM templates WHERE id = ?");
    query.addBindValue(templateId);

    if (query.exec() && query.next()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(query.value(0).toByteArray(), &error);
        if (error.error == QJsonParseError::NoError) {
            extractionRules = doc.object();
        }
    }

    return extractionRules;
}

QPair<bool, QString> TemplateManager::validateTemplate(const QString& filePath)
{
    QFileInfo fileInfo(filePath);

    // 检查文件是否存在
    if (!fileInfo.exists()) {
        return qMakePair(false, "文件不存在");
    }

    // 检查文件是否可读
    if (!fileInfo.isReadable()) {
        return qMakePair(false, "文件不可读");
    }

    // 检查文件大小
    if (fileInfo.size() == 0) {
        return qMakePair(false, "文件为空");
    }

    // 检查文件格式
    FileConverter::InputFormat format = getFileFormat(filePath);
    if (format == FileConverter::InputFormat::UNKNOWN) {
        return qMakePair(false, "不支持的文件格式");
    }

    // 检查是否有对应的转换器
    QSharedPointer<FileConverter> converter = getConverter(filePath);
    if (!converter) {
        return qMakePair(false, "无法处理此文件格式");
    }

    // 尝试提取字段以验证文件完整性
    QMap<QString, FileConverter::FieldInfo> fields;
    FileConverter::ConvertStatus status = converter->extractFields(filePath, fields);
    if (status != FileConverter::ConvertStatus::SUCCESS) {
        return qMakePair(false, QString("文件解析失败: %1").arg(converter->getLastError()));
    }

    return qMakePair(true, "验证通过");
}

bool TemplateManager::extractFieldsFromTemplate(int templateId,
                                                QMap<QString, FileConverter::FieldInfo>& fields)
{
    TemplateInfo info = getTemplate(templateId);
    if (info.id < 0) {
        return false;
    }

    QSharedPointer<FileConverter> converter = getConverter(info.filePath);
    if (!converter) {
        return false;
    }

    FileConverter::ConvertStatus status = converter->extractFields(info.filePath, fields);
    return status == FileConverter::ConvertStatus::SUCCESS;
}

QStringList TemplateManager::getAllTags() const
{
    QStringList tags;

    if (!m_initialized) {
        return tags;
    }

    QSqlQuery query(m_database);
    query.exec(
        "SELECT DISTINCT tags FROM templates WHERE is_active = 1 AND tags IS NOT NULL AND tags != "
        "''");

    while (query.next()) {
        QString tagString = query.value(0).toString();
        if (!tagString.isEmpty()) {
            QStringList templateTags = tagString.split(',', Qt::SkipEmptyParts);
            for (const QString& tag : templateTags) {
                QString trimmedTag = tag.trimmed();
                if (!trimmedTag.isEmpty() && !tags.contains(trimmedTag)) {
                    tags.append(trimmedTag);
                }
            }
        }
    }

    tags.sort();
    return tags;
}

QJsonObject TemplateManager::getTemplateStatistics() const
{
    QJsonObject stats;

    if (!m_initialized) {
        return stats;
    }

    QSqlQuery query(m_database);

    // 总模板数
    query.exec("SELECT COUNT(*) FROM templates WHERE is_active = 1");
    if (query.next()) {
        stats["totalTemplates"] = query.value(0).toInt();
    }

    // 按格式统计
    query.exec("SELECT format, COUNT(*) FROM templates WHERE is_active = 1 GROUP BY format");
    QJsonObject formatStats;
    while (query.next()) {
        formatStats[query.value(0).toString()] = query.value(1).toInt();
    }
    stats["formatStatistics"] = formatStats;

    // 最近创建的模板数
    query.exec(
        "SELECT COUNT(*) FROM templates WHERE is_active = 1 AND created_at >= datetime('now', '-7 "
        "days')");
    if (query.next()) {
        stats["recentTemplates"] = query.value(0).toInt();
    }

    return stats;
}

bool TemplateManager::exportTemplateConfig(int templateId, const QString& exportPath)
{
    TemplateInfo info = getTemplate(templateId);
    if (info.id < 0) {
        return false;
    }

    QJsonObject config;
    config["version"] = "1.0";
    config["templateInfo"] = QJsonObject{{"id", info.id},
                                         {"name", info.name},
                                         {"description", info.description},
                                         {"format", info.format},
                                         {"tags", info.tags}};
    config["fieldMapping"] = info.fieldMapping;
    config["extractionRules"] = info.extractionRules;
    config["exportedAt"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(config);
    QFile file(exportPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

int TemplateManager::importTemplateConfig(const QString& configPath)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return -1;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        return -1;
    }

    QJsonObject config = doc.object();
    QJsonObject templateInfo = config["templateInfo"].toObject();

    // 创建新的模板记录
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO templates (name, description, format, field_mapping, extraction_rules, 
                              created_at, updated_at, is_active, tags)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(templateInfo["name"].toString());
    query.addBindValue(templateInfo["description"].toString());
    query.addBindValue(templateInfo["format"].toString());
    query.addBindValue(
        QJsonDocument(config["fieldMapping"].toObject()).toJson(QJsonDocument::Compact));
    query.addBindValue(
        QJsonDocument(config["extractionRules"].toObject()).toJson(QJsonDocument::Compact));
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(true);
    query.addBindValue(templateInfo["tags"].toString());

    if (!query.exec()) {
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool TemplateManager::createTables()
{
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS templates (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            description TEXT,
            file_path TEXT NOT NULL,
            file_hash TEXT NOT NULL,
            format TEXT NOT NULL,
            field_mapping TEXT,
            extraction_rules TEXT,
            created_at DATETIME NOT NULL,
            updated_at DATETIME NOT NULL,
            is_active BOOLEAN DEFAULT 1,
            tags TEXT
        )
    )";

    QSqlQuery query(m_database);
    return query.exec(createTableSql);
}

QString TemplateManager::calculateFileHash(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(&file);

    return hash.result().toHex();
}

QString TemplateManager::copyTemplateFile(const QString& sourcePath, int templateId)
{
    QString templateDir = createTemplateDirectory(templateId);
    QFileInfo sourceInfo(sourcePath);
    QString targetPath = QDir(templateDir).absoluteFilePath(sourceInfo.fileName());

    if (QFile::copy(sourcePath, targetPath)) {
        return targetPath;
    }

    return QString();
}

QString TemplateManager::createTemplateDirectory(int templateId)
{
    QString templateDir = QDir(m_storageRoot).absoluteFilePath(QString::number(templateId));
    QDir().mkpath(templateDir);
    return templateDir;
}

QString TemplateManager::getTemplateStorageRoot() const { return m_storageRoot; }

bool TemplateManager::validateFieldMapping(const QJsonObject& fieldMapping) const
{
    // 基本验证：检查字段映射格式
    for (auto it = fieldMapping.begin(); it != fieldMapping.end(); ++it) {
        if (!it.value().isObject()) {
            return false;
        }

        QJsonObject fieldConfig = it.value().toObject();
        if (!fieldConfig.contains("type") || !fieldConfig.contains("required")) {
            return false;
        }
    }

    return true;
}

bool TemplateManager::validateExtractionRules(const QJsonObject& extractionRules) const
{
    // 基本验证：检查抽取规则格式
    if (extractionRules.contains("patterns")) {
        if (!extractionRules["patterns"].isObject()) {
            return false;
        }
    }

    if (extractionRules.contains("keywords")) {
        if (!extractionRules["keywords"].isArray()) {
            return false;
        }
    }

    return true;
}

void TemplateManager::initializeConverters()
{
    // 初始化各种文件转换器
    m_converters[FileConverter::InputFormat::DOCX] =
        QSharedPointer<FileConverter>(new DocToXmlConverter(this));
    m_converters[FileConverter::InputFormat::PDF] =
        QSharedPointer<FileConverter>(new PdfToXmlConverter(this));
}

QSharedPointer<FileConverter> TemplateManager::getConverter(const QString& filePath) const
{
    FileConverter::InputFormat format = getFileFormat(filePath);
    return m_converters.value(format);
}
