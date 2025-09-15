/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 图片提取器抽象基类
 * @FilePath: \ReportMason\tools\base\ImageExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#ifndef IMAGEEXTRACTOR_H
#define IMAGEEXTRACTOR_H

#include "ContentExtractor.h"
#include <QSize>
#include <QRect>
#include <QJsonObject>

/**
 * @brief 图片信息结构
 */
struct ImageInfo {
    QString id;                    // 图片唯一ID
    QString originalPath;          // 原始路径（在文档中的路径）
    QString savedPath;             // 保存后的路径
    QString format;                // 图片格式（png, jpg, gif等）
    QSize size;                    // 图片尺寸
    QRect position;                // 在文档中的位置
    QString description;           // 图片描述
    QByteArray data;               // 图片数据
    QJsonObject metadata;          // 元数据
    bool isEmbedded;               // 是否为内嵌图片
    
    ImageInfo() : isEmbedded(false) {}
    
    ImageInfo(const QString &id, const QString &format, const QSize &size)
        : id(id), format(format), size(size), isEmbedded(false) {}
};

/**
 * @brief 图片提取器抽象基类
 * 
 * 定义了图片提取的通用接口
 */
class ImageExtractor : public ContentExtractor
{
    Q_OBJECT

public:
    explicit ImageExtractor(QObject *parent = nullptr);
    virtual ~ImageExtractor();

    /**
     * @brief 从文档中提取所有图片
     * @param filePath 文档路径
     * @param images 输出图片信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractImages(const QString &filePath, QList<ImageInfo> &images) = 0;

    /**
     * @brief 从文档中提取指定类型的图片
     * @param filePath 文档路径
     * @param imageType 图片类型（如"png", "jpg"等）
     * @param images 输出图片信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractImagesByType(const QString &filePath, const QString &imageType, QList<ImageInfo> &images) = 0;

    /**
     * @brief 从文档中提取指定位置的图片
     * @param filePath 文档路径
     * @param position 位置范围
     * @param images 输出图片信息列表
     * @return 提取状态
     */
    virtual ExtractStatus extractImagesByPosition(const QString &filePath, const QRect &position, QList<ImageInfo> &images) = 0;

    /**
     * @brief 获取图片数量
     * @param filePath 文档路径
     * @return 图片数量，-1表示错误
     */
    virtual int getImageCount(const QString &filePath) = 0;

    /**
     * @brief 保存图片到指定目录
     * @param image 图片信息
     * @param outputPath 输出路径
     * @return 是否成功
     */
    virtual bool saveImage(const ImageInfo &image, const QString &outputPath);

    /**
     * @brief 将图片转换为Base64编码
     * @param image 图片信息
     * @return Base64编码字符串
     */
    virtual QString imageToBase64(const ImageInfo &image) const;

    /**
     * @brief 从Base64创建图片信息
     * @param base64String Base64编码字符串
     * @param format 图片格式
     * @return 图片信息
     */
    virtual ImageInfo imageFromBase64(const QString &base64String, const QString &format);

    /**
     * @brief 获取支持的图片格式列表
     * @return 支持的图片格式列表
     */
    virtual QStringList getSupportedImageFormats() const;

    /**
     * @brief 验证图片格式是否支持
     * @param format 图片格式
     * @return 是否支持
     */
    virtual bool isImageFormatSupported(const QString &format) const;

protected:
    /**
     * @brief 处理图片数据
     * @param imageData 原始图片数据
     * @param imageInfo 图片信息（输出）
     * @return 是否成功
     */
    virtual bool processImageData(const QByteArray &imageData, ImageInfo &imageInfo);

    /**
     * @brief 检测图片格式
     * @param imageData 图片数据
     * @return 图片格式
     */
    virtual QString detectImageFormat(const QByteArray &imageData) const;

    /**
     * @brief 获取图片尺寸
     * @param imageData 图片数据
     * @return 图片尺寸
     */
    virtual QSize getImageSize(const QByteArray &imageData) const;

    /**
     * @brief 生成图片文件名
     * @param imageInfo 图片信息
     * @return 文件名
     */
    virtual QString generateImageFileName(const ImageInfo &imageInfo);

private:
    QStringList m_supportedFormats;  // 支持的图片格式列表
};

#endif // IMAGEEXTRACTOR_H
