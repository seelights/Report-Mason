#include "TestWidget.h"
#include "DocToXmlConverter.h"
#include "PdfToXmlConverter.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextStream>

TestWidget::TestWidget(QWidget *parent)
    : QWidget(parent)
    , m_logSystem(new LogSystem(this))
    , m_fieldExtractor(new FieldExtractor(this))
    , m_templateManager(new TemplateManager(this))
{
    setupUI();
    setupConnections();
    
    // 初始化日志系统
    m_logSystem->setLogWidget(m_logEdit);
    m_logSystem->setLogFile(QStringLiteral("test_results.log"));
    m_logSystem->setLogLevel(LogSystem::INFO);
    
    // 初始化测试选项
    m_testCombo->addItem(QStringLiteral("所有测试"));
    m_testCombo->addItem(QStringLiteral("字段提取器测试"));
    m_testCombo->addItem(QStringLiteral("模板管理器测试"));
    m_testCombo->addItem(QStringLiteral("文件转换器测试"));
    m_testCombo->addItem(QStringLiteral("日志系统测试"));
    
    m_logSystem->info(QStringLiteral("测试工具已启动"));
}

TestWidget::~TestWidget()
{
    m_logSystem->info(QStringLiteral("测试工具已关闭"));
}

void TestWidget::setupUI()
{
    setWindowTitle(QStringLiteral("测试工具"));
    setMinimumSize(1000, 700);
    
    // 创建主分割器
    m_mainSplitter = new QSplitter(Qt::Vertical, this);
    
    // 测试控制区域
    m_controlGroup = new QGroupBox(QStringLiteral("测试控制"), m_mainSplitter);
    QVBoxLayout *controlLayout = new QVBoxLayout(m_controlGroup);
    
    QHBoxLayout *controlBtnLayout = new QHBoxLayout();
    m_runTestsBtn = new QPushButton(QStringLiteral("运行测试"), m_controlGroup);
    m_clearResultsBtn = new QPushButton(QStringLiteral("清空结果"), m_controlGroup);
    m_saveResultsBtn = new QPushButton(QStringLiteral("保存结果"), m_controlGroup);
    m_testCombo = new QComboBox(m_controlGroup);
    
    controlBtnLayout->addWidget(m_testCombo);
    controlBtnLayout->addWidget(m_runTestsBtn);
    controlBtnLayout->addWidget(m_clearResultsBtn);
    controlBtnLayout->addWidget(m_saveResultsBtn);
    controlBtnLayout->addStretch();
    
    controlLayout->addLayout(controlBtnLayout);
    
    // 结果显示区域
    m_resultsGroup = new QGroupBox(QStringLiteral("测试结果"), m_mainSplitter);
    QVBoxLayout *resultsLayout = new QVBoxLayout(m_resultsGroup);
    
    m_resultsEdit = new QTextEdit(m_resultsGroup);
    m_resultsEdit->setReadOnly(true);
    m_resultsEdit->setPlaceholderText(QStringLiteral("测试结果将显示在这里..."));
    
    resultsLayout->addWidget(m_resultsEdit);
    
    // 日志区域
    m_logGroup = new QGroupBox(QStringLiteral("日志"), m_mainSplitter);
    QVBoxLayout *logLayout = new QVBoxLayout(m_logGroup);
    
    m_logEdit = new QTextEdit(m_logGroup);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumHeight(200);
    
    logLayout->addWidget(m_logEdit);
    
    // 状态栏
    QHBoxLayout *statusLayout = new QHBoxLayout();
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_statusLabel = new QLabel(QStringLiteral("就绪"));
    
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_progressBar);
    
    // 设置分割器比例
    m_mainSplitter->addWidget(m_controlGroup);
    m_mainSplitter->addWidget(m_resultsGroup);
    m_mainSplitter->addWidget(m_logGroup);
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 2);
    m_mainSplitter->setStretchFactor(2, 1);
    
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_mainSplitter);
    mainLayout->addLayout(statusLayout);
}

void TestWidget::setupConnections()
{
    connect(m_runTestsBtn, &QPushButton::clicked, this, &TestWidget::runTests);
    connect(m_clearResultsBtn, &QPushButton::clicked, this, &TestWidget::clearResults);
    connect(m_saveResultsBtn, &QPushButton::clicked, this, &TestWidget::saveResults);
}

void TestWidget::runTests()
{
    updateStatus(QStringLiteral("正在运行测试..."));
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);
    
    m_resultsEdit->clear();
    m_resultsEdit->append(QStringLiteral("=== 测试开始 ===\n"));
    
    QString selectedTest = m_testCombo->currentText();
    
    if (selectedTest == QStringLiteral("所有测试") || selectedTest == QStringLiteral("字段提取器测试")) {
        testFieldExtractor();
    }
    
    if (selectedTest == QStringLiteral("所有测试") || selectedTest == QStringLiteral("模板管理器测试")) {
        testTemplateManager();
    }
    
    if (selectedTest == QStringLiteral("所有测试") || selectedTest == QStringLiteral("文件转换器测试")) {
        testFileConverter();
    }
    
    if (selectedTest == QStringLiteral("所有测试") || selectedTest == QStringLiteral("日志系统测试")) {
        testLogSystem();
    }
    
    m_resultsEdit->append(QStringLiteral("\n=== 测试完成 ==="));
    updateStatus(QStringLiteral("测试完成"));
    m_progressBar->setVisible(false);
}

void TestWidget::clearResults()
{
    m_resultsEdit->clear();
    updateStatus(QStringLiteral("结果已清空"));
}

void TestWidget::saveResults()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("保存测试结果"),
        QStringLiteral("test_results.txt"),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)")
    );
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            // Qt 6中QTextStream默认使用UTF-8编码，不需要设置setCodec
            stream << m_resultsEdit->toPlainText();
            file.close();
            updateStatus(QStringLiteral("结果已保存"));
            m_logSystem->info(QStringLiteral("测试结果已保存到: ") + fileName);
        }
    }
}

void TestWidget::updateStatus(const QString &message)
{
    m_statusLabel->setText(message);
    m_logSystem->debug(message);
}

void TestWidget::testFieldExtractor()
{
    m_resultsEdit->append(QStringLiteral("\n--- 字段提取器测试 ---"));
    m_logSystem->info(QStringLiteral("开始字段提取器测试"));
    
    // 测试数据
    QString testContent = QStringLiteral(
        "实验报告\n"
        "学生姓名: 张三\n"
        "学生学号: 2021001\n"
        "班级: 计算机科学与技术1班\n"
        "实验目的: 学习Qt编程\n"
        "实验原理: Qt是一个跨平台的C++应用程序开发框架\n"
        "实验步骤: 1. 安装Qt 2. 创建项目 3. 编写代码\n"
        "实验结果: 成功创建了测试程序\n"
        "实验分析: 通过本次实验，掌握了Qt的基本使用方法\n"
        "结论: 实验完成，达到了预期目标\n"
    );
    
    // 测试字段提取
    QMap<QString, QString> fields = m_fieldExtractor->extractFields(testContent);
    
    m_resultsEdit->append(QStringLiteral("提取到的字段数量: ") + QString::number(fields.size()));
    
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        m_resultsEdit->append(QStringLiteral("  %1: %2").arg(it.key(), it.value()));
    }
    
    m_logSystem->info(QStringLiteral("字段提取器测试完成"));
}

void TestWidget::testTemplateManager()
{
    m_resultsEdit->append(QStringLiteral("\n--- 模板管理器测试 ---"));
    m_logSystem->info(QStringLiteral("开始模板管理器测试"));
    
    // 测试模板搜索
    QList<TemplateInfo> templates = m_templateManager->searchTemplates(QStringLiteral("test"));
    m_resultsEdit->append(QStringLiteral("搜索到的模板数量: ") + QString::number(templates.size()));
    
    // 测试模板导入
    QString templateName = QStringLiteral("测试模板");
    QString description = QStringLiteral("这是一个测试模板");
    QStringList tags = {QStringLiteral("测试"), QStringLiteral("示例")};
    
    // 创建一个临时模板文件用于测试
    QString tempTemplatePath = QDir::tempPath() + QStringLiteral("/test_template.xml");
    QFile tempFile(tempTemplatePath);
    if (tempFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&tempFile);
        stream << QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        stream << QStringLiteral("<template>\n");
        stream << QStringLiteral("  <name>") << templateName << QStringLiteral("</name>\n");
        stream << QStringLiteral("  <description>") << description << QStringLiteral("</description>\n");
        stream << QStringLiteral("</template>\n");
        tempFile.close();
        
        int templateId = m_templateManager->importTemplate(tempTemplatePath, templateName, description, tags);
        bool importResult = (templateId != -1);
        m_resultsEdit->append(QStringLiteral("模板导入结果: ") + (importResult ? QStringLiteral("成功") : QStringLiteral("失败")));
        
        // 清理临时文件
        QFile::remove(tempTemplatePath);
    } else {
        m_resultsEdit->append(QStringLiteral("模板导入结果: 失败（无法创建临时文件）"));
    }
    
    m_logSystem->info(QStringLiteral("模板管理器测试完成"));
}

void TestWidget::testFileConverter()
{
    m_resultsEdit->append(QStringLiteral("\n--- 文件转换器测试 ---"));
    m_logSystem->info(QStringLiteral("开始文件转换器测试"));
    
    // 测试DOC转换器
    DocToXmlConverter docConverter;
    m_resultsEdit->append(QStringLiteral("DOC转换器创建: 成功"));
    
    // 测试PDF转换器
    PdfToXmlConverter pdfConverter;
    m_resultsEdit->append(QStringLiteral("PDF转换器创建: 成功"));
    
    m_logSystem->info(QStringLiteral("文件转换器测试完成"));
}

void TestWidget::testLogSystem()
{
    m_resultsEdit->append(QStringLiteral("\n--- 日志系统测试 ---"));
    m_logSystem->info(QStringLiteral("开始日志系统测试"));
    
    // 测试不同级别的日志
    m_logSystem->debug(QStringLiteral("这是调试信息"));
    m_logSystem->info(QStringLiteral("这是信息"));
    m_logSystem->warning(QStringLiteral("这是警告"));
    m_logSystem->error(QStringLiteral("这是错误"));
    m_logSystem->critical(QStringLiteral("这是严重错误"));
    
    m_resultsEdit->append(QStringLiteral("日志系统测试: 成功"));
    m_logSystem->info(QStringLiteral("日志系统测试完成"));
}
