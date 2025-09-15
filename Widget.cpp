#include "Widget.h"
#include <QApplication>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QTextCursor>

Widget::Widget(QWidget* parent)
    : QWidget(parent), m_docConverter(nullptr), m_pdfConverter(nullptr), m_templateManager(nullptr),
      m_fieldExtractor(nullptr)
{
    setupUI();
    setupConnections();

    // 初始化核心功能类
    m_docConverter = new DocToXmlConverter(this);
    m_pdfConverter = new PdfToXmlConverter(this);
    m_templateManager = new TemplateManager(this);
    m_fieldExtractor = new FieldExtractor(this);

    updateStatus("应用程序已启动");
    logMessage("ReportMason 界面初始化完成");
}

Widget::~Widget() { logMessage("应用程序正在关闭"); }

void Widget::setupUI()
{
    setWindowTitle("ReportMason - 实验报告处理工具");
    setMinimumSize(1200, 800);

    m_mainLayout = new QVBoxLayout(this);

    // 创建主分割器
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_mainLayout->addWidget(m_splitter);

    // 左侧面板
    QWidget* leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    // 文件选择区域
    m_fileGroup = new QGroupBox("文件操作", this);
    QGridLayout* fileLayout = new QGridLayout(m_fileGroup);

    fileLayout->addWidget(new QLabel("输入文件:"), 0, 0);
    m_inputFileEdit = new QLineEdit;
    m_inputFileEdit->setPlaceholderText("选择要处理的文件...");
    fileLayout->addWidget(m_inputFileEdit, 0, 1);
    m_inputFileBtn = new QPushButton("浏览...");
    fileLayout->addWidget(m_inputFileBtn, 0, 2);

    fileLayout->addWidget(new QLabel("输出文件:"), 1, 0);
    m_outputFileEdit = new QLineEdit;
    m_outputFileEdit->setPlaceholderText("选择输出位置...");
    fileLayout->addWidget(m_outputFileEdit, 1, 1);
    m_outputFileBtn = new QPushButton("浏览...");
    fileLayout->addWidget(m_outputFileBtn, 1, 2);

    fileLayout->addWidget(new QLabel("输出格式:"), 2, 0);
    m_formatCombo = new QComboBox;
    m_formatCombo->addItems({"XML", "JSON", "TXT", "HTML"});
    fileLayout->addWidget(m_formatCombo, 2, 1);

    m_convertBtn = new QPushButton("开始转换");
    m_convertBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    fileLayout->addWidget(m_convertBtn, 2, 2);

    m_clearBtn = new QPushButton("清空");
    fileLayout->addWidget(m_clearBtn, 3, 2);

    leftLayout->addWidget(m_fileGroup);

    // 字段提取区域
    m_fieldGroup = new QGroupBox("字段提取", this);
    QVBoxLayout* fieldLayout = new QVBoxLayout(m_fieldGroup);

    fieldLayout->addWidget(new QLabel("文档内容:"));
    m_contentEdit = new QTextEdit;
    m_contentEdit->setPlaceholderText("粘贴文档内容或从文件加载...");
    m_contentEdit->setMaximumHeight(150);
    fieldLayout->addWidget(m_contentEdit);

    QHBoxLayout* fieldBtnLayout = new QHBoxLayout;
    m_extractBtn = new QPushButton("提取字段");
    m_extractBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; }");
    m_clearFieldsBtn = new QPushButton("清空字段");
    fieldBtnLayout->addWidget(m_extractBtn);
    fieldBtnLayout->addWidget(m_clearFieldsBtn);
    fieldBtnLayout->addStretch();
    fieldLayout->addLayout(fieldBtnLayout);

    fieldLayout->addWidget(new QLabel("提取的字段:"));
    m_fieldList = new QListWidget;
    m_fieldList->setMaximumHeight(200);
    fieldLayout->addWidget(m_fieldList);

    leftLayout->addWidget(m_fieldGroup);

    // 模板管理区域
    m_templateGroup = new QGroupBox("模板管理", this);
    QVBoxLayout* templateLayout = new QVBoxLayout(m_templateGroup);

    QHBoxLayout* templateBtnLayout = new QHBoxLayout;
    m_templateCombo = new QComboBox;
    m_templateCombo->addItems({"标准实验报告", "学术论文", "技术报告", "自定义模板"});
    templateBtnLayout->addWidget(m_templateCombo);
    m_loadTemplateBtn = new QPushButton("加载模板");
    m_applyTemplateBtn = new QPushButton("应用模板");
    m_saveTemplateBtn = new QPushButton("保存模板");
    templateBtnLayout->addWidget(m_loadTemplateBtn);
    templateBtnLayout->addWidget(m_applyTemplateBtn);
    templateBtnLayout->addWidget(m_saveTemplateBtn);
    templateLayout->addLayout(templateBtnLayout);

    templateLayout->addWidget(new QLabel("处理结果:"));
    m_resultEdit = new QTextEdit;
    m_resultEdit->setPlaceholderText("转换结果将显示在这里...");
    templateLayout->addWidget(m_resultEdit);

    leftLayout->addWidget(m_templateGroup);

    m_splitter->addWidget(leftPanel);

    // 右侧面板
    QWidget* rightPanel = new QWidget;
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    // 状态和日志区域
    m_statusGroup = new QGroupBox("状态和日志", this);
    QVBoxLayout* statusLayout = new QVBoxLayout(m_statusGroup);

    m_statusLabel = new QLabel("就绪");
    m_statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    statusLayout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);
    statusLayout->addWidget(m_progressBar);

    m_logEdit = new QTextEdit;
    m_logEdit->setMaximumHeight(300);
    m_logEdit->setReadOnly(true);
    statusLayout->addWidget(m_logEdit);

    rightLayout->addWidget(m_statusGroup);

    // 帮助按钮
    QHBoxLayout* helpLayout = new QHBoxLayout;
    QPushButton* helpBtn = new QPushButton("帮助");
    QPushButton* aboutBtn = new QPushButton("关于");
    helpLayout->addWidget(helpBtn);
    helpLayout->addWidget(aboutBtn);
    helpLayout->addStretch();
    rightLayout->addLayout(helpLayout);

    m_splitter->addWidget(rightPanel);

    // 设置分割器比例
    m_splitter->setStretchFactor(0, 2);
    m_splitter->setStretchFactor(1, 1);
}

void Widget::setupConnections()
{
    // 文件操作连接
    connect(m_inputFileBtn, &QPushButton::clicked, this, &Widget::selectInputFile);
    connect(m_outputFileBtn, &QPushButton::clicked, this, &Widget::selectOutputFile);
    connect(m_convertBtn, &QPushButton::clicked, this, &Widget::convertFile);
    connect(m_clearBtn, &QPushButton::clicked, this, &Widget::clearResults);

    // 字段操作连接
    connect(m_extractBtn, &QPushButton::clicked, this, &Widget::extractFields);
    connect(m_clearFieldsBtn, &QPushButton::clicked, this, &Widget::clearFields);

    // 模板操作连接
    connect(m_loadTemplateBtn, &QPushButton::clicked, this, &Widget::loadTemplate);
    connect(m_applyTemplateBtn, &QPushButton::clicked, this, &Widget::applyTemplate);
    connect(m_saveTemplateBtn, &QPushButton::clicked, this, &Widget::saveTemplate);
}

void Widget::selectInputFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "选择输入文件", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "所有支持的文件 (*.doc *.docx *.pdf *.txt);;Word文档 (*.doc *.docx);;PDF文档 "
        "(*.pdf);;文本文件 (*.txt);;所有文件 (*.*)");

    if (!fileName.isEmpty()) {
        m_inputFileEdit->setText(fileName);
        m_currentInputFile = fileName;

        // 自动设置输出文件名
        QFileInfo fileInfo(fileName);
        QString outputFileName =
            fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_converted.xml";
        m_outputFileEdit->setText(outputFileName);
        m_currentOutputFile = outputFileName;

        updateStatus("已选择输入文件: " + fileInfo.fileName());
        logMessage("选择输入文件: " + fileName);
    }
}

void Widget::selectOutputFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, "选择输出文件", m_outputFileEdit->text(),
        "XML文件 (*.xml);;JSON文件 (*.json);;文本文件 (*.txt);;HTML文件 (*.html);;所有文件 (*.*)");

    if (!fileName.isEmpty()) {
        m_outputFileEdit->setText(fileName);
        m_currentOutputFile = fileName;
        updateStatus("已选择输出文件: " + QFileInfo(fileName).fileName());
        logMessage("选择输出文件: " + fileName);
    }
}

void Widget::convertFile()
{
    if (m_currentInputFile.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择输入文件！");
        return;
    }

    if (m_currentOutputFile.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择输出文件！");
        return;
    }

    updateStatus("正在转换文件...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // 不确定进度

    logMessage("开始转换文件: " + m_currentInputFile);

    // 根据文件类型选择转换器
    QString result = convertFileToXml(m_currentInputFile);

    m_progressBar->setVisible(false);

    if (!result.isEmpty()) {
        // 保存结果到文件
        QFile outputFile(m_currentOutputFile);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&outputFile);
            stream << result;
            outputFile.close();
        }

        // 显示结果
        m_resultEdit->setPlainText(result);
        m_contentEdit->setPlainText(result);

        updateStatus("文件转换完成！");
        logMessage("文件转换成功，结果已保存到: " + m_currentOutputFile);

        QMessageBox::information(this, "成功",
                                 "文件转换完成！\n结果已保存到: " + m_currentOutputFile);
    } else {
        updateStatus("文件转换失败！");
        logMessage("文件转换失败: " + m_currentInputFile);
        QMessageBox::critical(this, "错误", "文件转换失败！\n请检查文件格式和内容。");
    }
}

void Widget::extractFields()
{
    QString content = m_contentEdit->toPlainText();
    if (content.size() == 0) {
        QMessageBox::warning(this, "警告", "请先输入文档内容！");
        return;
    }

    updateStatus("正在提取字段...");
    logMessage("开始提取字段...");

    m_extractedFields = m_fieldExtractor->extractFields(content);

    // 更新字段列表
    m_fieldList->clear();
    for (auto it = m_extractedFields.begin(); it != m_extractedFields.end(); ++it) {
        QString itemText = QString("%1: %2").arg(it.key()).arg(it.value());
        m_fieldList->addItem(itemText);
    }

    updateStatus(QString("字段提取完成，共提取 %1 个字段").arg(m_extractedFields.size()));
    logMessage(QString("字段提取完成，共提取 %1 个字段").arg(m_extractedFields.size()));
}

void Widget::clearFields()
{
    m_fieldList->clear();
    m_extractedFields.clear();
    updateStatus("已清空字段");
    logMessage("清空字段");
}

void Widget::loadTemplate()
{
    QString templateName = m_templateCombo->currentText();
    updateStatus("正在加载模板: " + templateName);
    logMessage("加载模板: " + templateName);

    // 搜索模板
    QList<TemplateInfo> templates = m_templateManager->searchTemplates(templateName);
    if (templates.size() > 0) {
        updateStatus("模板加载成功！");
        logMessage("模板加载成功: " + templateName);
    } else {
        updateStatus("模板加载失败！");
        logMessage("模板加载失败: " + templateName);
        QMessageBox::warning(this, "警告", "模板加载失败！");
    }
}

void Widget::applyTemplate()
{
    if (m_extractedFields.size() == 0) {
        QMessageBox::warning(this, "警告", "请先提取字段！");
        return;
    }

    updateStatus("正在应用模板...");
    logMessage("应用模板...");

    // 简化的模板应用：将字段转换为XML格式
    QString result = "<template>\n";
    for (auto it = m_extractedFields.begin(); it != m_extractedFields.end(); ++it) {
        result += QString("  <field name=\"%1\">%2</field>\n").arg(it.key()).arg(it.value());
    }
    result += "</template>";

    m_resultEdit->setPlainText(result);
    updateStatus("模板应用成功！");
    logMessage("模板应用成功");
}

void Widget::saveTemplate()
{
    QString templateName = m_templateCombo->currentText();
    QString templateContent = m_resultEdit->toPlainText();

    if (templateContent.size() == 0) {
        QMessageBox::warning(this, "警告", "没有模板内容可保存！");
        return;
    }

    updateStatus("正在保存模板...");
    logMessage("保存模板: " + templateName);

    // 简化的模板保存：将内容保存到临时文件，然后导入
    QString tempFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + 
                          "/" + templateName + "_template.xml";
    
    QFile tempFile(tempFilePath);
    if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&tempFile);
        stream << templateContent;
        tempFile.close();
        
        // 使用importTemplate方法导入模板
        int templateId = m_templateManager->importTemplate(tempFilePath, templateName, "用户创建的模板", QStringList() << "user_created");
        
        if (templateId != -1) {
            updateStatus("模板保存成功！");
            logMessage("模板保存成功: " + templateName);
            QMessageBox::information(this, "成功", "模板保存成功！");
        } else {
            updateStatus("模板保存失败！");
            logMessage("模板保存失败: " + templateName);
            QMessageBox::critical(this, "错误", "模板保存失败！");
        }
        
        // 清理临时文件
        QFile::remove(tempFilePath);
    } else {
        updateStatus("模板保存失败！");
        logMessage("模板保存失败: " + templateName);
        QMessageBox::critical(this, "错误", "无法创建临时文件！");
    }
}

void Widget::clearResults()
{
    m_inputFileEdit->clear();
    m_outputFileEdit->clear();
    m_contentEdit->clear();
    m_resultEdit->clear();
    m_fieldList->clear();

    m_currentInputFile.clear();
    m_currentOutputFile.clear();
    m_extractedFields.clear();

    updateStatus(QStringLiteral("已清空所有结果"));
    logMessage(QStringLiteral("清空所有结果"));
}

void Widget::showHelp()
{
    QMessageBox::information(this, QStringLiteral("帮助"),
                             "ReportMason 使用说明:\n\n"
                             "1. 选择输入文件 (支持 .doc, .docx, .pdf, .txt)\n"
                             "2. 选择输出文件和格式\n"
                             "3. 点击'开始转换'进行文件转换\n"
                             "4. 在'字段提取'区域提取文档字段\n"
                             "5. 选择模板并应用\n"
                             "6. 查看和保存结果\n\n"
                             "支持的功能:\n"
                             "- 多种文件格式转换\n"
                             "- 智能字段提取\n"
                             "- 模板管理和应用\n"
                             "- 批量处理\n"
                             "- 结果导出");
}

void Widget::showAbout()
{
    QMessageBox::about(this, QStringLiteral("关于 ReportMason"),
                       "ReportMason v1.0.0\n\n"
                       "实验报告处理工具\n\n"
                       "功能特点:\n"
                       "- 支持多种文档格式\n"
                       "- 智能字段提取\n"
                       "- 模板管理系统\n"
                       "- 用户友好界面\n\n"
                       "开发团队: ReportMason Team\n"
                       "技术支持: support@reportmason.com");
}

void Widget::updateStatus(const QString& message)
{
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");
}

void Widget::logMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);
    m_logEdit->append(logEntry);

    // 自动滚动到底部
    QTextCursor cursor = m_logEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logEdit->setTextCursor(cursor);
}

QString Widget::convertFileToXml(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();

    QMap<QString, FileConverter::FieldInfo> fields;
    FileConverter::ConvertStatus status;

    if (suffix == "doc" || suffix == "docx") {
        status = m_docConverter->extractFields(filePath, fields);
    } else if (suffix == "pdf") {
        status = m_pdfConverter->extractFields(filePath, fields);
    } else {
        // 对于其他格式，尝试使用DocToXmlConverter作为通用转换器
        status = m_docConverter->extractFields(filePath, fields);
    }

    if (status != FileConverter::ConvertStatus::SUCCESS) {
        logMessage("文件转换失败: " + filePath);
        return QString();
    }

    // 将字段转换为XML
    QByteArray xmlOutput;
    if (suffix == "doc" || suffix == "docx") {
        status = m_docConverter->convertToXml(fields, xmlOutput);
    } else if (suffix == "pdf") {
        status = m_pdfConverter->convertToXml(fields, xmlOutput);
    } else {
        status = m_docConverter->convertToXml(fields, xmlOutput);
    }

    if (status != FileConverter::ConvertStatus::SUCCESS) {
        logMessage("XML转换失败: " + filePath);
        return QString();
    }

    return QString::fromUtf8(xmlOutput);
}
