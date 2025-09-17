/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:16:21
 * @LastEditors: seelights
 * @Description: 内容处理工具类
 * @FilePath: \ReportMason\tools\utils\ContentUtils.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include <QString>
#include <QStringList>
#include <QSize>
#include <QRect>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include "QtCompat.h"

/**
 * @brief 内容处理工具类
 *
 * 提供内容提取和处理相关的工具函数
 */
class ContentUtils
{
public:
    /**
     * @brief 清理文本内容
     * @param text 原始文本
     * @return 清理后的文本
     */
    static QString cleanText(const QString& text);

    /**
     * @brief 检测文件格式
     * @param filePath 文件路径
     * @return 文件格式（小写）
     */
    static QString detectFileFormat(const QString& filePath);

    /**
     * @brief 生成唯一ID
     * @param prefix 前缀
     * @return 唯一ID
     */
    static QString generateUniqueId(const QString& prefix = QS("content"));

    /**
     * @brief 将数据转换为Base64编码
     * @param data 原始数据
     * @return Base64编码字符串
     */
    static QString encodeToBase64(const QByteArray& data);

    /**
     * @brief 从Base64解码数据
     * @param base64String Base64编码字符串
     * @return 解码后的数据
     */
    static QByteArray decodeFromBase64(const QString& base64String);

    /**
     * @brief 验证文件路径
     * @param filePath 文件路径
     * @return 是否有效
     */
    static bool validateFilePath(const QString& filePath);

    /**
     * @brief 创建目录（如果不存在）
     * @param dirPath 目录路径
     * @return 是否成功
     */
    static bool createDirectory(const QString& dirPath);

    /**
     * @brief 获取文件扩展名
     * @param filePath 文件路径
     * @return 文件扩展名（小写，不含点号）
     */
    static QString getFileExtension(const QString& filePath);

    /**
     * @brief 获取文件名（不含扩展名）
     * @param filePath 文件路径
     * @return 文件名（不含扩展名）
     */
    static QString getFileNameWithoutExtension(const QString& filePath);

    /**
     * @brief 合并路径
     * @param basePath 基础路径
     * @param relativePath 相对路径
     * @return 合并后的路径
     */
    static QString joinPath(const QString& basePath, const QString& relativePath);

    /**
     * @brief 转义XML内容
     * @param content 原始内容
     * @return 转义后的内容
     */
    static QString escapeXml(const QString& content);

    /**
     * @brief 转义JSON内容
     * @param content 原始内容
     * @return 转义后的内容
     */
    static QString escapeJson(const QString& content);

    /**
     * @brief 转义CSV内容
     * @param content 原始内容
     * @return 转义后的内容
     */
    static QString escapeCsv(const QString& content);

    /**
     * @brief 转义HTML内容
     * @param content 原始内容
     * @return 转义后的内容
     */
    static QString escapeHtml(const QString& content);

    /**
     * @brief 检测图片格式
     * @param imageData 图片数据
     * @return 图片格式
     */
    static QString detectImageFormat(const QByteArray& imageData);

    /**
     * @brief 获取图片尺寸
     * @param imageData 图片数据
     * @return 图片尺寸
     */
    static QSize getImageSize(const QByteArray& imageData);

    /**
     * @brief 压缩图片
     * @param imageData 原始图片数据
     * @param quality 压缩质量（0-100）
     * @return 压缩后的图片数据
     */
    static QByteArray compressImage(const QByteArray& imageData, int quality = 80);

    /**
     * @brief 调整图片尺寸
     * @param imageData 原始图片数据
     * @param size 目标尺寸
     * @return 调整后的图片数据
     */
    static QByteArray resizeImage(const QByteArray& imageData, const QSize& size);

    /**
     * @brief 验证JSON格式
     * @param jsonString JSON字符串
     * @return 是否有效
     */
    static bool validateJson(const QString& jsonString);

    /**
     * @brief 格式化JSON字符串
     * @param jsonString JSON字符串
     * @return 格式化后的JSON字符串
     */
    static QString formatJson(const QString& jsonString);

    /**
     * @brief 计算文件哈希值
     * @param filePath 文件路径
     * @return 文件哈希值
     */
    static QString calculateFileHash(const QString& filePath);

    /**
     * @brief 计算数据哈希值
     * @param data 数据
     * @return 数据哈希值
     */
    static QString calculateDataHash(const QByteArray& data);

private:
    ContentUtils() = delete; // 禁止实例化
};

