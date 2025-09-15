#ifndef TESTWIDGET_H
#define TESTWIDGET_H

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

class TestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestWidget(QWidget *parent = nullptr);
    ~TestWidget();

private slots:
    void runTests();
    void clearResults();
    void saveResults();

private:
    void setupUI();
    void setupConnections();
    void updateStatus(const QString &message);
    
    // 测试方法
    void testFieldExtractor();
    void testTemplateManager();
    void testFileConverter();
    void testLogSystem();

    // UI组件
    QSplitter *m_mainSplitter;
    
    // 测试控制区域
    QGroupBox *m_controlGroup;
    QPushButton *m_runTestsBtn;
    QPushButton *m_clearResultsBtn;
    QPushButton *m_saveResultsBtn;
    QComboBox *m_testCombo;
    
    // 结果显示区域
    QGroupBox *m_resultsGroup;
    QTextEdit *m_resultsEdit;
    
    // 日志区域
    QGroupBox *m_logGroup;
    QTextEdit *m_logEdit;
    
    // 状态栏
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // 功能组件
    LogSystem *m_logSystem;
    FieldExtractor *m_fieldExtractor;
    TemplateManager *m_templateManager;
};

#endif // TESTWIDGET_H
