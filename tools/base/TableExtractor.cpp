/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 表格提取器抽象基类实现
 * @FilePath: \ReportMason\tools\base\TableExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "QtCompat.h"
#include "TableExtractor.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QStringConverter>
#include <QDebug>
#include <QXmlStreamWriter>

TableExtractor::TableExtractor(QObject* parent) : ContentExtractor(parent) {}

TableExtractor::~TableExtractor() {}

bool TableExtractor::exportToCsv(const TableInfo& table, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString(QS("无法创建CSV文件: %1")).arg(outputPath));
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    for (int row = 0; row < table.rows; ++row) {
        QStringList rowData;
        for (int col = 0; col < table.columns; ++col) {
            QString content;
            if (row < table.cells.size() && col < table.cells[row].size()) {
                content = escapeCsvContent(table.cells[row][col].content);
            }
            rowData << content;
        }
        stream << rowData.join(QS(",")) << QS("\n");
    }

    file.close();
    return true;
}

bool TableExtractor::exportToHtml(const TableInfo& table, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString(QS("无法创建HTML文件: %1")).arg(outputPath));
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    stream << QS("<!DOCTYPE html>\n");
    stream << QS("<html><head><meta charset=\"UTF-8\"></head><body>\n");
    stream << QS("<table border=\"1\">\n");

    for (int row = 0; row < table.rows; ++row) {
        stream << QS("  <tr>\n");
        for (int col = 0; col < table.columns; ++col) {
            QString content;
            if (row < table.cells.size() && col < table.cells[row].size()) {
                content = table.cells[row][col].content;
            }
            stream << QS("    <td>") << content << QS("</td>\n");
        }
        stream << QS("  </tr>\n");
    }

    stream << QS("</table>\n");
    stream << QS("</body></html>\n");

    file.close();
    return true;
}

bool TableExtractor::exportToJson(const TableInfo& table, const QString& outputPath)
{
    QJsonObject json = tableToJson(table);

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

QJsonObject TableExtractor::tableToJson(const TableInfo& table) const
{
    QJsonObject json;
    json[QS("id")] = table.id;
    json[QS("rows")] = table.rows;
    json[QS("columns")] = table.columns;
    json[QS("title")] = table.title;
    json[QS("caption")] = table.caption;
    json[QS("properties")] = table.properties;

    QJsonArray cellsArray;
    for (int row = 0; row < table.rows; ++row) {
        QJsonArray rowArray;
        for (int col = 0; col < table.columns; ++col) {
            QJsonObject cellObj;
            if (row < table.cells.size() && col < table.cells[row].size()) {
                const CellInfo& cell = table.cells[row][col];
                cellObj[QS("row")] = cell.row;
                cellObj[QS("column")] = cell.column;
                cellObj[QS("content")] = cell.content;
                cellObj[QS("rowSpan")] = cell.rowSpan;
                cellObj[QS("colSpan")] = cell.colSpan;
                cellObj[QS("properties")] = cell.properties;
            }
            rowArray.append(cellObj);
        }
        cellsArray.append(rowArray);
    }
    json[QS("cells")] = cellsArray;

    return json;
}

TableInfo TableExtractor::tableFromJson(const QJsonObject& json) const
{
    TableInfo table;
    table.id = json[QS("id")].toString();
    table.rows = json[QS("rows")].toInt();
    table.columns = json[QS("columns")].toInt();
    table.title = json[QS("title")].toString();
    table.caption = json[QS("caption")].toString();
    table.properties = json[QS("properties")].toObject();

    table.cells.resize(table.rows);
    for (int i = 0; i < table.rows; ++i) {
        table.cells[i].resize(table.columns);
    }

    QJsonArray cellsArray = json[QS("cells")].toArray();
    for (int row = 0; row < cellsArray.size() && row < table.rows; ++row) {
        QJsonArray rowArray = cellsArray[row].toArray();
        for (int col = 0; col < rowArray.size() && col < table.columns; ++col) {
            QJsonObject cellObj = rowArray[col].toObject();
            CellInfo& cell = table.cells[row][col];
            cell.row = cellObj[QS("row")].toInt();
            cell.column = cellObj[QS("column")].toInt();
            cell.content = cellObj[QS("content")].toString();
            cell.rowSpan = cellObj[QS("rowSpan")].toInt();
            cell.colSpan = cellObj[QS("colSpan")].toInt();
            cell.properties = cellObj[QS("properties")].toObject();
        }
    }

    return table;
}

QString TableExtractor::validateTable(const TableInfo& table) const
{
    if (table.rows <= 0 || table.columns <= 0) {
        return QS("表格行数或列数无效");
    }

    if (table.cells.size() != table.rows) {
        return QS("表格行数与单元格数据不匹配");
    }

    for (int row = 0; row < table.rows; ++row) {
        if (table.cells[row].size() != table.columns) {
            return QS("第%1行列数与表格列数不匹配").arg(row + 1);
        }
    }

    return QString(); // 验证通过
}

bool TableExtractor::processTableData(const QJsonObject& rawData, TableInfo& tableInfo)
{
    // 基本实现，子类可以重写
    tableInfo.id = generateTableId();
    tableInfo.rows = rawData[QS("rows")].toInt();
    tableInfo.columns = rawData[QS("columns")].toInt();
    tableInfo.title = rawData[QS("title")].toString();
    tableInfo.caption = rawData[QS("caption")].toString();
    tableInfo.properties = rawData[QS("properties")].toObject();

    return true;
}

QString TableExtractor::generateTableId() { return generateUniqueId(QS("table")); }

QString TableExtractor::cleanCellContent(const QString& content) const
{
    return cleanText(content);
}

bool TableExtractor::detectTableBounds(const QList<CellInfo>& cells, int& rows, int& columns) const
{
    if (cells.isEmpty()) {
        rows = 0;
        columns = 0;
        return false;
    }

    int maxRow = 0;
    int maxCol = 0;

    for (const CellInfo& cell : cells) {
        maxRow = qMax(maxRow, cell.row);
        maxCol = qMax(maxCol, cell.column);
    }

    rows = maxRow + 1;
    columns = maxCol + 1;
    return true;
}

QString TableExtractor::escapeCsvContent(const QString& content) const
{
    QString escaped = content;

    // 如果内容包含逗号、引号或换行符，需要用引号包围
    if (escaped.contains(QS(",")) || escaped.contains(QS("\"")) || escaped.contains(QS("\n"))) {
        // 转义引号
        escaped.replace(QS("\""), QS("\"\""));
        // 用引号包围
        escaped = QS("\"") + escaped + QS("\"");
    }

    return escaped;
}

bool TableExtractor::exportToXml(const TableInfo& table, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QS("无法创建XML文件: %1").arg(outputPath));
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML声明
    writer.writeStartDocument("1.0", true);

    // 写入根元素
    writer.writeStartElement("Table");
    writer.writeAttribute("id", table.id);
    writer.writeAttribute("rows", QString::number(table.rows));
    writer.writeAttribute("columns", QString::number(table.columns));

    // 写入表格数据
    writer.writeStartElement("Data");
    for (int row = 0; row < table.rows; ++row) {
        writer.writeStartElement("Row");
        writer.writeAttribute("index", QString::number(row));

        for (int col = 0; col < table.columns; ++col) {
            writer.writeStartElement("Cell");
            writer.writeAttribute("row", QString::number(row));
            writer.writeAttribute("column", QString::number(col));

            if (row < table.cells.size() && col < table.cells[row].size()) {
                const CellInfo& cell = table.cells[row][col];
                writer.writeCharacters(cell.content);
            }

            writer.writeEndElement(); // Cell
        }

        writer.writeEndElement(); // Row
    }
    writer.writeEndElement(); // Data

    // 写入属性
    if (!table.properties.isEmpty()) {
        writer.writeStartElement("Properties");
        for (auto it = table.properties.begin(); it != table.properties.end(); ++it) {
            writer.writeStartElement("Property");
            writer.writeAttribute("name", it.key());
            writer.writeCharacters(it.value().toString());
            writer.writeEndElement(); // Property
        }
        writer.writeEndElement(); // Properties
    }

    writer.writeEndElement(); // Table
    writer.writeEndDocument();

    file.close();
    return true;
}

bool TableExtractor::exportToXml(const QList<TableInfo>& tables, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QS("无法创建XML文件: %1").arg(outputPath));
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML声明
    writer.writeStartDocument("1.0", true);

    // 写入根元素
    writer.writeStartElement("Tables");
    writer.writeAttribute("count", QString::number(tables.size()));

    // 写入每个表格
    for (const TableInfo& table : tables) {
        writer.writeStartElement("Table");
        writer.writeAttribute("id", table.id);
        writer.writeAttribute("rows", QString::number(table.rows));
        writer.writeAttribute("columns", QString::number(table.columns));

        // 写入表格数据
        writer.writeStartElement("Data");
        for (int row = 0; row < table.rows; ++row) {
            writer.writeStartElement("Row");
            writer.writeAttribute("index", QString::number(row));

            for (int col = 0; col < table.columns; ++col) {
                writer.writeStartElement("Cell");
                writer.writeAttribute("row", QString::number(row));
                writer.writeAttribute("column", QString::number(col));

                if (row < table.cells.size() && col < table.cells[row].size()) {
                    const CellInfo& cell = table.cells[row][col];
                    writer.writeCharacters(cell.content);
                }

                writer.writeEndElement(); // Cell
            }

            writer.writeEndElement(); // Row
        }
        writer.writeEndElement(); // Data

        // 写入属性
        if (!table.properties.isEmpty()) {
            writer.writeStartElement("Properties");
            for (auto it = table.properties.begin(); it != table.properties.end(); ++it) {
                writer.writeStartElement("Property");
                writer.writeAttribute("name", it.key());
                writer.writeCharacters(it.value().toString());
                writer.writeEndElement(); // Property
            }
            writer.writeEndElement(); // Properties
        }

        writer.writeEndElement(); // Table
    }

    writer.writeEndElement(); // Tables
    writer.writeEndDocument();

    file.close();
    return true;
}
