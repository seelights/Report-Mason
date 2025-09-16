/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: PDF图表提取器
 * @FilePath: \ReportMason\tools\pdf\PdfChartExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#ifndef PDFCHARTEXTRACTOR_H
#define PDFCHARTEXTRACTOR_H

#include "../base/ChartExtractor.h"

/**
 * @brief PDF图表提取器
 *
 * 专门用于从PDF文件中提取图表内容
 */
class PdfChartExtractor : public ChartExtractor
{
    Q_OBJECT

public:
    explicit PdfChartExtractor(QObject* parent = nullptr);
    virtual ~PdfChartExtractor();

    // 实现基类接口
    bool isSupported(const QString& filePath) const override;
    QStringList getSupportedFormats() const override;
    ExtractStatus extractCharts(const QString& filePath, QList<ChartInfo>& charts) override;
    ExtractStatus extractChartsByType(const QString& filePath, ChartType chartType,
                                      QList<ChartInfo>& charts) override;
    ExtractStatus extractChartsByPosition(const QString& filePath, const QRect& position,
                                          QList<ChartInfo>& charts) override;
    int getChartCount(const QString& filePath) override;

protected:
    /**
     * @brief 解析PDF文件
     * @param filePath PDF文件路径
     * @param charts 输出图表信息列表
     * @return 是否成功
     */
    bool parsePdfFile(const QString& filePath, QList<ChartInfo>& charts);

    /**
     * @brief 提取页面中的图表
     * @param pageContent 页面内容
     * @param pageNumber 页码
     * @param charts 输出图表信息列表
     * @return 是否成功
     */
    bool extractChartsFromPage(const QByteArray& pageContent, int pageNumber,
                               QList<ChartInfo>& charts) const;

    /**
     * @brief 检测图表区域
     * @param pageContent 页面内容
     * @param chartRegions 图表区域列表（输出）
     * @return 是否成功
     */
    bool detectChartRegions(const QByteArray& pageContent, QList<QRect>& chartRegions) const;

    /**
     * @brief 解析图表内容
     * @param pageContent 页面内容
     * @param chartRegion 图表区域
     * @param chart 图表信息（输出）
     * @return 是否成功
     */
    bool parseChartContent(const QByteArray& pageContent, const QRect& chartRegion,
                           ChartInfo& chart) const;

    /**
     * @brief 识别图表类型
     * @param pageContent 页面内容
     * @param chartRegion 图表区域
     * @return 图表类型
     */
    ChartType identifyChartType(const QByteArray& pageContent, const QRect& chartRegion) const;

    /**
     * @brief 提取图表数据
     * @param pageContent 页面内容
     * @param chartRegion 图表区域
     * @param series 数据系列（输出）
     * @return 是否成功
     */
    bool extractChartData(const QByteArray& pageContent, const QRect& chartRegion,
                          QList<DataSeries>& series) const;

    /**
     * @brief 获取图表在页面中的位置
     * @param pageContent 页面内容
     * @param chartIndex 图表索引
     * @param position 位置（输出）
     * @return 是否成功
     */
    bool getChartPosition(const QByteArray& pageContent, int chartIndex, QRect& position) const;

    /**
     * @brief 获取图表尺寸
     * @param pageContent 页面内容
     * @param chartRegion 图表区域
     * @param size 尺寸（输出）
     * @return 是否成功
     */
    bool getChartSize(const QByteArray& pageContent, const QRect& chartRegion, QSize& size) const;

    /**
     * @brief 验证图表数据
     * @param chart 图表信息
     * @return 是否有效
     */
    bool validateChartData(const ChartInfo& chart) const;

private:
    static const QStringList SUPPORTED_EXTENSIONS;
};

#endif // PDFCHARTEXTRACTOR_H
