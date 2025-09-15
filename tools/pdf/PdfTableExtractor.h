/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: PDF表格提取器
 * @FilePath: \ReportMason\tools\pdf\PdfTableExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#ifndef PDFTABLEEXTRACTOR_H
#define PDFTABLEEXTRACTOR_H

#include "../base/TableExtractor.h"

/**
 * @brief PDF表格提取器
 * 
 * 专门用于从PDF文件中提取表格内容
 */
class PdfTableExtractor : public TableExtractor
{
    Q_OBJECT

public:
    explicit PdfTableExtractor(QObject *parent = nullptr);
    virtual ~PdfTableExtractor();

    // 实现基类接口
    bool isSupported(const QString &filePath) const override;
    QStringList getSupportedFormats() const override;
    ExtractStatus extractTables(const QString &filePath, QList<TableInfo> &tables) override;
    ExtractStatus extractTablesByPosition(const QString &filePath, const QRect &position, QList<TableInfo> &tables) override;
    int getTableCount(const QString &filePath) override;

protected:
    /**
     * @brief 解析PDF文件
     * @param filePath PDF文件路径
     * @param tables 输出表格信息列表
     * @return 是否成功
     */
    bool parsePdfFile(const QString &filePath, QList<TableInfo> &tables) const;

    /**
     * @brief 提取页面中的表格
     * @param pageContent 页面内容
     * @param pageNumber 页码
     * @param tables 输出表格信息列表
     * @return 是否成功
     */
    bool extractTablesFromPage(const QByteArray &pageContent, int pageNumber, QList<TableInfo> &tables) const;

    /**
     * @brief 检测表格边界
     * @param pageContent 页面内容
     * @param tableRegions 表格区域列表（输出）
     * @return 是否成功
     */
    bool detectTableBoundaries(const QByteArray &pageContent, QList<QRect> &tableRegions) const;

    /**
     * @brief 解析表格内容
     * @param pageContent 页面内容
     * @param tableRegion 表格区域
     * @param table 表格信息（输出）
     * @return 是否成功
     */
    bool parseTableContent(const QByteArray &pageContent, const QRect &tableRegion, TableInfo &table) const;

    /**
     * @brief 识别表格单元格
     * @param pageContent 页面内容
     * @param tableRegion 表格区域
     * @param cells 单元格列表（输出）
     * @return 是否成功
     */
    bool identifyTableCells(const QByteArray &pageContent, const QRect &tableRegion, QList<CellInfo> &cells) const;

    /**
     * @brief 获取表格在页面中的位置
     * @param pageContent 页面内容
     * @param tableIndex 表格索引
     * @param position 位置（输出）
     * @return 是否成功
     */
    bool getTablePosition(const QByteArray &pageContent, int tableIndex, QRect &position) const;

    /**
     * @brief 验证表格结构
     * @param table 表格信息
     * @return 是否有效
     */
    bool validateTableStructure(const TableInfo &table) const;

private:
    static const QStringList SUPPORTED_EXTENSIONS;
};

#endif // PDFTABLEEXTRACTOR_H
