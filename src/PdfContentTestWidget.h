/*
 * @Author: seelights
 * @Date: 2025-09-15 19:40:00
 * @LastEditTime: 2025-09-15 19:40:00
 * @LastEditors: seelights
 * @Description: PDF内容提取测试窗口
 * @FilePath: \ReportMason\src\PdfContentTestWidget.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#ifndef PDFCONTENTTESTWIDGET_H
#define PDFCONTENTTESTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>
#include <QProgressBar>
#include <QGroupBox>
#include <QTableWidget>
#include <QListWidget>
#include <QSplitter>
#include <QMessageBox>
#include "QtCompat.h"

// PDF内容提取器
#include "tools/pdf/PdfImageExtractor.h"
#include "tools/pdf/PdfTableExtractor.h"
#include "tools/pdf/PdfChartExtractor.h"

/**
 * @brief PDF内容提取测试窗口
 *
 * 专门用于测试PDF文件的内容提取功能
 */
class PdfContentTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PdfContentTestWidget(QWidget* parent = nullptr);
    ~PdfContentTestWidget();

private slots:
    // 文件操作
    void openPdfFile();
    void saveResults();
    void clearResults();

    // 内容提取测试
    void testImageExtraction();
    void testTableExtraction();
    void testChartExtraction();

    // 导出测试 - XML格式
    void testImageExportToXml();
    void testTableExportToXml();
    void testChartExportToXml();

    // 导出测试 - 标准格式
    void testImageExportToStandard();
    void testTableExportToStandard();
    void testChartExportToStandard();

private:
    void setupUI();
    void setupImageTab();
    void setupTableTab();
    void setupChartTab();

    void logMessage(const QString& message, const QString& type = QS("INFO"));
    void updateProgress(int value);
    void clearLog();

    // 显示结果
    void displayImageResults(const QList<ImageInfo>& images);
    void displayTableResults(const QList<TableInfo>& tables);
    void displayChartResults(const QList<ChartInfo>& charts);

private:
    // UI 组件
    QVBoxLayout* m_mainLayout;
    QTabWidget* m_tabWidget;
    QTextEdit* m_logTextEdit;
    QProgressBar* m_progressBar;
    QPushButton* m_openFileButton;
    QPushButton* m_saveResultsButton;
    QPushButton* m_clearButton;

    // 文件相关
    QString m_currentFilePath;
    QString m_outputDirectory;

    // 图片提取测试
    QWidget* m_imageTab;
    QPushButton* m_testImageExtractionButton;
    QPushButton* m_testImageExportToXmlButton;
    QPushButton* m_testImageExportToStandardButton;
    QListWidget* m_imageListWidget;
    QTableWidget* m_imageTableWidget;

    // 表格提取测试
    QWidget* m_tableTab;
    QPushButton* m_testTableExtractionButton;
    QPushButton* m_testTableExportToXmlButton;
    QPushButton* m_testTableExportToStandardButton;
    QTableWidget* m_tableTableWidget;

    // 图表提取测试
    QWidget* m_chartTab;
    QPushButton* m_testChartExtractionButton;
    QPushButton* m_testChartExportToXmlButton;
    QPushButton* m_testChartExportToStandardButton;
    QListWidget* m_chartListWidget;
    QTableWidget* m_chartTableWidget;

    // 提取器实例
    PdfImageExtractor* m_imageExtractor;
    PdfTableExtractor* m_tableExtractor;
    PdfChartExtractor* m_chartExtractor;

    // 当前提取结果
    QList<ImageInfo> m_currentImages;
    QList<TableInfo> m_currentTables;
    QList<ChartInfo> m_currentCharts;
};

#endif // PDFCONTENTTESTWIDGET_H
