/*
 * @Author: seelights
 * @Date: 2025-01-27 22:30:00
 * @LastEditTime: 2025-01-27 22:30:00
 * @LastEditors: seelights
 * @Description: 无损转换器测试窗口实现
 * @FilePath: \ReportMason\src\widgetTest\LosslessConverterTestWidget.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "LosslessConverterTestWidget.h"
#include "QtCompat.h"
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QClipboard>

LosslessConverterTestWidget::LosslessConverterTestWidget(QWidget *parent)
    : QWidget(parent)
    , m_converter(new LosslessDocumentConverter(this))
    , m_logSystem(new LogSystem(this))
    , m_isConverting(false)
    , m_hasResult(false)
{
    setupUI();
    setupConnections();
    updateUI();
    
    // 初始化日志系统
    m_logSystem->setLogWidget(m_debugOutput);
    m_logSystem->setLogFile(QS("lossless_converter_test.log"));
    m_logSystem->setLogLevel(LogSystem::DEBUG);
    m_logSystem->info(QS("无损转换器测试工具已启动"));
}

LosslessConverterTestWidget::~LosslessConverterTestWidget()
{
    if (m_converter) {
        m_converter->deleteLater();
    }
}

void LosslessConverterTestWidget::setupUI()
{
    setWindowTitle(QS("无损文档转换器测试"));
    setMinimumSize(1200, 800);

    m_mainLayout = new QVBoxLayout(this);

    // 文件选择区域
    m_fileGroup = new QGroupBox(QS("文件选择"), this);
    m_fileLayout = new QHBoxLayout(m_fileGroup);

    m_selectInputBtn = new QPushButton(QS("选择输入文件"), this);
    m_inputFileLabel = new QLabel(QS("未选择文件"), this);
    m_inputFileLabel->setStyleSheet(QS("QLabel { color: gray; }"));

    m_fileLayout->addWidget(m_selectInputBtn);
    m_fileLayout->addWidget(m_inputFileLabel, 1);

    // 输出设置区域
    m_outputGroup = new QGroupBox(QS("输出设置"), this);
    m_outputLayout = new QHBoxLayout(m_outputGroup);

    m_selectOutputBtn = new QPushButton(QS("选择输出目录"), this);
    m_outputDirLabel = new QLabel(QS("未选择目录"), this);
    m_outputDirLabel->setStyleSheet(QS("QLabel { color: gray; }"));

    m_outputLayout->addWidget(m_selectOutputBtn);
    m_outputLayout->addWidget(m_outputDirLabel, 1);

    // 控制按钮区域
    m_controlGroup = new QGroupBox(QS("操作控制"), this);
    m_controlLayout = new QHBoxLayout(m_controlGroup);

    m_startBtn = new QPushButton(QS("开始无损转换"), this);
    m_openFileBtn = new QPushButton(QS("打开XML文件"), this);
    m_openDirBtn = new QPushButton(QS("打开输出目录"), this);
    m_clearBtn = new QPushButton(QS("清空结果"), this);

    m_startBtn->setStyleSheet(
        QS("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }"));
    m_openFileBtn->setEnabled(false);
    m_openDirBtn->setEnabled(false);

    m_controlLayout->addWidget(m_startBtn);
    m_controlLayout->addWidget(m_openFileBtn);
    m_controlLayout->addWidget(m_openDirBtn);
    m_controlLayout->addWidget(m_clearBtn);
    m_controlLayout->addStretch();

    // 进度显示区域
    m_progressGroup = new QGroupBox(QS("转换进度"), this);
    m_progressLayout = new QVBoxLayout(m_progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_statusLabel = new QLabel(QS("就绪"), this);

    m_progressLayout->addWidget(m_progressBar);
    m_progressLayout->addWidget(m_statusLabel);

    // 设置选项区域
    m_settingGroup = new QGroupBox(QS("显示选项"), this);
    m_settingLayout = new QHBoxLayout(m_settingGroup);

    m_verboseModeCheck = new QCheckBox(QS("详细模式"), this);
    m_showFormatInfoCheck = new QCheckBox(QS("显示格式信息"), this);
    m_showPositionInfoCheck = new QCheckBox(QS("显示位置信息"), this);
    m_showElementTreeCheck = new QCheckBox(QS("显示元素树"), this);
    m_validateConversionCheck = new QCheckBox(QS("验证转换完整性"), this);
    m_outputFormatCombo = new QComboBox(this);

    m_verboseModeCheck->setChecked(true);
    m_showFormatInfoCheck->setChecked(true);
    m_showPositionInfoCheck->setChecked(true);
    m_showElementTreeCheck->setChecked(true);
    m_validateConversionCheck->setChecked(true);

    m_outputFormatCombo->addItem(QS("XML格式"));
    m_outputFormatCombo->addItem(QS("JSON格式"));
    m_outputFormatCombo->addItem(QS("结构化文本"));

    m_settingLayout->addWidget(m_verboseModeCheck);
    m_settingLayout->addWidget(m_showFormatInfoCheck);
    m_settingLayout->addWidget(m_showPositionInfoCheck);
    m_settingLayout->addWidget(m_showElementTreeCheck);
    m_settingLayout->addWidget(m_validateConversionCheck);
    m_settingLayout->addWidget(new QLabel(QS("输出格式:"), this));
    m_settingLayout->addWidget(m_outputFormatCombo);
    m_settingLayout->addStretch();

    // 调试输出区域
    m_debugGroup = new QGroupBox(QS("调试输出"), this);
    m_debugLayout = new QVBoxLayout(m_debugGroup);
    
    m_debugOutput = new QTextEdit(this);
    m_debugOutput->setReadOnly(true);
    m_debugOutput->setFont(QFont(QS("Consolas"), 9));
    m_debugOutput->setMaximumHeight(150);
    
    m_debugBtnLayout = new QHBoxLayout();
    m_clearDebugBtn = new QPushButton(QS("清空调试"), this);
    m_saveDebugBtn = new QPushButton(QS("保存调试"), this);
    
    m_debugBtnLayout->addWidget(m_clearDebugBtn);
    m_debugBtnLayout->addWidget(m_saveDebugBtn);
    m_debugBtnLayout->addStretch();
    
    m_debugLayout->addWidget(m_debugOutput);
    m_debugLayout->addLayout(m_debugBtnLayout);

    // 结果展示区域
    m_resultGroup = new QGroupBox(QS("转换结果"), this);
    m_resultLayout = new QVBoxLayout(m_resultGroup);

    m_resultTabs = new QTabWidget(this);

    // XML结果标签页
    m_xmlResultEdit = new QTextEdit(this);
    m_xmlResultEdit->setReadOnly(true);
    m_xmlResultEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_xmlResultEdit, QS("XML结果"));

    // 格式信息标签页
    m_formatInfoEdit = new QTextEdit(this);
    m_formatInfoEdit->setReadOnly(true);
    m_formatInfoEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_formatInfoEdit, QS("格式信息"));

    // 位置信息标签页
    m_positionInfoEdit = new QTextEdit(this);
    m_positionInfoEdit->setReadOnly(true);
    m_positionInfoEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_positionInfoEdit, QS("位置信息"));

    // 元素树标签页
    m_elementTreeWidget = new QTreeWidget(this);
    m_elementTreeWidget->setHeaderLabels(QStringList()
                                         << QS("元素ID") << QS("类型") << QS("内容") << QS("位置") << QS("格式"));
    m_resultTabs->addTab(m_elementTreeWidget, QS("元素树"));

    // 验证结果标签页
    m_validationEdit = new QTextEdit(this);
    m_validationEdit->setReadOnly(true);
    m_validationEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_validationEdit, QS("验证结果"));

    m_resultLayout->addWidget(m_resultTabs);

    // 添加到主布局
    m_mainLayout->addWidget(m_fileGroup);
    m_mainLayout->addWidget(m_outputGroup);
    m_mainLayout->addWidget(m_controlGroup);
    m_mainLayout->addWidget(m_progressGroup);
    m_mainLayout->addWidget(m_settingGroup);
    m_mainLayout->addWidget(m_debugGroup);
    m_mainLayout->addWidget(m_resultGroup, 1);
}

void LosslessConverterTestWidget::setupConnections()
{
    // 文件操作
    connect(m_selectInputBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onSelectInputFile);
    connect(m_selectOutputBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onSelectOutputDir);
    connect(m_startBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onStartConversion);
    connect(m_openFileBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onOpenOutputFile);
    connect(m_openDirBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onOpenOutputDir);
    connect(m_clearBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onClearResults);
    
    // 调试按钮
    connect(m_clearDebugBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onClearDebugOutput);
    connect(m_saveDebugBtn, &QPushButton::clicked, this, &LosslessConverterTestWidget::onSaveDebugOutput);

    // 转换器信号
    connect(m_converter, &LosslessDocumentConverter::conversionProgress,
            this, &LosslessConverterTestWidget::onConversionProgress);
    connect(m_converter, &LosslessDocumentConverter::conversionFinished,
            this, &LosslessConverterTestWidget::onConversionFinished);
}

void LosslessConverterTestWidget::updateUI()
{
    bool hasInput = !m_inputFilePath.isEmpty();
    bool hasOutput = !m_outputDirPath.isEmpty();
    
    m_startBtn->setEnabled(hasInput && hasOutput && !m_isConverting);
    m_openFileBtn->setEnabled(m_hasResult);
    m_openDirBtn->setEnabled(hasOutput);
}

void LosslessConverterTestWidget::onSelectInputFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QS("选择输入文件"),
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        QS("支持的文件 (*.docx *.pdf);;Word文档 (*.docx);;PDF文档 (*.pdf);;所有文件 (*.*)")
    );
    
    if (!fileName.isEmpty()) {
        m_inputFilePath = fileName;
        m_inputFileLabel->setText(QFileInfo(fileName).fileName());
        m_inputFileLabel->setStyleSheet(QS("QLabel { color: black; }"));
        m_logSystem->info(QS("选择输入文件: ") + fileName);
        updateUI();
    }
}

void LosslessConverterTestWidget::onSelectOutputDir()
{
    QString dirName = QFileDialog::getExistingDirectory(
        this,
        QS("选择输出目录"),
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)
    );
    
    if (!dirName.isEmpty()) {
        m_outputDirPath = dirName;
        m_outputDirLabel->setText(QDir(dirName).dirName());
        m_outputDirLabel->setStyleSheet(QS("QLabel { color: black; }"));
        m_logSystem->info(QS("选择输出目录: ") + dirName);
        updateUI();
    }
}

void LosslessConverterTestWidget::onStartConversion()
{
    if (m_inputFilePath.isEmpty() || m_outputDirPath.isEmpty()) {
        QMessageBox::warning(this, QS("警告"), QS("请先选择输入文件和输出目录"));
        return;
    }
    
    m_isConverting = true;
    m_hasResult = false;
    updateUI();
    
    // 生成输出文件名
    QFileInfo inputInfo(m_inputFilePath);
    QString outputFileName = inputInfo.baseName() + QS("_lossless.xml");
    QString outputPath = QDir(m_outputDirPath).filePath(outputFileName);
    
    m_logSystem->info(QS("开始无损转换..."));
    m_logSystem->info(QS("输入文件: ") + m_inputFilePath);
    m_logSystem->info(QS("输出文件: ") + outputPath);
    
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_statusLabel->setText(QS("正在转换..."));
    
    // 启动转换
    LosslessDocumentConverter::ConvertStatus status = m_converter->convertToLosslessXml(m_inputFilePath, outputPath);
    
    if (status == LosslessDocumentConverter::ConvertStatus::SUCCESS) {
        m_lastXmlPath = outputPath;
        m_hasResult = true;
        
        // 显示结果
        displayXmlResult(outputPath);
        if (m_showFormatInfoCheck->isChecked()) {
            displayFormatInfo(outputPath);
        }
        if (m_showPositionInfoCheck->isChecked()) {
            displayPositionInfo(outputPath);
        }
        if (m_showElementTreeCheck->isChecked()) {
            displayElementTree(outputPath);
        }
        if (m_validateConversionCheck->isChecked()) {
            validateConversion(m_inputFilePath, outputPath);
        }
        
        m_logSystem->info(QS("无损转换完成"));
    } else {
        m_logSystem->error(QS("无损转换失败"));
    }
    
    m_isConverting = false;
    updateUI();
}

void LosslessConverterTestWidget::onOpenOutputFile()
{
    if (!m_lastXmlPath.isEmpty() && QFileInfo(m_lastXmlPath).exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_lastXmlPath));
    }
}

void LosslessConverterTestWidget::onOpenOutputDir()
{
    if (!m_outputDirPath.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_outputDirPath));
    }
}

void LosslessConverterTestWidget::onClearResults()
{
    clearResults();
}

void LosslessConverterTestWidget::onClearDebugOutput()
{
    m_logSystem->clear();
}

void LosslessConverterTestWidget::onSaveDebugOutput()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QS("保存调试输出"),
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QS("/debug_output.txt"),
        QS("文本文件 (*.txt);;所有文件 (*.*)")
    );
    
    if (!fileName.isEmpty()) {
        m_logSystem->saveToFile(fileName);
    }
}

void LosslessConverterTestWidget::onConversionProgress(int progress, const QString &message)
{
    m_progressBar->setValue(progress);
    m_statusLabel->setText(message);
    m_logSystem->info(QString(QS("进度 %1%: %2")).arg(progress).arg(message));
}

void LosslessConverterTestWidget::onConversionFinished(LosslessDocumentConverter::ConvertStatus status, const QString &message)
{
    m_progressBar->setVisible(false);
    m_statusLabel->setText(message);
    
    if (status == LosslessDocumentConverter::ConvertStatus::SUCCESS) {
        m_logSystem->info(QS("转换成功: ") + message);
    } else {
        m_logSystem->error(QS("转换失败: ") + message);
    }
}

void LosslessConverterTestWidget::clearResults()
{
    m_xmlResultEdit->clear();
    m_formatInfoEdit->clear();
    m_positionInfoEdit->clear();
    m_elementTreeWidget->clear();
    m_validationEdit->clear();
    m_hasResult = false;
    m_lastXmlPath.clear();
    updateUI();
}

void LosslessConverterTestWidget::displayXmlResult(const QString &xmlPath)
{
    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_logSystem->error(QS("无法打开XML文件: ") + xmlPath);
        return;
    }
    
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QString content = stream.readAll();
    file.close();
    
    m_xmlResultEdit->setPlainText(content);
    m_logSystem->info(QS("XML结果已显示"));
}

void LosslessConverterTestWidget::displayElementTree(const QString &xmlPath)
{
    m_elementTreeWidget->clear();
    
    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_logSystem->error(QS("无法打开XML文件: ") + xmlPath);
        return;
    }
    
    QXmlStreamReader reader(&file);
    QStack<QTreeWidgetItem*> itemStack;
    
    while (!reader.atEnd()) {
        reader.readNext();
        
        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, elementName);
            
            // 添加属性
            QXmlStreamAttributes attributes = reader.attributes();
            QStringList attributeList;
            for (const QXmlStreamAttribute &attr : attributes) {
                attributeList << attr.name().toString() + QS("=") + attr.value().toString();
            }
            if (!attributeList.isEmpty()) {
                item->setText(1, attributeList.join(QS(", ")));
            }
            
            if (itemStack.isEmpty()) {
                m_elementTreeWidget->addTopLevelItem(item);
            } else {
                itemStack.top()->addChild(item);
            }
            itemStack.push(item);
            
        } else if (reader.isEndElement()) {
            if (!itemStack.isEmpty()) {
                itemStack.pop();
            }
        }
    }
    
    file.close();
    m_elementTreeWidget->expandAll();
    m_logSystem->info(QS("元素树已显示"));
}

void LosslessConverterTestWidget::displayFormatInfo(const QString &xmlPath)
{
    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_logSystem->error(QS("无法打开XML文件: ") + xmlPath);
        return;
    }
    
    QXmlStreamReader reader(&file);
    QStringList formatInfo;
    
    while (!reader.atEnd()) {
        reader.readNext();
        
        if (reader.isStartElement() && reader.name().toString() == QS("Format")) {
            QXmlStreamAttributes attributes = reader.attributes();
            QStringList formatAttrs;
            for (const QXmlStreamAttribute &attr : attributes) {
                formatAttrs << attr.name().toString() + QS(": ") + attr.value().toString();
            }
            if (!formatAttrs.isEmpty()) {
                formatInfo << QS("格式信息: ") + formatAttrs.join(QS(", "));
            }
        }
    }
    
    file.close();
    m_formatInfoEdit->setPlainText(formatInfo.join(QS("\n")));
    m_logSystem->info(QS("格式信息已显示"));
}

void LosslessConverterTestWidget::displayPositionInfo(const QString &xmlPath)
{
    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_logSystem->error(QS("无法打开XML文件: ") + xmlPath);
        return;
    }
    
    QXmlStreamReader reader(&file);
    QStringList positionInfo;
    
    while (!reader.atEnd()) {
        reader.readNext();
        
        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
            QXmlStreamAttributes attributes = reader.attributes();
            
            QStringList posAttrs;
            if (attributes.hasAttribute(QS("x"))) posAttrs << QS("x: ") + attributes.value(QS("x")).toString();
            if (attributes.hasAttribute(QS("y"))) posAttrs << QS("y: ") + attributes.value(QS("y")).toString();
            if (attributes.hasAttribute(QS("width"))) posAttrs << QS("width: ") + attributes.value(QS("width")).toString();
            if (attributes.hasAttribute(QS("height"))) posAttrs << QS("height: ") + attributes.value(QS("height")).toString();
            if (attributes.hasAttribute(QS("page"))) posAttrs << QS("page: ") + attributes.value(QS("page")).toString();
            
            if (!posAttrs.isEmpty()) {
                positionInfo << elementName + QS(" - ") + posAttrs.join(QS(", "));
            }
        }
    }
    
    file.close();
    m_positionInfoEdit->setPlainText(positionInfo.join(QS("\n")));
    m_logSystem->info(QS("位置信息已显示"));
}

void LosslessConverterTestWidget::validateConversion(const QString &originalPath, const QString &xmlPath)
{
    QStringList validationResults;
    
    // 基本验证
    QFileInfo originalInfo(originalPath);
    QFileInfo xmlInfo(xmlPath);
    
    validationResults << QS("=== 转换完整性验证 ===");
    validationResults << QS("原始文件: ") + originalInfo.fileName();
    validationResults << QS("XML文件: ") + xmlInfo.fileName();
    validationResults << QS("原始文件大小: ") + QString::number(originalInfo.size()) + QS(" 字节");
    validationResults << QS("XML文件大小: ") + QString::number(xmlInfo.size()) + QS(" 字节");
    
    // 检查XML结构
    QFile file(xmlPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QXmlStreamReader reader(&file);
        int elementCount = 0;
        
        while (!reader.atEnd()) {
            reader.readNext();
            if (reader.isStartElement()) {
                elementCount++;
            }
        }
        
        validationResults << QS("XML元素数量: ") + QString::number(elementCount);
        file.close();
    }
    
    validationResults << QS("验证完成");
    
    m_validationEdit->setPlainText(validationResults.join(QS("\n")));
    m_logSystem->info(QS("转换验证已完成"));
}

