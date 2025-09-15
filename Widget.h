/*
 * Widget.h
 * ReportMason 主界面类
 *
 * 功能说明：
 * 1. 提供用户界面
 * 2. 文件选择和转换功能
 * 3. 字段提取和显示
 * 4. 模板管理和应用
 * 5. 结果导出和保存
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QComboBox>
#include <QListWidget>
#include <QGroupBox>
#include <QSplitter>

#include "src/FileConverter.h"
#include "src/DocToXmlConverter.h"
#include "src/PdfToXmlConverter.h"
#include "src/TemplateManager.h"
#include "src/KZipUtils.h"
#include "src/FieldExtractor.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    // 文件操作
    void selectInputFile();
    void selectOutputFile();
    void convertFile();
    void clearResults();

    // 字段操作
    void extractFields();
    void clearFields();

    // 模板操作
    void loadTemplate();
    void applyTemplate();
    void saveTemplate();

    // 帮助和关于
    void showHelp();
    void showAbout();

private:
    void setupUI();
    void setupConnections();
    void updateStatus(const QString &message);
    void logMessage(const QString &message);
    QString convertFileToXml(const QString &filePath);

    // UI组件
    QVBoxLayout *m_mainLayout;
    QSplitter *m_splitter;

    // 文件选择区域
    QGroupBox *m_fileGroup;
    QLineEdit *m_inputFileEdit;
    QLineEdit *m_outputFileEdit;
    QPushButton *m_inputFileBtn;
    QPushButton *m_outputFileBtn;
    QComboBox *m_formatCombo;
    QPushButton *m_convertBtn;
    QPushButton *m_clearBtn;

    // 字段提取区域
    QGroupBox *m_fieldGroup;
    QTextEdit *m_contentEdit;
    QPushButton *m_extractBtn;
    QListWidget *m_fieldList;
    QPushButton *m_clearFieldsBtn;

    // 模板管理区域
    QGroupBox *m_templateGroup;
    QComboBox *m_templateCombo;
    QPushButton *m_loadTemplateBtn;
    QPushButton *m_applyTemplateBtn;
    QPushButton *m_saveTemplateBtn;
    QTextEdit *m_resultEdit;

    // 状态和日志区域
    QGroupBox *m_statusGroup;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QTextEdit *m_logEdit;

    // 核心功能类
    DocToXmlConverter *m_docConverter;
    PdfToXmlConverter *m_pdfConverter;
    TemplateManager *m_templateManager;
    FieldExtractor *m_fieldExtractor;

    // 当前数据
    QString m_currentInputFile;
    QString m_currentOutputFile;
    QMap<QString, QString> m_extractedFields;
};

#endif // WIDGET_H