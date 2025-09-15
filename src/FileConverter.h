#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>
#include <QByteArray>
#include <QFileInfo>
#include <QDir>

/**
 * @brief 文件转换器基类
 * 
 * 定义了统一的文件转换接口，支持从各种格式（Word、PDF等）
 * 转换到项目所需的XML格式，用于模板填充和内容管理
 */
class FileConverter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 支持的输入文件格式
     */
    enum class InputFormat {
        DOCX,   ///< Microsoft Word文档 (.docx)
        PDF,    ///< PDF文档 (.pdf)
        PPTX,   ///< Microsoft PowerPoint演示文稿 (.pptx)
        UNKNOWN ///< 未知格式
    };

    /**
     * @brief 转换结果状态
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
     * @brief 字段信息结构
     */
    struct FieldInfo {
        QString name;           ///< 字段名称
        QString content;        ///< 字段内容
        QString description;    ///< 字段描述
        bool required;          ///< 是否必填
        QStringList keywords;   ///< 关联关键词
        
        FieldInfo() : required(false) {}
        FieldInfo(const QString& n, const QString& c = QString(), bool r = false) 
            : name(n), content(c), required(r) {}
    };

    explicit FileConverter(QObject *parent = nullptr);
    virtual ~FileConverter();

    /**
     * @brief 检查是否支持指定的文件格式
     * @param filePath 文件路径
     * @return 是否支持该格式
     */
    virtual bool isSupported(const QString &filePath) const = 0;

    /**
     * @brief 从文件中提取字段信息
     * @param filePath 输入文件路径
     * @param fields 输出的字段列表
     * @return 转换状态
     */
    virtual ConvertStatus extractFields(const QString &filePath, QMap<QString, FieldInfo> &fields) = 0;

    /**
     * @brief 将字段信息转换为XML格式
     * @param fields 字段信息映射
     * @param xmlOutput 输出的XML内容
     * @return 转换状态
     */
    virtual ConvertStatus convertToXml(const QMap<QString, FieldInfo> &fields, QByteArray &xmlOutput) = 0;

    /**
     * @brief 从文件直接转换到XML（一步完成）
     * @param inputPath 输入文件路径
     * @param outputPath 输出XML文件路径
     * @return 转换状态
     */
    virtual ConvertStatus convertFileToXml(const QString &inputPath, const QString &outputPath);

    /**
     * @brief 获取支持的输入格式列表
     * @return 支持的格式列表
     */
    virtual QStringList getSupportedFormats() const = 0;

    /**
     * @brief 获取最后错误信息
     * @return 错误信息字符串
     */
    QString getLastError() const;

    /**
     * @brief 设置模板配置
     * @param config 模板配置JSON对象
     */
    void setTemplateConfig(const QJsonObject &config);

    /**
     * @brief 获取模板配置
     * @return 模板配置JSON对象
     */
    QJsonObject getTemplateConfig() const;

    /**
     * @brief 验证字段内容的完整性
     * @param fields 字段信息映射
     * @return 验证结果，空字符串表示验证通过
     */
    QString validateFields(const QMap<QString, FieldInfo> &fields) const;

    /**
     * @brief 从文本内容智能抽取字段
     * @param text 输入文本
     * @param fields 输出的字段映射
     * @return 是否抽取成功
     */
    virtual bool extractFieldsFromText(const QString &text, QMap<QString, FieldInfo> &fields) const;

protected:
    /**
     * @brief 设置错误信息
     * @param error 错误信息
     */
    void setLastError(const QString &error);

    /**
     * @brief 生成标准XML头部
     * @return XML头部字符串
     */
    QString generateXmlHeader() const;

    /**
     * @brief 生成XML字段节点
     * @param field 字段信息
     * @return XML节点字符串
     */
    QString generateXmlFieldNode(const FieldInfo &field) const;

    /**
     * @brief 验证文件路径
     * @param filePath 文件路径
     * @return 验证结果
     */
    bool validateFilePath(const QString &filePath) const;

    /**
     * @brief 清理和标准化文本内容
     * @param text 输入文本
     * @return 清理后的文本
     */
    QString cleanText(const QString &text) const;

private:
    QString m_lastError;        ///< 最后错误信息
    QJsonObject m_templateConfig; ///< 模板配置
};

/**
 * @brief 获取文件格式枚举
 * @param filePath 文件路径
 * @return 文件格式
 */
FileConverter::InputFormat getFileFormat(const QString &filePath);

/**
 * @brief 获取格式对应的MIME类型
 * @param format 文件格式
 * @return MIME类型字符串
 */
QString getMimeType(FileConverter::InputFormat format);

/**
 * @brief 获取格式对应的文件扩展名
 * @param format 文件格式
 * @return 扩展名列表
 */
QStringList getFileExtensions(FileConverter::InputFormat format);
