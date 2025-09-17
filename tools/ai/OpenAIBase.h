#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include "LlmTypes.h"
#include "../src/QtCompat.h"

/**
 * @brief OpenAI兼容API的抽象基类
 * 提供通用的请求构建、鉴权、错误处理等功能
 * 派生类只需要实现具体的响应解析逻辑
 */
class OpenAIBase : public QObject
{
    Q_OBJECT

public:
    explicit OpenAIBase(QObject* parent = nullptr);
    virtual ~OpenAIBase();

    /**
     * @brief 设置大模型上下文配置
     * @param ctx 配置信息
     */
    void setContext(const LlmContext& ctx);
    
    /**
     * @brief 获取当前配置
     * @return 配置信息
     */
    const LlmContext& context() const { return m_ctx; }

    /**
     * @brief 检查是否正在处理请求
     * @return true表示忙碌中
     */
    virtual bool busy() const { return m_reply != nullptr; }

    /**
     * @brief 取消当前请求
     */
    virtual void cancel();

    /**
     * @brief 开始AI对话请求
     * @param messages 对话消息列表
     * 派生类需要实现此方法来处理具体的请求逻辑
     */
    virtual void start(const QList<ChatMessage>& messages) = 0;

signals:
    /**
     * @brief 流式输出增量数据
     * @param delta 增量文本
     */
    void chunk(const QString& delta);

    /**
     * @brief 请求完成，返回完整结果
     * @param fullText 完整响应文本
     */
    void finished(const QString& fullText);

    /**
     * @brief 发生错误
     * @param message 错误信息
     */
    void error(const QString& message);

    /**
     * @brief 使用量统计信息
     * @param usage 使用量数据
     */
    void usageReady(const Usage& usage);

protected:
    /**
     * @brief 发送POST请求到聊天完成API
     * @param messages 消息列表
     * @param stream 是否使用流式输出
     * @return 网络回复对象，失败返回nullptr
     */
    QNetworkReply* postChat(const QList<ChatMessage>& messages, bool stream);

    /**
     * @brief 清理网络回复对象
     */
    void clearReply();

    /**
     * @brief 解析错误响应
     * @param data 响应数据
     * @param httpStatus HTTP状态码
     * @return 错误信息
     */
    QString parseError(const QByteArray& data, int httpStatus);

    /**
     * @brief 构建请求体JSON
     * @param messages 消息列表
     * @param stream 是否流式
     * @return JSON文档
     */
    QJsonDocument buildRequestBody(const QList<ChatMessage>& messages, bool stream);

protected:
    QNetworkAccessManager m_nam;      // 网络访问管理器
    LlmContext m_ctx;                 // 配置上下文
    QNetworkReply* m_reply = nullptr; // 当前网络回复
    QTimer* m_timeoutTimer = nullptr; // 超时定时器
};

