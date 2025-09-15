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

FieldExtractor::FieldExtractor(QObject *parent)
    : QObject(parent)
    , m_strategy(ExtractionStrategy::REGEX_PATTERN)
    , m_caseSensitive(false)
    , m_multilineMode(false)
{
    initializePredefinedRules();
    loadConfiguration();
}

FieldExtractor::~FieldExtractor()
{
    saveConfiguration();
}

void FieldExtractor::setExtractionStrategy(ExtractionStrategy strategy)
{
    m_strategy = strategy;
}

FieldExtractor::ExtractionStrategy FieldExtractor::getExtractionStrategy() const
{
    return m_strategy;
}

void FieldExtractor::setCaseSensitive(bool caseSensitive)
{
    m_caseSensitive = caseSensitive;
}

bool FieldExtractor::isCaseSensitive() const
{
    return m_caseSensitive;
}

void FieldExtractor::setMultilineMode(bool multilineMode)
{
    m_multilineMode = multilineMode;
}

bool FieldExtractor::isMultilineMode() const
{
    return m_multilineMode;
}

void FieldExtractor::addExtractionRule(const QString &fieldName, const ExtractionRule &rule)
{
    m_extractionRules[fieldName] = rule;
}

void FieldExtractor::removeExtractionRule(const QString &fieldName)
{
    m_extractionRules.remove(fieldName);
}

FieldExtractor::ExtractionRule FieldExtractor::getExtractionRule(const QString &fieldName) const
{
    return m_extractionRules.value(fieldName, ExtractionRule());
}

QMap<QString, FieldExtractor::ExtractionRule> FieldExtractor::getAllRules() const
{
    return m_extractionRules;
}

void FieldExtractor::clearRules()
{
    m_extractionRules.clear();
}

QMap<QString, QString> FieldExtractor::extractFields(const QString &content)
{
    QMap<QString, QString> extractedFields;
    
    if (content.isEmpty()) {
        return extractedFields;
    }
    
    QString processedContent = preprocessContent(content);
    
    // 遍历所有提取规则
    for (auto it = m_extractionRules.begin(); it != m_extractionRules.end(); ++it) {
        const ExtractionRule &rule = it.value();
        
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

QString FieldExtractor::extractFieldValue(const QString &content, const ExtractionRule &rule)
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

QString FieldExtractor::extractUsingRegex(const QString &content, const ExtractionRule &rule)
{
    QString result;
    
    // 尝试每个正则表达式模式
    for (const QString &pattern : rule.regexPatterns) {
        QRegularExpression regex(pattern);
        regex.setPatternOptions(m_caseSensitive ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption);
        
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

QString FieldExtractor::extractUsingKeywords(const QString &content, const ExtractionRule &rule)
{
    QString result;
    
    // 查找关键词
    for (const QString &keyword : rule.keywords) {
        int keywordPos = content.indexOf(keyword, 0, m_caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
        if (keywordPos != -1) {
            // 提取关键词后的内容
            int startPos = keywordPos + keyword.length();
            QString remaining = content.mid(startPos).trimmed();
            
            // 查找下一个关键词或行尾
            int endPos = remaining.length();
            for (const QString &nextKeyword : rule.keywords) {
                int nextPos = remaining.indexOf(nextKeyword, 0, m_caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
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

QString FieldExtractor::extractUsingPosition(const QString &content, const ExtractionRule &rule)
{
    // 基于位置的提取逻辑
    // 这里可以根据具体需求实现
    Q_UNUSED(rule);
    return QString();
}

QString FieldExtractor::preprocessContent(const QString &content)
{
    QString processed = content;
    
    // 移除多余的空白字符
    processed = processed.simplified();
    
    // 统一换行符
    processed.replace(QRegularExpression("\\r\\n|\\r"), "\\n");
    
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
    titleRule.fieldName = "Title";
    titleRule.displayName = "实验标题";
    titleRule.description = "实验标题";
    titleRule.regexPatterns = {"实验标题[：:]\\s*(.+?)(?:\\n|$)", "实验名称[：:]\\s*(.+?)(?:\\n|$)", "实验标题[：:]\\s*(.+?)(?:\\n|$)"};
    titleRule.keywords = {"实验标题", "实验名称", "实验标题", "实验目的"};
    titleRule.enabled = true;
    titleRule.defaultValue = "";
    titleRule.tags = {"title", "name", "subject"};
    titleRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["Title"] = titleRule;
    
    // StudentName规则
    ExtractionRule studentNameRule;
    studentNameRule.fieldName = "StudentName";
    studentNameRule.displayName = "学生姓名";
    studentNameRule.description = "学生姓名";
    studentNameRule.regexPatterns = {"学生姓名[：:]\\s*(.+?)(?:\\n|$)", "姓名[：:]\\s*(.+?)(?:\\n|$)", "学生姓名\\s*([^\\s\\n]+)"};
    studentNameRule.keywords = {"学生姓名", "姓名", "学生姓名"};
    studentNameRule.enabled = true;
    studentNameRule.defaultValue = "";
    studentNameRule.tags = {"name", "student"};
    studentNameRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["StudentName"] = studentNameRule;
    
    // StudentID规则
    ExtractionRule studentIDRule;
    studentIDRule.fieldName = "StudentID";
    studentIDRule.displayName = "学生学号";
    studentIDRule.description = "学生学号";
    studentIDRule.regexPatterns = {"学生学号[：:]\\s*([0-9]+)", "学号[：:]\\s*([0-9]+)", "ID[：:]\\s*([0-9]+)"};
    studentIDRule.keywords = {"学生学号", "ID", "学生学号"};
    studentIDRule.enabled = true;
    studentIDRule.defaultValue = "";
    studentIDRule.tags = {"id", "student_id"};
    studentIDRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["StudentID"] = studentIDRule;
    
    // Class规则
    ExtractionRule classRule;
    classRule.fieldName = "Class";
    classRule.displayName = "班级";
    classRule.description = "班级";
    classRule.regexPatterns = {"班级[：:]\\s*(.+?)(?:\\n|$)", "班级\\s*([^\\s\\n]+)", "专业班级[：:]\\s*(.+?)(?:\\n|$)"};
    classRule.keywords = {"班级", "专业班级", "班级名称"};
    classRule.enabled = false;
    classRule.defaultValue = "";
    classRule.tags = {"class", "major"};
    classRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["Class"] = classRule;
    
    // Abstract规则
    ExtractionRule abstractRule;
    abstractRule.fieldName = "Abstract";
    abstractRule.displayName = "摘要";
    abstractRule.description = "摘要";
    abstractRule.regexPatterns = {"摘要[：:]\\s*(.+?)(?=关键词|结论|$)", "实验摘要[：:]\\s*(.+?)(?=关键词|结论|$)"};
    abstractRule.keywords = {"摘要", "实验摘要", "内容摘要"};
    abstractRule.enabled = false;
    abstractRule.defaultValue = "";
    abstractRule.tags = {"summary", "overview"};
    abstractRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["Abstract"] = abstractRule;
    
    // Keywords规则
    ExtractionRule keywordsRule;
    keywordsRule.fieldName = "Keywords";
    keywordsRule.displayName = "关键词";
    keywordsRule.description = "关键词";
    keywordsRule.regexPatterns = {"关键词[：:]\\s*(.+?)(?:\\n|$)", "关键词[：:]\\s*(.+?)(?:\\n|$)"};
    keywordsRule.keywords = {"关键词", "关键词", "标签关键词"};
    keywordsRule.enabled = false;
    keywordsRule.defaultValue = "";
    keywordsRule.tags = {"keywords", "tags"};
    keywordsRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["Keywords"] = keywordsRule;
    
    // ExperimentObjective规则
    ExtractionRule objectiveRule;
    objectiveRule.fieldName = "ExperimentObjective";
    objectiveRule.displayName = "实验目的";
    objectiveRule.description = "实验目的";
    objectiveRule.regexPatterns = {"实验目的[：:]\\s*(.+?)(?=实验原理|实验步骤|$)", "目的[：:]\\s*(.+?)(?=原理|步骤|$)"};
    objectiveRule.keywords = {"实验目的", "目的", "实验目标"};
    objectiveRule.enabled = false;
    objectiveRule.defaultValue = "";
    objectiveRule.tags = {"objective", "purpose"};
    objectiveRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["ExperimentObjective"] = objectiveRule;
    
    // ExperimentPrinciple规则
    ExtractionRule principleRule;
    principleRule.fieldName = "ExperimentPrinciple";
    principleRule.displayName = "实验原理";
    principleRule.description = "实验原理";
    principleRule.regexPatterns = {"实验原理[：:]\\s*(.+?)(?=实验步骤|实验分析|$)", "原理[：:]\\s*(.+?)(?=步骤|分析|$)"};
    principleRule.keywords = {"实验原理", "原理", "理论基础"};
    principleRule.enabled = false;
    principleRule.defaultValue = "";
    principleRule.tags = {"principle", "theory"};
    principleRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["ExperimentPrinciple"] = principleRule;
    
    // ExperimentSteps规则
    ExtractionRule stepsRule;
    stepsRule.fieldName = "ExperimentSteps";
    stepsRule.displayName = "实验步骤";
    stepsRule.description = "实验步骤";
    stepsRule.regexPatterns = {"实验步骤[：:]\\s*(.+?)(?=实验结果|实验分析|$)", "步骤[：:]\\s*(.+?)(?=结果|分析|$)"};
    stepsRule.keywords = {"实验步骤", "步骤", "实验过程"};
    stepsRule.enabled = false;
    stepsRule.defaultValue = "";
    stepsRule.tags = {"steps", "procedure"};
    stepsRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["ExperimentSteps"] = stepsRule;
    
    // ExperimentResults规则
    ExtractionRule resultsRule;
    resultsRule.fieldName = "ExperimentResults";
    resultsRule.displayName = "实验结果";
    resultsRule.description = "实验结果";
    resultsRule.regexPatterns = {"实验结果[：:]\\s*(.+?)(?=实验分析|结论|$)", "结果[：:]\\s*(.+?)(?=分析|结论|$)"};
    resultsRule.keywords = {"实验结果", "结果", "实验数据"};
    resultsRule.enabled = false;
    resultsRule.defaultValue = "";
    resultsRule.tags = {"results", "data"};
    resultsRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["ExperimentResults"] = resultsRule;
    
    // ExperimentAnalysis规则
    ExtractionRule analysisRule;
    analysisRule.fieldName = "ExperimentAnalysis";
    analysisRule.displayName = "实验分析";
    analysisRule.description = "实验分析";
    analysisRule.regexPatterns = {"实验分析[：:]\\s*(.+?)(?=结论|$)", "分析[：:]\\s*(.+?)(?=结论|$)"};
    analysisRule.keywords = {"实验分析", "分析", "结果分析"};
    analysisRule.enabled = false;
    analysisRule.defaultValue = "";
    analysisRule.tags = {"analysis", "discussion"};
    analysisRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["ExperimentAnalysis"] = analysisRule;
    
    // Conclusion规则
    ExtractionRule conclusionRule;
    conclusionRule.fieldName = "Conclusion";
    conclusionRule.displayName = "结论";
    conclusionRule.description = "结论";
    conclusionRule.regexPatterns = {"结论[：:]\\s*(.+?)$", "总结[：:]\\s*(.+?)$"};
    conclusionRule.keywords = {"结论", "总结", "实验结论"};
    conclusionRule.enabled = false;
    conclusionRule.defaultValue = "";
    conclusionRule.tags = {"conclusion", "summary"};
    conclusionRule.strategy = ExtractionStrategy::REGEX_PATTERN;
    m_extractionRules["Conclusion"] = conclusionRule;
}

QMap<QString, FieldExtractor::ExtractionRule> FieldExtractor::getPredefinedRules() const
{
    return m_extractionRules;
}

void FieldExtractor::loadConfiguration()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);
    
    QFile configFile(configPath + "/field_extractor_config.json");
    if (configFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll());
        QJsonObject config = doc.object();
        
        // 加载配置
        m_caseSensitive = config["caseSensitive"].toBool(false);
        m_multilineMode = config["multilineMode"].toBool(false);
        m_strategy = static_cast<ExtractionStrategy>(config["strategy"].toInt(0));
        
        // 加载自定义规则
        QJsonArray rulesArray = config["customRules"].toArray();
        for (const QJsonValue &value : rulesArray) {
            QJsonObject ruleObj = value.toObject();
            QString fieldName = ruleObj["fieldName"].toString();
            if (!fieldName.isEmpty()) {
                ExtractionRule rule;
                rule.fieldName = ruleObj["fieldName"].toString();
                rule.displayName = ruleObj["displayName"].toString();
                rule.enabled = ruleObj["enabled"].toBool(true);
                rule.description = ruleObj["description"].toString();
                
                QJsonArray patternsArray = ruleObj["regexPatterns"].toArray();
                for (const QJsonValue &patternValue : patternsArray) {
                    rule.regexPatterns.append(patternValue.toString());
                }
                
                QJsonArray keywordsArray = ruleObj["keywords"].toArray();
                for (const QJsonValue &keywordValue : keywordsArray) {
                    rule.keywords.append(keywordValue.toString());
                }
                
                QJsonArray tagsArray = ruleObj["tags"].toArray();
                for (const QJsonValue &tagValue : tagsArray) {
                    rule.tags.append(tagValue.toString());
                }
                
                rule.strategy = static_cast<ExtractionStrategy>(ruleObj["strategy"].toInt(0));
                
                m_extractionRules[fieldName] = rule;
            }
        }
    }
}

void FieldExtractor::saveConfiguration()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);
    
    QFile configFile(configPath + "/field_extractor_config.json");
    if (configFile.open(QIODevice::WriteOnly)) {
        QJsonObject config;
        config["caseSensitive"] = m_caseSensitive;
        config["multilineMode"] = m_multilineMode;
        config["strategy"] = static_cast<int>(m_strategy);
        
        QJsonArray rulesArray;
        for (auto it = m_extractionRules.begin(); it != m_extractionRules.end(); ++it) {
            QJsonObject ruleObj;
            ruleObj["fieldName"] = it.key();
            ruleObj["displayName"] = it.value().displayName;
            ruleObj["enabled"] = it.value().enabled;
            ruleObj["description"] = it.value().description;
            
            QJsonArray patternsArray;
            for (const QString &pattern : it.value().regexPatterns) {
                patternsArray.append(pattern);
            }
            ruleObj["regexPatterns"] = patternsArray;
            
            QJsonArray keywordsArray;
            for (const QString &keyword : it.value().keywords) {
                keywordsArray.append(keyword);
            }
            ruleObj["keywords"] = keywordsArray;
            
            QJsonArray tagsArray;
            for (const QString &tag : it.value().tags) {
                tagsArray.append(tag);
            }
            ruleObj["tags"] = tagsArray;
            
            ruleObj["strategy"] = static_cast<int>(it.value().strategy);
            
            rulesArray.append(ruleObj);
        }
        config["customRules"] = rulesArray;
        
        QJsonDocument doc(config);
        configFile.write(doc.toJson());
    }
}