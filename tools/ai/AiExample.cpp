#include "../src/QtCompat.h"
#include "AiManager.h"
#include "OpenAINonStreamClient.h"
#include "OpenAIStreamClient.h"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

/**
 * @brief AI工具使用示例
 * 演示如何使用AI管理器进行各种AI对话操作
 */
class AiExample : public QObject
{
    Q_OBJECT

public:
    explicit AiExample(QObject* parent = nullptr) : QObject(parent) {}

    /**
     * @brief 运行所有示例
     */
    void runExamples()
    {
        qDebug() << "=== AI工具使用示例 ===";
        
        // 示例1：快速聊天
        runQuickChatExample();
        
        // 示例2：非流式客户端
        runNonStreamExample();
        
        // 示例3：流式客户端
        runStreamExample();
        
        // 示例4：自定义配置
        runCustomConfigExample();
    }

private slots:
    void onNonStreamFinished(const QString& result)
    {
        qDebug() << "非流式结果:" << result;
    }
    
    void onStreamChunk(const QString& chunk)
    {
        qDebug() << "流式增量:" << chunk;
    }
    
    void onStreamFinished(const QString& result)
    {
        qDebug() << "流式完成:" << result;
    }
    
    void onError(const QString& error)
    {
        qWarning() << "错误:" << error;
    }

private:
    /**
     * @brief 示例1：快速聊天
     */
    void runQuickChatExample()
    {
        qDebug() << "\n--- 示例1：快速聊天 ---";
        
        AiManager* manager = AiManager::instance();
        
        // 准备消息
        QList<ChatMessage> messages = {
            {QS("system"), QS("你是一个有帮助的助手")},
            {QS("user"), QS("请简单介绍一下Qt框架")}
        };
        
        // 快速发送请求
        AiResponse response = manager->quickChat(messages, QS("DeepSeek"));
        
        if (response.success) {
            qDebug() << "快速聊天结果:" << response.content;
            if (response.usage.isValid()) {
                qDebug() << "使用量:" << response.usage.promptTokens << "+" 
                         << response.usage.completionTokens << "=" << response.usage.totalTokens;
            }
        } else {
            qWarning() << "快速聊天失败:" << response.errorMessage;
        }
    }
    
    /**
     * @brief 示例2：非流式客户端
     */
    void runNonStreamExample()
    {
        qDebug() << "\n--- 示例2：非流式客户端 ---";
        
        AiManager* manager = AiManager::instance();
        auto* client = manager->createNonStreamClient(QS("DeepSeek"));
        
        if (!client) {
            qWarning() << "创建非流式客户端失败";
            return;
        }
        
        // 连接信号
        connect(client, &OpenAINonStreamClient::finished, this, &AiExample::onNonStreamFinished);
        connect(client, &OpenAINonStreamClient::error, this, &AiExample::onError);
        
        // 准备消息
        QList<ChatMessage> messages = {
            {QS("user"), QS("用一句话总结C++的特点")}
        };
        
        // 发送请求
        client->start(messages);
    }
    
    /**
     * @brief 示例3：流式客户端
     */
    void runStreamExample()
    {
        qDebug() << "\n--- 示例3：流式客户端 ---";
        
        AiManager* manager = AiManager::instance();
        auto* client = manager->createStreamClient(QS("DeepSeek"));
        
        if (!client) {
            qWarning() << "创建流式客户端失败";
            return;
        }
        
        // 连接信号
        connect(client, &OpenAIStreamClient::chunk, this, &AiExample::onStreamChunk);
        connect(client, &OpenAIStreamClient::finished, this, &AiExample::onStreamFinished);
        connect(client, &OpenAIStreamClient::error, this, &AiExample::onError);
        
        // 准备消息
        QList<ChatMessage> messages = {
            {QS("user"), QS("写一首关于编程的短诗")}
        };
        
        // 发送请求
        client->start(messages);
    }
    
    /**
     * @brief 示例4：自定义配置
     */
    void runCustomConfigExample()
    {
        qDebug() << "\n--- 示例4：自定义配置 ---";
        
        AiManager* manager = AiManager::instance();
        
        // 创建自定义配置
        LlmContext customConfig;
        customConfig.provider = Provider::DeepSeek;
        customConfig.baseUrl = QS("https://api.deepseek.com");
        customConfig.apiKey = QS("your-api-key-here"); // 请替换为真实的API密钥
        customConfig.model = QS("deepseek-chat");
        customConfig.temperature = 0.5;  // 降低随机性
        customConfig.maxTokens = 100;    // 限制输出长度
        
        // 添加自定义配置
        manager->addProvider(QS("自定义DeepSeek"), customConfig);
        
        // 使用自定义配置
        auto* client = manager->createNonStreamClient(QS("自定义DeepSeek"));
        if (client) {
            connect(client, &OpenAINonStreamClient::finished, [](const QString& result) {
                qDebug() << "自定义配置结果:" << result;
            });
            
            QList<ChatMessage> messages = {
                {QS("user"), QS("解释什么是设计模式")}
            };
            
            client->start(messages);
        }
    }
};

// 主函数示例
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    AiExample example;
    
    // 延迟运行示例，确保应用完全启动
    QTimer::singleShot(100, &example, &AiExample::runExamples);
    
    // 设置退出定时器
    QTimer::singleShot(10000, &app, &QCoreApplication::quit);
    
    return app.exec();
}

#include "AiExample.moc"
