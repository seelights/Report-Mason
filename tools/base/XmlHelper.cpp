/*
 * @Author: seelights
 * @Date: 2025-01-XX XX:XX:XX
 * @LastEditTime: 2025-01-XX XX:XX:XX
 * @LastEditors: seelights
 * @Description: XML生成辅助类实现
 * @FilePath: \ReportMason\tools\base\XmlHelper.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "XmlHelper.h"
#include "ChartExtractor.h"
#include "ImageExtractor.h"
#include "QtCompat.h"
#include "TableExtractor.h"
#include <QDebug>

QByteArray XmlHelper::generateObjectXml(
    const QString& rootElement,
    const QMap<QString, QString>& attributes,
    std::function<void(QXmlStreamWriter&)> contentCallback)
{
    QByteArray result;
    QBuffer buffer(&result);
    buffer.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&buffer);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML声明
    writer.writeStartDocument(QS("1.0"), true);

    // 写入根元素
    writer.writeStartElement(rootElement);
    writeAttributes(writer, attributes);

    // 写入内容
    if (contentCallback) {
        contentCallback(writer);
    }

    writer.writeEndElement(); // rootElement
    writer.writeEndDocument();

    return result;
}

template<typename T>
QByteArray XmlHelper::generateListXml(
    const QString& rootElement,
    const QString& countAttribute,
    const QString& itemElement,
    const QList<T>& items,
    std::function<void(QXmlStreamWriter&, const T&)> itemCallback)
{
    QByteArray result;
    QBuffer buffer(&result);
    buffer.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&buffer);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // 写入XML声明
    writer.writeStartDocument(QS("1.0"), true);

    // 写入根元素
    writer.writeStartElement(rootElement);
    writer.writeAttribute(countAttribute, QString::number(items.size()));

    // 写入每个项目
    for (const T& item : items) {
        writer.writeStartElement(itemElement);
        if (itemCallback) {
            itemCallback(writer, item);
        }
        writer.writeEndElement(); // itemElement
    }

    writer.writeEndElement(); // rootElement
    writer.writeEndDocument();

    return result;
}

void XmlHelper::writeAttributes(QXmlStreamWriter& writer, const QMap<QString, QString>& attributes)
{
    for (auto it = attributes.begin(); it != attributes.end(); ++it) {
        writer.writeAttribute(it.key(), it.value());
    }
}

void XmlHelper::writeJsonObject(QXmlStreamWriter& writer, const QString& elementName, const QJsonObject& jsonObject)
{
    if (jsonObject.isEmpty()) {
        return;
    }

    writer.writeStartElement(elementName);
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        writer.writeStartElement(QS("Property"));
        writer.writeAttribute(QS("name"), it.key());
        writer.writeCharacters(it.value().toString());
        writer.writeEndElement(); // Property
    }
    writer.writeEndElement(); // elementName
}

void XmlHelper::writeStringList(QXmlStreamWriter& writer, const QString& elementName, const QString& itemName, const QStringList& items)
{
    if (items.isEmpty()) {
        return;
    }

    writer.writeStartElement(elementName);
    for (const QString& item : items) {
        writer.writeTextElement(itemName, item);
    }
    writer.writeEndElement(); // elementName
}

void XmlHelper::writeBase64Data(QXmlStreamWriter& writer, const QString& elementName, const QByteArray& data)
{
    if (data.isEmpty()) {
        return;
    }

    writer.writeStartElement(elementName);
    writer.writeAttribute(QS("encoding"), QS("base64"));
    writer.writeCharacters(encodeToBase64(data));
    writer.writeEndElement(); // elementName
}

QString XmlHelper::encodeToBase64(const QByteArray& data)
{
    return QString::fromUtf8(data.toBase64());
}

// 显式模板实例化
template QByteArray XmlHelper::generateListXml<ImageInfo>(
    const QString&, const QString&, const QString&, 
    const QList<ImageInfo>&, 
    std::function<void(QXmlStreamWriter&, const ImageInfo&)>);

template QByteArray XmlHelper::generateListXml<TableInfo>(
    const QString&, const QString&, const QString&, 
    const QList<TableInfo>&, 
    std::function<void(QXmlStreamWriter&, const TableInfo&)>);

template QByteArray XmlHelper::generateListXml<ChartInfo>(
    const QString&, const QString&, const QString&, 
    const QList<ChartInfo>&, 
    std::function<void(QXmlStreamWriter&, const ChartInfo&)>);
