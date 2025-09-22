/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-21 01:32:34
 * @LastEditors: seelights
 * @Description: PDF图片提取器
 * @FilePath: \ReportMason\tools\pdf\PdfImageExtractor.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include "../base/ImageExtractor.h"
#include "poppler-qt6.h" // 使用Qt6版本的Poppler
#include <memory> // 用于std::unique_ptr

/**
 * @brief PDF图片提取器
 *
 * 专门用于从PDF文件中提取图片内容
 */
class PdfImageExtractor : public ImageExtractor
{
    Q_OBJECT

public:
    explicit PdfImageExtractor(QObject* parent = nullptr);
    virtual ~PdfImageExtractor();

    // 实现基类接口
    bool isSupported(const QString& filePath) const override;
    QStringList getSupportedFormats() const override;
    ExtractStatus extractImages(const QString& filePath, QList<ImageInfo>& images) override;
    ExtractStatus extractImagesByType(const QString& filePath, const QString& imageType,
                                      QList<ImageInfo>& images) override;
    ExtractStatus extractImagesByPosition(const QString& filePath, const QRect& position,
                                          QList<ImageInfo>& images) override;
    int getImageCount(const QString& filePath) override;

protected:
    /**
     * @brief 解析PDF文件
     * @param filePath PDF文件路径
     * @param images 输出图片信息列表
     * @return 是否成功
     */
    bool parsePdfFile(const QString& filePath, QList<ImageInfo>& images);

    /**
     * @brief 提取页面中的图片
     * @param pageContent 页面内容
     * @param pageNumber 页码
     * @param images 输出图片信息列表
     * @return 是否成功
     */
    bool extractImagesFromPage(const QByteArray& pageContent, int pageNumber,
                               QList<ImageInfo>& images) const;

    /**
     * @brief 解析图片对象
     * @param imageData 图片数据
     * @param imageInfo 图片信息（输出）
     * @return 是否成功
     */
    bool parseImageObject(const QByteArray& imageData, ImageInfo& imageInfo) const;

    /**
     * @brief 获取图片在页面中的位置
     * @param pageContent 页面内容
     * @param imageIndex 图片索引
     * @param position 位置（输出）
     * @return 是否成功
     */
    bool getImagePosition(const QByteArray& pageContent, int imageIndex, QRect& position) const;

    /**
     * @brief 获取PDF图片尺寸
     * @param imageData 图片数据
     * @param size 尺寸（输出）
     * @return 是否成功
     */
    bool getPdfImageSize(const QByteArray& imageData, QSize& size) const;

    /**
     * @brief 检测PDF中的图片格式
     * @param imageData 图片数据
     * @return 图片格式
     */
    QString detectPdfImageFormat(const QByteArray& imageData) const;

    /**
     * @brief 解码PDF图片数据
     * @param encodedData 编码的图片数据
     * @param format 图片格式
     * @param decodedData 解码后的数据（输出）
     * @return 是否成功
     */
    bool decodePdfImageData(const QByteArray& encodedData, const QString& format,
                            QByteArray& decodedData) const;

    // Poppler相关方法
    bool parsePdfWithPoppler(const QString& filePath, QList<ImageInfo>& images);
    QImage renderPageWithPoppler(int pageNumber, int dpi = 150) const;
    bool loadPopplerDocument(const QString& filePath);
    void closePopplerDocument();

private:
    static const QStringList SUPPORTED_EXTENSIONS;
    
    // Poppler文档对象（Qt版本，使用unique_ptr）
    std::unique_ptr<Poppler::Document> m_popplerDocument;
    QString m_currentPdfPath;
};

