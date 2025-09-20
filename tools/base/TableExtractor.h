/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 表格提取器抽象基类
 * @FilePath: \ReportMason\tools\base\TableExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include "ContentExtractor.h"
#include <QSize>
#include <QRect>
#include <QJsonObject>

/**
 * @brief 单元格信息结构
 */
struct CellInfo
{
    int row;                // 行号
    int column;             // 列号
    QString content;        // 单元格内容
    QJsonObject properties; // 单元格属性（格式、样式等）
    int rowSpan;            // 行跨度
    int colSpan;            // 列跨度

    CellInfo() : row(0), column(0), rowSpan(1), colSpan(1) {}

    CellInfo(int r, int c, const QString& text)
        : row(r), column(c), content(text), rowSpan(1), colSpan(1)
    {
    }
};

/**
 * @brief 表格信息结构
 */
struct TableInfo
{
    QString id;                   // 表格唯一ID
    int rows;                     // 行数
    int columns;                  // 列数
    QList<QList<CellInfo>> cells; // 单元格数据
    QJsonObject properties;       // 表格属性（样式、边框等）
    QString title;                // 表格标题
    QString caption;              // 表格说明
    QRect position;               // 在文档中的位置

    TableInfo() : rows(0), columns(0) {}

    TableInfo(int r, int c) : rows(r), columns(c)
    {
        cells.resize(r);
        for (int i = 0; i < r; ++i) {
            cells[i].resize(c);
        }
    }
};

/**
 * @brief 表格提取器抽象基类
 *
 * 定义了表格提取的通用接口
 */
class TableExtractor : public ContentExtractor
{
    Q_OBJECT

public:
    explicit TableExtractor(QObject* parent = nullptr);
    virtual ~TableExtractor();

    /**
     * @brief 从文档中提取所有表格
     * @param filePath 文档路径
     * @param tables 输出表格信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractTables(const QString& filePath, QList<TableInfo>& tables) = 0;

    /**
     * @brief 从文档中提取指定位置的表格
     * @param filePath 文档路径
     * @param position 位置范围
     * @param tables 输出表格信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractTablesByPosition(const QString& filePath, const QRect& position,
                                                  QList<TableInfo>& tables) = 0;

    /**
     * @brief 获取表格数量
     * @param filePath 文档路径
     * @return 表格数量，-1表示错误
     */
    virtual int getTableCount(const QString& filePath) = 0;

    /**
     * @brief 将表格导出为CSV格式
     * @param table 表格信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToCsv(const TableInfo& table, const QString& outputPath);

    /**
     * @brief 将表格导出为HTML格式
     * @param table 表格信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToHtml(const TableInfo& table, const QString& outputPath);

    /**
     * @brief 将表格导出为JSON格式
     * @param table 表格信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToJson(const TableInfo& table, const QString& outputPath);

    /**
     * @brief 将表格导出为XML格式
     * @param table 表格信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToXml(const TableInfo& table, const QString& outputPath);

    /**
     * @brief 将表格列表导出为XML格式
     * @param tables 表格列表
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool exportToXml(const QList<TableInfo>& tables, const QString& outputPath);

    /**
     * @brief 将表格导出为XML格式（返回字节数组）
     * @param table 表格信息
     * @return XML字节数组
     */
    virtual QByteArray exportToXmlByteArray(const TableInfo& table);

    /**
     * @brief 将表格列表导出为XML格式（返回字节数组）
     * @param tables 表格列表
     * @return XML字节数组
     */
    virtual QByteArray exportToXmlByteArray(const QList<TableInfo>& tables);

    /**
     * @brief 将表格转换为JSON对象
     * @param table 表格信息
     * @return JSON对象
     */
    virtual QJsonObject tableToJson(const TableInfo& table) const;

    /**
     * @brief 从JSON对象创建表格信息
     * @param json JSON对象
     * @return 表格信息
     */
    virtual TableInfo tableFromJson(const QJsonObject& json) const;

    /**
     * @brief 验证表格数据
     * @param table 表格信息
     * @return 验证错误信息，空字符串表示验证通过
     */
    virtual QString validateTable(const TableInfo& table) const;

protected:
    /**
     * @brief 处理表格数据
     * @param rawData 原始表格数据
     * @param tableInfo 表格信息（输出）
     * @return 是否成功
     */
    virtual bool processTableData(const QJsonObject& rawData, TableInfo& tableInfo);

    /**
     * @brief 生成表格ID
     * @return 表格ID
     */
    virtual QString generateTableId();

    /**
     * @brief 清理单元格内容
     * @param content 原始内容
     * @return 清理后的内容
     */
    virtual QString cleanCellContent(const QString& content) const;

    /**
     * @brief 检测表格边界
     * @param cells 单元格列表
     * @param rows 行数（输出）
     * @param columns 列数（输出）
     * @return 是否成功
     */
    virtual bool detectTableBounds(const QList<CellInfo>& cells, int& rows, int& columns) const;

private:
    /**
     * @brief 转义CSV内容
     * @param content 原始内容
     * @return 转义后的内容
     */
    QString escapeCsvContent(const QString& content) const;
};

