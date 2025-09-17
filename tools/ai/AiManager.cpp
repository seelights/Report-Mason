#include "../src/QtCompat.h"
#include "AiManager.h"
#include "OpenAINonStreamClient.h"
#include "OpenAIStreamClient.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QEventLoop>

AiManager* AiManager::s_instance = nullptr;

AiManager::AiManager(QObject* parent) : QObject(parent), m_settings(nullptr)
{
    try {
        // 初始化设置
        QString configPath = getConfigFilePath();
        m_settings = new QSettings(configPath, QSettings::IniFormat, this);

        // 初始化预设配置
        initializePresets();

        // 从配置文件加载
        loadConfig();
    } catch (const std::exception& e) {
        qDebug() << "AiManager构造函数异常：" << e.what();
    } catch (...) {
        qDebug() << "AiManager构造函数发生未知异常";
    }
}

AiManager::~AiManager() { saveConfig(); }

AiManager* AiManager::instance()
{
    if (!s_instance) {
        try {
            s_instance = new AiManager();
        } catch (const std::exception& e) {
            qDebug() << "创建AiManager实例失败：" << e.what();
            return nullptr;
        } catch (...) {
            qDebug() << "创建AiManager实例时发生未知异常";
            return nullptr;
        }
    }
    return s_instance;
}

void AiManager::addProvider(const QString& name, const LlmContext& context)
{
    m_providers[name] = context;
    emit providerUpdated(name);
}

void AiManager::removeProvider(const QString& name)
{
    m_providers.remove(name);
    if (m_defaultProvider == name) {
        m_defaultProvider.clear();
        if (!m_providers.isEmpty()) {
            m_defaultProvider = m_providers.keys().first();
        }
        emit defaultProviderChanged(m_defaultProvider);
    }
    emit providerUpdated(name);
}

LlmContext AiManager::getProvider(const QString& name) const
{
    if (m_providers.contains(name)) {
        return m_providers[name];
    }

    // 返回默认配置
    if (!m_defaultProvider.isEmpty() && m_providers.contains(m_defaultProvider)) {
        return m_providers[m_defaultProvider];
    }

    // 返回第一个可用配置
    if (!m_providers.isEmpty()) {
        return m_providers.values().first();
    }

    // 返回空配置
    return LlmContext();
}

QStringList AiManager::getProviderNames() const { return m_providers.keys(); }

void AiManager::setDefaultProvider(const QString& name)
{
    if (m_providers.contains(name) && m_defaultProvider != name) {
        m_defaultProvider = name;
        emit defaultProviderChanged(name);
    }
}

OpenAINonStreamClient* AiManager::createNonStreamClient(const QString& providerName)
{
    LlmContext context = getProvider(providerName);
    if (!context.isValid()) {
        qWarning() << "无效的AI配置:" << providerName;
        return nullptr;
    }

    auto* client = new OpenAINonStreamClient(this);
    client->setContext(context);
    return client;
}

OpenAIStreamClient* AiManager::createStreamClient(const QString& providerName)
{
    LlmContext context = getProvider(providerName);
    if (!context.isValid()) {
        qWarning() << "无效的AI配置:" << providerName;
        return nullptr;
    }

    auto* client = new OpenAIStreamClient(this);
    client->setContext(context);
    return client;
}

AiResponse AiManager::quickChat(const QList<ChatMessage>& messages, const QString& providerName)
{
    auto* client = createNonStreamClient(providerName);
    if (!client) {
        return AiResponse(QString(), false);
    }

    AiResponse result;
    bool finished = false;

    // 连接信号
    connect(client, &OpenAINonStreamClient::finished, [&](const QString& content) {
        result.content = content;
        result.success = true;
        finished = true;
    });

    connect(client, &OpenAINonStreamClient::error, [&](const QString& error) {
        result.errorMessage = error;
        result.success = false;
        finished = true;
    });

    connect(client, &OpenAINonStreamClient::usageReady,
            [&](const Usage& usage) { result.usage = usage; });

    // 发送请求
    client->start(messages);

    // 等待完成（简单实现，实际应用中应该使用事件循环）
    QEventLoop loop;
    connect(client, &OpenAINonStreamClient::finished, &loop, &QEventLoop::quit);
    connect(client, &OpenAINonStreamClient::error, &loop, &QEventLoop::quit);
    loop.exec();

    client->deleteLater();
    return result;
}

void AiManager::loadConfig(const QString& configPath)
{
    QSettings* settings = m_settings;
    if (!configPath.isEmpty()) {
        settings = new QSettings(configPath, QSettings::IniFormat, this);
    }

    // 加载默认配置名称
    m_defaultProvider = settings->value(QS("defaultProvider")).toString();

    // 加载所有配置
    int size = settings->beginReadArray(QS("providers"));
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);

        QString name = settings->value(QS("name")).toString();
        if (name.isEmpty())
            continue;

        LlmContext context;
        context.provider = static_cast<Provider>(settings->value(QS("provider"), 0).toInt());
        context.aiType = static_cast<AiType>(settings->value(QS("aiType"), 0).toInt());
        context.baseUrl = settings->value(QS("baseUrl")).toString();
        context.apiKey = settings->value(QS("apiKey")).toString();
        context.model = settings->value(QS("model")).toString();
        context.temperature = settings->value(QS("temperature"), 0.7).toDouble();
        context.topP = settings->value(QS("topP"), 1.0).toDouble();
        context.maxTokens = settings->value(QS("maxTokens"), 0).toInt();
        context.timeoutMs = settings->value(QS("timeoutMs"), 60000).toInt();
        context.stream = settings->value(QS("stream"), false).toBool();

        m_providers[name] = context;
    }
    settings->endArray();

    if (!configPath.isEmpty()) {
        settings->deleteLater();
    }
}

void AiManager::saveConfig(const QString& configPath)
{
    QSettings* settings = m_settings;
    if (!configPath.isEmpty()) {
        settings = new QSettings(configPath, QSettings::IniFormat, this);
    }

    // 保存默认配置名称
    settings->setValue(QS("defaultProvider"), m_defaultProvider);

    // 保存所有配置
    settings->beginWriteArray(QS("providers"));
    int index = 0;
    for (auto it = m_providers.begin(); it != m_providers.end(); ++it) {
        settings->setArrayIndex(index++);

        const LlmContext& context = it.value();
        settings->setValue(QS("name"), it.key());
        settings->setValue(QS("provider"), static_cast<int>(context.provider));
        settings->setValue(QS("aiType"), static_cast<int>(context.aiType));
        settings->setValue(QS("baseUrl"), context.baseUrl);
        settings->setValue(QS("apiKey"), context.apiKey);
        settings->setValue(QS("model"), context.model);
        settings->setValue(QS("temperature"), context.temperature);
        settings->setValue(QS("topP"), context.topP);
        settings->setValue(QS("maxTokens"), context.maxTokens);
        settings->setValue(QS("timeoutMs"), context.timeoutMs);
        settings->setValue(QS("stream"), context.stream);
    }
    settings->endArray();

    if (!configPath.isEmpty()) {
        settings->deleteLater();
    }
}

LlmContext AiManager::getPresetConfig(Provider provider)
{
    LlmContext context;
    context.provider = provider;

    switch (provider) {
    case Provider::OpenAI:
        context.baseUrl = QS("https://api.openai.com");
        context.model = QS("gpt-4o");
        context.aiType = AiType::Chat;
        break;

    case Provider::DeepSeek:
        context.baseUrl = QS("https://api.deepseek.com");
        context.model = QS("deepseek-chat");
        context.aiType = AiType::Chat;
        break;

    case Provider::DoubaoArk:
        context.baseUrl = QS("https://ark.cn-beijing.volces.com/api/v3");
        context.model = QS("doubao-pro-4k");
        context.aiType = AiType::Chat;
        break;

    default:
        break;
    }

    // 设置默认参数
    context.temperature = 0.7;
    context.topP = 1.0;
    context.maxTokens = 0;
    context.timeoutMs = 60000;
    context.stream = false;

    return context;
}

void AiManager::initializePresets()
{
    // 添加预设配置
    addProvider(QS("OpenAI"), getPresetConfig(Provider::OpenAI));
    addProvider(QS("DeepSeek"), getPresetConfig(Provider::DeepSeek));
    addProvider(QS("豆包"), getPresetConfig(Provider::DoubaoArk));

    // 设置默认配置
    if (m_defaultProvider.isEmpty()) {
        m_defaultProvider = QS("DeepSeek");
    }
}

QString AiManager::getConfigFilePath() const
{
    try {
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        if (configDir.isEmpty()) {
            // 如果无法获取标准路径，使用当前目录
            configDir = QS("./config");
        }
        QDir().mkpath(configDir);
        return configDir + QS("/ai_config.ini");
    } catch (...) {
        // 如果出现异常，返回一个简单的路径
        return QS("./ai_config.ini");
    }
}
