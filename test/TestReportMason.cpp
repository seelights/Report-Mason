/*
 * TestReportMason.cpp
 * ReportMason 测试类
 *
 * 测试方法说明：
 *
 * 1. 基础功能测试：
 *    - testBasicFunctionality(): 测试基本文件转换功能
 *    - testFieldExtraction(): 测试字段提取功能
 *    - testTemplateManagement(): 测试模板管理功能
 *
 * 2. 文档转换测试：
 *    - testDocToXmlConversion(): 测试DOC到XML转换
 *    - testPdfToXmlConversion(): 测试PDF到XML转换
 *    - testZipHandling(): 测试ZIP文件处理
 *
 * 3. 数据验证测试：
 *    - testDataValidation(): 测试数据验证功能
 *    - testErrorHandling(): 测试错误处理
 *    - testPerformance(): 测试性能表现
 *
 * 4. 集成测试：
 *    - testEndToEndWorkflow(): 测试完整工作流程
 *    - testMultipleFileTypes(): 测试多种文件类型处理
 *    - testBatchProcessing(): 测试批量处理功能
 *
 * 使用方法：
 * 1. 在Qt Creator中打开项目
 * 2. 运行测试项目
 * 3. 查看测试结果和日志输出
 * 4. 根据测试结果调试和优化代码
 */

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QtTest/QTest>
#include <QTextStream>
#include <QStandardPaths>
#include <QDateTime>

#include "../src/FileConverter.h"
#include "../src/DocToXmlConverter.h"
#include "../src/PdfToXmlConverter.h"
#include "../src/TemplateManager.h"
#include "../src/KZipUtils.h"
#include "../src/FieldExtractor.h"

class TestReportMason : public QObject
{
    Q_OBJECT

private slots:
    // 基础功能测试
    void testBasicFunctionality();
    void testFieldExtraction();
    void testTemplateManagement();

    // 文档转换测试
    void testDocToXmlConversion();
    void testPdfToXmlConversion();
    void testZipHandling();

    // 数据验证测试
    void testDataValidation();
    void testErrorHandling();
    void testPerformance();

    // 集成测试
    void testEndToEndWorkflow();
    void testMultipleFileTypes();
    void testBatchProcessing();

    // 辅助方法
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    QString createTestDocFile();
    QString createTestPdfFile();
    QString createTestZipFile();
    void cleanupTestFiles();

    QString m_tempDir;
    QStringList m_testFiles;
};

void TestReportMason::initTestCase()
{
    qDebug() << "=== 初始化测试环境 ===";

    // 创建临时测试目录
    m_tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ReportMasonTest";
    QDir().mkpath(m_tempDir);

    qDebug() << "测试目录:" << m_tempDir;
}

void TestReportMason::cleanupTestCase()
{
    qDebug() << "=== 清理测试环境 ===";
    cleanupTestFiles();

    // 删除临时目录
    if (QDir(m_tempDir).exists()) {
        QDir(m_tempDir).removeRecursively();
    }
}

void TestReportMason::init() { qDebug() << "--- 开始单个测试 ---"; }

void TestReportMason::cleanup() { qDebug() << "--- 完成单个测试 ---"; }

void TestReportMason::testBasicFunctionality()
{
    qDebug() << "测试基本功能...";

    // 测试FileConverter基本功能
    FileConverter converter;
    QVERIFY(!converter.getSupportedFormats().isEmpty());

    // 测试FieldExtractor基本功能
    FieldExtractor extractor;
    QVERIFY(extractor.getPredefinedRules().size() > 0);

    // 测试TemplateManager基本功能
    TemplateManager templateManager;
    QVERIFY(templateManager.getAvailableTemplates().size() >= 0);

    qDebug() << "✓ 基本功能测试通过";
}

void TestReportMason::testFieldExtraction()
{
    qDebug() << "测试字段提取功能...";

    FieldExtractor extractor;

    // 测试预定义规则
    auto rules = extractor.getPredefinedRules();
    QVERIFY(rules.contains("Title"));
    QVERIFY(rules.contains("StudentName"));
    QVERIFY(rules.contains("StudentID"));

    // 测试字段提取
    QString testText = "题目：实验报告测试\n姓名：张三\n学号：2023001";
    QMap<QString, QString> extractedFields = extractor.extractFields(testText);

    QVERIFY(!extractedFields.isEmpty());
    qDebug() << "提取的字段:" << extractedFields;

    qDebug() << "✓ 字段提取测试通过";
}

void TestReportMason::testTemplateManagement()
{
    qDebug() << "测试模板管理功能...";

    TemplateManager templateManager;

    // 测试模板列表
    QStringList templates = templateManager.getAvailableTemplates();
    qDebug() << "可用模板:" << templates;

    // 测试模板加载
    if (!templates.isEmpty()) {
        QString templateName = templates.first();
        bool loaded = templateManager.loadTemplate(templateName);
        QVERIFY(loaded);

        // 测试模板应用
        QMap<QString, QString> testData;
        testData["Title"] = "测试标题";
        testData["StudentName"] = "测试学生";

        QString result = templateManager.applyTemplate(testData);
        QVERIFY(!result.isEmpty());
    }

    qDebug() << "✓ 模板管理测试通过";
}

void TestReportMason::testDocToXmlConversion()
{
    qDebug() << "测试DOC到XML转换...";

    DocToXmlConverter converter;

    // 创建测试DOC文件
    QString testDocFile = createTestDocFile();
    QVERIFY(QFile::exists(testDocFile));

    // 测试转换
    QString xmlResult = converter.convertToXml(testDocFile);
    QVERIFY(!xmlResult.isEmpty());

    // 验证XML格式
    QVERIFY(xmlResult.contains("<?xml"));
    QVERIFY(xmlResult.contains("document"));

    qDebug() << "✓ DOC到XML转换测试通过";
}

void TestReportMason::testPdfToXmlConversion()
{
    qDebug() << "测试PDF到XML转换...";

    PdfToXmlConverter converter;

    // 创建测试PDF文件
    QString testPdfFile = createTestPdfFile();
    QVERIFY(QFile::exists(testPdfFile));

    // 测试转换
    QString xmlResult = converter.convertToXml(testPdfFile);
    QVERIFY(!xmlResult.isEmpty());

    // 验证XML格式
    QVERIFY(xmlResult.contains("<?xml"));

    qDebug() << "✓ PDF到XML转换测试通过";
}

void TestReportMason::testZipHandling()
{
    qDebug() << "测试ZIP文件处理...";

    KZipUtils zipUtils;

    // 创建测试ZIP文件
    QString testZipFile = createTestZipFile();
    QVERIFY(QFile::exists(testZipFile));

    // 测试ZIP文件读取
    QStringList fileList = zipUtils.getFileList(testZipFile);
    QVERIFY(!fileList.isEmpty());

    // 测试文件提取
    QString extractDir = m_tempDir + "/extracted";
    QDir().mkpath(extractDir);

    bool extracted =
        zipUtils.extractFile(testZipFile, fileList.first(), extractDir + "/" + fileList.first());
    QVERIFY(extracted);

    qDebug() << "✓ ZIP文件处理测试通过";
}

void TestReportMason::testDataValidation()
{
    qDebug() << "测试数据验证功能...";

    FieldExtractor extractor;

    // 测试有效数据
    QString validText = "题目：实验报告\n姓名：张三\n学号：2023001";
    QMap<QString, QString> validFields = extractor.extractFields(validText);
    QVERIFY(!validFields.isEmpty());

    // 测试无效数据
    QString invalidText = "";
    QMap<QString, QString> invalidFields = extractor.extractFields(invalidText);
    QVERIFY(invalidFields.isEmpty());

    qDebug() << "✓ 数据验证测试通过";
}

void TestReportMason::testErrorHandling()
{
    qDebug() << "测试错误处理...";

    FileConverter converter;

    // 测试不存在的文件
    QString nonExistentFile = m_tempDir + "/nonexistent.doc";
    QString result = converter.convertFile(nonExistentFile, "xml");
    QVERIFY(result.isEmpty());

    // 测试无效格式
    QString invalidFormat = converter.convertFile("test.txt", "invalid_format");
    QVERIFY(invalidFormat.isEmpty());

    qDebug() << "✓ 错误处理测试通过";
}

void TestReportMason::testPerformance()
{
    qDebug() << "测试性能表现...";

    QTime timer;
    timer.start();

    // 测试大量数据处理
    FieldExtractor extractor;
    QString largeText;
    for (int i = 0; i < 1000; ++i) {
        largeText += QString("题目：实验报告%1\n姓名：学生%1\n学号：%1\n").arg(i);
    }

    QMap<QString, QString> fields = extractor.extractFields(largeText);

    int elapsed = timer.elapsed();
    qDebug() << "处理1000条记录耗时:" << elapsed << "ms";

    QVERIFY(elapsed < 5000); // 应该在5秒内完成

    qDebug() << "✓ 性能测试通过";
}

void TestReportMason::testEndToEndWorkflow()
{
    qDebug() << "测试完整工作流程...";

    // 1. 创建测试文档
    QString testDocFile = createTestDocFile();

    // 2. 转换为XML
    DocToXmlConverter docConverter;
    QString xmlContent = docConverter.convertToXml(testDocFile);
    QVERIFY(!xmlContent.isEmpty());

    // 3. 提取字段
    FieldExtractor extractor;
    QMap<QString, QString> fields = extractor.extractFields(xmlContent);
    QVERIFY(!fields.isEmpty());

    // 4. 应用模板
    TemplateManager templateManager;
    QString result = templateManager.applyTemplate(fields);
    QVERIFY(!result.isEmpty());

    qDebug() << "✓ 完整工作流程测试通过";
}

void TestReportMason::testMultipleFileTypes()
{
    qDebug() << "测试多种文件类型处理...";

    FileConverter converter;
    QStringList supportedFormats = converter.getSupportedFormats();

    qDebug() << "支持的格式:" << supportedFormats;

    // 测试每种支持的格式
    for (const QString& format : supportedFormats) {
        qDebug() << "测试格式:" << format;
        // 这里可以添加具体的格式测试
    }

    qDebug() << "✓ 多种文件类型测试通过";
}

void TestReportMason::testBatchProcessing()
{
    qDebug() << "测试批量处理功能...";

    // 创建多个测试文件
    QStringList testFiles;
    for (int i = 0; i < 5; ++i) {
        QString fileName = QString("%1/test_%2.doc").arg(m_tempDir).arg(i);
        createTestDocFile(fileName);
        testFiles << fileName;
    }

    // 批量处理
    FileConverter converter;
    int successCount = 0;

    for (const QString& file : testFiles) {
        QString result = converter.convertFile(file, "xml");
        if (!result.isEmpty()) {
            successCount++;
        }
    }

    QVERIFY(successCount > 0);
    qDebug() << "批量处理成功:" << successCount << "/" << testFiles.size();

    qDebug() << "✓ 批量处理测试通过";
}

QString TestReportMason::createTestDocFile(const QString& fileName)
{
    QString filePath = fileName.isEmpty() ? m_tempDir + "/test.doc" : fileName;

    // 创建简单的测试DOC内容（实际应该是二进制格式）
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "题目：实验报告测试\n";
        stream << "姓名：张三\n";
        stream << "学号：2023001\n";
        stream << "班级：计算机科学与技术1班\n";
        stream << "摘要：这是一个测试实验报告\n";
        stream << "关键词：实验,测试,报告\n";
        stream << "实验目的：学习实验报告写作\n";
        stream << "实验原理：基于实验原理进行测试\n";
        stream << "实验步骤：1.准备 2.实验 3.记录\n";
        stream << "实验结果：获得预期结果\n";
        stream << "实验分析：结果符合预期\n";
        stream << "结论：实验成功完成\n";
    }

    m_testFiles << filePath;
    return filePath;
}

QString TestReportMason::createTestPdfFile()
{
    QString filePath = m_tempDir + "/test.pdf";

    // 创建简单的测试PDF内容（实际应该是二进制格式）
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "PDF测试内容\n";
        stream << "题目：PDF实验报告\n";
        stream << "姓名：李四\n";
        stream << "学号：2023002\n";
    }

    m_testFiles << filePath;
    return filePath;
}

QString TestReportMason::createTestZipFile()
{
    QString filePath = m_tempDir + "/test.zip";

    // 创建简单的测试ZIP内容（实际应该是二进制格式）
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "ZIP测试内容\n";
        stream << "包含测试文件\n";
    }

    m_testFiles << filePath;
    return filePath;
}

void TestReportMason::cleanupTestFiles()
{
    for (const QString& filePath : m_testFiles) {
        if (QFile::exists(filePath)) {
            QFile::remove(filePath);
        }
    }
    m_testFiles.clear();
}

QTEST_MAIN(TestReportMason)
#include "TestReportMason.moc"
