#pragma once

#include "OpenAIBase.h"

/**
 * @brief OpenAI兼容的非流式客户端
 * 一次性返回完整的AI响应结果
 */
class OpenAINonStreamClient : public OpenAIBase
{
    Q_OBJECT

public:
    explicit OpenAINonStreamClient(QObject* parent = nullptr);

    /**
     * @brief 开始非流式AI对话
     * @param messages 对话消息列表
     */
    void start(const QList<ChatMessage>& messages) override;

private slots:
    /**
     * @brief 处理网络请求完成
     */
    void onRequestFinished();

private:
    /**
     * @brief 解析非流式响应
     * @param data 响应数据
     * @return 解析结果
     */
    AiResponse parseResponse(const QByteArray& data);
};

