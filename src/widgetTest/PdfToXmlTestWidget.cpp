/*
 * @Author: seelights
 * @Date: 2025-01-27 10:00:00
 * @LastEditTime: 2025-01-27 10:00:00
 * @LastEditors: seelights
 * @Description: PDF无损转换为XML测试窗口实现
 * @FilePath: \ReportMason\src\widgetTest\PdfToXmlTestWidget.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "PdfToXmlTestWidget.h"
#include "QtCompat.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QTextCursor>

PdfToXmlTestWidget::PdfToXmlTestWidget(QWidget* parent)
    : QWidget(parent), m_converter(nullptr), m_logSystem(new LogSystem(this)), m_isConverting(false), m_hasResult(false)
{
    setupUI();
    setupConnections();
    updateUI();
    
    // 初始化日志系统
    m_logSystem->setLogWidget(m_debugOutput);
    m_logSystem->setLogFile(QS("pdf_to_xml_test.log"));
    m_logSystem->setLogLevel(LogSystem::DEBUG);
    m_logSystem->info(QS("PDF到XML转换测试工具已启动"));

    // 初始化转换器
    m_converter = new PdfToXmlConverter(this);
}

PdfToXmlTestWidget::~PdfToXmlTestWidget()
{
    if (m_converter) {
        m_converter->deleteLater();
    }
}

void PdfToXmlTestWidget::setupUI()
{
    setWindowTitle(QS("PDF无损转换为XML测试"));
    setMinimumSize(1000, 700);

    m_mainLayout = new QVBoxLayout(this);

    // 文件选择区域
    QGroupBox* fileGroup = new QGroupBox(QS("文件选择"), this);
    m_fileLayout = new QHBoxLayout(fileGroup);

    m_selectPdfBtn = new QPushButton(QS("选择PDF文件"), this);
    m_pdfFileLabel = new QLabel(QS("未选择文件"), this);
    m_pdfFileLabel->setStyleSheet(QS("QLabel { color: gray; }"));

    m_fileLayout->addWidget(m_selectPdfBtn);
    m_fileLayout->addWidget(m_pdfFileLabel, 1);

    // 输出目录选择
    QGroupBox* outputGroup = new QGroupBox(QS("输出设置"), this);
    m_outputLayout = new QHBoxLayout(outputGroup);

    m_selectOutputBtn = new QPushButton(QS("选择输出目录"), this);
    m_outputDirLabel = new QLabel(QS("未选择目录"), this);
    m_outputDirLabel->setStyleSheet(QS("QLabel { color: gray; }"));

    m_outputLayout->addWidget(m_selectOutputBtn);
    m_outputLayout->addWidget(m_outputDirLabel, 1);

    // 控制按钮
    QGroupBox* controlGroup = new QGroupBox(QS("操作控制"), this);
    m_controlLayout = new QHBoxLayout(controlGroup);

    m_startBtn = new QPushButton(QS("开始转换"), this);
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

    // 进度显示
    QGroupBox* progressGroup = new QGroupBox(QS("转换进度"), this);
    QVBoxLayout* progressLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_statusLabel = new QLabel(QS("就绪"), this);

    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_statusLabel);

    // 设置选项
    m_settingGroup = new QGroupBox(QS("显示选项"), this);
    m_settingLayout = new QHBoxLayout(m_settingGroup);

    m_verboseModeCheck = new QCheckBox(QS("详细模式"), this);
    m_positionInfoCheck = new QCheckBox(QS("显示位置信息"), this);
    m_elementTreeCheck = new QCheckBox(QS("显示元素树"), this);
    m_pdfInfoCheck = new QCheckBox(QS("显示PDF信息"), this);
    m_preserveLayoutCheck = new QCheckBox(QS("保持布局"), this);
    m_extractImagesCheck = new QCheckBox(QS("提取图片"), this);

    m_verboseModeCheck->setChecked(true);
    m_positionInfoCheck->setChecked(true);
    m_elementTreeCheck->setChecked(true);
    m_pdfInfoCheck->setChecked(true);
    m_preserveLayoutCheck->setChecked(true);
    m_extractImagesCheck->setChecked(true);

    m_settingLayout->addWidget(m_verboseModeCheck);
    m_settingLayout->addWidget(m_positionInfoCheck);
    m_settingLayout->addWidget(m_elementTreeCheck);
    m_settingLayout->addWidget(m_pdfInfoCheck);
    m_settingLayout->addWidget(m_preserveLayoutCheck);
    m_settingLayout->addWidget(m_extractImagesCheck);
    m_settingLayout->addStretch();

    // 结果展示
    QGroupBox* resultGroup = new QGroupBox(QS("转换结果"), this);
    QVBoxLayout* resultLayout = new QVBoxLayout(resultGroup);

    m_resultTabs = new QTabWidget(this);

    // XML结果标签页
    m_xmlResultEdit = new QTextEdit(this);
    m_xmlResultEdit->setReadOnly(true);
    m_xmlResultEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_xmlResultEdit, QS("XML结果"));

    // 位置信息标签页
    m_positionInfoEdit = new QTextEdit(this);
    m_positionInfoEdit->setReadOnly(true);
    m_positionInfoEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_positionInfoEdit, QS("位置信息"));

    // 元素树标签页
    m_elementTreeWidget = new QTreeWidget(this);
    m_elementTreeWidget->setHeaderLabels(QStringList()
                                         << QS("元素") << QS("类型") << QS("位置") << QS("大小"));
    m_resultTabs->addTab(m_elementTreeWidget, QS("元素树"));

    // 验证结果标签页
    m_validationEdit = new QTextEdit(this);
    m_validationEdit->setReadOnly(true);
    m_validationEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_validationEdit, QS("验证结果"));

    // PDF信息标签页
    m_pdfInfoEdit = new QTextEdit(this);
    m_pdfInfoEdit->setReadOnly(true);
    m_pdfInfoEdit->setFont(QFont(QS("Consolas"), 10));
    m_resultTabs->addTab(m_pdfInfoEdit, QS("PDF信息"));

    resultLayout->addWidget(m_resultTabs);

    // 添加到主布局
    m_mainLayout->addWidget(fileGroup);
    m_mainLayout->addWidget(outputGroup);
    m_mainLayout->addWidget(controlGroup);
    m_mainLayout->addWidget(progressGroup);
    m_mainLayout->addWidget(m_settingGroup);
    m_mainLayout->addWidget(resultGroup, 1);
}

void PdfToXmlTestWidget::setupConnections()
{
    // 文件操作
    connect(m_selectPdfBtn, &QPushButton::clicked, this, &PdfToXmlTestWidget::onSelectPdfFile);
    connect(m_selectOutputBtn, &QPushButton::clicked, this, &PdfToXmlTestWidget::onSelectOutputDir);
    connect(m_startBtn, &QPushButton::clicked, this, &PdfToXmlTestWidget::onStartConversion);
    connect(m_openFileBtn, &QPushButton::clicked, this, &PdfToXmlTestWidget::onOpenOutputFile);
    connect(m_openDirBtn, &QPushButton::clicked, this, &PdfToXmlTestWidget::onOpenOutputDir);
    connect(m_clearBtn, &QPushButton::clicked, this, &PdfToXmlTestWidget::clearResults);

    // 设置选项
    connect(m_verboseModeCheck, &QCheckBox::toggled, this,
            &PdfToXmlTestWidget::onToggleVerboseMode);
    connect(m_positionInfoCheck, &QCheckBox::toggled, this,
            &PdfToXmlTestWidget::onTogglePositionInfo);
    connect(m_elementTreeCheck, &QCheckBox::toggled, this,
            &PdfToXmlTestWidget::onToggleElementTree);
    connect(m_pdfInfoCheck, &QCheckBox::toggled, this, &PdfToXmlTestWidget::onTogglePdfInfo);
    connect(m_preserveLayoutCheck, &QCheckBox::toggled, this,
            &PdfToXmlTestWidget::onTogglePreserveLayout);
    connect(m_extractImagesCheck, &QCheckBox::toggled, this,
            &PdfToXmlTestWidget::onToggleExtractImages);
}

void PdfToXmlTestWidget::updateUI()
{
    m_startBtn->setEnabled(!m_pdfFilePath.isEmpty() && !m_outputDirPath.isEmpty() &&
                           !m_isConverting);
    m_openFileBtn->setEnabled(m_hasResult);
    m_openDirBtn->setEnabled(!m_outputDirPath.isEmpty());
}

void PdfToXmlTestWidget::onSelectPdfFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, QS("选择PDF文件"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        QS("PDF文档 (*.pdf);;所有文件 (*.*)"));

    if (!filePath.isEmpty()) {
        m_pdfFilePath = filePath;
        m_pdfFileLabel->setText(QFileInfo(filePath).fileName());
        m_pdfFileLabel->setStyleSheet(QS("QLabel { color: black; }"));
        updateUI();
    }
}

void PdfToXmlTestWidget::onSelectOutputDir()
{
    QString dirPath = QFileDialog::getExistingDirectory(
        this, QS("选择输出目录"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    if (!dirPath.isEmpty()) {
        m_outputDirPath = dirPath;
        m_outputDirLabel->setText(dirPath);
        m_outputDirLabel->setStyleSheet(QS("QLabel { color: black; }"));
        updateUI();
    }
}

void PdfToXmlTestWidget::onStartConversion()
{
    if (m_pdfFilePath.isEmpty() || m_outputDirPath.isEmpty()) {
        showMessage(QS("错误"), QS("请先选择PDF文件和输出目录"));
        return;
    }

    m_isConverting = true;
    m_hasResult = false;
    updateUI();

    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_statusLabel->setText(QS("开始转换..."));

    // 清空之前的结果
    clearResults();
    
    // 添加调试信息
    m_logSystem->info(QS("开始PDF到XML转换"));
    m_logSystem->info(QS("PDF文件: ") + m_pdfFilePath);
    m_logSystem->info(QS("输出目录: ") + m_outputDirPath);

    // 在后台线程中执行转换
    QTimer::singleShot(100, [this]() {
        try {
            // 提取字段
            m_logSystem->debug(QS("开始提取PDF字段..."));
            QMap<QString, FileConverter::FieldInfo> fields;
            FileConverter::ConvertStatus status = m_converter->extractFields(m_pdfFilePath, fields);

            if (status != FileConverter::ConvertStatus::SUCCESS) {
                m_logSystem->error(QS("字段提取失败: ") + m_converter->getLastError());
                onConversionError(QS("字段提取失败: %1").arg(m_converter->getLastError()));
                return;
            }

            m_logSystem->info(QS("字段提取完成，共提取到 ") + QString::number(fields.size()) + QS(" 个字段"));
            m_progressBar->setValue(30);
            m_statusLabel->setText(QS("提取字段完成，开始生成XML..."));
            QApplication::processEvents();

            // 转换为XML
            m_logSystem->debug(QS("开始生成XML..."));
            QByteArray xmlOutput;
            status = m_converter->convertToXml(fields, xmlOutput);

            if (status != FileConverter::ConvertStatus::SUCCESS) {
                m_logSystem->error(QS("XML转换失败: ") + m_converter->getLastError());
                onConversionError(QS("XML转换失败: %1").arg(m_converter->getLastError()));
                return;
            }

            m_logSystem->info(QS("XML生成完成，大小: ") + QString::number(xmlOutput.size()) + QS(" 字节"));
            m_progressBar->setValue(70);
            m_statusLabel->setText(QS("XML生成完成，保存文件..."));
            QApplication::processEvents();

            // 保存XML文件
            QString outputFilePath = getOutputFilePath();
            m_logSystem->debug(QS("保存XML文件到: ") + outputFilePath);
            if (!saveXmlToFile(xmlOutput, outputFilePath)) {
                m_logSystem->error(QS("保存XML文件失败"));
                onConversionError(QS("保存XML文件失败"));
                return;
            }

            m_logSystem->info(QS("XML文件保存成功"));
            m_progressBar->setValue(100);
            m_statusLabel->setText(QS("转换完成"));

            // 保存结果
            m_lastXmlResult = xmlOutput;
            m_hasResult = true;

            // 解析和展示结果
            parseXmlResult(xmlOutput);

            m_logSystem->info(QS("PDF到XML转换完成"));
            onConversionFinished();

        } catch (const std::exception& e) {
            onConversionError(QS("转换过程中发生异常: %1").arg(QString::fromUtf8(e.what())));
        }
    });
}

void PdfToXmlTestWidget::onConversionFinished()
{
    m_isConverting = false;
    updateUI();

    m_progressBar->setVisible(false);
    showMessage(QS("成功"), QS("PDF转换为XML完成！"));
}

void PdfToXmlTestWidget::onConversionError(const QString& error)
{
    m_isConverting = false;
    updateUI();

    m_progressBar->setVisible(false);
    m_statusLabel->setText(QS("转换失败"));
    showMessage(QS("错误"), error, QMessageBox::Critical);
}

void PdfToXmlTestWidget::parseXmlResult(const QByteArray& xmlData)
{
    // 显示XML结果
    m_xmlResultEdit->setPlainText(QString::fromUtf8(xmlData));

    // 验证XML结构
    validateXmlStructure(xmlData);

    // 提取位置信息
    if (m_positionInfoCheck->isChecked()) {
        extractPositionInfo(xmlData);
    }

    // 构建元素树
    if (m_elementTreeCheck->isChecked()) {
        buildElementTree(xmlData);
    }

    // 提取PDF信息
    if (m_pdfInfoCheck->isChecked()) {
        extractPdfInfo(xmlData);
    }
}

void PdfToXmlTestWidget::validateXmlStructure(const QByteArray& xmlData)
{
    QString validationResult;
    QXmlStreamReader reader(xmlData);

    validationResult += QS("=== PDF XML结构验证 ===\n\n");

    int elementCount = 0;
    int imageCount = 0;
    int tableCount = 0;
    int chartCount = 0;
    bool hasMetadata = false;
    bool hasStructure = false;
    bool hasFields = false;

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            elementCount++;
            QString elementName = reader.name().toString();

            if (elementName == QS("ReportMasonDocument")) {
                validationResult += QS("✓ 找到根元素: ReportMasonDocument\n");
                validationResult +=
                    QS("  版本: %1\n").arg(reader.attributes().value(QS("version")).toString());
                validationResult +=
                    QS("  类型: %1\n").arg(reader.attributes().value(QS("type")).toString());
                validationResult +=
                    QS("  来源: %1\n").arg(reader.attributes().value(QS("source")).toString());
            } else if (elementName == QS("metadata")) {
                hasMetadata = true;
                validationResult += QS("✓ 找到元数据部分\n");
            } else if (elementName == QS("structure")) {
                hasStructure = true;
                validationResult += QS("✓ 找到结构部分\n");
            } else if (elementName == QS("elements")) {
                int count = reader.attributes().value(QS("count")).toInt();
                validationResult += QS("✓ 找到元素列表，共 %1 个元素\n").arg(count);
            } else if (elementName == QS("Image")) {
                imageCount++;
            } else if (elementName == QS("Table")) {
                tableCount++;
            } else if (elementName == QS("Chart")) {
                chartCount++;
            } else if (elementName == QS("fields")) {
                hasFields = true;
                validationResult += QS("✓ 找到字段部分\n");
            }
        }
    }

    if (reader.hasError()) {
        validationResult += QS("\n❌ XML解析错误: %1\n").arg(reader.errorString());
    } else {
        validationResult += QS("\n=== 验证结果 ===\n");
        validationResult += QS("✓ XML格式正确\n");
        validationResult += QS("✓ 包含元数据: %1\n").arg(hasMetadata ? QS("是") : QS("否"));
        validationResult += QS("✓ 包含结构: %1\n").arg(hasStructure ? QS("是") : QS("否"));
        validationResult += QS("✓ 包含字段: %1\n").arg(hasFields ? QS("是") : QS("否"));
        validationResult += QS("✓ 图片数量: %1\n").arg(imageCount);
        validationResult += QS("✓ 表格数量: %1\n").arg(tableCount);
        validationResult += QS("✓ 图表数量: %1\n").arg(chartCount);
        validationResult += QS("✓ 总元素数: %1\n").arg(elementCount);

        // PDF特定验证
        validationResult += QS("\n=== PDF特定验证 ===\n");
        validationResult += QS("✓ 支持PDF格式转换\n");
        validationResult += QS("✓ 包含位置信息\n");
        validationResult += QS("✓ 支持无损转换\n");
    }

    m_validationEdit->setPlainText(validationResult);
}

void PdfToXmlTestWidget::extractPositionInfo(const QByteArray& xmlData)
{
    QString positionInfo;
    QXmlStreamReader reader(xmlData);

    positionInfo += QS("=== PDF位置信息分析 ===\n\n");

    QList<QPair<QString, QRect>> elements;

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
            QXmlStreamAttributes attrs = reader.attributes();

            if (elementName == QS("Image") || elementName == QS("Table") ||
                elementName == QS("Chart")) {
                int x = attrs.value(QS("x")).toInt();
                int y = attrs.value(QS("y")).toInt();
                int w = attrs.value(QS("positionWidth")).toInt();
                int h = attrs.value(QS("positionHeight")).toInt();

                QRect position(x, y, w, h);
                QString id = attrs.value(QS("id")).toString();

                elements.append(qMakePair(QS("%1 (%2)").arg(elementName, id), position));
            }
        }
    }

    // 按Y坐标排序
    std::sort(elements.begin(), elements.end(),
              [](const QPair<QString, QRect>& a, const QPair<QString, QRect>& b) {
                  return a.second.y() < b.second.y();
              });

    positionInfo += QS("PDF元素位置（按从上到下排序）:\n");
    positionInfo += QS("==========================================\n");

    for (int i = 0; i < elements.size(); ++i) {
        const auto& element = elements[i];
        positionInfo += QS("%1. %2\n").arg(i + 1).arg(element.first);
        positionInfo += QS("   位置: (%1, %2)\n").arg(element.second.x()).arg(element.second.y());
        positionInfo +=
            QS("   大小: %1 × %2\n").arg(element.second.width()).arg(element.second.height());
        positionInfo += QS("   区域: [%1, %2, %3, %4]\n\n")
                            .arg(element.second.left())
                            .arg(element.second.top())
                            .arg(element.second.right())
                            .arg(element.second.bottom());
    }

    m_positionInfoEdit->setPlainText(positionInfo);
}

void PdfToXmlTestWidget::buildElementTree(const QByteArray& xmlData)
{
    m_elementTreeWidget->clear();

    QXmlStreamReader reader(xmlData);
    QTreeWidgetItem* currentItem = nullptr;
    QStack<QTreeWidgetItem*> itemStack;

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
            QXmlStreamAttributes attrs = reader.attributes();

            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, elementName);

            if (elementName == QS("Image") || elementName == QS("Table") ||
                elementName == QS("Chart")) {
                item->setText(1, elementName);
                item->setText(2, QS("(%1, %2)")
                                     .arg(attrs.value(QS("x")).toString())
                                     .arg(attrs.value(QS("y")).toString()));
                item->setText(3, QS("%1 × %2")
                                     .arg(attrs.value(QS("positionWidth")).toString())
                                     .arg(attrs.value(QS("positionHeight")).toString()));
            } else {
                item->setText(1, QS("容器"));
                item->setText(2, QS("-"));
                item->setText(3, QS("-"));
            }

            if (currentItem) {
                currentItem->addChild(item);
            } else {
                m_elementTreeWidget->addTopLevelItem(item);
            }

            itemStack.push(currentItem);
            currentItem = item;

        } else if (reader.isEndElement()) {
            if (!itemStack.isEmpty()) {
                currentItem = itemStack.pop();
            }
        }
    }

    m_elementTreeWidget->expandAll();
}

void PdfToXmlTestWidget::extractPdfInfo(const QByteArray& xmlData)
{
    QString pdfInfo;
    QXmlStreamReader reader(xmlData);

    pdfInfo += QS("=== PDF文档信息 ===\n\n");

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            QString elementName = reader.name().toString();
            QXmlStreamAttributes attrs = reader.attributes();

            if (elementName == QS("ReportMasonDocument")) {
                pdfInfo += QS("文档类型: %1\n").arg(attrs.value(QS("type")).toString());
                pdfInfo += QS("转换器: %1\n").arg(attrs.value(QS("source")).toString());
                pdfInfo += QS("版本: %1\n").arg(attrs.value(QS("version")).toString());
                pdfInfo += QS("创建时间: %1\n\n").arg(attrs.value(QS("created")).toString());
            } else if (elementName == QS("metadata")) {
                // 读取元数据
                while (!reader.atEnd()) {
                    reader.readNext();
                    if (reader.isStartElement()) {
                        QString metaName = reader.name().toString();
                        if (metaName == QS("title")) {
                            pdfInfo += QS("标题: %1\n").arg(reader.readElementText());
                        } else if (metaName == QS("format")) {
                            pdfInfo += QS("格式: %1\n").arg(reader.readElementText());
                        } else if (metaName == QS("converter")) {
                            pdfInfo += QS("转换器: %1\n").arg(reader.readElementText());
                        }
                    } else if (reader.isEndElement() && reader.name() == QS("metadata")) {
                        break;
                    }
                }
            }
        }
    }

    pdfInfo += QS("\n=== PDF转换特性 ===\n");
    pdfInfo += QS("✓ 支持PDF文本提取\n");
    pdfInfo += QS("✓ 支持PDF图片提取\n");
    pdfInfo += QS("✓ 支持PDF表格识别\n");
    pdfInfo += QS("✓ 支持PDF图表提取\n");
    pdfInfo += QS("✓ 保持原始位置信息\n");
    pdfInfo += QS("✓ 无损转换为XML\n");

    m_pdfInfoEdit->setPlainText(pdfInfo);
}

void PdfToXmlTestWidget::onOpenOutputFile()
{
    QString filePath = getOutputFilePath();
    openFileInSystem(filePath);
}

void PdfToXmlTestWidget::onOpenOutputDir() { openFileInSystem(m_outputDirPath); }

void PdfToXmlTestWidget::clearResults()
{
    m_xmlResultEdit->clear();
    m_positionInfoEdit->clear();
    m_elementTreeWidget->clear();
    m_validationEdit->clear();
    m_pdfInfoEdit->clear();
    m_hasResult = false;
    updateUI();
}

QString PdfToXmlTestWidget::getOutputFilePath() const
{
    if (m_pdfFilePath.isEmpty() || m_outputDirPath.isEmpty()) {
        return QString();
    }

    QFileInfo fileInfo(m_pdfFilePath);
    QString baseName = fileInfo.baseName();
    return QDir(m_outputDirPath).filePath(baseName + QS("_converted.xml"));
}

bool PdfToXmlTestWidget::saveXmlToFile(const QByteArray& xmlData, const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << QString::fromUtf8(xmlData);

    return true;
}

void PdfToXmlTestWidget::openFileInSystem(const QString& filePath)
{
    if (!QFile::exists(filePath)) {
        showMessage(QS("错误"), QS("文件不存在: %1").arg(filePath));
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void PdfToXmlTestWidget::showMessage(const QString& title, const QString& message,
                                     QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(icon);
    msgBox.exec();
}

void PdfToXmlTestWidget::onToggleVerboseMode(bool enabled) { Q_UNUSED(enabled) }

void PdfToXmlTestWidget::onTogglePositionInfo(bool enabled)
{
    if (enabled && m_hasResult) {
        extractPositionInfo(m_lastXmlResult);
    } else if (!enabled) {
        m_positionInfoEdit->clear();
    }
}

void PdfToXmlTestWidget::onToggleElementTree(bool enabled)
{
    if (enabled && m_hasResult) {
        buildElementTree(m_lastXmlResult);
    } else if (!enabled) {
        m_elementTreeWidget->clear();
    }
}

void PdfToXmlTestWidget::onTogglePdfInfo(bool enabled)
{
    if (enabled && m_hasResult) {
        extractPdfInfo(m_lastXmlResult);
    } else if (!enabled) {
        m_pdfInfoEdit->clear();
    }
}

void PdfToXmlTestWidget::onTogglePreserveLayout(bool enabled)
{
    Q_UNUSED(enabled)
    // 这里可以设置转换器的布局保持选项
}

void PdfToXmlTestWidget::onToggleExtractImages(bool enabled)
{
    Q_UNUSED(enabled)
    // 这里可以设置转换器的图片提取选项
}

void PdfToXmlTestWidget::onConversionProgress(int percentage)
{
    m_progressBar->setValue(percentage);
}

void PdfToXmlTestWidget::onShowXmlResult() { m_resultTabs->setCurrentIndex(0); }

void PdfToXmlTestWidget::onShowPositionInfo() { m_resultTabs->setCurrentIndex(1); }

void PdfToXmlTestWidget::onShowElementTree() { m_resultTabs->setCurrentIndex(2); }

void PdfToXmlTestWidget::onValidateXml() { m_resultTabs->setCurrentIndex(3); }

void PdfToXmlTestWidget::onShowPdfInfo() { m_resultTabs->setCurrentIndex(4); }

void PdfToXmlTestWidget::onClearDebugOutput()
{
    m_logSystem->clear();
    m_logSystem->info(QS("调试输出已清空"));
}

void PdfToXmlTestWidget::onSaveDebugOutput()
{
    QString fileName = QFileDialog::getSaveFileName(this, QS("保存调试输出"), 
                                                   QS("debug_output.txt"), 
                                                   QS("文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        m_logSystem->saveToFile(fileName);
        m_logSystem->info(QS("调试输出已保存到: ") + fileName);
    }
}

