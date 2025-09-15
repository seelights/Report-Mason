/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 表格提取器抽象基类实现
 * @FilePath: \ReportMason\tools\base\TableExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "TableExtractor.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QStringConverter>
#include <QDebug>

TableExtractor::TableExtractor(QObject* parent) : ContentExtractor(parent) {}

TableExtractor::~TableExtractor() {}

bool TableExtractor::exportToCsv(const TableInfo& table, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString("无法创建CSV文件: %1").arg(outputPath));
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
        stream << rowData.join(",") << "\n";
    }

    file.close();
    return true;
}

bool TableExtractor::exportToHtml(const TableInfo& table, const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString("无法创建HTML文件: %1").arg(outputPath));
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    stream << "<!DOCTYPE html>\n";
    stream << "<html><head><meta charset=\"UTF-8\"></head><body>\n";
    stream << "<table border=\"1\">\n";

    for (int row = 0; row < table.rows; ++row) {
        stream << "  <tr>\n";
        for (int col = 0; col < table.columns; ++col) {
            QString content;
            if (row < table.cells.size() && col < table.cells[row].size()) {
                content = table.cells[row][col].content;
            }
            stream << "    <td>" << content << "</td>\n";
        }
        stream << "  </tr>\n";
    }

    stream << "</table>\n";
    stream << "</body></html>\n";

    file.close();
    return true;
}

bool TableExtractor::exportToJson(const TableInfo& table, const QString& outputPath)
{
    QJsonObject json = tableToJson(table);

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(QString("无法创建JSON文件: %1").arg(outputPath));
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
    json["id"] = table.id;
    json["rows"] = table.rows;
    json["columns"] = table.columns;
    json["title"] = table.title;
    json["caption"] = table.caption;
    json["properties"] = table.properties;

    QJsonArray cellsArray;
    for (int row = 0; row < table.rows; ++row) {
        QJsonArray rowArray;
        for (int col = 0; col < table.columns; ++col) {
            QJsonObject cellObj;
            if (row < table.cells.size() && col < table.cells[row].size()) {
                const CellInfo& cell = table.cells[row][col];
                cellObj["row"] = cell.row;
                cellObj["column"] = cell.column;
                cellObj["content"] = cell.content;
                cellObj["rowSpan"] = cell.rowSpan;
                cellObj["colSpan"] = cell.colSpan;
                cellObj["properties"] = cell.properties;
            }
            rowArray.append(cellObj);
        }
        cellsArray.append(rowArray);
    }
    json["cells"] = cellsArray;

    return json;
}

TableInfo TableExtractor::tableFromJson(const QJsonObject& json) const
{
    TableInfo table;
    table.id = json["id"].toString();
    table.rows = json["rows"].toInt();
    table.columns = json["columns"].toInt();
    table.title = json["title"].toString();
    table.caption = json["caption"].toString();
    table.properties = json["properties"].toObject();

    table.cells.resize(table.rows);
    for (int i = 0; i < table.rows; ++i) {
        table.cells[i].resize(table.columns);
    }

    QJsonArray cellsArray = json["cells"].toArray();
    for (int row = 0; row < cellsArray.size() && row < table.rows; ++row) {
        QJsonArray rowArray = cellsArray[row].toArray();
        for (int col = 0; col < rowArray.size() && col < table.columns; ++col) {
            QJsonObject cellObj = rowArray[col].toObject();
            CellInfo& cell = table.cells[row][col];
            cell.row = cellObj["row"].toInt();
            cell.column = cellObj["column"].toInt();
            cell.content = cellObj["content"].toString();
            cell.rowSpan = cellObj["rowSpan"].toInt();
            cell.colSpan = cellObj["colSpan"].toInt();
            cell.properties = cellObj["properties"].toObject();
        }
    }

    return table;
}

QString TableExtractor::validateTable(const TableInfo& table) const
{
    if (table.rows <= 0 || table.columns <= 0) {
        return "表格行数或列数无效";
    }

    if (table.cells.size() != table.rows) {
        return "表格行数与单元格数据不匹配";
    }

    for (int row = 0; row < table.rows; ++row) {
        if (table.cells[row].size() != table.columns) {
            return QString("第%1行列数与表格列数不匹配").arg(row + 1);
        }
    }

    return QString(); // 验证通过
}

bool TableExtractor::processTableData(const QJsonObject& rawData, TableInfo& tableInfo)
{
    // 基本实现，子类可以重写
    tableInfo.id = generateTableId();
    tableInfo.rows = rawData["rows"].toInt();
    tableInfo.columns = rawData["columns"].toInt();
    tableInfo.title = rawData["title"].toString();
    tableInfo.caption = rawData["caption"].toString();
    tableInfo.properties = rawData["properties"].toObject();

    return true;
}

QString TableExtractor::generateTableId() { return generateUniqueId("table"); }

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
    if (escaped.contains(',') || escaped.contains('"') || escaped.contains('\n')) {
        // 转义引号
        escaped.replace('"', "\"\"");
        // 用引号包围
        escaped = '"' + escaped + '"';
    }

    return escaped;
}
