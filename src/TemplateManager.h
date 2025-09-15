#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QHash>
#include "FileConverter.h"

/**
 * @brief 模板信息结构
 */
struct TemplateInfo {
    int id;                     ///< 模板ID
    QString name;               ///< 模板名称
    QString description;        ///< 模板描述
    QString filePath;           ///< 模板文件路径
    QString fileHash;           ///< 文件哈希值
    QString format;             ///< 文件格式
    QJsonObject fieldMapping;   ///< 字段映射配置
    QJsonObject extractionRules;///< 抽取规则配置
    QDateTime createdAt;        ///< 创建时间
    QDateTime updatedAt;        ///< 更新时间
    bool isActive;              ///< 是否激活
    QString tags;               ///< 标签（逗号分隔）
    
    TemplateInfo() : id(-1), isActive(true) {}
};

/**
 * @brief 模板管理系统
 * 
 * 负责管理实验报告模板，包括：
 * - 模板的导入、存储和检索
 * - 字段映射配置管理
 * - 抽取规则配置管理
 * - 模板版本控制
 * - 数据库持久化存储
 */
class TemplateManager : public QObject
{
    Q_OBJECT

public:
    explicit TemplateManager(QObject *parent = nullptr);
    ~TemplateManager();

    /**
     * @brief 初始化模板管理器
     * @param dbPath 数据库文件路径
     * @return 是否初始化成功
     */
    bool initialize(const QString &dbPath = QString());

    /**
     * @brief 关闭模板管理器
     */
    void shutdown();

    /**
     * @brief 导入模板文件
     * @param filePath 模板文件路径
     * @param templateName 模板名称
     * @param description 模板描述
     * @param tags 标签列表
     * @return 导入的模板ID，-1表示失败
     */
    int importTemplate(const QString &filePath, 
                      const QString &templateName = QString(),
                      const QString &description = QString(),
                      const QStringList &tags = QStringList());

    /**
     * @brief 删除模板
     * @param templateId 模板ID
     * @return 是否删除成功
     */
    bool deleteTemplate(int templateId);

    /**
     * @brief 更新模板信息
     * @param templateInfo 模板信息
     * @return 是否更新成功
     */
    bool updateTemplate(const TemplateInfo &templateInfo);

    /**
     * @brief 获取模板信息
     * @param templateId 模板ID
     * @return 模板信息
     */
    TemplateInfo getTemplate(int templateId) const;

    /**
     * @brief 获取所有模板列表
     * @param activeOnly 是否只返回激活的模板
     * @return 模板信息列表
     */
    QList<TemplateInfo> getAllTemplates(bool activeOnly = true) const;

    /**
     * @brief 根据名称搜索模板
     * @param name 模板名称
     * @return 匹配的模板列表
     */
    QList<TemplateInfo> searchTemplates(const QString &name) const;

    /**
     * @brief 根据标签搜索模板
     * @param tag 标签
     * @return 匹配的模板列表
     */
    QList<TemplateInfo> searchTemplatesByTag(const QString &tag) const;

    /**
     * @brief 设置模板字段映射
     * @param templateId 模板ID
     * @param fieldMapping 字段映射配置
     * @return 是否设置成功
     */
    bool setTemplateFieldMapping(int templateId, const QJsonObject &fieldMapping);

    /**
     * @brief 获取模板字段映射
     * @param templateId 模板ID
     * @return 字段映射配置
     */
    QJsonObject getTemplateFieldMapping(int templateId) const;

    /**
     * @brief 设置模板抽取规则
     * @param templateId 模板ID
     * @param extractionRules 抽取规则配置
     * @return 是否设置成功
     */
    bool setTemplateExtractionRules(int templateId, const QJsonObject &extractionRules);

    /**
     * @brief 获取模板抽取规则
     * @param templateId 模板ID
     * @return 抽取规则配置
     */
    QJsonObject getTemplateExtractionRules(int templateId) const;

    /**
     * @brief 验证模板文件
     * @param filePath 模板文件路径
     * @return 验证结果和错误信息
     */
    QPair<bool, QString> validateTemplate(const QString &filePath);

    /**
     * @brief 从模板提取字段
     * @param templateId 模板ID
     * @param fields 输出的字段映射
     * @return 是否提取成功
     */
    bool extractFieldsFromTemplate(int templateId, QMap<QString, FileConverter::FieldInfo> &fields);

    /**
     * @brief 获取所有标签列表
     * @return 标签列表
     */
    QStringList getAllTags() const;

    /**
     * @brief 获取模板统计信息
     * @return 统计信息JSON对象
     */
    QJsonObject getTemplateStatistics() const;

    /**
     * @brief 导出模板配置
     * @param templateId 模板ID
     * @param exportPath 导出文件路径
     * @return 是否导出成功
     */
    bool exportTemplateConfig(int templateId, const QString &exportPath);

    /**
     * @brief 导入模板配置
     * @param configPath 配置文件路径
     * @return 导入的模板ID，-1表示失败
     */
    int importTemplateConfig(const QString &configPath);

signals:
    /**
     * @brief 模板导入完成信号
     * @param templateId 模板ID
     * @param success 是否成功
     * @param message 消息
     */
    void templateImported(int templateId, bool success, const QString &message);

    /**
     * @brief 模板删除信号
     * @param templateId 模板ID
     */
    void templateDeleted(int templateId);

    /**
     * @brief 模板更新信号
     * @param templateId 模板ID
     */
    void templateUpdated(int templateId);

protected:
    /**
     * @brief 创建数据库表
     * @return 是否创建成功
     */
    bool createTables();

    /**
     * @brief 计算文件哈希值
     * @param filePath 文件路径
     * @return 哈希值
     */
    QString calculateFileHash(const QString &filePath) const;

    /**
     * @brief 复制模板文件到存储目录
     * @param sourcePath 源文件路径
     * @param templateId 模板ID
     * @return 新的文件路径
     */
    QString copyTemplateFile(const QString &sourcePath, int templateId);

    /**
     * @brief 创建模板存储目录
     * @param templateId 模板ID
     * @return 目录路径
     */
    QString createTemplateDirectory(int templateId);

    /**
     * @brief 获取模板存储根目录
     * @return 根目录路径
     */
    QString getTemplateStorageRoot() const;

    /**
     * @brief 验证字段映射配置
     * @param fieldMapping 字段映射配置
     * @return 验证结果
     */
    bool validateFieldMapping(const QJsonObject &fieldMapping) const;

    /**
     * @brief 验证抽取规则配置
     * @param extractionRules 抽取规则配置
     * @return 验证结果
     */
    bool validateExtractionRules(const QJsonObject &extractionRules) const;

private:
    QSqlDatabase m_database;        ///< 数据库连接
    QString m_databasePath;         ///< 数据库文件路径
    QString m_storageRoot;          ///< 模板存储根目录
    bool m_initialized;             ///< 是否已初始化
    
    // 文件转换器映射
    QMap<FileConverter::InputFormat, QSharedPointer<FileConverter>> m_converters;
    
    /**
     * @brief 初始化文件转换器
     */
    void initializeConverters();
    
    /**
     * @brief 获取文件转换器
     * @param filePath 文件路径
     * @return 转换器指针
     */
    QSharedPointer<FileConverter> getConverter(const QString &filePath) const;
};
