/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: DOCX表格提取器
 * @FilePath: \ReportMason\tools\docx\DocxTableExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#ifndef DOCXTABLEEXTRACTOR_H
#define DOCXTABLEEXTRACTOR_H

#include "../base/TableExtractor.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/**
 * @brief DOCX表格提取器
 * 
 * 专门用于从DOCX文件中提取表格内容
 */
class DocxTableExtractor : public TableExtractor
{
    Q_OBJECT

public:
    explicit DocxTableExtractor(QObject *parent = nullptr);
    virtual ~DocxTableExtractor();

    // 实现基类接口
    bool isSupported(const QString &filePath) const override;
    QStringList getSupportedFormats() const override;
    ExtractStatus extractTables(const QString &filePath, QList<TableInfo> &tables) override;
    ExtractStatus extractTablesByPosition(const QString &filePath, const QRect &position, QList<TableInfo> &tables) override;
    int getTableCount(const QString &filePath) override;

protected:
    /**
     * @brief 从ZIP文件中读取文件内容
     * @param zipPath ZIP文件路径
     * @param internalPath 内部文件路径
     * @return 文件内容
     */
    QByteArray readFileFromZip(const QString &zipPath, const QString &internalPath) const;

    /**
     * @brief 解析document.xml中的表格信息
     * @param xmlContent document.xml内容
     * @param tables 输出表格信息列表
     * @return 是否成功
     */
    bool parseDocumentXml(const QByteArray &xmlContent, QList<TableInfo> &tables) const;

    /**
     * @brief 解析tbl元素
     * @param reader XML读取器
     * @param tables 输出表格信息列表
     * @return 是否成功
     */
    bool parseTableElement(QXmlStreamReader &reader, QList<TableInfo> &tables) const;

    /**
     * @brief 解析tr元素（表格行）
     * @param reader XML读取器
     * @param table 表格信息（输出）
     * @return 是否成功
     */
    bool parseTableRow(QXmlStreamReader &reader, TableInfo &table) const;

    /**
     * @brief 解析tc元素（表格单元格）
     * @param reader XML读取器
     * @param cell 单元格信息（输出）
     * @return 是否成功
     */
    bool parseTableCell(QXmlStreamReader &reader, CellInfo &cell) const;

    /**
     * @brief 解析单元格内容
     * @param reader XML读取器
     * @param content 内容（输出）
     * @return 是否成功
     */
    bool parseCellContent(QXmlStreamReader &reader, QString &content) const;

    /**
     * @brief 获取表格属性
     * @param reader XML读取器
     * @param properties 属性（输出）
     * @return 是否成功
     */
    bool getTableProperties(QXmlStreamReader &reader, QJsonObject &properties) const;

    /**
     * @brief 获取单元格属性
     * @param reader XML读取器
     * @param properties 属性（输出）
     * @return 是否成功
     */
    bool getCellProperties(QXmlStreamReader &reader, QJsonObject &properties) const;

    /**
     * @brief 获取表格在文档中的位置
     * @param reader XML读取器
     * @param position 位置（输出）
     * @return 是否成功
     */
    bool getTablePosition(QXmlStreamReader &reader, QRect &position) const;

    /**
     * @brief 计算表格尺寸
     * @param table 表格信息
     * @return 表格尺寸
     */
    QSize calculateTableSize(const TableInfo &table) const;

private:
    static const QStringList SUPPORTED_EXTENSIONS;
    static const QString DOCX_DOCUMENT_PATH;
};

#endif // DOCXTABLEEXTRACTOR_H
