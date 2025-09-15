#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>
#include <QRegularExpression>
#include <QTextDocument>
#include "FileConverter.h"

/**
 * @brief 字段抽取器
 *
 * 提供智能的字段抽取功能，支持：
 * - 基于正则表达式的规则抽取
 * - 基于关键词的语义抽取
 * - 基于模板的智能匹配
 * - 支持多种抽取策略的组合使用
 */
class FieldExtractor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 抽取策略枚举
     */
    enum class ExtractionStrategy {
        REGEX_PATTERN,      ///< 正则表达式模式匹配
        KEYWORD_MATCHING,   ///< 关键词匹配
        POSITION_BASED,     ///< 基于位置的匹配
        TEMPLATE_BASED,     ///< 基于模板的智能匹配
        HYBRID,             ///< 混合策略
        CUSTOM              ///< 自定义策略
    };

    /**
     * @brief 抽取规则配置
     */
    struct ExtractionRule
    {
        QString fieldName;           ///< 字段名称
        QString displayName;         ///< 显示名称
        QString description;         ///< 字段描述
        QStringList regexPatterns;   ///< 正则表达式模式列表
        QStringList keywords;        ///< 关键词列表
        bool enabled;                ///< 是否启用
        QString defaultValue;        ///< 默认值
        QStringList tags;            ///< 标签列表
        ExtractionStrategy strategy; ///< 抽取策略

        ExtractionRule() : enabled(true), strategy(ExtractionStrategy::REGEX_PATTERN) {}
    };

    explicit FieldExtractor(QObject* parent = nullptr);
    ~FieldExtractor();

    /**
     * @brief 从文本中抽取字段
     * @param content 输入文本内容
     * @return 提取的字段映射
     */
    QMap<QString, QString> extractFields(const QString &content);

    /**
     * @brief 设置抽取策略
     * @param strategy 抽取策略
     */
    void setExtractionStrategy(ExtractionStrategy strategy);

    /**
     * @brief 获取抽取策略
     * @return 抽取策略
     */
    ExtractionStrategy getExtractionStrategy() const;

    /**
     * @brief 设置是否区分大小写
     * @param caseSensitive 是否区分大小写
     */
    void setCaseSensitive(bool caseSensitive);

    /**
     * @brief 获取是否区分大小写
     * @return 是否区分大小写
     */
    bool isCaseSensitive() const;

    /**
     * @brief 设置多行模式
     * @param multilineMode 多行模式
     */
    void setMultilineMode(bool multilineMode);

    /**
     * @brief 获取多行模式
     * @return 多行模式
     */
    bool isMultilineMode() const;

    /**
     * @brief 从文本中抽取字段（使用预定义规则）
     * @param text 输入文本
     * @param templateType 模板类型
     * @param fields 输出的字段映射
     * @return 是否抽取成功
     */
    bool extractFieldsWithTemplate(const QString& text, const QString& templateType,
                                   QMap<QString, FileConverter::FieldInfo>& fields);

    /**
     * @brief 获取预定义规则
     * @return 预定义规则映射
     */
    QMap<QString, ExtractionRule> getPredefinedRules() const;

    /**
     * @brief 添加自定义抽取规则
     * @param fieldName 字段名称
     * @param rule 抽取规则
     */
    void addExtractionRule(const QString &fieldName, const ExtractionRule &rule);

    /**
     * @brief 移除抽取规则
     * @param fieldName 字段名称
     */
    void removeExtractionRule(const QString& fieldName);

    /**
     * @brief 获取单个抽取规则
     * @param fieldName 字段名称
     * @return 抽取规则
     */
    ExtractionRule getExtractionRule(const QString &fieldName) const;

    /**
     * @brief 获取所有抽取规则
     * @return 抽取规则映射
     */
    QMap<QString, ExtractionRule> getAllRules() const;

    /**
     * @brief 清除所有规则
     */
    void clearRules();

    /**
     * @brief 设置抽取配置
     * @param config 配置JSON对象
     */
    void setExtractionConfig(const QJsonObject& config);

    /**
     * @brief 获取抽取配置
     * @return 配置JSON对象
     */
    QJsonObject getExtractionConfig() const;

    /**
     * @brief 验证抽取结果
     * @param fields 字段映射
     * @param rules 抽取规则
     * @return 验证结果和错误信息
     */
    QPair<bool, QString> validateExtractionResult(
        const QMap<QString, FileConverter::FieldInfo>& fields, const QList<ExtractionRule>& rules);

    /**
     * @brief 获取支持的模板类型
     * @return 模板类型列表
     */
    QStringList getSupportedTemplateTypes() const;

    /**
     * @brief 智能合并重复字段
     * @param fields 字段映射
     * @return 合并后的字段映射
     */
    QMap<QString, FileConverter::FieldInfo> mergeDuplicateFields(
        const QMap<QString, FileConverter::FieldInfo>& fields);

protected:
    /**
     * @brief 使用正则表达式抽取字段
     * @param text 输入文本
     * @param rule 抽取规则
     * @param field 输出的字段信息
     * @return 是否抽取成功
     */
    bool extractWithRegex(const QString& text, const ExtractionRule& rule,
                          FileConverter::FieldInfo& field);

    /**
     * @brief 使用关键词匹配抽取字段
     * @param text 输入文本
     * @param rule 抽取规则
     * @param field 输出的字段信息
     * @return 是否抽取成功
     */
    bool extractWithKeywords(const QString& text, const ExtractionRule& rule,
                             FileConverter::FieldInfo& field);

    /**
     * @brief 使用模板匹配抽取字段
     * @param text 输入文本
     * @param rule 抽取规则
     * @param field 输出的字段信息
     * @return 是否抽取成功
     */
    bool extractWithTemplate(const QString& text, const ExtractionRule& rule,
                             FileConverter::FieldInfo& field);

    /**
     * @brief 使用混合策略抽取字段
     * @param text 输入文本
     * @param rule 抽取规则
     * @param field 输出的字段信息
     * @return 是否抽取成功
     */
    bool extractWithHybrid(const QString& text, const ExtractionRule& rule,
                           FileConverter::FieldInfo& field);

    /**
     * @brief 清理和标准化文本
     * @param text 输入文本
     * @return 清理后的文本
     */
    QString cleanText(const QString& text) const;

    /**
     * @brief 计算文本相似度
     * @param text1 文本1
     * @param text2 文本2
     * @return 相似度（0-1）
     */
    double calculateTextSimilarity(const QString& text1, const QString& text2) const;

    /**
     * @brief 初始化预定义规则
     */
    void initializePredefinedRules();

    /**
     * @brief 提取字段值
     * @param content 内容
     * @param rule 规则
     * @return 提取的值
     */
    QString extractFieldValue(const QString &content, const ExtractionRule &rule);

    /**
     * @brief 使用正则表达式提取
     * @param content 内容
     * @param rule 规则
     * @return 提取的值
     */
    QString extractUsingRegex(const QString &content, const ExtractionRule &rule);

    /**
     * @brief 使用关键词提取
     * @param content 内容
     * @param rule 规则
     * @return 提取的值
     */
    QString extractUsingKeywords(const QString &content, const ExtractionRule &rule);

    /**
     * @brief 使用位置提取
     * @param content 内容
     * @param rule 规则
     * @return 提取的值
     */
    QString extractUsingPosition(const QString &content, const ExtractionRule &rule);

    /**
     * @brief 预处理内容
     * @param content 内容
     * @return 预处理后的内容
     */
    QString preprocessContent(const QString &content);

    /**
     * @brief 加载配置
     */
    void loadConfiguration();

    /**
     * @brief 保存配置
     */
    void saveConfiguration();

    /**
     * @brief 加载模板配置
     * @param templateType 模板类型
     * @return 抽取规则列表
     */
    QList<ExtractionRule> loadTemplateRules(const QString& templateType);

private:
    QMap<QString, ExtractionRule> m_extractionRules;      ///< 抽取规则映射
    QJsonObject m_extractionConfig;                       ///< 抽取配置
    QMap<QString, QList<ExtractionRule>> m_templateRules; ///< 模板规则映射
    
    // 抽取配置
    ExtractionStrategy m_strategy;                        ///< 抽取策略
    bool m_caseSensitive;                                 ///< 是否区分大小写
    bool m_multilineMode;                                 ///< 多行模式

    // 预定义的实验报告字段模式
    static const QMap<QString, ExtractionRule> PREDEFINED_RULES;
};
