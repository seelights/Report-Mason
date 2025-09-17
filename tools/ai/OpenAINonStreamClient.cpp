#include "../src/QtCompat.h"
#include "OpenAINonStreamClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

OpenAINonStreamClient::OpenAINonStreamClient(QObject* parent)
    : OpenAIBase(parent)
{
}

void OpenAINonStreamClient::start(const QList<ChatMessage>& messages)
{
    QNetworkReply* reply = postChat(messages, false); // 非流式
    if (!reply) {
        return;
    }

    // 连接完成信号
    connect(reply, &QNetworkReply::finished, this, &OpenAINonStreamClient::onRequestFinished);
}

void OpenAINonStreamClient::onRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    const QByteArray data = reply->readAll();
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = parseError(data, httpStatus);
        emit error(errorMsg);
        clearReply();
        return;
    }

    // 解析响应
    AiResponse response = parseResponse(data);
    
    if (!response.success) {
        emit error(response.errorMessage);
        clearReply();
        return;
    }

    // 发送使用量统计
    if (response.usage.isValid()) {
        emit usageReady(response.usage);
    }

    // 发送完整结果
    emit chunk(response.content);  // 非流式可以只发一次全量
    emit finished(response.content);
    
    clearReply();
}

AiResponse OpenAINonStreamClient::parseResponse(const QByteArray& data)
{
    AiResponse response;
    
    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        response.success = false;
        response.errorMessage = QS("响应JSON解析失败: %1").arg(parseError.errorString());
        return response;
    }
    
    if (!doc.isObject()) {
        response.success = false;
        response.errorMessage = QS("响应不是有效的JSON对象");
        return response;
    }

    QJsonObject root = doc.object();

    // 提取响应内容
    QJsonArray choices = root.value(QS("choices")).toArray();
    if (choices.isEmpty()) {
        response.success = false;
        response.errorMessage = QS("响应中没有choices字段或为空");
        return response;
    }

    QJsonObject firstChoice = choices[0].toObject();
    QJsonObject message = firstChoice.value(QS("message")).toObject();
    QString content = message.value(QS("content")).toString();
    
    if (content.isEmpty()) {
        response.success = false;
        response.errorMessage = QS("响应内容为空");
        return response;
    }

    response.content = content;
    response.success = true;

    // 提取使用量统计
        if (root.contains(QS("usage"))) {
            QJsonObject usage = root.value(QS("usage")).toObject();
            response.usage.promptTokens = usage.value(QS("prompt_tokens")).toInt(-1);
            response.usage.completionTokens = usage.value(QS("completion_tokens")).toInt(-1);
            response.usage.totalTokens = usage.value(QS("total_tokens")).toInt(-1);
    }

    return response;
}
