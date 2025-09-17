#pragma once

#include <QObject>
#include <QMap>
#include <QSettings>
#include "LlmTypes.h"
#include "OpenAIBase.h"
#include "OpenAINonStreamClient.h"
#include "OpenAIStreamClient.h"

/**
 * @brief AI管理器
 * 统一管理多个AI服务提供商，提供便捷的配置和调用接口
 */
class AiManager : public QObject
{
    Q_OBJECT

public:
    explicit AiManager(QObject* parent = nullptr);
    ~AiManager();

    /**
     * @brief 获取单例实例
     * @return 单例指针
     */
    static AiManager* instance();

    /**
     * @brief 添加AI服务配置
     * @param name 配置名称
     * @param context 配置信息
     */
    void addProvider(const QString& name, const LlmContext& context);

    /**
     * @brief 移除AI服务配置
     * @param name 配置名称
     */
    void removeProvider(const QString& name);

    /**
     * @brief 获取AI服务配置
     * @param name 配置名称
     * @return 配置信息，不存在返回默认配置
     */
    LlmContext getProvider(const QString& name) const;

    /**
     * @brief 获取所有配置名称
     * @return 配置名称列表
     */
    QStringList getProviderNames() const;

    /**
     * @brief 设置默认配置名称
     * @param name 配置名称
     */
    void setDefaultProvider(const QString& name);

    /**
     * @brief 获取默认配置名称
     * @return 配置名称
     */
    QString getDefaultProvider() const { return m_defaultProvider; }

    /**
     * @brief 创建非流式客户端
     * @param providerName 配置名称，为空时使用默认配置
     * @return 客户端指针
     */
    OpenAINonStreamClient* createNonStreamClient(const QString& providerName = QString());

    /**
     * @brief 创建流式客户端
     * @param providerName 配置名称，为空时使用默认配置
     * @return 客户端指针
     */
    OpenAIStreamClient* createStreamClient(const QString& providerName = QString());

    /**
     * @brief 快速发送非流式请求
     * @param messages 消息列表
     * @param providerName 配置名称
     * @return 响应结果
     */
    AiResponse quickChat(const QList<ChatMessage>& messages, const QString& providerName = QString());

    /**
     * @brief 从配置文件加载配置
     * @param configPath 配置文件路径
     */
    void loadConfig(const QString& configPath = QString());

    /**
     * @brief 保存配置到文件
     * @param configPath 配置文件路径
     */
    void saveConfig(const QString& configPath = QString());

    /**
     * @brief 获取预设的配置模板
     * @param provider 服务提供商
     * @return 配置模板
     */
    static LlmContext getPresetConfig(Provider provider);

signals:
    /**
     * @brief 配置更新信号
     * @param name 配置名称
     */
    void providerUpdated(const QString& name);

    /**
     * @brief 默认配置变更信号
     * @param name 新的默认配置名称
     */
    void defaultProviderChanged(const QString& name);

private:
    void initializePresets();
    QString getConfigFilePath() const;

private:
    static AiManager* s_instance;
    QMap<QString, LlmContext> m_providers;
    QString m_defaultProvider;
    QSettings* m_settings;
};

