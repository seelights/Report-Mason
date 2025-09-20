#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QProgressBar>
#include <QScrollArea>
#include "QtCompat.h"
#include "tools/ai/AiManager.h"

/**
 * @brief AI功能测试窗口
 * 提供模型选择、参数配置、对话测试和人设编辑功能
 */
class AiTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AiTestWidget(QWidget *parent = nullptr);
    ~AiTestWidget();

private slots:
    void onModelChanged();
    void onModelComboChanged();  // 新增：专门处理模型组合框变化的槽函数
    void onParameterChanged();
    void onSendMessage();
    void onStreamChat();
    void onClearHistory();
    void onSavePersona();
    void onLoadPersona();
    void onAiResponseReceived(const QString& response);
    void onAiStreamReceived(const QString& delta);
    void onAiErrorOccurred(const QString& error);

private:
    void setupUI();
    void setupModelSelection();
    void setupParameterConfig();
    void setupChatInterface();
    void setupPersonaEditor();
    void connectSignals();
    void updateModelParameters();
    void loadPresetConfigs();
    
    // 辅助方法
    void safeClearComboBox(QComboBox* comboBox);
    
    // UI组件
    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;
    
    // 模型选择标签页
    QWidget *m_modelTab;
    QComboBox *m_providerCombo;
    QComboBox *m_modelCombo;
    QLineEdit *m_apiKeyEdit;
    QLineEdit *m_baseUrlEdit;
    
    // 参数配置标签页
    QWidget *m_parameterTab;
    QSpinBox *m_maxTokensSpin;
    QDoubleSpinBox *m_temperatureSpin;
    QDoubleSpinBox *m_topPSpin;
    QSpinBox *m_frequencyPenaltySpin;
    QSpinBox *m_presencePenaltySpin;
    
    // 对话界面标签页
    QWidget *m_chatTab;
    QTextEdit *m_chatHistory;
    QTextEdit *m_messageInput;
    QPushButton *m_sendButton;
    QPushButton *m_streamButton;
    QPushButton *m_clearButton;
    QProgressBar *m_progressBar;
    
    // 人设编辑标签页
    QWidget *m_personaTab;
    QTextEdit *m_systemPrompt;
    QTextEdit *m_personaDescription;
    QTextEdit *m_customInstructions;
    QPushButton *m_savePersonaButton;
    QPushButton *m_loadPersonaButton;
    
    // 数据成员
    AiManager *m_aiManager;
    OpenAIBase *m_currentClient;
    LlmContext m_currentContext;
    QList<ChatMessage> m_chatHistoryData;
    QString m_currentPersona;
    bool m_isStreaming;
};
