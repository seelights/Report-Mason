#include "AiTestWidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollBar>
#include <QDateTime>
#include <QApplication>
#include <QTimer>

AiTestWidget::AiTestWidget(QWidget* parent)
    : QWidget(parent), m_aiManager(nullptr), m_currentClient(nullptr), m_isStreaming(false)
{
    qDebug() << "开始创建AiTestWidget";
    setupUI();
    qDebug() << "UI设置完成";

    connectSignals();
    qDebug() << "信号连接完成";

    // 延迟初始化AiManager，避免在构造函数中立即创建

    m_aiManager = AiManager::instance();
    if (!m_aiManager) {
        qDebug() << "警告：无法获取AiManager实例";
    } else {
        qDebug() << "AiManager实例创建成功";
        loadPresetConfigs();
    }
}

AiTestWidget::~AiTestWidget()
{
    if (m_currentClient) {
        m_currentClient->cancel();
    }
}

void AiTestWidget::setupUI()
{
    setWindowTitle(QS("AI功能测试窗口"));
    setMinimumSize(1000, 700);

    m_mainLayout = new QVBoxLayout(this);
    m_tabWidget = new QTabWidget(this);

    // 创建各个标签页
    setupModelSelection();
    setupParameterConfig();
    setupChatInterface();
    setupPersonaEditor();

    m_mainLayout->addWidget(m_tabWidget);
}

void AiTestWidget::setupModelSelection()
{
    m_modelTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_modelTab);

    // 服务商选择
    QGroupBox* providerGroup = new QGroupBox(QS("服务商配置"), this);
    QVBoxLayout* providerLayout = new QVBoxLayout(providerGroup);

    QHBoxLayout* providerRow = new QHBoxLayout();
    providerRow->addWidget(new QLabel(QS("服务商:")));
    m_providerCombo = new QComboBox();
    m_providerCombo->addItems({QS("DeepSeek"), QS("OpenAI"), QS("Doubao")});
    providerRow->addWidget(m_providerCombo);
    providerRow->addStretch();
    providerLayout->addLayout(providerRow);

    QHBoxLayout* modelRow = new QHBoxLayout();
    modelRow->addWidget(new QLabel(QS("模型:")));
    m_modelCombo = new QComboBox();
    modelRow->addWidget(m_modelCombo);
    modelRow->addStretch();
    providerLayout->addLayout(modelRow);

    QHBoxLayout* apiKeyRow = new QHBoxLayout();
    apiKeyRow->addWidget(new QLabel(QS("API Key:")));
    m_apiKeyEdit = new QLineEdit();
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    apiKeyRow->addWidget(m_apiKeyEdit);
    providerLayout->addLayout(apiKeyRow);

    QHBoxLayout* baseUrlRow = new QHBoxLayout();
    baseUrlRow->addWidget(new QLabel(QS("Base URL:")));
    m_baseUrlEdit = new QLineEdit();
    baseUrlRow->addWidget(m_baseUrlEdit);
    providerLayout->addLayout(baseUrlRow);

    layout->addWidget(providerGroup);
    layout->addStretch();

    m_tabWidget->addTab(m_modelTab, QS("模型选择"));
}

void AiTestWidget::setupParameterConfig()
{
    m_parameterTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_parameterTab);

    QGroupBox* paramGroup = new QGroupBox(QS("模型参数"), this);
    QGridLayout* paramLayout = new QGridLayout(paramGroup);

    // Max Tokens
    paramLayout->addWidget(new QLabel(QS("最大Token数:")), 0, 0);
    m_maxTokensSpin = new QSpinBox();
    m_maxTokensSpin->setRange(1, 100000);
    m_maxTokensSpin->setValue(4000);
    paramLayout->addWidget(m_maxTokensSpin, 0, 1);

    // Temperature
    paramLayout->addWidget(new QLabel(QS("温度 (0.0-2.0):")), 1, 0);
    m_temperatureSpin = new QDoubleSpinBox();
    m_temperatureSpin->setRange(0.0, 2.0);
    m_temperatureSpin->setSingleStep(0.1);
    m_temperatureSpin->setValue(0.7);
    paramLayout->addWidget(m_temperatureSpin, 1, 1);

    // Top P
    paramLayout->addWidget(new QLabel(QS("Top P (0.0-1.0):")), 2, 0);
    m_topPSpin = new QDoubleSpinBox();
    m_topPSpin->setRange(0.0, 1.0);
    m_topPSpin->setSingleStep(0.1);
    m_topPSpin->setValue(1.0);
    paramLayout->addWidget(m_topPSpin, 2, 1);

    // Frequency Penalty
    paramLayout->addWidget(new QLabel(QS("频率惩罚 (-2.0-2.0):")), 3, 0);
    m_frequencyPenaltySpin = new QSpinBox();
    m_frequencyPenaltySpin->setRange(-200, 200);
    m_frequencyPenaltySpin->setValue(0);
    paramLayout->addWidget(m_frequencyPenaltySpin, 3, 1);

    // Presence Penalty
    paramLayout->addWidget(new QLabel(QS("存在惩罚 (-2.0-2.0):")), 4, 0);
    m_presencePenaltySpin = new QSpinBox();
    m_presencePenaltySpin->setRange(-200, 200);
    m_presencePenaltySpin->setValue(0);
    paramLayout->addWidget(m_presencePenaltySpin, 4, 1);

    layout->addWidget(paramGroup);
    layout->addStretch();

    m_tabWidget->addTab(m_parameterTab, QS("参数配置"));
}

void AiTestWidget::setupChatInterface()
{
    m_chatTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_chatTab);

    // 聊天历史
    m_chatHistory = new QTextEdit();
    m_chatHistory->setReadOnly(true);
    m_chatHistory->setMinimumHeight(400);
    layout->addWidget(new QLabel(QS("对话历史:")));
    layout->addWidget(m_chatHistory);

    // 输入区域
    QHBoxLayout* inputLayout = new QHBoxLayout();
    m_messageInput = new QTextEdit();
    m_messageInput->setMaximumHeight(100);
    m_messageInput->setPlaceholderText(QS("输入您的消息..."));
    inputLayout->addWidget(m_messageInput);

    QVBoxLayout* buttonLayout = new QVBoxLayout();
    m_sendButton = new QPushButton(QS("发送"));
    m_streamButton = new QPushButton(QS("流式发送"));
    m_clearButton = new QPushButton(QS("清空历史"));

    buttonLayout->addWidget(m_sendButton);
    buttonLayout->addWidget(m_streamButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addStretch();

    inputLayout->addLayout(buttonLayout);
    layout->addLayout(inputLayout);

    // 进度条
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    layout->addWidget(m_progressBar);

    m_tabWidget->addTab(m_chatTab, QS("对话测试"));
}

void AiTestWidget::setupPersonaEditor()
{
    m_personaTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_personaTab);

    // 系统提示词
    QGroupBox* systemGroup = new QGroupBox(QS("系统提示词"), this);
    QVBoxLayout* systemLayout = new QVBoxLayout(systemGroup);
    m_systemPrompt = new QTextEdit();
    m_systemPrompt->setPlaceholderText(QS("定义AI的角色和基本行为..."));
    m_systemPrompt->setMaximumHeight(150);
    systemLayout->addWidget(m_systemPrompt);
    layout->addWidget(systemGroup);

    // 人设描述
    QGroupBox* personaGroup = new QGroupBox(QS("人设描述"), this);
    QVBoxLayout* personaLayout = new QVBoxLayout(personaGroup);
    m_personaDescription = new QTextEdit();
    m_personaDescription->setPlaceholderText(QS("详细描述AI的性格、背景、专长等..."));
    m_personaDescription->setMaximumHeight(150);
    personaLayout->addWidget(m_personaDescription);
    layout->addWidget(personaGroup);

    // 自定义指令
    QGroupBox* instructionGroup = new QGroupBox(QS("自定义指令"), this);
    QVBoxLayout* instructionLayout = new QVBoxLayout(instructionGroup);
    m_customInstructions = new QTextEdit();
    m_customInstructions->setPlaceholderText(QS("添加特殊的对话规则或限制..."));
    m_customInstructions->setMaximumHeight(150);
    instructionLayout->addWidget(m_customInstructions);
    layout->addWidget(instructionGroup);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_savePersonaButton = new QPushButton(QS("保存人设"));
    m_loadPersonaButton = new QPushButton(QS("加载人设"));
    buttonLayout->addWidget(m_savePersonaButton);
    buttonLayout->addWidget(m_loadPersonaButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    layout->addStretch();

    m_tabWidget->addTab(m_personaTab, QS("人设编辑"));
}

void AiTestWidget::connectSignals()
{
    // 模型选择信号
    connect(m_providerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &AiTestWidget::onModelChanged);
    connect(m_modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &AiTestWidget::onModelComboChanged);  // 使用新的槽函数
    connect(m_apiKeyEdit, &QLineEdit::textChanged, this, &AiTestWidget::onParameterChanged);
    connect(m_baseUrlEdit, &QLineEdit::textChanged, this, &AiTestWidget::onParameterChanged);

    // 参数配置信号
    connect(m_maxTokensSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &AiTestWidget::onParameterChanged);
    connect(m_temperatureSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &AiTestWidget::onParameterChanged);
    connect(m_topPSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &AiTestWidget::onParameterChanged);
    connect(m_frequencyPenaltySpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &AiTestWidget::onParameterChanged);
    connect(m_presencePenaltySpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &AiTestWidget::onParameterChanged);

    // 对话界面信号
    connect(m_sendButton, &QPushButton::clicked, this, &AiTestWidget::onSendMessage);
    connect(m_streamButton, &QPushButton::clicked, this, &AiTestWidget::onStreamChat);
    connect(m_clearButton, &QPushButton::clicked, this, &AiTestWidget::onClearHistory);

    // 人设编辑信号
    connect(m_savePersonaButton, &QPushButton::clicked, this, &AiTestWidget::onSavePersona);
    connect(m_loadPersonaButton, &QPushButton::clicked, this, &AiTestWidget::onLoadPersona);
}

void AiTestWidget::updateModelParameters()
{
    // 检查UI组件是否已初始化
    if (!m_providerCombo || !m_modelCombo || !m_baseUrlEdit) {
        qDebug() << "警告：UI组件未完全初始化，跳过updateModelParameters";
        return;
    }

    QString provider = m_providerCombo->currentText();
    QString model = m_modelCombo->currentText();

    // 根据服务商更新模型列表
    // 使用安全方法清空QComboBox，避免当前选择项被删除引起的异常
    safeClearComboBox(m_modelCombo);

    if (provider == QS("DeepSeek")) {
        m_modelCombo->addItems({QS("deepseek-chat"), QS("deepseek-coder")});
        m_baseUrlEdit->setText(QS("https://api.deepseek.com"));
    } else if (provider == QS("OpenAI")) {
        m_modelCombo->addItems({QS("gpt-4"), QS("gpt-3.5-turbo"), QS("gpt-4-turbo")});
        m_baseUrlEdit->setText(QS("https://api.openai.com"));
    } else if (provider == QS("Doubao")) {
        m_modelCombo->addItems({QS("doubao-pro-4k"), QS("doubao-pro-32k"), QS("doubao-lite-4k")});
        m_baseUrlEdit->setText(QS("https://ark.cn-beijing.volces.com/api/v3"));
    }
}

void AiTestWidget::safeClearComboBox(QComboBox* comboBox)
{
    if (!comboBox) {
        qDebug() << "警告：尝试清空空指针的QComboBox";
        return;
    }

    try {
        // 检查是否有选项
        if (comboBox->count() > 0) {
            // 禁用信号，避免清空过程中的事件触发
            bool wasBlocked = comboBox->blockSignals(true);
            
            // 先设置当前索引为-1，避免选择项被删除时的异常
            comboBox->setCurrentIndex(-1);
            // 然后清空所有选项
            comboBox->clear();
            
            // 恢复信号状态
            comboBox->blockSignals(wasBlocked);
            
            qDebug() << "安全清空QComboBox，原有选项数：" << comboBox->count();
        } else {
            qDebug() << "QComboBox已经是空的，无需清空";
        }
    } catch (const std::exception& e) {
        qDebug() << "清空QComboBox时发生异常：" << e.what();
    } catch (...) {
        qDebug() << "清空QComboBox时发生未知异常";
    }
}

void AiTestWidget::loadPresetConfigs()
{
    qDebug() << "开始加载预设配置";

    try {
        // 设置默认值
        if (m_providerCombo) {
            qDebug() << "设置默认服务商为DeepSeek";
            
            // 禁用信号连接，避免在设置默认值时触发事件
            m_providerCombo->blockSignals(true);
            m_modelCombo->blockSignals(true);
            
            m_providerCombo->setCurrentText(QS("DeepSeek"));
            qDebug() << "调用updateModelParameters";
            updateModelParameters();
            
            // 重新启用信号连接
            m_providerCombo->blockSignals(false);
            m_modelCombo->blockSignals(false);
            
            qDebug() << "信号连接已重新启用";
        } else {
            qDebug() << "警告：m_providerCombo为空";
        }

        // 设置默认人设
        if (m_systemPrompt) {
            m_systemPrompt->setPlainText(QS("你是一个专业的AI助手，能够帮助用户解决各种问题。"));
        } else {
            qDebug() << "警告：m_systemPrompt为空";
        }
        if (m_personaDescription) {
            m_personaDescription->setPlainText(QS("我是一个友好、专业、乐于助人的AI助手。"));
        } else {
            qDebug() << "警告：m_personaDescription为空";
        }
        if (m_customInstructions) {
            m_customInstructions->setPlainText(
                QS("请用简洁明了的语言回答问题，避免过于复杂的术语。"));
        } else {
            qDebug() << "警告：m_customInstructions为空";
        }

        qDebug() << "预设配置加载完成";
    } catch (const std::exception& e) {
        qDebug() << "加载预设配置异常：" << e.what();
    } catch (...) {
        qDebug() << "加载预设配置发生未知异常";
    }
}

void AiTestWidget::onModelChanged()
{
    // 检查UI组件是否已初始化
    if (!m_providerCombo || !m_modelCombo) {
        qDebug() << "警告：UI组件未完全初始化，跳过onModelChanged";
        return;
    }
    
    qDebug() << "onModelChanged: 禁用QComboBox信号";
    
    // 禁用信号连接，避免在更新模型参数时触发事件
    m_providerCombo->blockSignals(true);
    m_modelCombo->blockSignals(true);
    
    // 安全地更新模型参数
    updateModelParameters();
    onParameterChanged();
    
    // 重新启用信号连接
    m_providerCombo->blockSignals(false);
    m_modelCombo->blockSignals(false);
    
    qDebug() << "onModelChanged: 信号连接已重新启用";
}

void AiTestWidget::onModelComboChanged()
{
    // 检查UI组件是否已初始化
    if (!m_modelCombo) {
        qDebug() << "警告：m_modelCombo未初始化，跳过onModelComboChanged";
        return;
    }
    
    qDebug() << "onModelComboChanged: 模型组合框发生变化，更新模型参数";
    
    // 禁用信号连接，避免在更新参数时触发其他事件
    m_modelCombo->blockSignals(true);
    
    // 只更新模型参数，不调用updateModelParameters（避免清空模型列表）
    onParameterChanged();
    
    // 重新启用信号连接
    m_modelCombo->blockSignals(false);
    
    qDebug() << "onModelComboChanged: 模型参数已更新";
}

void AiTestWidget::onParameterChanged()
{
    // 检查UI组件是否已初始化
    if (!m_providerCombo || !m_modelCombo || !m_apiKeyEdit || !m_baseUrlEdit || !m_maxTokensSpin ||
        !m_temperatureSpin || !m_topPSpin) {
        qDebug() << "警告：UI组件未完全初始化，跳过onParameterChanged";
        return;
    }

    // 更新当前上下文
    QString providerText = m_providerCombo->currentText();
    if (providerText == QS("DeepSeek")) {
        m_currentContext.provider = Provider::DeepSeek;
    } else if (providerText == QS("OpenAI")) {
        m_currentContext.provider = Provider::OpenAI;
    } else if (providerText == QS("Doubao")) {
        m_currentContext.provider = Provider::DoubaoArk;
    } else {
        m_currentContext.provider = Provider::CustomOpenAICompatible;
    }

    m_currentContext.model = m_modelCombo->currentText();
    m_currentContext.apiKey = m_apiKeyEdit->text();
    m_currentContext.baseUrl = m_baseUrlEdit->text();
    m_currentContext.maxTokens = m_maxTokensSpin->value();
    m_currentContext.temperature = m_temperatureSpin->value();
    m_currentContext.topP = m_topPSpin->value();
    // 注意：LlmContext结构体中没有frequencyPenalty和presencePenalty字段

    // 更新AI管理器
    if (m_aiManager) {
        m_aiManager->addProvider(QS("test"), m_currentContext);
        m_aiManager->setDefaultProvider(QS("test"));
    }
}

void AiTestWidget::onSendMessage()
{
    QString message = m_messageInput->toPlainText().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, QS("警告"), QS("请输入消息内容"));
        return;
    }

    if (!m_currentClient) {
        if (!m_aiManager) {
            QMessageBox::critical(this, QS("错误"), QS("AI管理器未初始化"));
            return;
        }
        m_currentClient = m_aiManager->createNonStreamClient();
        if (!m_currentClient) {
            QMessageBox::critical(this, QS("错误"), QS("无法创建AI客户端"));
            return;
        }

        connect(m_currentClient, &OpenAIBase::finished, this, &AiTestWidget::onAiResponseReceived);
        connect(m_currentClient, &OpenAIBase::error, this, &AiTestWidget::onAiErrorOccurred);
    }

    // 添加用户消息到历史
    m_chatHistoryData.append(ChatMessage(QS("user"), message));

    // 显示用户消息
    m_chatHistory->append(QS("<b>用户:</b> ") + message);
    m_chatHistory->append(QS(""));

    // 清空输入框
    m_messageInput->clear();

    // 显示进度条
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // 不确定进度

    // 发送请求
    m_currentClient->start(m_chatHistoryData);
}

void AiTestWidget::onStreamChat()
{
    QString message = m_messageInput->toPlainText().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, QS("警告"), QS("请输入消息内容"));
        return;
    }

    if (!m_currentClient) {
        if (!m_aiManager) {
            QMessageBox::critical(this, QS("错误"), QS("AI管理器未初始化"));
            return;
        }
        m_currentClient = m_aiManager->createStreamClient();
        if (!m_currentClient) {
            QMessageBox::critical(this, QS("错误"), QS("无法创建AI客户端"));
            return;
        }

        connect(m_currentClient, &OpenAIBase::chunk, this, &AiTestWidget::onAiStreamReceived);
        connect(m_currentClient, &OpenAIBase::error, this, &AiTestWidget::onAiErrorOccurred);
    }

    // 添加用户消息到历史
    m_chatHistoryData.append(ChatMessage(QS("user"), message));

    // 显示用户消息
    m_chatHistory->append(QS("<b>用户:</b> ") + message);
    m_chatHistory->append(QS("<b>AI:</b> "));

    // 清空输入框
    m_messageInput->clear();

    // 显示进度条
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);

    m_isStreaming = true;

    // 发送请求
    m_currentClient->start(m_chatHistoryData);
}

void AiTestWidget::onClearHistory()
{
    m_chatHistory->clear();
    m_chatHistoryData.clear();
    m_messageInput->clear();
}

void AiTestWidget::onSavePersona()
{
    QString fileName = QFileDialog::getSaveFileName(this, QS("保存人设"), QS("persona.json"),
                                                    QS("JSON文件 (*.json)"));

    if (fileName.isEmpty())
        return;

    QJsonObject persona;
    persona[QS("systemPrompt")] = m_systemPrompt->toPlainText();
    persona[QS("personaDescription")] = m_personaDescription->toPlainText();
    persona[QS("customInstructions")] = m_customInstructions->toPlainText();
    persona[QS("timestamp")] = QDateTime::currentDateTime().toString();

    QJsonDocument doc(persona);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        QMessageBox::information(this, QS("成功"), QS("人设已保存"));
    } else {
        QMessageBox::critical(this, QS("错误"), QS("无法保存人设文件"));
    }
}

void AiTestWidget::onLoadPersona()
{
    QString fileName =
        QFileDialog::getOpenFileName(this, QS("加载人设"), QS(""), QS("JSON文件 (*.json)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, QS("错误"), QS("无法打开人设文件"));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject persona = doc.object();

    m_systemPrompt->setPlainText(persona[QS("systemPrompt")].toString());
    m_personaDescription->setPlainText(persona[QS("personaDescription")].toString());
    m_customInstructions->setPlainText(persona[QS("customInstructions")].toString());

    QMessageBox::information(this, QS("成功"), QS("人设已加载"));
}

void AiTestWidget::onAiResponseReceived(const QString& response)
{
    m_progressBar->setVisible(false);

    // 添加AI回复到历史
    m_chatHistoryData.append(ChatMessage(QS("assistant"), response));

    // 显示AI回复
    m_chatHistory->append(QS("<b>AI:</b> ") + response);
    m_chatHistory->append(QS(""));

    // 滚动到底部
    m_chatHistory->verticalScrollBar()->setValue(m_chatHistory->verticalScrollBar()->maximum());
}

void AiTestWidget::onAiStreamReceived(const QString& delta)
{
    if (!m_isStreaming)
        return;

    // 追加流式内容
    m_chatHistory->insertPlainText(delta);

    // 滚动到底部
    m_chatHistory->verticalScrollBar()->setValue(m_chatHistory->verticalScrollBar()->maximum());
}

void AiTestWidget::onAiErrorOccurred(const QString& error)
{
    m_progressBar->setVisible(false);
    m_isStreaming = false;

    QMessageBox::critical(this, QS("AI错误"), error);

    // 显示错误信息
    m_chatHistory->append(QS("<font color='red'><b>错误:</b> ") + error + QS("</font>"));
    m_chatHistory->append(QS(""));
}
