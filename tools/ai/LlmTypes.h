#pragma once

#include <QString>
#include <QList>
#include "../src/QtCompat.h"

/**
 * @brief 聊天消息结构体
 */
struct ChatMessage
{
    QString role; // "system" | "user" | "assistant"
    QString content;

    ChatMessage() = default;
    ChatMessage(const QString& r, const QString& c) : role(r), content(c) {}
};

/**
 * @brief AI服务提供商枚举
 */
enum class Provider : int {
    OpenAI = 0,    // OpenAI GPT
    DeepSeek = 1,  // DeepSeek
    DoubaoArk = 2, // 火山方舟的 OpenAI-兼容网关
    CustomOpenAICompatible = 100
};

/**
 * @brief AI任务类型枚举
 */
enum class AiType : int {
    Chat = 0,       // 普通聊天
    Reasoning = 1,  // 推理任务
    Vision = 2,     // 视觉任务
    AudioToText = 3 // 语音转文字
};

/**
 * @brief 使用量统计结构体
 */
struct Usage
{
    int promptTokens = -1;     // 输入token数
    int completionTokens = -1; // 输出token数
    int totalTokens = -1;      // 总token数

    bool isValid() const { return promptTokens >= 0 || completionTokens >= 0 || totalTokens >= 0; }
};

/**
 * @brief 大模型上下文配置
 * 包含所有调用AI服务所需的配置信息
 */
struct LlmContext
{
    // 基本配置
    Provider provider = Provider::OpenAI; // 厂商类型
    AiType aiType = AiType::Chat;         // 任务类型
    QString baseUrl;                      // API基础URL，例如 https://api.openai.com
    QString apiKey;                       // API密钥
    QString model;                        // 模型名称

    // 生成参数
    double temperature = 0.7; // 温度参数，控制随机性
    double topP = 1.0;        // Top-p参数
    int maxTokens = 0;        // 最大生成token数，0表示不限制
    int timeoutMs = 60000;    // 请求超时时间（毫秒）

    // 流式控制
    bool stream = false; // 是否使用流式输出

    // 验证配置是否有效
    bool isValid() const { return !baseUrl.isEmpty() && !apiKey.isEmpty() && !model.isEmpty(); }

    // 获取完整的API端点URL
    QString getApiUrl() const
    {
        QString url = baseUrl;
        if (!url.endsWith(QS("/"))) {
            url += QS("/");
        }
        url += QS("v1/chat/completions");
        return url;
    }
};

/**
 * @brief AI响应结果结构体
 */
struct AiResponse
{
    QString content;      // 响应内容
    Usage usage;          // 使用量统计
    bool success = false; // 是否成功
    QString errorMessage; // 错误信息

    AiResponse() = default;
    AiResponse(const QString& c, bool s = true) : content(c), success(s) {}
    AiResponse(const QString& c, const Usage& u, bool s = true) : content(c), usage(u), success(s)
    {
    }
};

