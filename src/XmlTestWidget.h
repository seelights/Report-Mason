#ifndef XMLTESTWIDGET_H
#define XMLTESTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QGroupBox>
#include <QSplitter>
#include <QProgressBar>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCursor>

#include "LogSystem.h"
#include "FieldExtractor.h"
#include "TemplateManager.h"
#include "FileConverter.h"

class XmlTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit XmlTestWidget(QWidget *parent = nullptr);
    ~XmlTestWidget();

private slots:
    void openFile();
    void convertFile();
    void extractFields();
    void loadTemplate();
    void applyTemplate();
    void saveTemplate();
    void clearLog();
    void saveLog();

private:
    void setupUI();
    void setupConnections();
    void updateStatus(const QString &message);
    QString convertFileToXml(const QString &filePath);

    // UI组件
    QSplitter *m_mainSplitter;
    QSplitter *m_leftSplitter;
    QSplitter *m_rightSplitter;
    
    // 文件操作区域
    QGroupBox *m_fileGroup;
    QPushButton *m_openFileBtn;
    QPushButton *m_convertBtn;
    QTextEdit *m_contentEdit;
    
    // 字段提取区域
    QGroupBox *m_extractGroup;
    QPushButton *m_extractBtn;
    QListWidget *m_fieldsList;
    
    // 模板操作区域
    QGroupBox *m_templateGroup;
    QComboBox *m_templateCombo;
    QPushButton *m_loadTemplateBtn;
    QPushButton *m_applyTemplateBtn;
    QPushButton *m_saveTemplateBtn;
    QTextEdit *m_resultEdit;
    
    // 日志区域
    QGroupBox *m_logGroup;
    QPushButton *m_clearLogBtn;
    QPushButton *m_saveLogBtn;
    QTextEdit *m_logEdit;
    
    // 状态栏
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // 功能组件
    LogSystem *m_logSystem;
    FieldExtractor *m_fieldExtractor;
    TemplateManager *m_templateManager;
    
    // 数据
    QMap<QString, QString> m_extractedFields;
    QString m_currentFilePath;
};

#endif // XMLTESTWIDGET_H
