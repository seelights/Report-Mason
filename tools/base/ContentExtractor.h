/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:35
 * @LastEditTime: 2025-09-15 23:17:18
 * @LastEditors: seelights
 * @Description: 
 * @FilePath: \ReportMason\tools\base\ContentExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */
/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:29:46
 * @LastEditors: seelights
 * @Description: 内容提取器基类
 * @FilePath: \ReportMason\tools\base\ContentExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

/**
 * @brief 内容提取器基类
 * 
 * 定义了内容提取的通用接口和数据结构
 */
class ContentExtractor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 提取状态枚举
     */
    enum class ExtractStatus {
        SUCCESS,           // 成功
        FILE_NOT_FOUND,    // 文件未找到
        INVALID_FORMAT,    // 无效格式
        PARSE_ERROR,       // 解析错误
        WRITE_ERROR,       // 写入错误
        UNKNOWN_ERROR      // 未知错误
    };

    /**
     * @brief 内容类型枚举
     */
    enum class ContentType {
        TEXT,              // 文本
        IMAGE,             // 图片
        TABLE,             // 表格
        CHART,             // 图表
        SHAPE,             // 形状
        UNKNOWN            // 未知
    };

    explicit ContentExtractor(QObject *parent = nullptr);
    virtual ~ContentExtractor();

    /**
     * @brief 检查是否支持指定文件格式
     * @param filePath 文件路径
     * @return 是否支持
     */
    virtual bool isSupported(const QString &filePath) const = 0;

    /**
     * @brief 获取支持的文件格式列表
     * @return 支持的文件格式列表
     */
    virtual QStringList getSupportedFormats() const = 0;

    /**
     * @brief 获取最后错误信息
     * @return 错误信息
     */
    QString getLastError() const;

    /**
     * @brief 设置输出目录
     * @param outputDir 输出目录路径
     */
    void setOutputDirectory(const QString &outputDir);

    /**
     * @brief 获取输出目录
     * @return 输出目录路径
     */
    QString getOutputDirectory() const;

protected:
    /**
     * @brief 设置错误信息
     * @param error 错误信息
     */
    void setLastError(const QString &error);

    /**
     * @brief 验证文件路径
     * @param filePath 文件路径
     * @return 是否有效
     */
    bool validateFilePath(const QString &filePath) const;

    /**
     * @brief 清理文本内容
     * @param text 原始文本
     * @return 清理后的文本
     */
    QString cleanText(const QString &text) const;

    /**
     * @brief 生成唯一ID
     * @param prefix 前缀
     * @return 唯一ID
     */
    QString generateUniqueId(const QString &prefix = QStringLiteral("content"));

    /**
     * @brief 保存内容到文件
     * @param content 内容数据
     * @param fileName 文件名
     * @return 是否成功
     */
    bool saveContentToFile(const QByteArray &content, const QString &fileName);

    /**
     * @brief 将内容转换为Base64编码
     * @param content 内容数据
     * @return Base64编码字符串
     */
    QString encodeToBase64(const QByteArray &content) const;

    /**
     * @brief 从Base64解码内容
     * @param base64String Base64编码字符串
     * @return 解码后的内容
     */
    QByteArray decodeFromBase64(const QString &base64String) const;

private:
    QString m_lastError;           // 最后错误信息
    QString m_outputDirectory;     // 输出目录
    int m_idCounter;               // ID计数器
};

