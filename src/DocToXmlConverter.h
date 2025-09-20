#pragma once

#include "ChartExtractor.h"
#include "FileConverter.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QByteArray>
#include <QStringList>
#include <QList>
#include <QRect>

// 前向声明
class DocxImageExtractor;
class DocxTableExtractor;
class DocxChartExtractor;

/**
 * @brief Word文档转XML转换器
 *
 * 专门处理Microsoft Word文档(.docx)到XML格式的转换
 * 支持从Word文档中提取结构化内容控件(SDT)和文本内容
 * 并转换为项目所需的XML格式
 */
class DocToXmlConverter : public FileConverter
{
    Q_OBJECT
    using ExtractStatus = ChartExtractor::ExtractStatus;

public:
    explicit DocToXmlConverter(QObject* parent = nullptr);
    ~DocToXmlConverter();

    // 实现基类虚函数
    bool isSupported(const QString& filePath) const override;
    ConvertStatus extractFields(const QString& filePath, QMap<QString, FieldInfo>& fields) override;
    ConvertStatus convertToXml(const QMap<QString, FieldInfo>& fields,
                               QByteArray& xmlOutput) override;
    QStringList getSupportedFormats() const override;

    /**
     * @brief 从DOCX文件中提取内容控件(SDT)字段
     * @param docxPath DOCX文件路径
     * @param fields 输出的字段映射
     * @return 转换状态
     */
    ConvertStatus extractSdtFields(const QString& docxPath, QMap<QString, FieldInfo>& fields);

    /**
     * @brief 从DOCX文件中提取文本内容
     * @param docxPath DOCX文件路径
     * @param textContent 输出的文本内容
     * @return 转换状态
     */
    ConvertStatus extractTextContent(const QString& docxPath, QString& textContent);

    /**
     * @brief 创建填充后的DOCX文件
     * @param templatePath 模板DOCX文件路径
     * @param fields 要填充的字段数据
     * @param outputPath 输出DOCX文件路径
     * @return 转换状态
     */
    ConvertStatus createFilledDocx(const QString& templatePath,
                                   const QMap<QString, FieldInfo>& fields,
                                   const QString& outputPath);

protected:
    /**
     * @brief 从ZIP压缩包中读取XML文件
     * @param zipPath ZIP文件路径
     * @param internalPath 内部XML文件路径
     * @return XML内容
     */
    QByteArray readXmlFromZip(const QString& zipPath, const QString& internalPath);

    /**
     * @brief 解析DOCX的document.xml文件
     * @param xmlContent XML内容
     * @param fields 输出的字段映射
     * @return 是否解析成功
     */
    bool parseDocumentXml(const QByteArray& xmlContent, QMap<QString, FieldInfo>& fields);

    /**
     * @brief 解析SDT(结构化文档标签)元素
     * @param reader XML读取器
     * @param fields 输出的字段映射
     * @return 是否解析成功
     */
    bool parseSdtElement(QXmlStreamReader& reader, QMap<QString, FieldInfo>& fields);

    /**
     * @brief 提取SDT标签内容
     * @param reader XML读取器
     * @return 标签内容
     */
    QString extractSdtContent(QXmlStreamReader& reader);

    /**
     * @brief 填充SDT字段内容
     * @param xmlContent 原始XML内容
     * @param fields 字段数据
     * @return 填充后的XML内容
     */
    QByteArray fillSdtFields(const QByteArray& xmlContent, const QMap<QString, FieldInfo>& fields);

    /**
     * @brief 创建填充后的ZIP文件
     * @param templatePath 模板文件路径
     * @param modifiedXml 修改后的document.xml内容
     * @param outputPath 输出文件路径
     * @return 是否创建成功
     */
    bool createModifiedZip(const QString& templatePath, const QByteArray& modifiedXml,
                           const QString& outputPath);

    /**
     * @brief 从文本段落中智能识别字段
     * @param textContent 文本内容
     * @param fields 输出的字段映射
     */
    void extractFieldsFromParagraphs(const QString& textContent, QMap<QString, FieldInfo>& fields);

    /**
     * @brief 处理Word文档中的特殊格式
     * @param text 原始文本
     * @return 清理后的文本
     */
    QString processWordFormatting(const QString& text) const;

    /**
     * @brief 提取DOCX文档中的所有内容（文本、图片、表格、图表）
     * @param docxPath DOCX文件路径
     * @param fields 输出的字段映射
     * @return 转换状态
     */
    ConvertStatus extractAllContent(const QString& docxPath, QMap<QString, FieldInfo>& fields);

    /**
     * @brief 提取DOCX文档中的图片内容
     * @param docxPath DOCX文件路径
     * @param fields 输出的字段映射
     * @return 转换状态
     */
    ConvertStatus extractImageContent(const QString& docxPath, QMap<QString, FieldInfo>& fields);

    /**
     * @brief 提取DOCX文档中的表格内容
     * @param docxPath DOCX文件路径
     * @param fields 输出的字段映射
     * @return 转换状态
     */
    ConvertStatus extractTableContent(const QString& docxPath, QMap<QString, FieldInfo>& fields);

    /**
     * @brief 提取DOCX文档中的图表内容
     * @param docxPath DOCX文件路径
     * @param fields 输出的字段映射
     * @return 转换状态
     */
    ConvertStatus extractChartContent(const QString& docxPath, QMap<QString, FieldInfo>& fields);

private:
    static const QString DOCX_DOCUMENT_PATH;       ///< DOCX内部document.xml路径
    static const QString DOCX_STYLES_PATH;         ///< DOCX内部styles.xml路径
    static const QStringList SUPPORTED_EXTENSIONS; ///< 支持的扩展名

    // 内容提取器
    DocxImageExtractor* m_imageExtractor; ///< 图片提取器
    DocxTableExtractor* m_tableExtractor; ///< 表格提取器
    DocxChartExtractor* m_chartExtractor; ///< 图表提取器
    
    // 当前处理的文件路径
    QString m_currentFilePath; ///< 当前处理的文件路径
};
