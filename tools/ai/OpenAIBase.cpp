#include "../src/QtCompat.h"
#include "OpenAIBase.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDebug>

OpenAIBase::OpenAIBase(QObject* parent)
    : QObject(parent)
    , m_timeoutTimer(new QTimer(this))
{
    // 设置超时定时器为单次触发
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_reply) {
            emit error(QS("请求超时"));
            cancel();
        }
    });
}

OpenAIBase::~OpenAIBase()
{
    cancel();
}

void OpenAIBase::setContext(const LlmContext& ctx)
{
    if (busy()) {
        cancel();
    }
    m_ctx = ctx;
}

void OpenAIBase::cancel()
{
    if (m_reply) {
        disconnect(m_reply, nullptr, this, nullptr);
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
    }
}

QNetworkReply* OpenAIBase::postChat(const QList<ChatMessage>& messages, bool stream)
{
    if (busy()) {
        emit error(QS("客户端忙碌中，请等待当前请求完成"));
        return nullptr;
    }
    
    if (!m_ctx.isValid()) {
        emit error(QS("配置无效：baseUrl、apiKey、model 不能为空"));
        return nullptr;
    }

    // 构建请求URL
    QUrl url(m_ctx.getApiUrl());
    QNetworkRequest req(url);
    
    // 设置请求头
    req.setHeader(QNetworkRequest::ContentTypeHeader, QS("application/json"));
    req.setRawHeader("Authorization", (QS("Bearer ") + m_ctx.apiKey).toUtf8());
    
    if (stream) {
        req.setRawHeader("Accept", "text/event-stream");
    }

    // 设置超时
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    req.setTransferTimeout(m_ctx.timeoutMs);
#endif

    // 构建请求体
    QJsonDocument body = buildRequestBody(messages, stream);
    const QByteArray payload = body.toJson(QJsonDocument::Compact);

    // 发送请求
    m_reply = m_nam.post(req, payload);
    
    // 启动超时定时器
    if (m_timeoutTimer) {
        m_timeoutTimer->start(m_ctx.timeoutMs);
    }

    return m_reply;
}

void OpenAIBase::clearReply()
{
    if (m_reply) {
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
    }
}

QString OpenAIBase::parseError(const QByteArray& data, int httpStatus)
{
    QString errorMsg = QS("HTTP %1: ").arg(httpStatus);
    
    // 尝试解析JSON错误响应
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        
        // 检查是否有error字段
        if (obj.contains(QS("error"))) {
            QJsonObject errorObj = obj.value(QS("error")).toObject();
            QString message = errorObj.value(QS("message")).toString();
            QString type = errorObj.value(QS("type")).toString();
            
            if (!message.isEmpty()) {
                errorMsg += message;
                if (!type.isEmpty()) {
                    errorMsg += QS(" (类型: %1)").arg(type);
                }
            } else {
                errorMsg += QString::fromUtf8(data);
            }
        } else {
            errorMsg += QString::fromUtf8(data);
        }
    } else {
        errorMsg += QString::fromUtf8(data);
    }
    
    return errorMsg;
}

QJsonDocument OpenAIBase::buildRequestBody(const QList<ChatMessage>& messages, bool stream)
{
    // 构建消息数组
    QJsonArray jMsgs;
    for (const auto& msg : messages) {
        QJsonObject msgObj;
        msgObj[QS("role")] = msg.role;
        msgObj[QS("content")] = msg.content;
        jMsgs.append(msgObj);
    }

    // 构建请求体
    QJsonObject body;
    body[QS("model")] = m_ctx.model;
    body[QS("messages")] = jMsgs;
    body[QS("temperature")] = m_ctx.temperature;
    body[QS("top_p")] = m_ctx.topP;
    
    if (m_ctx.maxTokens > 0) {
        body[QS("max_tokens")] = m_ctx.maxTokens;
    }
    
    body[QS("stream")] = stream;

    return QJsonDocument(body);
}
