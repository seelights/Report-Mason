/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: DOCX图片提取器实现
 * @FilePath: \ReportMason\tools\docx\DocxImageExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "DocxImageExtractor.h"
#include "../utils/ContentUtils.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QBuffer>
#include <QImageReader>

// 静态常量定义
const QStringList DocxImageExtractor::SUPPORTED_EXTENSIONS = {QS("docx")};
const QString DocxImageExtractor::DOCX_DOCUMENT_PATH = QS("word/document.xml");
const QString DocxImageExtractor::DOCX_RELATIONSHIPS_PATH = QS("word/_rels/document.xml.rels");
const QString DocxImageExtractor::DOCX_MEDIA_PATH = QS("word/media/");

DocxImageExtractor::DocxImageExtractor(QObject* parent) : ImageExtractor(parent) {}

DocxImageExtractor::~DocxImageExtractor() {}

bool DocxImageExtractor::isSupported(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    return SUPPORTED_EXTENSIONS.contains(fileInfo.suffix().toLower());
}

QStringList DocxImageExtractor::getSupportedFormats() const { return SUPPORTED_EXTENSIONS; }

ImageExtractor::ExtractStatus DocxImageExtractor::extractImages(const QString& filePath,
                                                                QList<ImageInfo>& images)
{
    if (!isSupported(filePath)) {
        setLastError(QS("不支持的文件格式，仅支持.docx文件"));
        return ExtractStatus::INVALID_FORMAT;
    }

    // 验证ZIP文件
    if (!KZipUtils::isValidZip(filePath)) {
        setLastError(QS("无效的DOCX文件格式"));
        return ExtractStatus::INVALID_FORMAT;
    }

    // 读取document.xml
    QByteArray documentXml;
    if (!KZipUtils::readFileFromZip(filePath, DOCX_DOCUMENT_PATH, documentXml)) {
        setLastError(QS("无法读取document.xml文件"));
        return ExtractStatus::PARSE_ERROR;
    }

    // 读取关系文件
    QByteArray relationshipsXml;
    if (!KZipUtils::readFileFromZip(filePath, DOCX_RELATIONSHIPS_PATH, relationshipsXml)) {
        setLastError(QS("无法读取关系文件"));
        return ExtractStatus::PARSE_ERROR;
    }

    // 解析关系文件获取图片映射
    QMap<QString, QString> imageRelationships = parseImageRelationships(relationshipsXml);

    // 解析document.xml提取图片引用
    QList<QString> imageRefs = extractImageReferences(documentXml);

    // 从ZIP中提取图片数据
    for (const QString& imageRef : imageRefs) {
        if (imageRelationships.contains(imageRef)) {
            QString imagePath = imageRelationships[imageRef];
            if (imagePath.startsWith(QS("media/"))) {
                QByteArray imageData;
                if (KZipUtils::readFileFromZip(filePath, QS("word/") + imagePath, imageData)) {
                    ImageInfo imageInfo = createImageInfoFromData(imageData, imagePath);
                    if (!imageInfo.id.isEmpty()) {
                        images.append(imageInfo);
                    }
                }
            }
        }
    }

    return ExtractStatus::SUCCESS;
}

ImageExtractor::ExtractStatus DocxImageExtractor::extractImagesByType(const QString& filePath,
                                                                      const QString& imageType,
                                                                      QList<ImageInfo>& images)
{
    QList<ImageInfo> allImages;
    ExtractStatus status = extractImages(filePath, allImages);
    if (status != ExtractStatus::SUCCESS) {
        return status;
    }

    // 过滤指定类型的图片
    for (const ImageInfo& image : allImages) {
        if (image.format.toLower() == imageType.toLower()) {
            images.append(image);
        }
    }

    return ExtractStatus::SUCCESS;
}

ImageExtractor::ExtractStatus DocxImageExtractor::extractImagesByPosition(const QString& filePath,
                                                                          const QRect& position,
                                                                          QList<ImageInfo>& images)
{
    QList<ImageInfo> allImages;
    ExtractStatus status = extractImages(filePath, allImages);
    if (status != ExtractStatus::SUCCESS) {
        return status;
    }

    // 过滤指定位置的图片
    for (const ImageInfo& image : allImages) {
        if (position.intersects(image.position)) {
            images.append(image);
        }
    }

    return ExtractStatus::SUCCESS;
}

int DocxImageExtractor::getImageCount(const QString& filePath)
{
    QList<ImageInfo> images;
    ExtractStatus status = extractImages(filePath, images);
    if (status != ExtractStatus::SUCCESS) {
        return -1;
    }
    return images.size();
}

QByteArray DocxImageExtractor::readFileFromZip(const QString& zipPath,
                                               const QString& internalPath) const
{
    // 这里需要使用KZipUtils来读取ZIP文件
    // 暂时返回空数据，实际实现需要集成KZipUtils
    Q_UNUSED(zipPath)
    Q_UNUSED(internalPath)

    qDebug() << QS("DocxImageExtractor: 需要集成KZipUtils来读取ZIP文件");
    return QByteArray();
}

bool DocxImageExtractor::parseDocumentXml(const QByteArray& xmlContent,
                                          QList<ImageInfo>& images) const
{
    QXmlStreamReader reader(xmlContent);

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = reader.name().toString();

            if (elementName == QS("drawing")) {
                parseDrawingElement(reader, images);
            } else if (elementName == QS("pict")) {
                parsePictElement(reader, images);
            }
        }
    }

    return !reader.hasError();
}

bool DocxImageExtractor::parseDrawingElement(QXmlStreamReader& reader,
                                             QList<ImageInfo>& images) const
{
    Q_UNUSED(reader)
    Q_UNUSED(images)

    // 解析drawing元素中的图片信息
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 解析drawing元素");
    return true;
}

bool DocxImageExtractor::parsePicElement(QXmlStreamReader& reader, QList<ImageInfo>& images) const
{
    Q_UNUSED(reader)
    Q_UNUSED(images)

    // 解析pic元素中的图片信息
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 解析pic元素");
    return true;
}

bool DocxImageExtractor::parsePictElement(QXmlStreamReader& reader, QList<ImageInfo>& images) const
{
    Q_UNUSED(reader)
    Q_UNUSED(images)

    // 解析pict元素中的图片信息
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 解析pict元素");
    return true;
}

bool DocxImageExtractor::getImageFromRelationship(const QString& zipPath,
                                                  const QString& relationshipId,
                                                  ImageInfo& imageInfo) const
{
    Q_UNUSED(zipPath)
    Q_UNUSED(relationshipId)
    Q_UNUSED(imageInfo)

    // 从关系文件中获取图片信息
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 从关系文件获取图片信息");
    return true;
}

bool DocxImageExtractor::parseRelationship(const QString& zipPath, const QString& relationshipId,
                                           QString& imagePath, QString& imageType) const
{
    Q_UNUSED(zipPath)
    Q_UNUSED(relationshipId)
    Q_UNUSED(imagePath)
    Q_UNUSED(imageType)

    // 解析关系文件
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 解析关系文件");
    return true;
}

bool DocxImageExtractor::extractImageData(const QString& zipPath, const QString& imagePath,
                                          QByteArray& imageData) const
{
    Q_UNUSED(zipPath)
    Q_UNUSED(imagePath)
    Q_UNUSED(imageData)

    // 提取图片数据
    // 这里需要实现具体的提取逻辑
    qDebug() << QS("DocxImageExtractor: 提取图片数据");
    return true;
}

bool DocxImageExtractor::getImagePosition(QXmlStreamReader& reader, QRect& position) const
{
    Q_UNUSED(reader)
    Q_UNUSED(position)

    // 获取图片位置
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 获取图片位置");
    return true;
}

bool DocxImageExtractor::getImageSize(QXmlStreamReader& reader, QSize& size) const
{
    Q_UNUSED(reader)
    Q_UNUSED(size)

    // 获取图片尺寸
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 获取图片尺寸");
    return true;
}

QSize DocxImageExtractor::getImageSize(const QByteArray& imageData) const
{
    if (imageData.isEmpty()) {
        return QSize();
    }

    // 使用QImageReader来获取图片尺寸
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    return reader.size();
}

QMap<QString, QString> DocxImageExtractor::parseImageRelationships(
    const QByteArray& relationshipsXml) const
{
    QMap<QString, QString> relationships;

    QXmlStreamReader reader(relationshipsXml);
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement() && reader.name() == QS("Relationship")) {
            QString id = reader.attributes().value(QS("Id")).toString();
            QString target = reader.attributes().value(QS("Target")).toString();
            QString type = reader.attributes().value(QS("Type")).toString();

            // 检查是否是图片关系
            if (type.contains(QS("image")) || target.contains(QS("media/"))) {
                relationships[id] = target;
            }
        }
    }

    return relationships;
}

QList<QString> DocxImageExtractor::extractImageReferences(const QByteArray& documentXml) const
{
    QList<QString> imageRefs;

    QXmlStreamReader reader(documentXml);
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();

            // 查找图片引用
            if (elementName == QS("blip") || elementName == QS("pic")) {
                QString rId = reader.attributes().value(QS("r:embed")).toString();
                if (!rId.isEmpty()) {
                    imageRefs.append(rId);
                }
            }
        }
    }

    return imageRefs;
}

ImageInfo DocxImageExtractor::createImageInfoFromData(const QByteArray& imageData,
                                                      const QString& imagePath)
{
    ImageInfo imageInfo;

    if (imageData.isEmpty()) {
        return imageInfo;
    }

    // 检测图片格式
    QString format = detectImageFormat(imageData);
    if (format.isEmpty()) {
        return imageInfo;
    }

    // 获取图片尺寸
    QSize size = getImageSize(imageData);
    if (size.isEmpty()) {
        return imageInfo;
    }

    // 设置图片信息
    imageInfo.id = generateUniqueId(QS("img"));
    imageInfo.format = format;
    imageInfo.size = size;
    imageInfo.data = imageData;
    imageInfo.savedPath = generateImageFileName(imageInfo);

    return imageInfo;
}

bool DocxImageExtractor::getImageDescription(QXmlStreamReader& reader, QString& description) const
{
    Q_UNUSED(reader)
    Q_UNUSED(description)

    // 获取图片描述
    // 这里需要实现具体的解析逻辑
    qDebug() << QS("DocxImageExtractor: 获取图片描述");
    return true;
}
