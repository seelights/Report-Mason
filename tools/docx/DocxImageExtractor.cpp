/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-20 21:15:50
 * @LastEditors: seelights
 * @Description: DOCX图片提取器
 * @FilePath: \ReportMason\tools\docx\DocxImageExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "DocxImageExtractor.h"
#include "QtCompat.h"
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDateTime>
#include <QStandardPaths>

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

QStringList DocxImageExtractor::getSupportedFormats() const
{
    return SUPPORTED_EXTENSIONS;
}

ImageExtractor::ExtractStatus DocxImageExtractor::extractImages(const QString& filePath, QList<ImageInfo>& images)
{
    images.clear();
    
    if (!isSupported(filePath)) {
        return ExtractStatus::INVALID_FORMAT;
    }
    
    try {
        // 读取document.xml
        QByteArray documentXml;
        if (!KZipUtils::readFileFromZip(filePath, DOCX_DOCUMENT_PATH, documentXml)) {
            return ExtractStatus::FILE_NOT_FOUND;
        }
        
        // 提取图片引用
        QList<QString> imageRefs = extractImageReferences(documentXml);
        if (imageRefs.isEmpty()) {
            return ExtractStatus::SUCCESS; // 没有图片也算成功
        }
        
        // 提取位置信息
        QMap<QString, QRect> positions = extractImagePositions(filePath, imageRefs);
        
        // 读取关系文件
        QByteArray relationshipsXml;
        if (!KZipUtils::readFileFromZip(filePath, DOCX_RELATIONSHIPS_PATH, relationshipsXml)) {
            return ExtractStatus::PARSE_ERROR;
        }
        QMap<QString, QString> imageRelationships = parseImageRelationships(relationshipsXml);
        
        // 提取图片数据
        for (const QString& imageRef : imageRefs) {
            QString imagePath = imageRelationships.value(imageRef);
            if (!imagePath.isEmpty()) {
                QByteArray imageData;
                if (KZipUtils::readFileFromZip(filePath, QS("word/") + imagePath, imageData)) {
                    ImageInfo imageInfo = createImageInfoFromData(imageData, imagePath, positions.value(imageRef));
                    if (!imageInfo.originalPath.isEmpty()) {
                        images.append(imageInfo);
                    }
                }
            }
        }
        
        return ExtractStatus::SUCCESS;
        
    } catch (const std::exception& e) {
        return ExtractStatus::UNKNOWN_ERROR;
    }
}

QList<QString> DocxImageExtractor::extractImageReferences(const QByteArray& documentXml) const
{
    QList<QString> imageRefs;
    
    QXmlStreamReader reader(documentXml);
    while (!reader.atEnd()) {
        reader.readNext();
        
        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
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

QMap<QString, QRect> DocxImageExtractor::extractImagePositions(const QString& zipPath, const QStringList& imageRefs) const
{
    QMap<QString, QRect> positions;
    
    try {
        QByteArray documentXml;
        if (!KZipUtils::readFileFromZip(zipPath, DOCX_DOCUMENT_PATH, documentXml)) {
            return positions;
        }
        
        QXmlStreamReader reader(documentXml);
        int imageIndex = 0;
        
        while (!reader.atEnd() && imageIndex < imageRefs.size()) {
            reader.readNext();
            
            if (reader.isStartElement()) {
                QString elementName = reader.name().toString();
                QString namespaceUri = reader.namespaceUri().toString();
                
                if (elementName == QS("anchor") && namespaceUri.contains(QS("wp"))) {
                    QRect position = parseAnchorPosition(reader);
                    positions.insert(imageRefs[imageIndex], position);
                    imageIndex++;
                } else if (elementName == QS("inline") && namespaceUri.contains(QS("wp"))) {
                    QRect position = parseInlinePosition(reader);
                    positions.insert(imageRefs[imageIndex], position);
                    imageIndex++;
                }
            }
        }
        
    } catch (const std::exception& e) {
        // 忽略异常，返回空映射
    }
    
    return positions;
}

QRect DocxImageExtractor::parseAnchorPosition(QXmlStreamReader& reader) const
{
    QRect position(0, 0, 100, 100); // 默认位置和尺寸
    
    while (!reader.atEnd()) {
        reader.readNext();
        
        if (reader.isEndElement() && reader.name().toString() == QS("anchor")) {
            break;
        }
        
        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
            
            if (elementName == QS("simplePos")) {
                QString x = reader.attributes().value(QS("x")).toString();
                QString y = reader.attributes().value(QS("y")).toString();
                if (!x.isEmpty() && !y.isEmpty()) {
                    position.setX(emuToPixels(x.toLongLong()));
                    position.setY(emuToPixels(y.toLongLong()));
                }
            } else if (elementName == QS("positionH")) {
                QString posOffset = reader.attributes().value(QS("posOffset")).toString();
                if (!posOffset.isEmpty()) {
                    position.setX(emuToPixels(posOffset.toLongLong()));
                }
            } else if (elementName == QS("positionV")) {
                QString posOffset = reader.attributes().value(QS("posOffset")).toString();
                if (!posOffset.isEmpty()) {
                    position.setY(emuToPixels(posOffset.toLongLong()));
                }
            } else if (elementName == QS("extent")) {
                QString cx = reader.attributes().value(QS("cx")).toString();
                QString cy = reader.attributes().value(QS("cy")).toString();
                if (!cx.isEmpty() && !cy.isEmpty()) {
                    position.setWidth(emuToPixels(cx.toLongLong()));
                    position.setHeight(emuToPixels(cy.toLongLong()));
                }
            }
        }
    }
    
    return position;
}

QRect DocxImageExtractor::parseInlinePosition(QXmlStreamReader& reader) const
{
    QRect position(0, 0, 100, 100); // 默认位置和尺寸
    
    while (!reader.atEnd()) {
        reader.readNext();
        
        if (reader.isEndElement() && reader.name().toString() == QS("inline")) {
            break;
        }
        
        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
            
            if (elementName == QS("extent")) {
                QString cx = reader.attributes().value(QS("cx")).toString();
                QString cy = reader.attributes().value(QS("cy")).toString();
                if (!cx.isEmpty() && !cy.isEmpty()) {
                    position.setWidth(emuToPixels(cx.toLongLong()));
                    position.setHeight(emuToPixels(cy.toLongLong()));
                }
            }
        }
    }
    
    return position;
}

QMap<QString, QString> DocxImageExtractor::parseImageRelationships(const QByteArray& relationshipsXml) const
{
    QMap<QString, QString> relationships;
    
    QXmlStreamReader reader(relationshipsXml);
    while (!reader.atEnd()) {
        reader.readNext();
        
        if (reader.isStartElement() && reader.name().toString() == QS("Relationship")) {
            QString id = reader.attributes().value(QS("Id")).toString();
            QString target = reader.attributes().value(QS("Target")).toString();
            QString type = reader.attributes().value(QS("Type")).toString();
            
            // 检查是否为图片关系
            if (type.contains(QS("image")) && target.startsWith(QS("media/"))) {
                relationships.insert(id, target);
            }
        }
    }
    
    return relationships;
}

ImageInfo DocxImageExtractor::createImageInfoFromData(const QByteArray& imageData, const QString& imagePath, const QRect& position)
{
    ImageInfo imageInfo;
    imageInfo.originalPath = imagePath;
    imageInfo.position = position;
    imageInfo.data = imageData;
    imageInfo.format = QFileInfo(imagePath).suffix().toLower();
    imageInfo.isEmbedded = true;
    
    // 获取图片尺寸
    QSize imageSize = getImageSize(imageData);
    imageInfo.size = imageSize;
    
    // 生成唯一ID
    imageInfo.id = QS("img_") + QString::number(QDateTime::currentMSecsSinceEpoch()) + QS("_") + QString::number(qHash(imagePath));
    
    // 保存图片到临时文件
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QS("/ReportMason");
    QDir().mkpath(tempDir);
    QString tempFilePath = tempDir + QS("/") + imageInfo.id + QS(".") + imageInfo.format;
    
    QFile tempFile(tempFilePath);
    if (tempFile.open(QIODevice::WriteOnly)) {
        tempFile.write(imageData);
        tempFile.close();
        imageInfo.savedPath = tempFilePath;
    }
    
    return imageInfo;
}

QSize DocxImageExtractor::getImageSize(const QByteArray& imageData) const
{
    // 简单的图片尺寸获取，实际项目中可能需要更复杂的解析
    // 这里返回默认尺寸，实际使用时可以集成图片库来获取真实尺寸
    return QSize(100, 100);
}

int DocxImageExtractor::emuToPixels(qint64 emu) const
{
    // EMU到像素的转换：1英寸 = 914400 EMU，1英寸 = 96像素
    // 所以 1像素 = 914400 / 96 = 9525 EMU
    return static_cast<int>(emu / 9525);
}

// 实现基类的虚函数
ImageExtractor::ExtractStatus DocxImageExtractor::extractImagesByType(const QString& filePath, const QString& imageType, QList<ImageInfo>& images)
{
    // 先提取所有图片
    QList<ImageInfo> allImages;
    ExtractStatus status = extractImages(filePath, allImages);
    if (status != ExtractStatus::SUCCESS) {
        return status;
    }
    
    // 过滤指定类型的图片
    images.clear();
    for (const ImageInfo& image : allImages) {
        if (image.format.toLower() == imageType.toLower()) {
            images.append(image);
        }
    }
    
    return ExtractStatus::SUCCESS;
}

ImageExtractor::ExtractStatus DocxImageExtractor::extractImagesByPosition(const QString& filePath, const QRect& position, QList<ImageInfo>& images)
{
    // 先提取所有图片
    QList<ImageInfo> allImages;
    ExtractStatus status = extractImages(filePath, allImages);
    if (status != ExtractStatus::SUCCESS) {
        return status;
    }
    
    // 过滤指定位置的图片
    images.clear();
    for (const ImageInfo& image : allImages) {
        if (image.position.intersects(position)) {
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
        return 0;
    }
    
    return images.size();
}
