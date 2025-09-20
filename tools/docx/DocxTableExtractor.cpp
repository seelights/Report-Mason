/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: DOCX表格提取器实现
 * @FilePath: \ReportMason\tools\docx\DocxTableExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "QtCompat.h"
#include "DocxTableExtractor.h"
#include "../utils/ContentUtils.h"
#include <QFileInfo>
#include <QDebug>

// 静态常量定义
const QStringList DocxTableExtractor::SUPPORTED_EXTENSIONS = {QS("docx")};
const QString DocxTableExtractor::DOCX_DOCUMENT_PATH = QS("word/document.xml");

DocxTableExtractor::DocxTableExtractor(QObject* parent) : TableExtractor(parent) {}

DocxTableExtractor::~DocxTableExtractor() {}

bool DocxTableExtractor::isSupported(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    return SUPPORTED_EXTENSIONS.contains(fileInfo.suffix().toLower());
}

QStringList DocxTableExtractor::getSupportedFormats() const { return SUPPORTED_EXTENSIONS; }

TableExtractor::ExtractStatus DocxTableExtractor::extractTables(const QString& filePath,
                                                                QList<TableInfo>& tables)
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
    if (!parseDocumentXml(xmlContent, tables)) {
        setLastError(QS("解析DOCX文档XML失败"));
        return ExtractStatus::PARSE_ERROR;
    }

    qDebug() << "DocxTableExtractor: 成功提取" << tables.size() << "个表格";
    return ExtractStatus::SUCCESS;
}

TableExtractor::ExtractStatus DocxTableExtractor::extractTablesByPosition(const QString& filePath,
                                                                          const QRect& position,
                                                                          QList<TableInfo>& tables)
{
    QList<TableInfo> allTables;
    ExtractStatus status = extractTables(filePath, allTables);
    if (status != ExtractStatus::SUCCESS) {
        return status;
    }

    // 过滤指定位置的表格
    for (const TableInfo& table : allTables) {
        if (position.intersects(table.position)) {
            tables.append(table);
        }
    }

    return ExtractStatus::SUCCESS;
}

int DocxTableExtractor::getTableCount(const QString& filePath)
{
    QList<TableInfo> tables;
    ExtractStatus status = extractTables(filePath, tables);
    if (status != ExtractStatus::SUCCESS) {
        return -1;
    }
    return tables.size();
}

QByteArray DocxTableExtractor::readFileFromZip(const QString& zipPath,
                                               const QString& internalPath) const
{
    // 这里需要使用KZipUtils来读取ZIP文件
    // 暂时返回空数据，实际实现需要集成KZipUtils
    Q_UNUSED(zipPath)
    Q_UNUSED(internalPath)

    qDebug() << "DocxTableExtractor: 需要集成KZipUtils来读取ZIP文件";
    return QByteArray();
}

bool DocxTableExtractor::parseDocumentXml(const QByteArray& xmlContent, QList<TableInfo>& tables)
{
    QXmlStreamReader reader(xmlContent);

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();

            if (elementName == QS("tbl")) {
                parseTableElement(reader, tables);
            }
        }
    }

    return !reader.hasError();
}

bool DocxTableExtractor::parseTableElement(QXmlStreamReader& reader, QList<TableInfo>& tables)
{
    TableInfo table;
    table.id = generateUniqueId(QS("table"));
    table.rows = 0;
    table.columns = 0;

    // 解析表格行
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();

            if (elementName == QS("tr")) {
                // 解析表格行
                if (parseTableRow(reader, table)) {
                    table.rows++;
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            QString elementName = reader.name().toString();
            if (elementName == QS("tbl")) {
                break;
            }
        }
    }

    // 计算列数
    if (!table.cells.isEmpty()) {
        table.columns = 0;
        for (const QList<CellInfo>& row : table.cells) {
            table.columns = qMax(table.columns, row.size());
        }
    }

    if (table.rows > 0 && table.columns > 0) {
        tables.append(table);
        qDebug() << "DocxTableExtractor: 解析到表格" << table.rows << "行" << table.columns << "列";
        return true;
    }

    return false;
}

bool DocxTableExtractor::parseTableRow(QXmlStreamReader& reader, TableInfo& table)
{
    QList<CellInfo> row;
    int currentRow = table.rows;
    int currentCol = 0;

    // 解析表格单元格
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();

            if (elementName == QS("tc")) {
                // 解析表格单元格
                CellInfo cell;
                cell.row = currentRow;
                cell.column = currentCol;
                cell.content = parseTableCell(reader);
                row.append(cell);
                currentCol++;
            }
        } else if (token == QXmlStreamReader::EndElement) {
            QString elementName = reader.name().toString();
            if (elementName == QS("tr")) {
                break;
            }
        }
    }

    if (!row.isEmpty()) {
        table.cells.append(row);
        return true;
    }

    return false;
}

bool DocxTableExtractor::parseTableCell(QXmlStreamReader& reader, CellInfo& cell) const
{
    Q_UNUSED(reader)
    Q_UNUSED(cell)

    // 解析tc元素（表格单元格）
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxTableExtractor: 解析tc元素";
    return true;
}

QString DocxTableExtractor::parseTableCell(QXmlStreamReader& reader) const
{
    QString content;

    // 解析单元格内容
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();

            if (elementName == QS("t")) {
                // 读取文本内容
                QString text = reader.readElementText();
                content += text;
            } else if (elementName == QS("p")) {
                // 段落，递归解析
                QString paragraphContent;
                if (parseCellContent(reader, paragraphContent)) {
                    if (!content.isEmpty()) {
                        content += QS("\n");
                    }
                    content += paragraphContent;
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            QString elementName = reader.name().toString();
            if (elementName == QS("tc")) {
                break;
            }
        }
    }

    return content.trimmed();
}

bool DocxTableExtractor::parseCellContent(QXmlStreamReader& reader, QString& content) const
{
    QString cellContent = parseTableCell(reader);
    content = cellContent;
    return !cellContent.isEmpty();
}

bool DocxTableExtractor::getTableProperties(QXmlStreamReader& reader, QJsonObject& properties) const
{
    Q_UNUSED(reader)
    Q_UNUSED(properties)

    // 获取表格属性
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxTableExtractor: 获取表格属性";
    return true;
}

bool DocxTableExtractor::getCellProperties(QXmlStreamReader& reader, QJsonObject& properties) const
{
    Q_UNUSED(reader)
    Q_UNUSED(properties)

    // 获取单元格属性
    // 这里需要实现具体的解析逻辑
    qDebug() << "DocxTableExtractor: 获取单元格属性";
    return true;
}

bool DocxTableExtractor::getTablePosition(QXmlStreamReader& reader, QRect& position) const
{
    position = QRect(0, 0, 0, 0);
    bool hasPosition = false;
    bool hasSize = false;
    
    // 保存当前位置
    qint64 startPos = reader.characterOffset();
    
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();
            QXmlStreamAttributes attributes = reader.attributes();
            
            if (elementName == QS("tbl")) {
                // 表格元素，解析其属性
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
            } else if (elementName == QS("tblPr")) {
                // 表格属性，可能包含位置信息
                QString style = attributes.value(QS("style")).toString();
                if (!style.isEmpty()) {
                    // 从样式中提取位置信息（简化实现）
                    // 实际实现需要解析样式定义
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            QString elementName = reader.name().toString();
            if (elementName == QS("tbl")) {
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
        position.setWidth(400);  // 默认宽度
        position.setHeight(200); // 默认高度
    }
    
    return true;
}

QSize DocxTableExtractor::calculateTableSize(const TableInfo& table) const
{
    Q_UNUSED(table)

    // 计算表格尺寸
    // 这里需要实现具体的计算逻辑
    qDebug() << "DocxTableExtractor: 计算表格尺寸";
    return QSize();
}
