/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-20 21:15:50
 * @LastEditors: seelights
 * @Description: DOCX图片提取器
 * @FilePath: \ReportMason\tools\docx\DocxImageExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include "../base/ImageExtractor.h"
#include "../../src/KZipUtils.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/**
 * @brief DOCX图片提取器
 * 
 * 专门用于从DOCX文件中提取图片内容
 */
class DocxImageExtractor : public ImageExtractor
{
    Q_OBJECT

public:
    explicit DocxImageExtractor(QObject *parent = nullptr);
    virtual ~DocxImageExtractor();

    // 实现基类接口
    bool isSupported(const QString &filePath) const override;
    QStringList getSupportedFormats() const override;
    ExtractStatus extractImages(const QString &filePath, QList<ImageInfo> &images) override;
    ExtractStatus extractImagesByType(const QString &filePath, const QString &imageType, QList<ImageInfo> &images) override;
    ExtractStatus extractImagesByPosition(const QString &filePath, const QRect &position, QList<ImageInfo> &images) override;
    int getImageCount(const QString &filePath) override;

protected:
    /**
     * @brief 从ZIP文件中读取文件内容
     * @param zipPath ZIP文件路径
     * @param internalPath 内部文件路径
     * @return 文件内容
     */
    QByteArray readFileFromZip(const QString &zipPath, const QString &internalPath) const;

    /**
     * @brief 解析document.xml中的图片信息
     * @param xmlContent document.xml内容
     * @param images 输出图片信息列表
     * @return 是否成功
     */
    bool parseDocumentXml(const QByteArray &xmlContent, QList<ImageInfo> &images) const;

    /**
     * @brief 解析drawing元素
     * @param reader XML读取器
     * @param images 输出图片信息列表
     * @return 是否成功
     */
    bool parseDrawingElement(QXmlStreamReader &reader, QList<ImageInfo> &images) const;

    /**
     * @brief 解析pic元素
     * @param reader XML读取器
     * @param images 输出图片信息列表
     * @return 是否成功
     */
    bool parsePicElement(QXmlStreamReader &reader, QList<ImageInfo> &images) const;

    /**
     * @brief 解析pict元素
     * @param reader XML读取器
     * @param images 输出图片信息列表
     * @return 是否成功
     */
    bool parsePictElement(QXmlStreamReader &reader, QList<ImageInfo> &images) const;

    /**
     * @brief 从关系文件中获取图片信息
     * @param zipPath ZIP文件路径
     * @param relationshipId 关系ID
     * @param imageInfo 图片信息（输出）
     * @return 是否成功
     */
    bool getImageFromRelationship(const QString &zipPath, const QString &relationshipId, ImageInfo &imageInfo) const;

    /**
     * @brief 解析关系文件
     * @param zipPath ZIP文件路径
     * @param relationshipId 关系ID
     * @param imagePath 图片路径（输出）
     * @param imageType 图片类型（输出）
     * @return 是否成功
     */
    bool parseRelationship(const QString &zipPath, const QString &relationshipId, QString &imagePath, QString &imageType) const;

    /**
     * @brief 提取图片数据
     * @param zipPath ZIP文件路径
     * @param imagePath 图片路径
     * @param imageData 图片数据（输出）
     * @return 是否成功
     */
    bool extractImageData(const QString &zipPath, const QString &imagePath, QByteArray &imageData) const;

    /**
     * @brief 获取图片在文档中的位置
     * @param reader XML读取器
     * @param position 位置（输出）
     * @return 是否成功
     */
    bool getImagePosition(QXmlStreamReader &reader, QRect &position) const;

    /**
     * @brief 获取图片尺寸
     * @param reader XML读取器
     * @param size 尺寸（输出）
     * @return 是否成功
     */
    bool getImageSize(QXmlStreamReader &reader, QSize &size) const;

    /**
     * @brief 获取图片尺寸
     * @param imageData 图片数据
     * @return 图片尺寸
     */
    QSize getImageSize(const QByteArray &imageData) const;

    /**
     * @brief 获取图片描述
     * @param reader XML读取器
     * @param description 描述（输出）
     * @return 是否成功
     */
    bool getImageDescription(QXmlStreamReader &reader, QString &description) const;

    /**
     * @brief 解析图片关系文件
     * @param relationshipsXml 关系XML内容
     * @return 图片关系映射
     */
    QMap<QString, QString> parseImageRelationships(const QByteArray &relationshipsXml) const;

    /**
     * @brief 从document.xml中提取图片引用
     * @param documentXml document.xml内容
     * @return 图片引用列表
     */
    QList<QString> extractImageReferences(const QByteArray &documentXml) const;

    /**
     * @brief 从图片数据创建图片信息
     * @param imageData 图片数据
     * @param imagePath 图片路径
     * @param position 图片位置
     * @return 图片信息
     */
    ImageInfo createImageInfoFromData(const QByteArray &imageData, const QString &imagePath, const QRect &position = QRect());

    /**
     * @brief 从DOCX文档中提取图片位置信息
     * @param zipPath ZIP文件路径
     * @param imageRefs 图片引用列表
     * @return 图片位置映射
     */
    QMap<QString, QRect> extractImagePositions(const QString &zipPath, const QStringList &imageRefs) const;

    /**
     * @brief 解析drawing元素获取位置信息
     * @param reader XML读取器
     * @return 位置信息
     */
    QRect parseDrawingPosition(QXmlStreamReader &reader) const;
    QRect parseDrawingPositionNew(QXmlStreamReader &reader) const;
    QRect parseAnchorPosition(QXmlStreamReader &reader) const;
    QRect parseInlinePosition(QXmlStreamReader &reader) const;

    /**
     * @brief 解析pict元素获取位置信息
     * @param reader XML读取器
     * @return 位置信息
     */
    QRect parsePictPosition(QXmlStreamReader &reader) const;

    /**
     * @brief 将EMU单位转换为像素
     * @param emu EMU值
     * @return 像素值
     */
    int emuToPixels(qint64 emu) const;

private:
    static const QStringList SUPPORTED_EXTENSIONS;
    static const QString DOCX_DOCUMENT_PATH;
    static const QString DOCX_RELATIONSHIPS_PATH;
    static const QString DOCX_MEDIA_PATH;
};

