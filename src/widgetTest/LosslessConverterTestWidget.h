/*
 * @Author: seelights
 * @Date: 2025-01-27 22:30:00
 * @LastEditTime: 2025-01-27 22:30:00
 * @LastEditors: seelights
 * @Description: 无损转换器测试窗口
 * @FilePath: \ReportMason\src\widgetTest\LosslessConverterTestWidget.h
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

#include "LosslessDocumentConverter.h"
#include "LogSystem.h"

/**
 * @brief 无损转换器测试窗口
 * 
 * 用于测试新的LosslessDocumentConverter类的功能
 */
class LosslessConverterTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LosslessConverterTestWidget(QWidget *parent = nullptr);
    ~LosslessConverterTestWidget();

private slots:
    // 文件操作
    void onSelectInputFile();
    void onSelectOutputDir();
    void onStartConversion();
    void onOpenOutputFile();
    void onOpenOutputDir();
    void onClearResults();
    
    // 调试操作
    void onClearDebugOutput();
    void onSaveDebugOutput();
    
    // 转换器信号
    void onConversionProgress(int progress, const QString &message);
    void onConversionFinished(LosslessDocumentConverter::ConvertStatus status, const QString &message);

private:
    void setupUI();
    void setupConnections();
    void updateUI();
    void clearResults();
    
    // 结果处理
    void displayXmlResult(const QString &xmlPath);
    void displayElementTree(const QString &xmlPath);
    void displayFormatInfo(const QString &xmlPath);
    void displayPositionInfo(const QString &xmlPath);
    void validateConversion(const QString &originalPath, const QString &xmlPath);

private:
    // UI组件
    QVBoxLayout *m_mainLayout;
    
    // 文件选择区域
    QGroupBox *m_fileGroup;
    QHBoxLayout *m_fileLayout;
    QPushButton *m_selectInputBtn;
    QLabel *m_inputFileLabel;
    
    // 输出设置区域
    QGroupBox *m_outputGroup;
    QHBoxLayout *m_outputLayout;
    QPushButton *m_selectOutputBtn;
    QLabel *m_outputDirLabel;
    
    // 控制按钮区域
    QGroupBox *m_controlGroup;
    QHBoxLayout *m_controlLayout;
    QPushButton *m_startBtn;
    QPushButton *m_openFileBtn;
    QPushButton *m_openDirBtn;
    QPushButton *m_clearBtn;
    
    // 进度显示区域
    QGroupBox *m_progressGroup;
    QVBoxLayout *m_progressLayout;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // 设置选项区域
    QGroupBox *m_settingGroup;
    QHBoxLayout *m_settingLayout;
    QCheckBox *m_verboseModeCheck;
    QCheckBox *m_showFormatInfoCheck;
    QCheckBox *m_showPositionInfoCheck;
    QCheckBox *m_showElementTreeCheck;
    QCheckBox *m_validateConversionCheck;
    QComboBox *m_outputFormatCombo;
    
    // 调试输出区域
    QGroupBox *m_debugGroup;
    QVBoxLayout *m_debugLayout;
    QTextEdit *m_debugOutput;
    QHBoxLayout *m_debugBtnLayout;
    QPushButton *m_clearDebugBtn;
    QPushButton *m_saveDebugBtn;
    
    // 结果展示区域
    QGroupBox *m_resultGroup;
    QVBoxLayout *m_resultLayout;
    QTabWidget *m_resultTabs;
    QTextEdit *m_xmlResultEdit;
    QTextEdit *m_formatInfoEdit;
    QTextEdit *m_positionInfoEdit;
    QTreeWidget *m_elementTreeWidget;
    QTextEdit *m_validationEdit;
    
    // 数据
    QString m_inputFilePath;
    QString m_outputDirPath;
    QString m_lastXmlPath;
    LosslessDocumentConverter *m_converter;
    LogSystem *m_logSystem;
    
    // 状态
    bool m_isConverting;
    bool m_hasResult;
};
