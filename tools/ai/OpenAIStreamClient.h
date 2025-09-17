#pragma once

#include "OpenAIBase.h"

/**
 * @brief OpenAI兼容的流式客户端
 * 支持Server-Sent Events (SSE) 流式输出
 */
class OpenAIStreamClient : public OpenAIBase
{
    Q_OBJECT

public:
    explicit OpenAIStreamClient(QObject* parent = nullptr);

    /**
     * @brief 开始流式AI对话
     * @param messages 对话消息列表
     */
    void start(const QList<ChatMessage>& messages) override;

    /**
     * @brief 获取当前累积的完整文本
     * @return 完整文本
     */
    QString getAccumulatedText() const { return m_accumulated; }

    /**
     * @brief 清空累积文本
     */
    void clearAccumulated() { m_accumulated.clear(); }

private slots:
    /**
     * @brief 处理数据接收
     */
    void onReadyRead();

    /**
     * @brief 处理请求完成
     */
    void onRequestFinished();

private:
    /**
     * @brief 解析SSE数据块
     * @param data 原始数据
     */
    void parseSSEData(const QByteArray& data);

    /**
     * @brief 处理单个SSE事件
     * @param eventData 事件数据
     */
    void processSSEEvent(const QByteArray& eventData);

private:
    QByteArray m_sseBuffer;    // SSE数据缓冲区
    QString m_accumulated;     // 累积的完整文本
};

