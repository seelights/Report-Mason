/*
 * @Author: seelights
 * @Date: 2025-01-27 10:00:00
 * @LastEditTime: 2025-01-27 10:00:00
 * @LastEditors: seelights
 * @Description: DOCX无损转换为XML测试窗口
 * @FilePath: \ReportMason\src\widgetTest\DocxToXmlTestWidget.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QFileDialog>
#include <QProgressBar>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QTabWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStack>
#include <QDebug>

#include "DocToXmlConverter.h"
#include "FileConverter.h"
#include "LogSystem.h"

/**
 * @brief DOCX无损转换为XML测试窗口
 * 
 * 提供完整的DOCX文档转换测试功能，包括：
 * - 文件选择
 * - 转换过程监控
 * - XML结果展示
 * - 位置信息验证
 * - 无损转换验证
 */
class DocxToXmlTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DocxToXmlTestWidget(QWidget *parent = nullptr);
    virtual ~DocxToXmlTestWidget();

private slots:
    // 文件操作
    void onSelectDocxFile();
    void onSelectOutputDir();
    void onStartConversion();
    void onOpenOutputFile();
    void onOpenOutputDir();
    
    // 转换过程
    void onConversionProgress(int percentage);
    void onConversionFinished();
    void onConversionError(const QString &error);
    
    // 结果展示
    void onShowXmlResult();
    void onShowPositionInfo();
    void onShowElementTree();
    void onValidateXml();
    
    // 设置
    void onToggleVerboseMode(bool enabled);
    void onTogglePositionInfo(bool enabled);
    void onToggleElementTree(bool enabled);
    
    // 调试
    void onClearDebugOutput();
    void onSaveDebugOutput();

private:
    void setupUI();
    void setupConnections();
    void updateUI();
    void clearResults();
    void showMessage(const QString &title, const QString &message, QMessageBox::Icon icon = QMessageBox::Information);
    
    // XML解析和验证
    void parseXmlResult(const QByteArray &xmlData);
    void validateXmlStructure(const QByteArray &xmlData);
    void extractPositionInfo(const QByteArray &xmlData);
    void buildElementTree(const QByteArray &xmlData);
    
    // 文件操作
    QString getOutputFilePath() const;
    bool saveXmlToFile(const QByteArray &xmlData, const QString &filePath);
    void openFileInSystem(const QString &filePath);

private:
    // UI组件
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_fileLayout;
    QHBoxLayout *m_outputLayout;
    QHBoxLayout *m_controlLayout;
    QHBoxLayout *m_settingLayout;
    
    // 文件选择
    QPushButton *m_selectDocxBtn;
    QPushButton *m_selectOutputBtn;
    QLabel *m_docxFileLabel;
    QLabel *m_outputDirLabel;
    
    // 控制按钮
    QPushButton *m_startBtn;
    QPushButton *m_openFileBtn;
    QPushButton *m_openDirBtn;
    QPushButton *m_clearBtn;
    
    // 进度显示
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // 设置选项
    QGroupBox *m_settingGroup;
    QCheckBox *m_verboseModeCheck;
    QCheckBox *m_positionInfoCheck;
    QCheckBox *m_elementTreeCheck;
    
    // 调试输出
    QGroupBox *m_debugGroup;
    QTextEdit *m_debugOutput;
    QPushButton *m_clearDebugBtn;
    QPushButton *m_saveDebugBtn;
    
    // 结果展示
    QTabWidget *m_resultTabs;
    QTextEdit *m_xmlResultEdit;
    QTextEdit *m_positionInfoEdit;
    QTreeWidget *m_elementTreeWidget;
    QTextEdit *m_validationEdit;
    
    // 数据
    QString m_docxFilePath;
    QString m_outputDirPath;
    QByteArray m_lastXmlResult;
    DocToXmlConverter *m_converter;
    LogSystem *m_logSystem;
    
    // 状态
    bool m_isConverting;
    bool m_hasResult;
};
