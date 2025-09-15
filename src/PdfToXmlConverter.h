#pragma once

#include "FileConverter.h"
#include <QStringList>
#include <QByteArray>
#include <QMap>
#include <QRegularExpression>

/**
 * @brief PDF文档转XML转换器
 * 
 * 专门处理PDF文档(.pdf)到XML格式的转换
 * 支持从PDF文档中提取文本内容、表单字段和元数据
 * 并转换为项目所需的XML格式
 * 
 * 注意：由于PDF格式的复杂性，此转换器主要处理文本提取和智能字段识别
 * 对于复杂的PDF表单，建议使用专门的PDF处理库
 */
class PdfToXmlConverter : public FileConverter
{
    Q_OBJECT

public:
    explicit PdfToXmlConverter(QObject *parent = nullptr);
    ~PdfToXmlConverter();

    // 实现基类虚函数
    bool isSupported(const QString &filePath) const override;
    ConvertStatus extractFields(const QString &filePath, QMap<QString, FieldInfo> &fields) override;
    ConvertStatus convertToXml(const QMap<QString, FieldInfo> &fields, QByteArray &xmlOutput) override;
    QStringList getSupportedFormats() const override;

    /**
     * @brief 从PDF文件中提取文本内容
     * @param pdfPath PDF文件路径
     * @param textContent 输出的文本内容
     * @return 转换状态
     */
    ConvertStatus extractTextContent(const QString &pdfPath, QString &textContent);

    /**
     * @brief 从PDF文件中提取表单字段（如果存在）
     * @param pdfPath PDF文件路径
     * @param formFields 输出的表单字段
     * @return 转换状态
     */
    ConvertStatus extractFormFields(const QString &pdfPath, QMap<QString, FieldInfo> &formFields);

    /**
     * @brief 从PDF文件中提取元数据
     * @param pdfPath PDF文件路径
     * @param metadata 输出的元数据
     * @return 转换状态
     */
    ConvertStatus extractMetadata(const QString &pdfPath, QMap<QString, QString> &metadata);

    /**
     * @brief 设置PDF文本提取选项
     * @param preserveLayout 是否保持布局
     * @param extractImages 是否提取图片
     */
    void setExtractionOptions(bool preserveLayout = true, bool extractImages = false);

protected:
    /**
     * @brief 从PDF原始内容中提取文本（简单实现）
     * @param pdfData PDF文件数据
     * @param textContent 输出的文本内容
     * @return 是否提取成功
     */
    bool extractTextFromPdfData(const QByteArray &pdfData, QString &textContent);

    /**
     * @brief 解析PDF流对象中的文本
     * @param streamData 流数据
     * @param textContent 输出的文本内容
     * @return 是否解析成功
     */
    bool parsePdfStream(const QByteArray &streamData, QString &textContent);

    /**
     * @brief 从PDF文本中智能识别字段
     * @param textContent PDF文本内容
     * @param fields 输出的字段映射
     */
    void extractFieldsFromPdfText(const QString &textContent, QMap<QString, FieldInfo> &fields);

    /**
     * @brief 处理PDF文本的特殊格式
     * @param text 原始PDF文本
     * @return 清理后的文本
     */
    QString processPdfText(const QString &text) const;

    /**
     * @brief 识别PDF中的表格结构
     * @param textContent PDF文本内容
     * @param tables 输出的表格数据
     * @return 是否识别到表格
     */
    bool extractTables(const QString &textContent, QMap<QString, QStringList> &tables);

    /**
     * @brief 识别PDF中的列表结构
     * @param textContent PDF文本内容
     * @param lists 输出的列表数据
     * @return 是否识别到列表
     */
    bool extractLists(const QString &textContent, QMap<QString, QStringList> &lists);

private:
    static const QStringList SUPPORTED_EXTENSIONS; ///< 支持的扩展名
    
    bool m_preserveLayout;  ///< 是否保持布局
    bool m_extractImages;   ///< 是否提取图片
    
    // PDF文本提取的正则表达式模式
    static const QRegularExpression TEXT_PATTERN;
    static const QRegularExpression FORM_FIELD_PATTERN;
    static const QRegularExpression METADATA_PATTERN;
    
    // 实验报告相关的字段识别模式
    static const QMap<QString, QRegularExpression> FIELD_PATTERNS;
};
