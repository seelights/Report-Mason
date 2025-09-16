/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 图表提取器抽象基类
 * @FilePath: \ReportMason\tools\base\ChartExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#ifndef CHARTEXTRACTOR_H
#define CHARTEXTRACTOR_H

#include "ContentExtractor.h"
#include <QSize>
#include <QRect>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief 图表类型枚举
 */
enum class ChartType {
    BAR,       // 柱状图
    LINE,      // 折线图
    PIE,       // 饼图
    SCATTER,   // 散点图
    AREA,      // 面积图
    HISTOGRAM, // 直方图
    UNKNOWN    // 未知类型
};

/**
 * @brief 数据点信息结构
 */
struct DataPoint
{
    QString label;          // 标签
    double value;           // 数值
    QJsonObject properties; // 属性（颜色、样式等）

    DataPoint() : value(0.0) {}
    DataPoint(const QString& l, double v) : label(l), value(v) {}
};

/**
 * @brief 数据系列信息结构
 */
struct DataSeries
{
    QString name;           // 系列名称
    QStringList labels;     // 标签列表
    QList<double> values;   // 数值列表
    QJsonObject properties; // 系列属性
    QString color;          // 颜色

    DataSeries() {}
    DataSeries(const QString& n) : name(n) {}
};

/**
 * @brief 图表信息结构
 */
struct ChartInfo
{
    QString id;               // 图表唯一ID
    ChartType type;           // 图表类型
    QString title;            // 图表标题
    QString xAxisTitle;       // X轴标题
    QString yAxisTitle;       // Y轴标题
    QList<DataSeries> series; // 数据系列
    QSize size;               // 图表尺寸
    QRect position;           // 在文档中的位置
    QJsonObject properties;   // 图表属性
    QByteArray imageData;     // 图表图片数据
    QString imageFormat;      // 图片格式

    ChartInfo() : type(ChartType::UNKNOWN) {}
    ChartInfo(const QString& id, ChartType type) : id(id), type(type) {}
};

/**
 * @brief 图表提取器抽象基类
 *
 * 定义了图表提取的通用接口
 */
class ChartExtractor : public ContentExtractor
{
    Q_OBJECT

public:
    explicit ChartExtractor(QObject* parent = nullptr);
    virtual ~ChartExtractor();

    /**
     * @brief 从文档中提取所有图表
     * @param filePath 文档路径
     * @param charts 输出图表信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractCharts(const QString& filePath, QList<ChartInfo>& charts) = 0;

    /**
     * @brief 从文档中提取指定类型的图表
     * @param filePath 文档路径
     * @param chartType 图表类型
     * @param charts 输出图表信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractChartsByType(const QString& filePath, ChartType chartType,
                                              QList<ChartInfo>& charts) = 0;

    /**
     * @brief 从文档中提取指定位置的图表
     * @param filePath 文档路径
     * @param position 位置范围
     * @param charts 输出图表信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractChartsByPosition(const QString& filePath, const QRect& position,
                                                  QList<ChartInfo>& charts) = 0;

    /**
     * @brief 获取图表数量
     * @param filePath 文档路径
     * @return 图表数量，-1表示错误
     */
    virtual int getChartCount(const QString& filePath) = 0;

    /**
     * @brief 将图表导出为JSON格式
     * @param chart 图表信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToJson(const ChartInfo& chart, const QString& outputPath);

    /**
     * @brief 将图表导出为CSV格式
     * @param chart 图表信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToCsv(const ChartInfo& chart, const QString& outputPath);

    /**
     * @brief 将图表导出为XML格式
     * @param chart 图表信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToXml(const ChartInfo& chart, const QString& outputPath);

    /**
     * @brief 将图表列表导出为XML格式
     * @param charts 图表列表
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToXml(const QList<ChartInfo>& charts, const QString& outputPath);

    /**
     * @brief 将图表转换为JSON对象
     * @param chart 图表信息
     * @return JSON对象
     */
    virtual QJsonObject chartToJson(const ChartInfo& chart) const;

    /**
     * @brief 从JSON对象创建图表信息
     * @param json JSON对象
     * @return 图表信息
     */
    virtual ChartInfo chartFromJson(const QJsonObject& json) const;

    /**
     * @brief 保存图表图片
     * @param chart 图表信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool saveChartImage(const ChartInfo& chart, const QString& outputPath);

    /**
     * @brief 获取支持的图表类型列表
     * @return 支持的图表类型列表
     */
    virtual QStringList getSupportedChartTypes() const;

    /**
     * @brief 将图表类型转换为字符串
     * @param type 图表类型
     * @return 字符串表示
     */
    virtual QString chartTypeToString(ChartType type) const;

    /**
     * @brief 从字符串解析图表类型
     * @param typeString 字符串表示
     * @return 图表类型
     */
    virtual ChartType stringToChartType(const QString& typeString) const;

protected:
    /**
     * @brief 处理图表数据
     * @param rawData 原始图表数据
     * @param chartInfo 图表信息（输出）
     * @return 是否成功
     */
    virtual bool processChartData(const QJsonObject& rawData, ChartInfo& chartInfo);

    /**
     * @brief 生成图表ID
     * @return 图表ID
     */
    virtual QString generateChartId();

    /**
     * @brief 验证图表数据
     * @param chart 图表信息
     * @return 验证错误信息，空字符串表示验证通过
     */
    virtual QString validateChart(const ChartInfo& chart) const;

    /**
     * @brief 检测图表类型
     * @param properties 图表属性
     * @return 图表类型
     */
    virtual ChartType detectChartType(const QJsonObject& properties) const;

private:
    QStringList m_supportedTypes; // 支持的图表类型列表
};

#endif // CHARTEXTRACTOR_H
