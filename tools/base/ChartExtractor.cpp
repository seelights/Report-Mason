/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 图表提取器抽象基类实现
 * @FilePath: \ReportMason\tools\base\ChartExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "QtCompat.h"
#include "ChartExtractor.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QStringConverter>
#include <QDebug>
#include <QXmlStreamWriter>

ChartExtractor::ChartExtractor(QObject* parent) : ContentExtractor(parent)
{
    // 初始化支持的图表类型
    m_supportedTypes << QS("bar") << QS("line") << QS("pie") << QS("scatter") << QS("area") << QS("histogram");
}

ChartExtractor::~ChartExtractor() {}

bool ChartExtractor::exportToJson(const ChartInfo& chart, const QString& outputPath)
{
    QJsonObject json = chartToJson(chart);

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString(QS("无法创建JSON文件: %1")).arg(outputPath));
        return false;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    return true;
}

bool ChartExtractor::exportToCsv(const ChartInfo& chart, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString(QS("无法创建CSV文件: %1")).arg(outputPath));
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    // 写入标题
    stream << QS("Series,Label,Value\n");

    // 写入数据
    for (const DataSeries& series : chart.series) {
        for (int i = 0; i < series.labels.size() && i < series.values.size(); ++i) {
            stream << series.name << QS(",") << series.labels[i] << QS(",") << series.values[i] << QS("\n");
        }
    }

    file.close();
    return true;
}

QJsonObject ChartExtractor::chartToJson(const ChartInfo& chart) const
{
    QJsonObject json;
    json[QS("id")] = chart.id;
    json[QS("type")] = chartTypeToString(chart.type);
    json[QS("title")] = chart.title;
    json[QS("xAxisTitle")] = chart.xAxisTitle;
    json[QS("yAxisTitle")] = chart.yAxisTitle;
    json[QS("width")] = chart.size.width();
    json[QS("height")] = chart.size.height();
    json[QS("properties")] = chart.properties;

    QJsonArray seriesArray;
    for (const DataSeries& series : chart.series) {
        QJsonObject seriesObj;
        seriesObj[QS("name")] = series.name;
        seriesObj[QS("color")] = series.color;
        seriesObj[QS("properties")] = series.properties;

        QJsonArray labelsArray;
        for (const QString& label : series.labels) {
            labelsArray.append(label);
        }
        seriesObj[QS("labels")] = labelsArray;

        QJsonArray valuesArray;
        for (double value : series.values) {
            valuesArray.append(value);
        }
        seriesObj[QS("values")] = valuesArray;

        seriesArray.append(seriesObj);
    }
    json[QS("series")] = seriesArray;

    return json;
}

ChartInfo ChartExtractor::chartFromJson(const QJsonObject& json) const
{
    ChartInfo chart;
    chart.id = json[QS("id")].toString();
    chart.type = stringToChartType(json[QS("type")].toString());
    chart.title = json[QS("title")].toString();
    chart.xAxisTitle = json[QS("xAxisTitle")].toString();
    chart.yAxisTitle = json[QS("yAxisTitle")].toString();
    chart.size = QSize(json[QS("width")].toInt(), json[QS("height")].toInt());
    chart.properties = json[QS("properties")].toObject();

    QJsonArray seriesArray = json[QS("series")].toArray();
    for (const QJsonValue& value : seriesArray) {
        QJsonObject seriesObj = value.toObject();
        DataSeries series(seriesObj[QS("name")].toString());
        series.color = seriesObj[QS("color")].toString();
        series.properties = seriesObj[QS("properties")].toObject();

        QJsonArray labelsArray = seriesObj[QS("labels")].toArray();
        for (const QJsonValue& labelValue : labelsArray) {
            series.labels.append(labelValue.toString());
        }

        QJsonArray valuesArray = seriesObj[QS("values")].toArray();
        for (const QJsonValue& valueValue : valuesArray) {
            series.values.append(valueValue.toDouble());
        }

        chart.series.append(series);
    }

    return chart;
}

bool ChartExtractor::saveChartImage(const ChartInfo& chart, const QString& outputPath)
{
    if (chart.imageData.isEmpty()) {
        setLastError(QS("图表图片数据为空"));
        return false;
    }

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly)) {
        setLastError(QString(QS("无法创建图片文件: %1")).arg(outputPath));
        return false;
    }

    qint64 bytesWritten = file.write(chart.imageData);
    file.close();

    if (bytesWritten != chart.imageData.size()) {
        setLastError(QString(QS("图片文件写入不完整: %1")).arg(outputPath));
        return false;
    }

    return true;
}

QStringList ChartExtractor::getSupportedChartTypes() const { return m_supportedTypes; }

QString ChartExtractor::chartTypeToString(ChartType type) const
{
    switch (type) {
    case ChartType::BAR:
        return QS("bar");
    case ChartType::LINE:
        return QS("line");
    case ChartType::PIE:
        return QS("pie");
    case ChartType::SCATTER:
        return QS("scatter");
    case ChartType::AREA:
        return QS("area");
    case ChartType::HISTOGRAM:
        return QS("histogram");
    default:
        return QS("unknown");
    }
}

ChartType ChartExtractor::stringToChartType(const QString& typeString) const
{
    QString type = typeString.toLower();
    if (type == QS("bar"))
        return ChartType::BAR;
    if (type == QS("line"))
        return ChartType::LINE;
    if (type == QS("pie"))
        return ChartType::PIE;
    if (type == QS("scatter"))
        return ChartType::SCATTER;
    if (type == QS("area"))
        return ChartType::AREA;
    if (type == QS("histogram"))
        return ChartType::HISTOGRAM;
    return ChartType::UNKNOWN;
}

bool ChartExtractor::processChartData(const QJsonObject& rawData, ChartInfo& chartInfo)
{
    // 基本实现，子类可以重写
    chartInfo.id = generateChartId();
    chartInfo.type = detectChartType(rawData[QS("properties")].toObject());
    chartInfo.title = rawData[QS("title")].toString();
    chartInfo.xAxisTitle = rawData[QS("xAxisTitle")].toString();
    chartInfo.yAxisTitle = rawData[QS("yAxisTitle")].toString();
    chartInfo.properties = rawData[QS("properties")].toObject();

    return true;
}

QString ChartExtractor::generateChartId() { return generateUniqueId(QS("chart")); }

QString ChartExtractor::validateChart(const ChartInfo& chart) const
{
    if (chart.id.isEmpty()) {
        return QS("图表ID为空");
    }

    if (chart.type == ChartType::UNKNOWN) {
        return QS("图表类型未知");
    }

    if (chart.series.isEmpty()) {
        return QS("图表没有数据系列");
    }

    for (const DataSeries& series : chart.series) {
        if (series.labels.size() != series.values.size()) {
            return QString(QS("数据系列 '%1' 的标签和数值数量不匹配")).arg(series.name);
        }
    }

    return QString(); // 验证通过
}

ChartType ChartExtractor::detectChartType(const QJsonObject& properties) const
{
    QString chartType = properties[QS("chartType")].toString().toLower();
    return stringToChartType(chartType);
}

bool ChartExtractor::exportToXml(const ChartInfo& chart, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString(QS("无法创建XML文件: %1")).arg(outputPath));
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML声明
    writer.writeStartDocument(QS("1.0"), true);

    // 写入根元素
    writer.writeStartElement(QS("Chart"));
    writer.writeAttribute(QS("id"), chart.id);
    writer.writeAttribute(QS("type"), QString::number(static_cast<int>(chart.type)));
    writer.writeAttribute(QS("title"), chart.title);

    // 写入数据系列
    writer.writeStartElement(QS("DataSeries"));
    writer.writeAttribute(QS("count"), QString::number(chart.series.size()));
    
    for (const DataSeries& series : chart.series) {
        writer.writeStartElement(QS("Series"));
        writer.writeAttribute(QS("name"), series.name);
        
        // 写入标签
        writer.writeStartElement(QS("Labels"));
        for (const QString& label : series.labels) {
            writer.writeStartElement(QS("Label"));
            writer.writeCharacters(label);
            writer.writeEndElement(); // Label
        }
        writer.writeEndElement(); // Labels
        
        // 写入数值
        writer.writeStartElement(QS("Values"));
        for (double value : series.values) {
            writer.writeStartElement(QS("Value"));
            writer.writeCharacters(QString::number(value));
            writer.writeEndElement(); // Value
        }
        writer.writeEndElement(); // Values
        
        writer.writeEndElement(); // Series
    }
    writer.writeEndElement(); // DataSeries

    // 写入图片数据（如果有）
    if (!chart.imageData.isEmpty()) {
        writer.writeStartElement(QS("ImageData"));
        writer.writeAttribute(QS("encoding"), QS("base64"));
        writer.writeCharacters(encodeToBase64(chart.imageData));
        writer.writeEndElement(); // ImageData
    }

    // 写入属性
    if (!chart.properties.isEmpty()) {
        writer.writeStartElement(QS("Properties"));
        for (auto it = chart.properties.begin(); it != chart.properties.end(); ++it) {
            writer.writeStartElement(QS("Property"));
            writer.writeAttribute(QS("name"), it.key());
            writer.writeCharacters(it.value().toString());
            writer.writeEndElement(); // Property
        }
        writer.writeEndElement(); // Properties
    }

    writer.writeEndElement(); // Chart
    writer.writeEndDocument();

    file.close();
    return true;
}

bool ChartExtractor::exportToXml(const QList<ChartInfo>& charts, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString(QS("无法创建XML文件: %1")).arg(outputPath));
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML声明
    writer.writeStartDocument(QS("1.0"), true);

    // 写入根元素
    writer.writeStartElement(QS("Charts"));
    writer.writeAttribute(QS("count"), QString::number(charts.size()));

    // 写入每个图表
    for (const ChartInfo& chart : charts) {
        writer.writeStartElement(QS("Chart"));
        writer.writeAttribute(QS("id"), chart.id);
        writer.writeAttribute(QS("type"), QString::number(static_cast<int>(chart.type)));
        writer.writeAttribute(QS("title"), chart.title);

        // 写入数据系列
        writer.writeStartElement(QS("DataSeries"));
        writer.writeAttribute(QS("count"), QString::number(chart.series.size()));
        
        for (const DataSeries& series : chart.series) {
            writer.writeStartElement(QS("Series"));
            writer.writeAttribute(QS("name"), series.name);
            
            // 写入标签
            writer.writeStartElement(QS("Labels"));
            for (const QString& label : series.labels) {
                writer.writeStartElement(QS("Label"));
                writer.writeCharacters(label);
                writer.writeEndElement(); // Label
            }
            writer.writeEndElement(); // Labels
            
            // 写入数值
            writer.writeStartElement(QS("Values"));
            for (double value : series.values) {
                writer.writeStartElement(QS("Value"));
                writer.writeCharacters(QString::number(value));
                writer.writeEndElement(); // Value
            }
            writer.writeEndElement(); // Values
            
            writer.writeEndElement(); // Series
        }
        writer.writeEndElement(); // DataSeries

        // 写入图片数据（如果有）
        if (!chart.imageData.isEmpty()) {
            writer.writeStartElement(QS("ImageData"));
            writer.writeAttribute(QS("encoding"), QS("base64"));
            writer.writeCharacters(encodeToBase64(chart.imageData));
            writer.writeEndElement(); // ImageData
        }

        // 写入属性
        if (!chart.properties.isEmpty()) {
            writer.writeStartElement(QS("Properties"));
            for (auto it = chart.properties.begin(); it != chart.properties.end(); ++it) {
                writer.writeStartElement(QS("Property"));
                writer.writeAttribute(QS("name"), it.key());
                writer.writeCharacters(it.value().toString());
                writer.writeEndElement(); // Property
            }
            writer.writeEndElement(); // Properties
        }

        writer.writeEndElement(); // Chart
    }

    writer.writeEndElement(); // Charts
    writer.writeEndDocument();

    file.close();
    return true;
}
