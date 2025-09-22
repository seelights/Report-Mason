/*
 * @Author: seelights
 * @Date: 2025-01-27 22:00:00
 * @LastEditTime: 2025-09-21 01:09:43
 * @LastEditors: seelights
 * @Description: 无损文档转换器 - 保持完整格式和位置信息
 * @FilePath: \ReportMason\src\LosslessDocumentConverter.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include "QtCompat.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QRect>
#include <QFont>
#include <QColor>
#include <QMap>
#include <QList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

/**
 * @brief 文档元素类型
 */
enum class DocumentElementType {
    TEXT,           ///< 文本
    IMAGE,          ///< 图片
    TABLE,          ///< 表格
    CHART,          ///< 图表
    SHAPE,          ///< 形状
    HYPERLINK,      ///< 超链接
    FOOTNOTE,       ///< 脚注
    HEADER,         ///< 页眉
    FOOTER,         ///< 页脚
    PAGE_BREAK,     ///< 分页符
    LINE_BREAK,     ///< 换行符
    PARAGRAPH,      ///< 段落
    SIGNATURE       ///< 数字签名
};

/**
 * @brief 格式信息结构
 */
struct FormatInfo {
    QFont font;                 ///< 字体信息
    QColor textColor;           ///< 文字颜色
    QColor backgroundColor;     ///< 背景颜色
    bool bold;                  ///< 粗体
    bool italic;                ///< 斜体
    bool underline;             ///< 下划线
    bool strikethrough;         ///< 删除线
    int fontSize;               ///< 字号
    QString fontFamily;         ///< 字体族
    Qt::Alignment alignment;    ///< 对齐方式
    double lineSpacing;         ///< 行间距
    double paragraphSpacing;    ///< 段落间距
    int leftIndent;             ///< 左缩进
    int rightIndent;            ///< 右缩进
    int firstLineIndent;        ///< 首行缩进
    
    FormatInfo() : bold(false), italic(false), underline(false), strikethrough(false),
                   fontSize(12), alignment(Qt::AlignLeft), lineSpacing(1.0), 
                   paragraphSpacing(0.0), leftIndent(0), rightIndent(0), firstLineIndent(0) {}
};

/**
 * @brief 位置信息结构
 */
struct PositionInfo {
    QRect boundingBox;          ///< 边界框
    int pageNumber;             ///< 页码
    int zOrder;                 ///< 层级顺序
    bool isInline;              ///< 是否为内联元素
    QString anchorId;           ///< 锚点ID（用于关联）
    QList<QString> relatedIds;  ///< 相关元素ID列表
    
    PositionInfo() : pageNumber(1), zOrder(0), isInline(false) {}
};

/**
 * @brief 文档元素结构
 */
struct DocumentElement {
    QString id;                     ///< 唯一标识符
    DocumentElementType type;       ///< 元素类型
    QString content;                ///< 内容（文本或引用）
    FormatInfo format;              ///< 格式信息
    PositionInfo position;          ///< 位置信息
    QMap<QString, QString> attributes; ///< 额外属性
    QByteArray binaryData;          ///< 二进制数据（图片等）
    QString mimeType;               ///< MIME类型
    QList<DocumentElement> children; ///< 子元素
    
    DocumentElement() : type(DocumentElementType::TEXT) {}
};

/**
 * @brief 无损文档转换器
 * 
 * 这个类实现了真正的无损文档转换，保持：
 * 1. 完整的格式信息（字体、颜色、样式等）
 * 2. 精确的位置信息（坐标、层级、关系）
 * 3. 元素间的结构关系
 * 4. 嵌入元素的关联关系
 */
class LosslessDocumentConverter : public QObject
{
    Q_OBJECT

public:
    explicit LosslessDocumentConverter(QObject *parent = nullptr);
    ~LosslessDocumentConverter();

    /**
     * @brief 支持的输入格式
     */
    enum class InputFormat {
        DOCX,   ///< Microsoft Word文档
        PDF,    ///< PDF文档
        UNKNOWN ///< 未知格式
    };

    /**
     * @brief 转换状态
     */
    enum class ConvertStatus {
        SUCCESS,        ///< 转换成功
        FILE_NOT_FOUND, ///< 文件不存在
        INVALID_FORMAT, ///< 格式不支持
        PARSE_ERROR,    ///< 解析错误
        WRITE_ERROR,    ///< 写入错误
        UNKNOWN_ERROR   ///< 未知错误
    };

    /**
     * @brief 将文档转换为无损XML
     * @param filePath 输入文件路径
     * @param outputPath 输出XML文件路径
     * @return 转换状态
     */
    ConvertStatus convertToLosslessXml(const QString &filePath, const QString &outputPath);

    /**
     * @brief 将文档转换为无损XML（返回字节数组）
     * @param filePath 输入文件路径
     * @return XML字节数组
     */
    QByteArray convertToLosslessXmlByteArray(const QString &filePath);

    /**
     * @brief 从无损XML还原为原格式
     * @param xmlPath XML文件路径
     * @param outputPath 输出文件路径
     * @param targetFormat 目标格式
     * @return 转换状态
     */
    ConvertStatus restoreFromLosslessXml(const QString &xmlPath, const QString &outputPath, InputFormat targetFormat);

    /**
     * @brief 检查文件格式是否支持
     * @param filePath 文件路径
     * @return 是否支持
     */
    bool isSupported(const QString &filePath) const;

    /**
     * @brief 获取支持的格式列表
     * @return 格式列表
     */
    QStringList getSupportedFormats() const;

signals:
    /**
     * @brief 转换进度信号
     * @param progress 进度百分比 (0-100)
     * @param message 进度消息
     */
    void conversionProgress(int progress, const QString &message);

    /**
     * @brief 转换完成信号
     * @param status 转换状态
     * @param message 结果消息
     */
    void conversionFinished(ConvertStatus status, const QString &message);

private:
    /**
     * @brief 解析DOCX文档
     * @param filePath DOCX文件路径
     * @param elements 解析出的元素列表
     * @return 解析状态
     */
    ConvertStatus parseDocxDocument(const QString &filePath, QList<DocumentElement> &elements);

    /**
     * @brief 解析PDF文档
     * @param filePath PDF文件路径
     * @param elements 解析出的元素列表
     * @return 解析状态
     */
    ConvertStatus parsePdfDocument(const QString &filePath, QList<DocumentElement> &elements);

    /**
     * @brief 将元素列表写入XML
     * @param elements 元素列表
     * @param writer XML写入器
     * @return 写入状态
     */
    ConvertStatus writeElementsToXml(const QList<DocumentElement> &elements, QXmlStreamWriter &writer);

    /**
     * @brief 从XML读取元素列表
     * @param reader XML读取器
     * @param elements 读取的元素列表
     * @return 读取状态
     */
    ConvertStatus readElementsFromXml(QXmlStreamReader &reader, QList<DocumentElement> &elements);

    /**
     * @brief 生成元素ID
     * @param type 元素类型
     * @param index 索引
     * @return 唯一ID
     */
    QString generateElementId(DocumentElementType type, int index);

    /**
     * @brief 建立元素关系
     * @param elements 元素列表
     */
    void establishElementRelationships(QList<DocumentElement> &elements);

    /**
     * @brief 验证转换完整性
     * @param originalPath 原始文件路径
     * @param xmlPath XML文件路径
     * @return 是否完整
     */
    bool validateConversionIntegrity(const QString &originalPath, const QString &xmlPath);

    // 辅助方法声明
    void parseParagraphFormat(QXmlStreamReader &reader, FormatInfo &format);
    void parseRunFormat(QXmlStreamReader &reader, FormatInfo &format);
    void parseDrawingElement(QXmlStreamReader &reader, DocumentElement &element, const QString &filePath);
    void parseTableElement(QXmlStreamReader &reader, DocumentElement &element);
    void parsePdfTextFormat(void *textBox, FormatInfo &format); // 使用void*避免Poppler类型问题
    void writeElementToXml(const DocumentElement &element, QXmlStreamWriter &writer);
    void extractTextBoxFormatInfo(void *textBox, DocumentElement &element);
    
    // PDF内容检测方法
    QList<QRect> detectImageRegions(const QImage &pageImage);
    QList<QRect> detectTableRegions(const QString &pageText, void *page);
    QList<QRect> detectChartRegions(void *page);
    QRect floodFillRegion(const QImage &image, int startX, int startY, QVector<QVector<bool>> &visited);

private:
    QMap<QString, InputFormat> m_supportedFormats;
    int m_elementCounter;
};

/**
 * @brief 文档元素比较器（用于排序）
 */
struct DocumentElementComparator {
    bool operator()(const DocumentElement &a, const DocumentElement &b) const {
        // 对于DOCX文档，按页码和元素在文档中的出现顺序排序
        // 对于PDF文档，按页码、Y坐标、X坐标排序
        if (a.position.pageNumber != b.position.pageNumber) {
            return a.position.pageNumber < b.position.pageNumber;
        }
        
        // 如果位置信息有效（非零），按位置排序
        if (!a.position.boundingBox.isNull() && !b.position.boundingBox.isNull()) {
            if (a.position.boundingBox.y() != b.position.boundingBox.y()) {
                return a.position.boundingBox.y() < b.position.boundingBox.y();
            }
            return a.position.boundingBox.x() < b.position.boundingBox.x();
        }
        
        // 如果位置信息无效，按ID中的时间戳排序（保持文档顺序）
        // ID格式: type_timestamp_counter
        QStringList aParts = a.id.split(QS("_"));
        QStringList bParts = b.id.split(QS("_"));
        if (aParts.size() >= 2 && bParts.size() >= 2) {
            bool ok1, ok2;
            qint64 aTime = aParts[1].toLongLong(&ok1);
            qint64 bTime = bParts[1].toLongLong(&ok2);
            if (ok1 && ok2) {
                if (aTime != bTime) {
                    return aTime < bTime;
                }
                // 如果时间戳相同，按计数器排序
                if (aParts.size() >= 3 && bParts.size() >= 3) {
                    int aCounter = aParts[2].toInt(&ok1);
                    int bCounter = bParts[2].toInt(&ok2);
                    if (ok1 && ok2) {
                        return aCounter < bCounter;
                    }
                }
            }
        }
        
        // 最后的备选方案：按ID字符串排序
        return a.id < b.id;
    }
};
