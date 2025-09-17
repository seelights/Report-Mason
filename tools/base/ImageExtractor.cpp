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
#include "QtCompat.h"
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QImageWriter>
#include <QBuffer>
#include <QDebug>
#include <QXmlStreamWriter>

ImageExtractor::ImageExtractor(QObject* parent) : ContentExtractor(parent)
{
    // 初始化支持的图片格式
    m_supportedFormats << QS("png") << QS("jpg") << QS("jpeg") << QS("gif") << QS("bmp")
                       << QS("tiff") << QS("webp");
}

ImageExtractor::~ImageExtractor() {}

bool ImageExtractor::saveImage(const ImageInfo& image, const QString& outputPath)
{
    if (image.data.isEmpty()) {
        setLastError(QS("图片数据为空"));
        return false;
    }

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly)) {
        setLastError(QS("无法创建图片文件: %1").arg(outputPath));
        return false;
    }

    qint64 bytesWritten = file.write(image.data);
    file.close();

    if (bytesWritten != image.data.size()) {
        setLastError(QS("图片文件写入不完整: %1").arg(outputPath));
        return false;
    }

    return true;
}

QString ImageExtractor::imageToBase64(const ImageInfo& image) const
{
    return encodeToBase64(image.data);
}

ImageInfo ImageExtractor::imageFromBase64(const QString& base64String, const QString& format)
{
    ImageInfo imageInfo;
    imageInfo.data = decodeFromBase64(base64String);
    imageInfo.format = format;
    imageInfo.id = generateUniqueId(QS("img"));

    if (!imageInfo.data.isEmpty()) {
        imageInfo.size = getImageSize(imageInfo.data);
        imageInfo.savedPath = generateImageFileName(imageInfo);
    }

    return imageInfo;
}

QStringList ImageExtractor::getSupportedImageFormats() const { return m_supportedFormats; }

bool ImageExtractor::isImageFormatSupported(const QString& format) const
{
    return m_supportedFormats.contains(format.toLower());
}

bool ImageExtractor::processImageData(const QByteArray& imageData, ImageInfo& imageInfo)
{
    if (imageData.isEmpty()) {
        return false;
    }

    // 检测图片格式
    QString format = detectImageFormat(imageData);
    if (format.isEmpty()) {
        setLastError(QS("无法识别图片格式"));
        return false;
    }

    // 获取图片尺寸
    QSize size = getImageSize(imageData);
    if (size.isEmpty()) {
        setLastError(QS("无法获取图片尺寸"));
        return false;
    }

    // 设置图片信息
    imageInfo.data = imageData;
    imageInfo.format = format;
    imageInfo.size = size;
    imageInfo.id = generateUniqueId(QS("img"));
    imageInfo.savedPath = generateImageFileName(imageInfo);

    return true;
}

QString ImageExtractor::detectImageFormat(const QByteArray& imageData) const
{
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    QString format = QString::fromUtf8(reader.format());

    if (format.isEmpty()) {
        // 尝试通过文件头检测
        if (imageData.startsWith("\x89PNG")) {
            return QS("png");
        } else if (imageData.startsWith("\xFF\xD8\xFF")) {
            return QS("jpg");
        } else if (imageData.startsWith("GIF8")) {
            return QS("gif");
        } else if (imageData.startsWith("BM")) {
            return QS("bmp");
        } else if (imageData.startsWith("II*\x00") || imageData.startsWith("MM\x00*")) {
            return QS("tiff");
        }
    }

    return format.toLower();
}

QSize ImageExtractor::getImageSize(const QByteArray& imageData) const
{
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    return reader.size();
}

QString ImageExtractor::generateImageFileName(const ImageInfo& imageInfo)
{
    QString fileName = QS("%1.%2").arg(imageInfo.id, imageInfo.format);
    return QDir(getOutputDirectory()).absoluteFilePath(fileName);
}

bool ImageExtractor::exportToXml(const ImageInfo& image, const QString& outputPath)
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
    writer.writeStartDocument(QS("1.0"), true);

    // 写入根元素
    writer.writeStartElement(QS("Image"));
    writer.writeAttribute(QS("id"), image.id);
    writer.writeAttribute(QS("format"), image.format);
    writer.writeAttribute(QS("width"), QString::number(image.size.width()));
    writer.writeAttribute(QS("height"), QString::number(image.size.height()));

    // 写入图片数据（Base64编码）
    writer.writeStartElement(QS("Data"));
    writer.writeAttribute(QS("encoding"), QS("base64"));
    writer.writeCharacters(imageToBase64(image));
    writer.writeEndElement(); // Data

    // 写入保存路径
    if (!image.savedPath.isEmpty()) {
        writer.writeStartElement(QS("SavedPath"));
        writer.writeCharacters(image.savedPath);
        writer.writeEndElement(); // SavedPath
    }

    // 写入元数据
    if (!image.metadata.isEmpty()) {
        writer.writeStartElement(QS("Metadata"));
        for (auto it = image.metadata.begin(); it != image.metadata.end(); ++it) {
            writer.writeStartElement(QS("Property"));
            writer.writeAttribute(QS("name"), it.key());
            writer.writeCharacters(it.value().toString());
            writer.writeEndElement(); // Property
        }
        writer.writeEndElement(); // Metadata
    }

    writer.writeEndElement(); // Image
    writer.writeEndDocument();

    file.close();
    return true;
}

bool ImageExtractor::exportToXml(const QList<ImageInfo>& images, const QString& outputPath)
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
    writer.writeStartDocument(QS("1.0"), true);

    // 写入根元素
    writer.writeStartElement(QS("Images"));
    writer.writeAttribute(QS("count"), QString::number(images.size()));

    // 写入每个图片
    for (const ImageInfo& image : images) {
        writer.writeStartElement(QS("Image"));
        writer.writeAttribute(QS("id"), image.id);
        writer.writeAttribute(QS("format"), image.format);
        writer.writeAttribute(QS("width"), QString::number(image.size.width()));
        writer.writeAttribute(QS("height"), QString::number(image.size.height()));

        // 写入图片数据（Base64编码）
        writer.writeStartElement(QS("Data"));
        writer.writeAttribute(QS("encoding"), QS("base64"));
        writer.writeCharacters(imageToBase64(image));
        writer.writeEndElement(); // Data

        // 写入保存路径
        if (!image.savedPath.isEmpty()) {
            writer.writeStartElement(QS("SavedPath"));
            writer.writeCharacters(image.savedPath);
            writer.writeEndElement(); // SavedPath
        }

        // 写入元数据
        if (!image.metadata.isEmpty()) {
            writer.writeStartElement(QS("Metadata"));
            for (auto it = image.metadata.begin(); it != image.metadata.end(); ++it) {
                writer.writeStartElement(QS("Property"));
                writer.writeAttribute(QS("name"), it.key());
                writer.writeCharacters(it.value().toString());
                writer.writeEndElement(); // Property
            }
            writer.writeEndElement(); // Metadata
        }

        writer.writeEndElement(); // Image
    }

    writer.writeEndElement(); // Images
    writer.writeEndDocument();

    file.close();
    return true;
}
