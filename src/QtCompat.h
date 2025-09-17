#pragma once

#include <QString>
#include <QStringLiteral>

// Qt 6.8.2 兼容性宏定义
// 解决 QString::QString(const char*) 构造函数私有化问题

// 字符串字面量宏
#define QS(str) QStringLiteral(str)

// 字符串连接宏
#define QS_CONCAT(str1, str2) QStringLiteral(str1) + str2

// 强制替换所有字符串字面量的宏
#define STR(str) QStringLiteral(str)
#define L(str) QStringLiteral(str)

// 常用字符串常量
namespace QtCompat {
    // 常用字符串
    const QString EMPTY = QStringLiteral("");
    const QString SPACE = QStringLiteral(" ");
    const QString NEWLINE = QStringLiteral("\n");
    const QString CRLF = QStringLiteral("\r\n");
    const QString TAB = QStringLiteral("\t");
    
    // 文件相关
    const QString FILE_NOT_FOUND = QStringLiteral("文件不存在");
    const QString FILE_NOT_READABLE = QStringLiteral("文件不可读");
    const QString FILE_EMPTY = QStringLiteral("文件为空");
    const QString FILE_UNSUPPORTED = QStringLiteral("不支持的文件格式");
    const QString FILE_PROCESS_ERROR = QStringLiteral("无法处理此文件格式");
    const QString FILE_PARSE_ERROR = QStringLiteral("文件解析失败");
    const QString VALIDATION_SUCCESS = QStringLiteral("验证通过");
    
    // 操作相关
    const QString SUCCESS = QStringLiteral("成功");
    const QString FAILED = QStringLiteral("失败");
    const QString ERROR = QStringLiteral("错误");
    const QString WARNING = QStringLiteral("警告");
    const QString INFO = QStringLiteral("信息");
    
    // 导出相关
    const QString EXPORT_SUCCESS = QStringLiteral("导出成功");
    const QString EXPORT_FAILED = QStringLiteral("导出失败");
    const QString IMPORT_SUCCESS = QStringLiteral("导入成功");
    const QString IMPORT_FAILED = QStringLiteral("导入失败");
    
    // 数据库相关
    const QString DB_CONNECT_ERROR = QStringLiteral("数据库连接失败");
    const QString DB_QUERY_ERROR = QStringLiteral("数据库查询失败");
    const QString DB_INSERT_ERROR = QStringLiteral("数据库插入失败");
    const QString DB_UPDATE_ERROR = QStringLiteral("数据库更新失败");
    const QString DB_DELETE_ERROR = QStringLiteral("数据库删除失败");
    
    // 模板相关
    const QString TEMPLATE_IMPORTED = QStringLiteral("模板导入成功");
    const QString TEMPLATE_DELETED = QStringLiteral("模板删除成功");
    const QString TEMPLATE_UPDATED = QStringLiteral("模板更新成功");
    const QString TEMPLATE_NOT_FOUND = QStringLiteral("模板未找到");
    const QString TEMPLATE_EXISTS = QStringLiteral("相同内容的模板已存在");
    
    // 内容提取相关
    const QString EXTRACTION_SUCCESS = QStringLiteral("内容提取成功");
    const QString EXTRACTION_FAILED = QStringLiteral("内容提取失败");
    const QString NO_CONTENT = QStringLiteral("没有可提取的内容");
    const QString CONTENT_EMPTY = QStringLiteral("内容为空");
    
    // 格式相关
    const QString FORMAT_DOCX = QStringLiteral("docx");
    const QString FORMAT_PDF = QStringLiteral("pdf");
    const QString FORMAT_XML = QStringLiteral("xml");
    const QString FORMAT_JSON = QStringLiteral("json");
    const QString FORMAT_CSV = QStringLiteral("csv");
    const QString FORMAT_HTML = QStringLiteral("html");
    
    // 属性键
    const QString PROP_SOURCE = QStringLiteral("source");
    const QString PROP_METHOD = QStringLiteral("extractionMethod");
    const QString PROP_TYPE = QStringLiteral("type");
    const QString PROP_PATTERN = QStringLiteral("pattern");
    const QString PROP_FILE_SIZE = QStringLiteral("fileSize");
    const QString PROP_PAGE_NUMBER = QStringLiteral("pageNumber");
    const QString PROP_NOTE = QStringLiteral("note");
    
    // 数据库字段
    const QString DB_ID = QStringLiteral("id");
    const QString DB_NAME = QStringLiteral("name");
    const QString DB_DESCRIPTION = QStringLiteral("description");
    const QString DB_FILE_PATH = QStringLiteral("file_path");
    const QString DB_FILE_HASH = QStringLiteral("file_hash");
    const QString DB_FORMAT = QStringLiteral("format");
    const QString DB_CREATED_AT = QStringLiteral("created_at");
    const QString DB_UPDATED_AT = QStringLiteral("updated_at");
    const QString DB_IS_ACTIVE = QStringLiteral("is_active");
    const QString DB_TAGS = QStringLiteral("tags");
    const QString DB_FIELD_MAPPING = QStringLiteral("field_mapping");
    const QString DB_EXTRACTION_RULES = QStringLiteral("extraction_rules");
    
    // SQL查询
    const QString SQL_SELECT_ALL = QStringLiteral("SELECT * FROM templates");
    const QString SQL_SELECT_BY_ID = QStringLiteral("SELECT * FROM templates WHERE id = ?");
    const QString SQL_SELECT_BY_HASH = QStringLiteral("SELECT id FROM templates WHERE file_hash = ?");
    const QString SQL_INSERT_TEMPLATE = QStringLiteral("INSERT INTO templates (name, description, file_path, file_hash, format, created_at, updated_at, is_active, tags, field_mapping, extraction_rules) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    const QString SQL_UPDATE_PATH = QStringLiteral("UPDATE templates SET file_path = ? WHERE id = ?");
    const QString SQL_DELETE_TEMPLATE = QStringLiteral("DELETE FROM templates WHERE id = ?");
    const QString SQL_UPDATE_FIELD_MAPPING = QStringLiteral("UPDATE templates SET field_mapping = ?, updated_at = ? WHERE id = ?");
    const QString SQL_SELECT_FIELD_MAPPING = QStringLiteral("SELECT field_mapping FROM templates WHERE id = ?");
    const QString SQL_UPDATE_EXTRACTION_RULES = QStringLiteral("UPDATE templates SET extraction_rules = ?, updated_at = ? WHERE id = ?");
    const QString SQL_SELECT_EXTRACTION_RULES = QStringLiteral("SELECT extraction_rules FROM templates WHERE id = ?");
    const QString SQL_SELECT_BY_NAME = QStringLiteral("SELECT * FROM templates WHERE name LIKE ? AND is_active = 1 ORDER BY created_at DESC");
    const QString SQL_SELECT_BY_TAG = QStringLiteral("SELECT * FROM templates WHERE tags LIKE ? AND is_active = 1 ORDER BY created_at DESC");
    const QString SQL_SELECT_DISTINCT_TAGS = QStringLiteral("SELECT DISTINCT tags FROM templates WHERE is_active = 1 AND tags IS NOT NULL AND tags != ''");
    const QString SQL_COUNT_TEMPLATES = QStringLiteral("SELECT COUNT(*) FROM templates WHERE is_active = 1");
    const QString SQL_COUNT_BY_FORMAT = QStringLiteral("SELECT format, COUNT(*) FROM templates WHERE is_active = 1 GROUP BY format");
    
    // 文件扩展名
    const QString EXT_PDF = QStringLiteral(".pdf");
    const QString EXT_DOCX = QStringLiteral(".docx");
    const QString EXT_XML = QStringLiteral(".xml");
    const QString EXT_JSON = QStringLiteral(".json");
    const QString EXT_CSV = QStringLiteral(".csv");
    const QString EXT_HTML = QStringLiteral(".html");
    
    // 消息框标题
    const QString MSG_SUCCESS = QStringLiteral("成功");
    const QString MSG_ERROR = QStringLiteral("错误");
    const QString MSG_WARNING = QStringLiteral("警告");
    const QString MSG_INFO = QStringLiteral("信息");
    const QString MSG_CONFIRM = QStringLiteral("确认");
    
    // 操作提示
    const QString MSG_NO_TABLES = QStringLiteral("没有可导出的表格");
    const QString MSG_NO_CHARTS = QStringLiteral("没有可导出的图表");
    const QString MSG_NO_IMAGES = QStringLiteral("没有可导出的图片");
    const QString MSG_EXPORT_SUCCESS = QStringLiteral("导出成功！");
    const QString MSG_EXPORT_FAILED = QStringLiteral("导出失败！");
    const QString MSG_IMPORT_SUCCESS = QStringLiteral("导入成功！");
    const QString MSG_IMPORT_FAILED = QStringLiteral("导入失败！");
    
    // 日志消息
    const QString LOG_START_EXTRACT = QStringLiteral("开始提取内容...");
    const QString LOG_EXTRACT_SUCCESS = QStringLiteral("内容提取成功");
    const QString LOG_EXTRACT_FAILED = QStringLiteral("内容提取失败");
    const QString LOG_START_EXPORT = QStringLiteral("开始导出...");
    const QString LOG_EXPORT_SUCCESS = QStringLiteral("导出成功");
    const QString LOG_EXPORT_FAILED = QStringLiteral("导出失败");
    
    // 常用方法
    inline QString formatError(const QString& operation, const QString& error) {
        return QStringLiteral("%1失败: %2").arg(operation, error);
    }
    
    inline QString formatSuccess(const QString& operation) {
        return QStringLiteral("%1成功").arg(operation);
    }
    
    inline QString formatMessage(const QString& prefix, const QString& message) {
        return QStringLiteral("%1: %2").arg(prefix, message);
    }
}

// 便捷宏定义
#define QS_ERROR(msg) QtCompat::formatMessage(QtCompat::ERROR, msg)
#define QS_SUCCESS(msg) QtCompat::formatMessage(QtCompat::SUCCESS, msg)
#define QS_WARNING(msg) QtCompat::formatMessage(QtCompat::WARNING, msg)
#define QS_INFO(msg) QtCompat::formatMessage(QtCompat::INFO, msg)

