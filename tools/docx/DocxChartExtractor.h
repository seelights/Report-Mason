/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: DOCX图表提取器
 * @FilePath: \ReportMason\tools\docx\DocxChartExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include "../base/ChartExtractor.h"
#include "../../src/KZipUtils.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/**
 * @brief DOCX图表提取器
 * 
 * 专门用于从DOCX文件中提取图表内容
 */
class DocxChartExtractor : public ChartExtractor
{
    Q_OBJECT

public:
    explicit DocxChartExtractor(QObject *parent = nullptr);
    virtual ~DocxChartExtractor();

    // 实现基类接口
    bool isSupported(const QString &filePath) const override;
    QStringList getSupportedFormats() const override;
    ExtractStatus extractCharts(const QString &filePath, QList<ChartInfo> &charts) override;
    ExtractStatus extractChartsByType(const QString &filePath, ChartType chartType, QList<ChartInfo> &charts) override;
    ExtractStatus extractChartsByPosition(const QString &filePath, const QRect &position, QList<ChartInfo> &charts) override;
    int getChartCount(const QString &filePath) override;

protected:
    /**
     * @brief 从ZIP文件中读取文件内容
     * @param zipPath ZIP文件路径
     * @param internalPath 内部文件路径
     * @return 文件内容
     */
    QByteArray readFileFromZip(const QString &zipPath, const QString &internalPath) const;

    /**
     * @brief 解析document.xml中的图表信息
     * @param xmlContent document.xml内容
     * @param charts 输出图表信息列表
     * @return 是否成功
     */
    bool parseDocumentXml(const QByteArray &xmlContent, QList<ChartInfo> &charts) const;

    /**
     * @brief 解析chart元素
     * @param reader XML读取器
     * @param charts 输出图表信息列表
     * @return 是否成功
     */
    bool parseChartElement(QXmlStreamReader &reader, QList<ChartInfo> &charts) const;

    /**
     * @brief 解析图表数据
     * @param zipPath ZIP文件路径
     * @param chartPath 图表文件路径
     * @param chart 图表信息（输出）
     * @return 是否成功
     */
    bool parseChartData(const QString &zipPath, const QString &chartPath, ChartInfo &chart) const;

    /**
     * @brief 解析Excel图表文件
     * @param chartData 图表数据
     * @param chart 图表信息（输出）
     * @return 是否成功
     */
    bool parseExcelChart(const QByteArray &chartData, ChartInfo &chart) const;

    /**
     * @brief 获取图表类型
     * @param chartData 图表数据
     * @return 图表类型
     */
    ChartType detectChartType(const QByteArray &chartData) const;

    /**
     * @brief 获取图表标题
     * @param chartData 图表数据
     * @return 图表标题
     */
    QString getChartTitle(const QByteArray &chartData) const;

    /**
     * @brief 获取图表数据系列
     * @param chartData 图表数据
     * @param series 数据系列（输出）
     * @return 是否成功
     */
    bool getChartSeries(const QByteArray &chartData, QList<DataSeries> &series) const;

    /**
     * @brief 获取图表在文档中的位置
     * @param reader XML读取器
     * @param position 位置（输出）
     * @return 是否成功
     */
    bool getChartPosition(QXmlStreamReader &reader, QRect &position) const;

    /**
     * @brief 获取图表尺寸
     * @param reader XML读取器
     * @param size 尺寸（输出）
     * @return 是否成功
     */
    bool getChartSize(QXmlStreamReader &reader, QSize &size) const;

    /**
     * @brief 从关系文件中获取图表信息
     * @param zipPath ZIP文件路径
     * @param relationshipId 关系ID
     * @param chartPath 图表路径（输出）
     * @return 是否成功
     */
    bool getChartFromRelationship(const QString &zipPath, const QString &relationshipId, QString &chartPath) const;

private:
    static const QStringList SUPPORTED_EXTENSIONS;
    static const QString DOCX_DOCUMENT_PATH;
    static const QString DOCX_RELATIONSHIPS_PATH;
    static const QString DOCX_CHARTS_PATH;
};

