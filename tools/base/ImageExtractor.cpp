/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 图片提取器抽象基类实现
 * @FilePath: \ReportMason\tools\base\ImageExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "ImageExtractor.h"
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QImageWriter>
#include <QBuffer>
#include <QDebug>

ImageExtractor::ImageExtractor(QObject *parent)
    : ContentExtractor(parent)
{
    // 初始化支持的图片格式
    m_supportedFormats << "png" << "jpg" << "jpeg" << "gif" << "bmp" << "tiff" << "webp";
}

ImageExtractor::~ImageExtractor()
{
}

bool ImageExtractor::saveImage(const ImageInfo &image, const QString &outputPath)
{
    if (image.data.isEmpty()) {
        setLastError("图片数据为空");
        return false;
    }

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly)) {
        setLastError(QString("无法创建图片文件: %1").arg(outputPath));
        return false;
    }

    qint64 bytesWritten = file.write(image.data);
    file.close();

    if (bytesWritten != image.data.size()) {
        setLastError(QString("图片文件写入不完整: %1").arg(outputPath));
        return false;
    }

    return true;
}

QString ImageExtractor::imageToBase64(const ImageInfo &image) const
{
    return encodeToBase64(image.data);
}

ImageInfo ImageExtractor::imageFromBase64(const QString &base64String, const QString &format)
{
    ImageInfo imageInfo;
    imageInfo.data = decodeFromBase64(base64String);
    imageInfo.format = format;
    imageInfo.id = generateUniqueId("img");
    
    if (!imageInfo.data.isEmpty()) {
        imageInfo.size = getImageSize(imageInfo.data);
        imageInfo.savedPath = generateImageFileName(imageInfo);
    }
    
    return imageInfo;
}

QStringList ImageExtractor::getSupportedImageFormats() const
{
    return m_supportedFormats;
}

bool ImageExtractor::isImageFormatSupported(const QString &format) const
{
    return m_supportedFormats.contains(format.toLower());
}

bool ImageExtractor::processImageData(const QByteArray &imageData, ImageInfo &imageInfo)
{
    if (imageData.isEmpty()) {
        return false;
    }

    // 检测图片格式
    QString format = detectImageFormat(imageData);
    if (format.isEmpty()) {
        setLastError("无法识别图片格式");
        return false;
    }

    // 获取图片尺寸
    QSize size = getImageSize(imageData);
    if (size.isEmpty()) {
        setLastError("无法获取图片尺寸");
        return false;
    }

    // 设置图片信息
    imageInfo.data = imageData;
    imageInfo.format = format;
    imageInfo.size = size;
    imageInfo.id = generateUniqueId("img");
    imageInfo.savedPath = generateImageFileName(imageInfo);

    return true;
}

QString ImageExtractor::detectImageFormat(const QByteArray &imageData) const
{
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    QString format = reader.format();
    
    if (format.isEmpty()) {
        // 尝试通过文件头检测
        if (imageData.startsWith("\x89PNG")) {
            return "png";
        } else if (imageData.startsWith("\xFF\xD8\xFF")) {
            return "jpg";
        } else if (imageData.startsWith("GIF8")) {
            return "gif";
        } else if (imageData.startsWith("BM")) {
            return "bmp";
        } else if (imageData.startsWith("II*\x00") || imageData.startsWith("MM\x00*")) {
            return "tiff";
        }
    }

    return format.toLower();
}

QSize ImageExtractor::getImageSize(const QByteArray &imageData) const
{
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    return reader.size();
}

QString ImageExtractor::generateImageFileName(const ImageInfo &imageInfo)
{
    QString fileName = QString("%1.%2").arg(imageInfo.id, imageInfo.format);
    return QDir(getOutputDirectory()).absoluteFilePath(fileName);
}
