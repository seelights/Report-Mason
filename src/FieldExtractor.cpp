/*
 * FieldExtractor.cpp
 * 字段提取器实现
 *
 * @Author: seelights@git.cn
 * @Date: 2025-01-27
 * @Description: 实现从文档中提取结构化字段的功能
 * @FilePath: \ReportMason\src\FieldExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "QtCompat.h"
#include "FieldExtractor.h"
#include <QRegularExpression>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>

FieldExtractor::FieldExtractor(QObject* parent)
    : QObject(parent), m_strategy(ExtractionStrategy::REGEX_PATTERN), m_caseSensitive(false),
      m_multilineMode(false)
{
    initializePredefinedRules();
    loadConfiguration();
}

FieldExtractor::~FieldExtractor() { saveConfiguration(); }

void FieldExtractor::setExtractionStrategy(ExtractionStrategy strategy) { m_strategy = strategy; }

FieldExtractor::ExtractionStrategy FieldExtractor::getExtractionStrategy() const
{
    return m_strategy;
}

void FieldExtractor::setCaseSensitive(bool caseSensitive) { m_caseSensitive = caseSensitive; }

bool FieldExtractor::isCaseSensitive() const { return m_caseSensitive; }

void FieldExtractor::setMultilineMode(bool multilineMode) { m_multilineMode = multilineMode; }

bool FieldExtractor::isMultilineMode() const { return m_multilineMode; }

void FieldExtractor::addExtractionRule(const QString& fieldName, const ExtractionRule& rule)
{
    m_extractionRules[fieldName] = rule;
}

void FieldExtractor::removeExtractionRule(const QString& fieldName)
{
    m_extractionRules.remove(fieldName);
}

FieldExtractor::ExtractionRule FieldExtractor::getExtractionRule(const QString& fieldName) const
{
    return m_extractionRules.value(fieldName, ExtractionRule());
}

QMap<QString, FieldExtractor::ExtractionRule> FieldExtractor::getAllRules() const
{
    return m_extractionRules;
}

void FieldExtractor::clearRules() { m_extractionRules.clear(); }

QMap<QString, QString> FieldExtractor::extractFields(const QString& content)
{
    QMap<QString, QString> extractedFields;

    if (content.isEmpty()) {
        return extractedFields;
    }

    QString processedContent = preprocessContent(content);

    // 遍历所有提取规则
    for (auto it = m_extractionRules.begin(); it != m_extractionRules.end(); ++it) {
        const ExtractionRule& rule = it.value();

        if (!rule.enabled) {
            continue;
        }

        QString extractedValue = extractFieldValue(processedContent, rule);
        if (!extractedValue.isEmpty()) {
            extractedFields[it.key()] = extractedValue;
        }
    }

    return extractedFields;
}

QString FieldExtractor::extractFieldValue(const QString& content, const ExtractionRule& rule)
{
    QString result;

    switch (m_strategy) {
    case ExtractionStrategy::REGEX_PATTERN:
        result = extractUsingRegex(content, rule);
        break;
    case ExtractionStrategy::KEYWORD_MATCHING:
        result = extractUsingKeywords(content, rule);
        break;
    case ExtractionStrategy::POSITION_BASED:
        result = extractUsingPosition(content, rule);
        break;
    default:
        result = extractUsingRegex(content, rule);
        break;
    }

    return result;
}

QString FieldExtractor::extractUsingRegex(const QString& content, const ExtractionRule& rule)
{
    QString result;

    // 尝试每个正则表达式模式
    for (const QString& pattern : rule.regexPatterns) {
        QRegularExpression regex(pattern);
        regex.setPatternOptions(m_caseSensitive ? QRegularExpression::NoPatternOption
                                                : QRegularExpression::CaseInsensitiveOption);

        if (m_multilineMode) {
            regex.setPatternOptions(regex.patternOptions() | QRegularExpression::MultilineOption);
        }

        QRegularExpressionMatch match = regex.match(content);
        if (match.hasMatch()) {
            result = match.captured(1).trimmed();
            if (!result.isEmpty()) {
                break;
            }
        }
    }

    return result;
}

QString FieldExtractor::extractUsingKeywords(const QString& content, const ExtractionRule& rule)
{
    QString result;

    // 查找关键词
    for (const QString& keyword : rule.keywords) {
        int keywordPos =
            content.indexOf(keyword, 0, m_caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
        if (keywordPos != -1) {
            // 提取关键词后的内容
            int startPos = keywordPos + keyword.length();
            QString remaining = content.mid(startPos).trimmed();

            // 查找下一个关键词或行尾
            int endPos = remaining.length();
            for (const QString& nextKeyword : rule.keywords) {
                int nextPos = remaining.indexOf(
                    nextKeyword, 0, m_caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
                if (nextPos != -1 && nextPos < endPos) {
                    endPos = nextPos;
                }
            }

            result = remaining.left(endPos).trimmed();
            if (!result.isEmpty()) {
                break;
            }
        }
    }

    return result;
}

QString FieldExtractor::extractUsingPosition(const QString& content, const ExtractionRule& rule)
{
    // 基于位置的提取逻辑
    // 这里可以根据具体需求实现
    Q_UNUSED(rule);
    return QString();
}

QString FieldExtractor::preprocessContent(const QString& content)
{
    QString processed = content;

    // 移除多余的空白字符
    processed = processed.simplified();

    // 统一换行符
    processed.replace(QRegularExpression(QS("\\r\\n|\\r")), QS("\\n"));

    // 移除特殊字符（可选）
    // processed.remove(QRegularExpression("[\\x00-\\x1F\\x7F]"));

    return processed;
}

void FieldExtractor::initializePredefinedRules()
{
    // 预定义的提取规则
    m_extractionRules.clear();

    // Title规则
    ExtractionRule titleRule;
    titleRule.fieldName = QS("Title");
    titleRule.displayName = QS("实验标题");
    titleRule.description = QS("实验标题");
    titleRule.regexPatterns = {QS("实验标题[：:]\\s*(.+?)(?:\\n|$)"),
                               QS("实验名称[：:]\\s*(.+?)(?:\\n|$)"),
                               QS("实验标题[：:]\\s*(.+?)(?:\\n|$)")};
    titleRule.keywords = {QS("实验标题"), QS("实验名称"), QS("实验标题"), QS("实验目的")};
    titleRule.enabled = true;
    titleRule.defaultValue = QS("");
    titleRule.tags = {QS("title"), QS("name"), QS("subject")};
    titleRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("Title")] = titleRule;

    // StudentName规则
    ExtractionRule studentNameRule;
    studentNameRule.fieldName = QS("StudentName");
    studentNameRule.displayName = QS("学生姓名");
    studentNameRule.description = QS("学生姓名");
    studentNameRule.regexPatterns = {QS("学生姓名[：:]\\s*(.+?)(?:\\n|$)"),
                                     QS("姓名[：:]\\s*(.+?)(?:\\n|$)"),
                                     QS("学生姓名\\s*([^\\s\\n]+)")};
    studentNameRule.keywords = {QS("学生姓名"), QS("姓名"), QS("学生姓名")};
    studentNameRule.enabled = true;
    studentNameRule.defaultValue = QS("");
    studentNameRule.tags = {QS("name"), QS("student")};
    studentNameRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("StudentName")] = studentNameRule;

    // StudentID规则
    ExtractionRule studentIDRule;
    studentIDRule.fieldName = QS("StudentID");
    studentIDRule.displayName = QS("学生学号");
    studentIDRule.description = QS("学生学号");
    studentIDRule.regexPatterns = {QS("学生学号[：:]\\s*([0-9]+)"), QS("学号[：:]\\s*([0-9]+)"),
                                   QS("ID[：:]\\s*([0-9]+)")};
    studentIDRule.keywords = {QS("学生学号"), QS("ID"), QS("学生学号")};
    studentIDRule.enabled = true;
    studentIDRule.defaultValue = QS("");
    studentIDRule.tags = {QS("id"), QS("student_id")};
    studentIDRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("StudentID")] = studentIDRule;

    // Class规则
    ExtractionRule classRule;
    classRule.fieldName = QS("Class");
    classRule.displayName = QS("班级");
    classRule.description = QS("班级");
    classRule.regexPatterns = {QS("班级[：:]\\s*(.+?)(?:\\n|$)"), QS("班级\\s*([^\\s\\n]+)"),
                               QS("专业班级[：:]\\s*(.+?)(?:\\n|$)")};
    classRule.keywords = {QS("班级"), QS("专业班级"), QS("班级名称")};
    classRule.enabled = false;
    classRule.defaultValue = QS("");
    classRule.tags = {QS("class"), QS("major")};
    classRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("Class")] = classRule;

    // Abstract规则
    ExtractionRule abstractRule;
    abstractRule.fieldName = QS("Abstract");
    abstractRule.displayName = QS("摘要");
    abstractRule.description = QS("摘要");
    abstractRule.regexPatterns = {QS("摘要[：:]\\s*(.+?)(?=关键词|结论|$)"),
                                  QS("实验摘要[：:]\\s*(.+?)(?=关键词|结论|$)")};
    abstractRule.keywords = {QS("摘要"), QS("实验摘要"), QS("内容摘要")};
    abstractRule.enabled = false;
    abstractRule.defaultValue = QS("");
    abstractRule.tags = {QS("summary"), QS("overview")};
    abstractRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("Abstract")] = abstractRule;

    // Keywords规则
    ExtractionRule keywordsRule;
    keywordsRule.fieldName = QS("Keywords");
    keywordsRule.displayName = QS("关键词");
    keywordsRule.description = QS("关键词");
    keywordsRule.regexPatterns = {QS("关键词[：:]\\s*(.+?)(?:\\n|$)"),
                                  QS("关键词[：:]\\s*(.+?)(?:\\n|$)")};
    keywordsRule.keywords = {QS("关键词"), QS("关键词"), QS("标签关键词")};
    keywordsRule.enabled = false;
    keywordsRule.defaultValue = QS("");
    keywordsRule.tags = {QS("keywords"), QS("tags")};
    keywordsRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("Keywords")] = keywordsRule;

    // ExperimentObjective规则
    ExtractionRule objectiveRule;
    objectiveRule.fieldName = QS("ExperimentObjective");
    objectiveRule.displayName = QS("实验目的");
    objectiveRule.description = QS("实验目的");
    objectiveRule.regexPatterns = {QS("实验目的[：:]\\s*(.+?)(?=实验原理|实验步骤|$)"),
                                   QS("目的[：:]\\s*(.+?)(?=原理|步骤|$)")};
    objectiveRule.keywords = {QS("实验目的"), QS("目的"), QS("实验目标")};
    objectiveRule.enabled = false;
    objectiveRule.defaultValue = QS("");
    objectiveRule.tags = {QS("objective"), QS("purpose")};
    objectiveRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("ExperimentObjective")] = objectiveRule;

    // ExperimentPrinciple规则
    ExtractionRule principleRule;
    principleRule.fieldName = QS("ExperimentPrinciple");
    principleRule.displayName = QS("实验原理");
    principleRule.description = QS("实验原理");
    principleRule.regexPatterns = {QS("实验原理[：:]\\s*(.+?)(?=实验步骤|实验分析|$)"),
                                   QS("原理[：:]\\s*(.+?)(?=步骤|分析|$)")};
    principleRule.keywords = {QS("实验原理"), QS("原理"), QS("理论基础")};
    principleRule.enabled = false;
    principleRule.defaultValue = QS("");
    principleRule.tags = {QS("principle"), QS("theory")};
    principleRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("ExperimentPrinciple")] = principleRule;

    // ExperimentSteps规则
    ExtractionRule stepsRule;
    stepsRule.fieldName = QS("ExperimentSteps");
    stepsRule.displayName = QS("实验步骤");
    stepsRule.description = QS("实验步骤");
    stepsRule.regexPatterns = {QS("实验步骤[：:]\\s*(.+?)(?=实验结果|实验分析|$)"),
                               QS("步骤[：:]\\s*(.+?)(?=结果|分析|$)")};
    stepsRule.keywords = {QS("实验步骤"), QS("步骤"), QS("实验过程")};
    stepsRule.enabled = false;
    stepsRule.defaultValue = QS("");
    stepsRule.tags = {QS("steps"), QS("procedure")};
    stepsRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("ExperimentSteps")] = stepsRule;

    // ExperimentResults规则
    ExtractionRule resultsRule;
    resultsRule.fieldName = QS("ExperimentResults");
    resultsRule.displayName = QS("实验结果");
    resultsRule.description = QS("实验结果");
    resultsRule.regexPatterns = {QS("实验结果[：:]\\s*(.+?)(?=实验分析|结论|$)"),
                                 QS("结果[：:]\\s*(.+?)(?=分析|结论|$)")};
    resultsRule.keywords = {QS("实验结果"), QS("结果"), QS("实验数据")};
    resultsRule.enabled = false;
    resultsRule.defaultValue = QS("");
    resultsRule.tags = {QS("results"), QS("data")};
    resultsRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("ExperimentResults")] = resultsRule;

    // ExperimentAnalysis规则
    ExtractionRule analysisRule;
    analysisRule.fieldName = QS("ExperimentAnalysis");
    analysisRule.displayName = QS("实验分析");
    analysisRule.description = QS("实验分析");
    analysisRule.regexPatterns = {QS("实验分析[：:]\\s*(.+?)(?=结论|$)"),
                                  QS("分析[：:]\\s*(.+?)(?=结论|$)")};
    analysisRule.keywords = {QS("实验分析"), QS("分析"), QS("结果分析")};
    analysisRule.enabled = false;
    analysisRule.defaultValue = QS("");
    analysisRule.tags = {QS("analysis"), QS("discussion")};
    analysisRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("ExperimentAnalysis")] = analysisRule;

    // Conclusion规则
    ExtractionRule conclusionRule;
    conclusionRule.fieldName = QS("Conclusion");
    conclusionRule.displayName = QS("结论");
    conclusionRule.description = QS("结论");
    conclusionRule.regexPatterns = {QS("结论[：:]\\s*(.+?)$"), QS("总结[：:]\\s*(.+?)$")};
    conclusionRule.keywords = {QS("结论"), QS("总结"), QS("实验结论")};
    conclusionRule.enabled = false;
    conclusionRule.defaultValue = QS("");
    conclusionRule.tags = {QS("conclusion"), QS("summary")};
    conclusionRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules[QS("Conclusion")] = conclusionRule;
}

QMap<QString, FieldExtractor::ExtractionRule> FieldExtractor::getPredefinedRules() const
{
    return m_extractionRules;
}

void FieldExtractor::loadConfiguration()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);

    QFile configFile(configPath + QS("/field_extractor_config.json"));
    if (configFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll());
        QJsonObject config = doc.object();

        // 加载配置
        m_caseSensitive = config[QS("caseSensitive")].toBool(false);
        m_multilineMode = config[QS("multilineMode")].toBool(false);
        m_strategy = static_cast<ExtractionStrategy>(config[QS("strategy")].toInt(0));

        // 加载自定义规则
        QJsonArray rulesArray = config[QS("customRules")].toArray();
        for (const QJsonValue& value : rulesArray) {
            QJsonObject ruleObj = value.toObject();
            QString fieldName = ruleObj[QS("fieldName")].toString();
            if (!fieldName.isEmpty()) {
                ExtractionRule rule;
                rule.fieldName = ruleObj[QS("fieldName")].toString();
                rule.displayName = ruleObj[QS("displayName")].toString();
                rule.enabled = ruleObj[QS("enabled")].toBool(true);
                rule.description = ruleObj[QS("description")].toString();

                QJsonArray patternsArray = ruleObj[QS("regexPatterns")].toArray();
                for (const QJsonValue& patternValue : patternsArray) {
                    rule.regexPatterns.append(patternValue.toString());
                }

                QJsonArray keywordsArray = ruleObj[QS("keywords")].toArray();
                for (const QJsonValue& keywordValue : keywordsArray) {
                    rule.keywords.append(keywordValue.toString());
                }

                QJsonArray tagsArray = ruleObj[QS("tags")].toArray();
                for (const QJsonValue& tagValue : tagsArray) {
                    rule.tags.append(tagValue.toString());
                }

                rule.strategy = static_cast<ExtractionStrategy>(ruleObj[QS("strategy")].toInt(0));

                m_extractionRules[fieldName] = rule;
            }
        }
    }
}

void FieldExtractor::saveConfiguration()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);

    QFile configFile(configPath + QS("/field_extractor_config.json"));
    if (configFile.open(QIODevice::WriteOnly)) {
        QJsonObject config;
        config[QS("caseSensitive")] = m_caseSensitive;
        config[QS("multilineMode")] = m_multilineMode;
        config[QS("strategy")] = static_cast<int>(m_strategy);

        QJsonArray rulesArray;
        for (auto it = m_extractionRules.begin(); it != m_extractionRules.end(); ++it) {
            QJsonObject ruleObj;
            ruleObj[QS("fieldName")] = it.key();
            ruleObj[QS("displayName")] = it.value().displayName;
            ruleObj[QS("enabled")] = it.value().enabled;
            ruleObj[QS("description")] = it.value().description;

            QJsonArray patternsArray;
            for (const QString& pattern : it.value().regexPatterns) {
                patternsArray.append(pattern);
            }
            ruleObj[QS("regexPatterns")] = patternsArray;

            QJsonArray keywordsArray;
            for (const QString& keyword : it.value().keywords) {
                keywordsArray.append(keyword);
            }
            ruleObj[QS("keywords")] = keywordsArray;

            QJsonArray tagsArray;
            for (const QString& tag : it.value().tags) {
                tagsArray.append(tag);
            }
            ruleObj[QS("tags")] = tagsArray;

            ruleObj[QS("strategy")] = static_cast<int>(it.value().strategy);

            rulesArray.append(ruleObj);
        }
        config[QS("customRules")] = rulesArray;

        QJsonDocument doc(config);
        configFile.write(doc.toJson());
    }
}
