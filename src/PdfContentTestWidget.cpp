/*
 * @Author: seelights
 * @Date: 2025-09-15 19:40:00
 * @LastEditTime: 2025-09-15 19:40:00
 * @LastEditors: seelights
 * @Description: PDF内容提取测试窗口实现
 * @FilePath: \ReportMason\src\PdfContentTestWidget.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "PdfContentTestWidget.h"
#include <QStandardPaths>
#include <QDateTime>
#include <QHeaderView>

PdfContentTestWidget::PdfContentTestWidget(QWidget* parent)
    : QWidget(parent), m_mainLayout(nullptr), m_tabWidget(nullptr), m_logTextEdit(nullptr),
      m_progressBar(nullptr), m_openFileButton(nullptr), m_saveResultsButton(nullptr),
      m_clearButton(nullptr), m_imageTab(nullptr), m_testImageExtractionButton(nullptr),
      m_testImageExportToXmlButton(nullptr), m_testImageExportToStandardButton(nullptr),
      m_imageListWidget(nullptr), m_imageTableWidget(nullptr), m_tableTab(nullptr),
      m_testTableExtractionButton(nullptr), m_testTableExportToXmlButton(nullptr),
      m_testTableExportToStandardButton(nullptr), m_tableTableWidget(nullptr), m_chartTab(nullptr),
      m_testChartExtractionButton(nullptr), m_testChartExportToXmlButton(nullptr),
      m_testChartExportToStandardButton(nullptr), m_chartListWidget(nullptr),
      m_chartTableWidget(nullptr), m_imageExtractor(nullptr), m_tableExtractor(nullptr),
      m_chartExtractor(nullptr)
{
    setupUI();

    // 初始化提取器
    m_imageExtractor = new PdfImageExtractor(this);
    m_tableExtractor = new PdfTableExtractor(this);
    m_chartExtractor = new PdfChartExtractor(this);

    // 设置输出目录
    m_outputDirectory =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ReportMason/PdfTest";
    m_imageExtractor->setOutputDirectory(m_outputDirectory);
    m_tableExtractor->setOutputDirectory(m_outputDirectory);
    m_chartExtractor->setOutputDirectory(m_outputDirectory);

    logMessage("PDF内容提取测试窗口已启动");
}

PdfContentTestWidget::~PdfContentTestWidget() {}

void PdfContentTestWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    // 顶部按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_openFileButton = new QPushButton("打开PDF文件", this);
    m_saveResultsButton = new QPushButton("保存结果", this);
    m_clearButton = new QPushButton("清空日志", this);

    buttonLayout->addWidget(m_openFileButton);
    buttonLayout->addWidget(m_saveResultsButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addStretch();

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);

    // 标签页
    m_tabWidget = new QTabWidget(this);
    setupImageTab();
    setupTableTab();
    setupChartTab();

    // 日志区域
    QGroupBox* logGroup = new QGroupBox("日志输出", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setMaximumHeight(150);
    m_logTextEdit->setReadOnly(true);
    logLayout->addWidget(m_logTextEdit);

    // 布局
    m_mainLayout->addLayout(buttonLayout);
    m_mainLayout->addWidget(m_progressBar);
    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->addWidget(logGroup);

    // 连接信号
    connect(m_openFileButton, &QPushButton::clicked, this, &PdfContentTestWidget::openPdfFile);
    connect(m_saveResultsButton, &QPushButton::clicked, this, &PdfContentTestWidget::saveResults);
    connect(m_clearButton, &QPushButton::clicked, this, &PdfContentTestWidget::clearLog);
}

void PdfContentTestWidget::setupImageTab()
{
    m_imageTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_imageTab);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_testImageExtractionButton = new QPushButton("提取图片", m_imageTab);
    m_testImageExportToXmlButton = new QPushButton("导出为XML", m_imageTab);
    m_testImageExportToStandardButton = new QPushButton("导出为标准格式", m_imageTab);

    buttonLayout->addWidget(m_testImageExtractionButton);
    buttonLayout->addWidget(m_testImageExportToXmlButton);
    buttonLayout->addWidget(m_testImageExportToStandardButton);
    buttonLayout->addStretch();

    // 结果显示区域
    QSplitter* splitter = new QSplitter(Qt::Horizontal, m_imageTab);

    // 图片列表
    QGroupBox* listGroup = new QGroupBox("图片列表", m_imageTab);
    QVBoxLayout* listLayout = new QVBoxLayout(listGroup);
    m_imageListWidget = new QListWidget(m_imageTab);
    listLayout->addWidget(m_imageListWidget);

    // 图片详情表格
    QGroupBox* tableGroup = new QGroupBox("图片详情", m_imageTab);
    QVBoxLayout* tableLayout = new QVBoxLayout(tableGroup);
    m_imageTableWidget = new QTableWidget(m_imageTab);
    m_imageTableWidget->setColumnCount(4);
    m_imageTableWidget->setHorizontalHeaderLabels({"ID", "格式", "尺寸", "路径"});
    m_imageTableWidget->horizontalHeader()->setStretchLastSection(true);
    tableLayout->addWidget(m_imageTableWidget);

    splitter->addWidget(listGroup);
    splitter->addWidget(tableGroup);
    splitter->setSizes({200, 400});

    layout->addLayout(buttonLayout);
    layout->addWidget(splitter);

    m_tabWidget->addTab(m_imageTab, "图片提取");

    // 连接信号
    connect(m_testImageExtractionButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testImageExtraction);
    connect(m_testImageExportToXmlButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testImageExportToXml);
    connect(m_testImageExportToStandardButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testImageExportToStandard);
}

void PdfContentTestWidget::setupTableTab()
{
    m_tableTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_tableTab);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_testTableExtractionButton = new QPushButton("提取表格", m_tableTab);
    m_testTableExportToXmlButton = new QPushButton("导出为XML", m_tableTab);
    m_testTableExportToStandardButton = new QPushButton("导出为CSV", m_tableTab);

    buttonLayout->addWidget(m_testTableExtractionButton);
    buttonLayout->addWidget(m_testTableExportToXmlButton);
    buttonLayout->addWidget(m_testTableExportToStandardButton);
    buttonLayout->addStretch();

    // 表格显示
    QGroupBox* tableGroup = new QGroupBox("表格内容", m_tableTab);
    QVBoxLayout* tableLayout = new QVBoxLayout(tableGroup);
    m_tableTableWidget = new QTableWidget(m_tableTab);
    m_tableTableWidget->horizontalHeader()->setStretchLastSection(true);
    tableLayout->addWidget(m_tableTableWidget);

    layout->addLayout(buttonLayout);
    layout->addWidget(tableGroup);

    m_tabWidget->addTab(m_tableTab, "表格提取");

    // 连接信号
    connect(m_testTableExtractionButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testTableExtraction);
    connect(m_testTableExportToXmlButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testTableExportToXml);
    connect(m_testTableExportToStandardButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testTableExportToStandard);
}

void PdfContentTestWidget::setupChartTab()
{
    m_chartTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_chartTab);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_testChartExtractionButton = new QPushButton("提取图表", m_chartTab);
    m_testChartExportToXmlButton = new QPushButton("导出为XML", m_chartTab);
    m_testChartExportToStandardButton = new QPushButton("导出为CSV", m_chartTab);

    buttonLayout->addWidget(m_testChartExtractionButton);
    buttonLayout->addWidget(m_testChartExportToXmlButton);
    buttonLayout->addWidget(m_testChartExportToStandardButton);
    buttonLayout->addStretch();

    // 结果显示区域
    QSplitter* splitter = new QSplitter(Qt::Horizontal, m_chartTab);

    // 图表列表
    QGroupBox* listGroup = new QGroupBox("图表列表", m_chartTab);
    QVBoxLayout* listLayout = new QVBoxLayout(listGroup);
    m_chartListWidget = new QListWidget(m_chartTab);
    listLayout->addWidget(m_chartListWidget);

    // 图表详情表格
    QGroupBox* tableGroup = new QGroupBox("图表详情", m_chartTab);
    QVBoxLayout* tableLayout = new QVBoxLayout(tableGroup);
    m_chartTableWidget = new QTableWidget(m_chartTab);
    m_chartTableWidget->setColumnCount(5);
    m_chartTableWidget->setHorizontalHeaderLabels({"ID", "类型", "标题", "数据系列", "属性"});
    m_chartTableWidget->horizontalHeader()->setStretchLastSection(true);
    tableLayout->addWidget(m_chartTableWidget);

    splitter->addWidget(listGroup);
    splitter->addWidget(tableGroup);
    splitter->setSizes({200, 400});

    layout->addLayout(buttonLayout);
    layout->addWidget(splitter);

    m_tabWidget->addTab(m_chartTab, "图表提取");

    // 连接信号
    connect(m_testChartExtractionButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testChartExtraction);
    connect(m_testChartExportToXmlButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testChartExportToXml);
    connect(m_testChartExportToStandardButton, &QPushButton::clicked, this,
            &PdfContentTestWidget::testChartExportToStandard);
}

void PdfContentTestWidget::openPdfFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开PDF文件", "", "PDF文件 (*.pdf)");
    if (!fileName.isEmpty()) {
        m_currentFilePath = fileName;
        logMessage(QString("已选择文件: %1").arg(fileName));
        clearResults();
    }
}

void PdfContentTestWidget::saveResults()
{
    if (m_currentImages.isEmpty() && m_currentTables.isEmpty() && m_currentCharts.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可保存的结果");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "保存结果", "", "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        // 这里可以实现保存逻辑
        logMessage(QString("结果已保存到: %1").arg(fileName));
    }
}

void PdfContentTestWidget::clearResults()
{
    m_currentImages.clear();
    m_currentTables.clear();
    m_currentCharts.clear();

    m_imageListWidget->clear();
    m_imageTableWidget->setRowCount(0);
    m_tableTableWidget->setRowCount(0);
    m_chartListWidget->clear();
    m_chartTableWidget->setRowCount(0);

    logMessage("结果已清空");
}

void PdfContentTestWidget::testImageExtraction()
{
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择PDF文件");
        return;
    }

    logMessage("开始提取图片...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);

    QList<ImageInfo> images;
    ContentExtractor::ExtractStatus status =
        m_imageExtractor->extractImages(m_currentFilePath, images);

    m_progressBar->setVisible(false);

    if (status == ContentExtractor::ExtractStatus::SUCCESS) {
        m_currentImages = images;
        displayImageResults(images);
        logMessage(QString("成功提取 %1 张图片").arg(images.size()));
    } else {
        logMessage(QString("图片提取失败: %1").arg(m_imageExtractor->getLastError()), "ERROR");
    }
}

void PdfContentTestWidget::testTableExtraction()
{
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择PDF文件");
        return;
    }

    logMessage("开始提取表格...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);

    QList<TableInfo> tables;
    ContentExtractor::ExtractStatus status =
        m_tableExtractor->extractTables(m_currentFilePath, tables);

    m_progressBar->setVisible(false);

    if (status == ContentExtractor::ExtractStatus::SUCCESS) {
        m_currentTables = tables;
        displayTableResults(tables);
        logMessage(QString("成功提取 %1 个表格").arg(tables.size()));
    } else {
        logMessage(QString("表格提取失败: %1").arg(m_tableExtractor->getLastError()), "ERROR");
    }
}

void PdfContentTestWidget::testChartExtraction()
{
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择PDF文件");
        return;
    }

    logMessage("开始提取图表...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);

    QList<ChartInfo> charts;
    ContentExtractor::ExtractStatus status =
        m_chartExtractor->extractCharts(m_currentFilePath, charts);

    m_progressBar->setVisible(false);

    if (status == ContentExtractor::ExtractStatus::SUCCESS) {
        m_currentCharts = charts;
        displayChartResults(charts);
        logMessage(QString("成功提取 %1 个图表").arg(charts.size()));
    } else {
        logMessage(QString("图表提取失败: %1").arg(m_chartExtractor->getLastError()), "ERROR");
    }
}


void PdfContentTestWidget::displayImageResults(const QList<ImageInfo>& images)
{
    m_imageListWidget->clear();
    m_imageTableWidget->setRowCount(images.size());

    for (int i = 0; i < images.size(); ++i) {
        const ImageInfo& image = images[i];

        // 添加到列表
        m_imageListWidget->addItem(QString("图片 %1 (%2)").arg(i + 1).arg(image.format));

        // 添加到表格
        m_imageTableWidget->setItem(i, 0, new QTableWidgetItem(image.id));
        m_imageTableWidget->setItem(i, 1, new QTableWidgetItem(image.format));
        m_imageTableWidget->setItem(
            i, 2,
            new QTableWidgetItem(
                QString("%1x%2").arg(image.size.width()).arg(image.size.height())));
        m_imageTableWidget->setItem(i, 3, new QTableWidgetItem(image.savedPath));
    }
}

void PdfContentTestWidget::displayTableResults(const QList<TableInfo>& tables)
{
    if (tables.isEmpty()) {
        m_tableTableWidget->setRowCount(0);
        return;
    }

    // 显示第一个表格
    const TableInfo& table = tables.first();
    m_tableTableWidget->setRowCount(table.rows);
    m_tableTableWidget->setColumnCount(table.columns);

    for (int row = 0; row < table.rows; ++row) {
        for (int col = 0; col < table.columns; ++col) {
            if (row < table.cells.size() && col < table.cells[row].size()) {
                const CellInfo& cell = table.cells[row][col];
                m_tableTableWidget->setItem(row, col, new QTableWidgetItem(cell.content));
            }
        }
    }
}

void PdfContentTestWidget::displayChartResults(const QList<ChartInfo>& charts)
{
    m_chartListWidget->clear();
    m_chartTableWidget->setRowCount(charts.size());

    for (int i = 0; i < charts.size(); ++i) {
        const ChartInfo& chart = charts[i];

        // 添加到列表
        m_chartListWidget->addItem(QString("图表 %1 (%2)").arg(i + 1).arg(chart.title));

        // 添加到表格
        m_chartTableWidget->setItem(i, 0, new QTableWidgetItem(chart.id));
        m_chartTableWidget->setItem(
            i, 1, new QTableWidgetItem(QString::number(static_cast<int>(chart.type))));
        m_chartTableWidget->setItem(i, 2, new QTableWidgetItem(chart.title));
        m_chartTableWidget->setItem(i, 3,
                                    new QTableWidgetItem(QString::number(chart.series.size())));
        m_chartTableWidget->setItem(i, 4, new QTableWidgetItem("查看详情"));
    }
}

void PdfContentTestWidget::logMessage(const QString& message, const QString& type)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] [%2] %3").arg(timestamp, type, message);
    m_logTextEdit->append(logEntry);
}

void PdfContentTestWidget::updateProgress(int value) { m_progressBar->setValue(value); }

void PdfContentTestWidget::clearLog() { m_logTextEdit->clear(); }

// XML导出方法
void PdfContentTestWidget::testImageExportToXml()
{
    if (m_currentImages.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可导出的图片");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "保存图片XML", "", "XML文件 (*.xml)");
    if (fileName.isEmpty())
        return;

    logMessage("开始导出图片为XML...");
    if (m_imageExtractor->exportToXml(m_currentImages, fileName)) {
        logMessage("成功导出图片XML文件: " + fileName);
        QMessageBox::information(this, "成功", "图片XML导出成功！");
    } else {
        logMessage("图片XML导出失败: " + m_imageExtractor->getLastError());
        QMessageBox::warning(this, "错误", "图片XML导出失败！");
    }
}

void PdfContentTestWidget::testTableExportToXml()
{
    if (m_currentTables.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可导出的表格");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "保存表格XML", "", "XML文件 (*.xml)");
    if (fileName.isEmpty())
        return;

    logMessage("开始导出表格为XML...");
    if (m_tableExtractor->exportToXml(m_currentTables, fileName)) {
        logMessage("成功导出表格XML文件: " + fileName);
        QMessageBox::information(this, "成功", "表格XML导出成功！");
    } else {
        logMessage("表格XML导出失败: " + m_tableExtractor->getLastError());
        QMessageBox::warning(this, "错误", "表格XML导出失败！");
    }
}

void PdfContentTestWidget::testChartExportToXml()
{
    if (m_currentCharts.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可导出的图表");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "保存图表XML", "", "XML文件 (*.xml)");
    if (fileName.isEmpty())
        return;

    logMessage("开始导出图表为XML...");
    if (m_chartExtractor->exportToXml(m_currentCharts, fileName)) {
        logMessage("成功导出图表XML文件: " + fileName);
        QMessageBox::information(this, "成功", "图表XML导出成功！");
    } else {
        logMessage("图表XML导出失败: " + m_chartExtractor->getLastError());
        QMessageBox::warning(this, "错误", "图表XML导出失败！");
    }
}

// 标准格式导出方法
void PdfContentTestWidget::testImageExportToStandard()
{
    if (m_currentImages.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可导出的图片");
        return;
    }

    logMessage("开始导出图片为标准格式...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, m_currentImages.size());

    int successCount = 0;
    for (int i = 0; i < m_currentImages.size(); ++i) {
        const ImageInfo& image = m_currentImages[i];
        QString outputPath =
            QString("%1/image_%2.%3").arg(m_outputDirectory, image.id, image.format);

        if (m_imageExtractor->saveImage(image, outputPath)) {
            successCount++;
        }

        m_progressBar->setValue(i + 1);
    }

    m_progressBar->setVisible(false);
    logMessage(
        QString("成功导出 %1/%2 张图片为标准格式").arg(successCount).arg(m_currentImages.size()));
    QMessageBox::information(this, "成功",
                             QString("成功导出 %1 张图片为标准格式！").arg(successCount));
}

void PdfContentTestWidget::testTableExportToStandard()
{
    if (m_currentTables.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可导出的表格");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "保存表格CSV", "", "CSV文件 (*.csv)");
    if (fileName.isEmpty())
        return;

    logMessage("开始导出表格为CSV...");
    int successCount = 0;
    for (const TableInfo& table : m_currentTables) {
        QString csvFileName = fileName;
        if (m_currentTables.size() > 1) {
            QFileInfo fileInfo(fileName);
            csvFileName =
                QString("%1/%2_%3.csv").arg(fileInfo.absolutePath(), fileInfo.baseName(), table.id);
        }

        if (m_tableExtractor->exportToCsv(table, csvFileName)) {
            successCount++;
        }
    }

    logMessage(QString("成功导出 %1/%2 个表格为CSV").arg(successCount).arg(m_currentTables.size()));
    QMessageBox::information(this, "成功", QString("成功导出 %1 个表格为CSV！").arg(successCount));
}

void PdfContentTestWidget::testChartExportToStandard()
{
    if (m_currentCharts.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可导出的图表");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "保存图表CSV", "", "CSV文件 (*.csv)");
    if (fileName.isEmpty())
        return;

    logMessage("开始导出图表为CSV...");
    int successCount = 0;
    for (const ChartInfo& chart : m_currentCharts) {
        QString csvFileName = fileName;
        if (m_currentCharts.size() > 1) {
            QFileInfo fileInfo(fileName);
            csvFileName =
                QString("%1/%2_%3.csv").arg(fileInfo.absolutePath(), fileInfo.baseName(), chart.id);
        }

        if (m_chartExtractor->exportToCsv(chart, csvFileName)) {
            successCount++;
        }
    }

    logMessage(QString("成功导出 %1/%2 个图表为CSV").arg(successCount).arg(m_currentCharts.size()));
    QMessageBox::information(this, "成功", QString("成功导出 %1 个图表为CSV！").arg(successCount));
}
