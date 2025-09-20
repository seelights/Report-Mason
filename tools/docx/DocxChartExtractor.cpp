/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-20 15:17:23
 * @LastEditors: seelights
 * @Description: DOCX图表提取器实现
 * @FilePath: \ReportMason\tools\docx\DocxChartExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "QtCompat.h"
#include "DocxChartExtractor.h"
#include "../utils/ContentUtils.h"
#include <QFileInfo>
#include <QDebug>

// 静态常量定义
const QStringList DocxChartExtractor::SUPPORTED_EXTENSIONS = {QS("docx")};
const QString DocxChartExtractor::DOCX_DOCUMENT_PATH = QS("word/document.xml");
const QString DocxChartExtractor::DOCX_RELATIONSHIPS_PATH = QS("word/_rels/document.xml.rels");
const QString DocxChartExtractor::DOCX_CHARTS_PATH = QS("word/charts/");

DocxChartExtractor::DocxChartExtractor(QObject* parent) : ChartExtractor(parent) {}

DocxChartExtractor::~DocxChartExtractor() {}

bool DocxChartExtractor::isSupported(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    return SUPPORTED_EXTENSIONS.contains(fileInfo.suffix().toLower());
}

QStringList DocxChartExtractor::getSupportedFormats() const { return SUPPORTED_EXTENSIONS; }

ChartExtractor::ExtractStatus DocxChartExtractor::extractCharts(const QString& filePath,
                                                                QList<ChartInfo>& charts)
{
    if (!isSupported(filePath)) {
        setLastError(QS("不支持的文件格式，仅支持.docx文件"));
        return ExtractStatus::INVALID_FORMAT;
    }

    if (!validateFilePath(filePath)) {
        setLastError(QS("文件不存在或无法读取"));
        return ExtractStatus::FILE_NOT_FOUND;
    }

    // 验证ZIP文件
    if (!KZipUtils::isValidZip(filePath)) {
        setLastError(QS("无效的DOCX文件格式"));
        return ExtractStatus::INVALID_FORMAT;
    }

    // 读取document.xml
    QByteArray xmlContent;
    if (!KZipUtils::readFileFromZip(filePath, DOCX_DOCUMENT_PATH, xmlContent)) {
        setLastError(QS("无法读取DOCX文档内容"));
        return ExtractStatus::PARSE_ERROR;
    }

    // 解析XML内容
    if (!parseDocumentXml(xmlContent, charts)) {
        setLastError(QS("解析DOCX文档XML失败"));
        return ExtractStatus::PARSE_ERROR;
    }

    qDebug() << "DocxChartExtractor: 成功提取" << charts.size() << "个图表";
    return ExtractStatus::SUCCESS;
}

ChartExtractor::ExtractStatus DocxChartExtractor::extractChartsByType(const QString& filePath,
                                                                      ChartType chartType,
                                                                      QList<ChartInfo>& charts)
{
    QList<ChartInfo> allCharts;
    ExtractStatus status = extractCharts(filePath, allCharts);
    if (status != ExtractStatus::SUCCESS) {
        return status;
    }

    // 过滤指定类型的图表
    for (const ChartInfo& chart : allCharts) {
        if (chart.type == chartType) {
            charts.append(chart);
        }
    }

    return ExtractStatus::SUCCESS;
}

ChartExtractor::ExtractStatus DocxChartExtractor::extractChartsByPosition(const QString& filePath,
                                                                          const QRect& position,
                                                                          QList<ChartInfo>& charts)
{
    QList<ChartInfo> allCharts;
    ExtractStatus status = extractCharts(filePath, allCharts);
    if (status != ExtractStatus::SUCCESS) {
        return status;
    }

    // 过滤指定位置的图表
    for (const ChartInfo& chart : allCharts) {
        if (position.intersects(chart.position)) {
            charts.append(chart);
        }
    }

    return ExtractStatus::SUCCESS;
}

int DocxChartExtractor::getChartCount(const QString& filePath)
{
    QList<ChartInfo> charts;
    ExtractStatus status = extractCharts(filePath, charts);
    if (status != ExtractStatus::SUCCESS) {
        return -1;
    }
    return charts.size();
}

QByteArray DocxChartExtractor::readFileFromZip(const QString& zipPath,
                                               const QString& internalPath) const
{
    // 这里需要使用KZipUtils来读取ZIP文件
    // 暂时返回空数据，实际实现需要集成KZipUtils
    Q_UNUSED(zipPath)
    Q_UNUSED(internalPath)

    qDebug() << "DocxChartExtractor: 需要集成KZipUtils来读取ZIP文件";
    return QByteArray();
}

bool DocxChartExtractor::parseDocumentXml(const QByteArray& xmlContent,
                                          QList<ChartInfo>& charts) const
{
    QXmlStreamReader reader(xmlContent);

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();

            if (elementName == QS("chart")) {
                parseChartElement(reader, charts);
            }
        }
    }

    return !reader.hasError();
}

bool DocxChartExtractor::parseChartElement(QXmlStreamReader& reader, QList<ChartInfo>& charts) const
{
    Q_UNUSED(reader)
    Q_UNUSED(charts)

    // 解析chart元素中的图表信息
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxChartExtractor: 解析chart元素";
    return true;
}

bool DocxChartExtractor::parseChartData(const QString& zipPath, const QString& chartPath,
                                        ChartInfo& chart) const
{
    Q_UNUSED(zipPath)
    Q_UNUSED(chartPath)
    Q_UNUSED(chart)

    // 解析图表数据
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxChartExtractor: 解析图表数据";
    return true;
}

bool DocxChartExtractor::parseExcelChart(const QByteArray& chartData, ChartInfo& chart) const
{
    Q_UNUSED(chartData)
    Q_UNUSED(chart)

    // 解析Excel图表文件
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxChartExtractor: 解析Excel图表文件";
    return true;
}

ChartType DocxChartExtractor::detectChartType(const QByteArray& chartData) const
{
    Q_UNUSED(chartData)

    // 检测图表类型
    // 这里需要实现具体的检测逻辑
    qDebug() << "DocxChartExtractor: 检测图表类型";
    return ChartType::UNKNOWN;
}

QString DocxChartExtractor::getChartTitle(const QByteArray& chartData) const
{
    Q_UNUSED(chartData)

    // 获取图表标题
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxChartExtractor: 获取图表标题";
    return QString();
}

bool DocxChartExtractor::getChartSeries(const QByteArray& chartData,
                                        QList<DataSeries>& series) const
{
    Q_UNUSED(chartData)
    Q_UNUSED(series)

    // 获取图表数据系列
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxChartExtractor: 获取图表数据系列";
    return true;
}

bool DocxChartExtractor::getChartPosition(QXmlStreamReader& reader, QRect& position) const
{
    position = QRect(0, 0, 0, 0);
    bool hasPosition = false;
    bool hasSize = false;
    
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();
            QXmlStreamAttributes attributes = reader.attributes();
            
            if (elementName == QS("c:chart")) {
                // 图表元素，解析其属性
                QString left = attributes.value(QS("left")).toString();
                QString top = attributes.value(QS("top")).toString();
                QString width = attributes.value(QS("width")).toString();
                QString height = attributes.value(QS("height")).toString();
                
                if (!left.isEmpty() && !top.isEmpty()) {
                    bool ok1, ok2;
                    int x = left.toInt(&ok1);
                    int y = top.toInt(&ok2);
                    if (ok1 && ok2) {
                        position.setX(x);
                        position.setY(y);
                        hasPosition = true;
                    }
                }
                
                if (!width.isEmpty() && !height.isEmpty()) {
                    bool ok1, ok2;
                    int w = width.toInt(&ok1);
                    int h = height.toInt(&ok2);
                    if (ok1 && ok2) {
                        position.setWidth(w);
                        position.setHeight(h);
                        hasSize = true;
                    }
                }
            } else if (elementName == QS("wp:extent")) {
                // 图表尺寸信息
                QString cx = attributes.value(QS("cx")).toString();
                QString cy = attributes.value(QS("cy")).toString();
                
                bool ok1, ok2;
                qint64 widthEmu = cx.toLongLong(&ok1);
                qint64 heightEmu = cy.toLongLong(&ok2);
                
                if (ok1 && ok2) {
                    // 转换EMU到像素
                    position.setWidth(static_cast<int>(widthEmu / 9525));
                    position.setHeight(static_cast<int>(heightEmu / 9525));
                    hasSize = true;
                }
            } else if (elementName == QS("wp:posOffset")) {
                // 位置偏移
                QString value = reader.readElementText();
                bool ok;
                qint64 emu = value.toLongLong(&ok);
                if (ok) {
                    if (!hasPosition) {
                        position.setX(static_cast<int>(emu / 9525));
                        hasPosition = true;
                    } else {
                        position.setY(static_cast<int>(emu / 9525));
                    }
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            QString elementName = reader.name().toString();
            if (elementName == QS("c:chart") || elementName == QS("drawing")) {
                break;
            }
        }
    }
    
    // 如果没有找到位置信息，使用默认值
    if (!hasPosition) {
        position.setX(0);
        position.setY(0);
    }
    if (!hasSize) {
        position.setWidth(300);  // 默认宽度
        position.setHeight(200); // 默认高度
    }
    
    return true;
}

bool DocxChartExtractor::getChartSize(QXmlStreamReader& reader, QSize& size) const
{
    Q_UNUSED(reader)
    Q_UNUSED(size)

    // 获取图表尺寸
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxChartExtractor: 获取图表尺寸";
    return true;
}

bool DocxChartExtractor::getChartFromRelationship(const QString& zipPath,
                                                  const QString& relationshipId,
                                                  QString& chartPath) const
{
    Q_UNUSED(zipPath)
    Q_UNUSED(relationshipId)
    Q_UNUSED(chartPath)

    // 从关系文件获取图表信息
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxChartExtractor: 从关系文件获取图表信息";
    return true;
}
