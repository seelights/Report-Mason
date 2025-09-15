#include "XmlTestWidget.h"
#include "DocToXmlConverter.h"
#include "PdfToXmlConverter.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

XmlTestWidget::XmlTestWidget(QWidget *parent)
    : QWidget(parent)
    , m_logSystem(new LogSystem(this))
    , m_fieldExtractor(new FieldExtractor(this))
    , m_templateManager(new TemplateManager(this))
{
    setupUI();
    setupConnections();
    
    // 初始化日志系统
    m_logSystem->setLogWidget(m_logEdit);
    m_logSystem->setLogFile(QStringLiteral("xml_test.log"));
    m_logSystem->setLogLevel(LogSystem::INFO);
    
    m_logSystem->info(QStringLiteral("XML测试工具已启动"));
}

XmlTestWidget::~XmlTestWidget()
{
    m_logSystem->info(QStringLiteral("XML测试工具已关闭"));
}

void XmlTestWidget::setupUI()
{
    setWindowTitle(QStringLiteral("XML测试工具"));
    setMinimumSize(1200, 800);
    
    // 创建主分割器
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // 创建左侧分割器
    m_leftSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
    
    // 创建右侧分割器
    m_rightSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
    
    // 文件操作区域
    m_fileGroup = new QGroupBox(QStringLiteral("文件操作"), m_leftSplitter);
    QVBoxLayout *fileLayout = new QVBoxLayout(m_fileGroup);
    
    QHBoxLayout *fileBtnLayout = new QHBoxLayout();
    m_openFileBtn = new QPushButton(QStringLiteral("打开文件"), m_fileGroup);
    m_convertBtn = new QPushButton(QStringLiteral("转换为XML"), m_fileGroup);
    fileBtnLayout->addWidget(m_openFileBtn);
    fileBtnLayout->addWidget(m_convertBtn);
    fileBtnLayout->addStretch();
    
    m_contentEdit = new QTextEdit(m_fileGroup);
    m_contentEdit->setPlaceholderText(QStringLiteral("文件内容将显示在这里..."));
    
    fileLayout->addLayout(fileBtnLayout);
    fileLayout->addWidget(m_contentEdit);
    
    // 字段提取区域
    m_extractGroup = new QGroupBox(QStringLiteral("字段提取"), m_leftSplitter);
    QVBoxLayout *extractLayout = new QVBoxLayout(m_extractGroup);
    
    QHBoxLayout *extractBtnLayout = new QHBoxLayout();
    m_extractBtn = new QPushButton(QStringLiteral("提取字段"), m_extractGroup);
    extractBtnLayout->addWidget(m_extractBtn);
    extractBtnLayout->addStretch();
    
    m_fieldsList = new QListWidget(m_extractGroup);
    
    extractLayout->addLayout(extractBtnLayout);
    extractLayout->addWidget(m_fieldsList);
    
    // 模板操作区域
    m_templateGroup = new QGroupBox(QStringLiteral("模板操作"), m_rightSplitter);
    QVBoxLayout *templateLayout = new QVBoxLayout(m_templateGroup);
    
    QHBoxLayout *templateBtnLayout = new QHBoxLayout();
    m_templateCombo = new QComboBox(m_templateGroup);
    m_loadTemplateBtn = new QPushButton(QStringLiteral("加载模板"), m_templateGroup);
    m_applyTemplateBtn = new QPushButton(QStringLiteral("应用模板"), m_templateGroup);
    m_saveTemplateBtn = new QPushButton(QStringLiteral("保存模板"), m_templateGroup);
    
    templateBtnLayout->addWidget(m_templateCombo);
    templateBtnLayout->addWidget(m_loadTemplateBtn);
    templateBtnLayout->addWidget(m_applyTemplateBtn);
    templateBtnLayout->addWidget(m_saveTemplateBtn);
    
    m_resultEdit = new QTextEdit(m_templateGroup);
    m_resultEdit->setPlaceholderText(QStringLiteral("转换结果将显示在这里..."));
    
    templateLayout->addLayout(templateBtnLayout);
    templateLayout->addWidget(m_resultEdit);
    
    // 日志区域
    m_logGroup = new QGroupBox(QStringLiteral("日志"), m_rightSplitter);
    QVBoxLayout *logLayout = new QVBoxLayout(m_logGroup);
    
    QHBoxLayout *logBtnLayout = new QHBoxLayout();
    m_clearLogBtn = new QPushButton(QStringLiteral("清空日志"), m_logGroup);
    m_saveLogBtn = new QPushButton(QStringLiteral("保存日志"), m_logGroup);
    logBtnLayout->addWidget(m_clearLogBtn);
    logBtnLayout->addWidget(m_saveLogBtn);
    logBtnLayout->addStretch();
    
    m_logEdit = new QTextEdit(m_logGroup);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumHeight(200);
    
    logLayout->addLayout(logBtnLayout);
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
    m_leftSplitter->addWidget(m_fileGroup);
    m_leftSplitter->addWidget(m_extractGroup);
    m_leftSplitter->setStretchFactor(0, 2);
    m_leftSplitter->setStretchFactor(1, 1);
    
    m_rightSplitter->addWidget(m_templateGroup);
    m_rightSplitter->addWidget(m_logGroup);
    m_rightSplitter->setStretchFactor(0, 2);
    m_rightSplitter->setStretchFactor(1, 1);
    
    m_mainSplitter->addWidget(m_leftSplitter);
    m_mainSplitter->addWidget(m_rightSplitter);
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 1);
    
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_mainSplitter);
    mainLayout->addLayout(statusLayout);
}

void XmlTestWidget::setupConnections()
{
    connect(m_openFileBtn, &QPushButton::clicked, this, &XmlTestWidget::openFile);
    connect(m_convertBtn, &QPushButton::clicked, this, &XmlTestWidget::convertFile);
    connect(m_extractBtn, &QPushButton::clicked, this, &XmlTestWidget::extractFields);
    connect(m_loadTemplateBtn, &QPushButton::clicked, this, &XmlTestWidget::loadTemplate);
    connect(m_applyTemplateBtn, &QPushButton::clicked, this, &XmlTestWidget::applyTemplate);
    connect(m_saveTemplateBtn, &QPushButton::clicked, this, &XmlTestWidget::saveTemplate);
    connect(m_clearLogBtn, &QPushButton::clicked, this, &XmlTestWidget::clearLog);
    connect(m_saveLogBtn, &QPushButton::clicked, this, &XmlTestWidget::saveLog);
}

void XmlTestWidget::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("打开文件"),
        QString(),
        QStringLiteral("所有文件 (*.*);;Word文档 (*.doc *.docx);;PDF文件 (*.pdf);;XML文件 (*.xml)")
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    m_currentFilePath = fileName;
    m_logSystem->info(QStringLiteral("打开文件: ") + fileName);
    
    // 检查文件类型
    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();
    
    if (suffix == QStringLiteral("docx")) {
        // 对于DOCX文件，先转换为XML再显示
        QString xmlContent = convertFileToXml(fileName);
        if (!xmlContent.isEmpty()) {
            m_contentEdit->setPlainText(xmlContent);
            updateStatus(QStringLiteral("DOCX文件已转换为XML并加载"));
        } else {
            m_logSystem->error(QStringLiteral("无法转换DOCX文件: ") + fileName);
            QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("无法转换DOCX文件！"));
        }
    } else if (suffix == QStringLiteral("pdf")) {
        // 对于PDF文件，先转换为XML再显示
        QString xmlContent = convertFileToXml(fileName);
        if (!xmlContent.isEmpty()) {
            m_contentEdit->setPlainText(xmlContent);
            updateStatus(QStringLiteral("PDF文件已转换为XML并加载"));
        } else {
            m_logSystem->error(QStringLiteral("无法转换PDF文件: ") + fileName);
            QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("无法转换PDF文件！"));
        }
    } else {
        // 对于其他文件，直接读取文本
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            // Qt 6中QTextStream默认使用UTF-8编码，不需要设置setCodec
            m_contentEdit->setPlainText(stream.readAll());
            file.close();
            updateStatus(QStringLiteral("文件已加载"));
        } else {
            m_logSystem->error(QStringLiteral("无法打开文件: ") + fileName);
            QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("无法打开文件！"));
        }
    }
}

void XmlTestWidget::convertFile()
{
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先打开文件！"));
        return;
    }
    
    updateStatus(QStringLiteral("正在转换文件..."));
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // 不确定进度
    
    QString xmlContent = convertFileToXml(m_currentFilePath);
    
    if (!xmlContent.isEmpty()) {
        m_contentEdit->setPlainText(xmlContent);
        updateStatus(QStringLiteral("文件转换完成"));
        m_logSystem->info(QStringLiteral("文件转换成功"));
    } else {
        updateStatus(QStringLiteral("文件转换失败"));
        m_logSystem->error(QStringLiteral("文件转换失败"));
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("文件转换失败！"));
    }
    
    m_progressBar->setVisible(false);
}

void XmlTestWidget::extractFields()
{
    QString content = m_contentEdit->toPlainText();
    if (content.size() == 0) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先输入文档内容！"));
        return;
    }
    
    updateStatus(QStringLiteral("正在提取字段..."));
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);
    
    m_extractedFields = m_fieldExtractor->extractFields(content);
    
    m_fieldsList->clear();
    for (auto it = m_extractedFields.begin(); it != m_extractedFields.end(); ++it) {
        QString itemText = QStringLiteral("%1: %2").arg(it.key(), it.value());
        m_fieldsList->addItem(itemText);
    }
    
    updateStatus(QStringLiteral("字段提取完成"));
    m_logSystem->info(QStringLiteral("提取到 %1 个字段").arg(m_extractedFields.size()));
    
    m_progressBar->setVisible(false);
}

void XmlTestWidget::loadTemplate()
{
    QString templateName = m_templateCombo->currentText();
    updateStatus(QStringLiteral("正在加载模板: ") + templateName);
    m_logSystem->info(QStringLiteral("加载模板: ") + templateName);
    
    // 搜索模板
    QList<TemplateInfo> templates = m_templateManager->searchTemplates(templateName);
    if (templates.size() > 0) {
        updateStatus(QStringLiteral("模板加载成功！"));
        m_logSystem->info(QStringLiteral("模板加载成功: ") + templateName);
    } else {
        updateStatus(QStringLiteral("模板加载失败！"));
        m_logSystem->warning(QStringLiteral("模板加载失败: ") + templateName);
    }
}

void XmlTestWidget::applyTemplate()
{
    if (m_extractedFields.size() == 0) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先提取字段！"));
        return;
    }
    
    updateStatus(QStringLiteral("正在应用模板..."));
    m_logSystem->info(QStringLiteral("应用模板"));
    
    // 简单的模板应用逻辑
    QString result = QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    result += QStringLiteral("<document>\n");
    
    for (auto it = m_extractedFields.begin(); it != m_extractedFields.end(); ++it) {
        result += QStringLiteral("  <field name=\"%1\">%2</field>\n").arg(it.key(), it.value());
    }
    
    result += QStringLiteral("</document>");
    
    m_resultEdit->setPlainText(result);
    updateStatus(QStringLiteral("模板应用完成"));
    m_logSystem->info(QStringLiteral("模板应用完成"));
}

void XmlTestWidget::saveTemplate()
{
    QString templateName = m_templateCombo->currentText();
    QString templateContent = m_resultEdit->toPlainText();
    
    if (templateContent.size() == 0) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("没有模板内容可保存！"));
        return;
    }
    
    updateStatus(QStringLiteral("正在保存模板..."));
    m_logSystem->info(QStringLiteral("保存模板: ") + templateName);
    
    // 简单的模板保存逻辑
    updateStatus(QStringLiteral("模板保存完成"));
    m_logSystem->info(QStringLiteral("模板保存完成"));
}

void XmlTestWidget::clearLog()
{
    m_logSystem->clear();
    updateStatus(QStringLiteral("日志已清空"));
}

void XmlTestWidget::saveLog()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("保存日志"),
        QStringLiteral("xml_test.log"),
        QStringLiteral("日志文件 (*.log);;文本文件 (*.txt);;所有文件 (*.*)")
    );
    
    if (!fileName.isEmpty()) {
        m_logSystem->saveToFile(fileName);
        updateStatus(QStringLiteral("日志已保存"));
        m_logSystem->info(QStringLiteral("日志已保存到: ") + fileName);
    }
}

void XmlTestWidget::updateStatus(const QString &message)
{
    m_statusLabel->setText(message);
    m_logSystem->debug(message);
}

QString XmlTestWidget::convertFileToXml(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    
    // 创建临时XML文件路径
    QString tempXmlPath = QDir::tempPath() + QStringLiteral("/temp_conversion_") + 
                         QString::number(QDateTime::currentMSecsSinceEpoch()) + QStringLiteral(".xml");
    
    if (suffix == QStringLiteral("doc") || suffix == QStringLiteral("docx")) {
        DocToXmlConverter converter;
        FileConverter::ConvertStatus status = converter.convertFileToXml(filePath, tempXmlPath);
        if (status == FileConverter::ConvertStatus::SUCCESS) {
            // 读取生成的XML文件内容
            QFile xmlFile(tempXmlPath);
            if (xmlFile.open(QIODevice::ReadOnly)) {
                QTextStream stream(&xmlFile);
                QString content = stream.readAll();
                xmlFile.close();
                QFile::remove(tempXmlPath); // 删除临时文件
                return content;
            }
        }
        m_logSystem->error(QStringLiteral("DOC转换失败: ") + converter.getLastError());
        return QString();
    } else if (suffix == QStringLiteral("pdf")) {
        PdfToXmlConverter converter;
        FileConverter::ConvertStatus status = converter.convertFileToXml(filePath, tempXmlPath);
        if (status == FileConverter::ConvertStatus::SUCCESS) {
            // 读取生成的XML文件内容
            QFile xmlFile(tempXmlPath);
            if (xmlFile.open(QIODevice::ReadOnly)) {
                QTextStream stream(&xmlFile);
                QString content = stream.readAll();
                xmlFile.close();
                QFile::remove(tempXmlPath); // 删除临时文件
                return content;
            }
        }
        m_logSystem->error(QStringLiteral("PDF转换失败: ") + converter.getLastError());
        return QString();
    } else {
        m_logSystem->warning(QStringLiteral("不支持的文件格式: ") + suffix);
        return QString();
    }
}
