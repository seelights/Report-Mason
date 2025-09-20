/*
 * @Author: seelights
 * @Date: 2025-01-XX XX:XX:XX
 * @LastEditTime: 2025-01-XX XX:XX:XX
 * @LastEditors: seelights
 * @Description: XML生成辅助类
 * @FilePath: \ReportMason\tools\base\XmlHelper.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include <QByteArray>
#include <QXmlStreamWriter>
#include <QBuffer>
#include <QJsonObject>
#include <QStringList>

/**
 * @brief XML生成辅助类
 * 
 * 提供统一的XML生成功能，减少重复代码
 */
class XmlHelper
{
public:
    /**
     * @brief 生成单个对象的XML
     * @param rootElement 根元素名称
     * @param attributes 属性映射
     * @param contentCallback 内容生成回调函数
     * @return XML字节数组
     */
    static QByteArray generateObjectXml(
        const QString& rootElement,
        const QMap<QString, QString>& attributes,
        std::function<void(QXmlStreamWriter&)> contentCallback = nullptr);

    /**
     * @brief 生成对象列表的XML
     * @param rootElement 根元素名称
     * @param countAttribute 数量属性名称
     * @param itemElement 子元素名称
     * @param items 对象列表
     * @param itemCallback 每个对象的XML生成回调
     * @return XML字节数组
     */
    template<typename T>
    static QByteArray generateListXml(
        const QString& rootElement,
        const QString& countAttribute,
        const QString& itemElement,
        const QList<T>& items,
        std::function<void(QXmlStreamWriter&, const T&)> itemCallback);

    /**
     * @brief 写入属性到XML写入器
     * @param writer XML写入器
     * @param attributes 属性映射
     */
    static void writeAttributes(QXmlStreamWriter& writer, const QMap<QString, QString>& attributes);

    /**
     * @brief 写入JSON对象到XML
     * @param writer XML写入器
     * @param elementName 元素名称
     * @param jsonObject JSON对象
     */
    static void writeJsonObject(QXmlStreamWriter& writer, const QString& elementName, const QJsonObject& jsonObject);

    /**
     * @brief 写入字符串列表到XML
     * @param writer XML写入器
     * @param elementName 元素名称
     * @param itemName 子元素名称
     * @param items 字符串列表
     */
    static void writeStringList(QXmlStreamWriter& writer, const QString& elementName, const QString& itemName, const QStringList& items);

    /**
     * @brief 写入Base64编码数据到XML
     * @param writer XML写入器
     * @param elementName 元素名称
     * @param data 二进制数据
     */
    static void writeBase64Data(QXmlStreamWriter& writer, const QString& elementName, const QByteArray& data);

private:
    /**
     * @brief 将二进制数据编码为Base64
     * @param data 二进制数据
     * @return Base64编码字符串
     */
    static QString encodeToBase64(const QByteArray& data);
};
