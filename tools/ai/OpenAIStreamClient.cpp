#include "../src/QtCompat.h"
#include "OpenAIStreamClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

OpenAIStreamClient::OpenAIStreamClient(QObject* parent)
    : OpenAIBase(parent)
{
}

void OpenAIStreamClient::start(const QList<ChatMessage>& messages)
{
    // 清空缓冲区
    m_sseBuffer.clear();
    m_accumulated.clear();

    QNetworkReply* reply = postChat(messages, true); // 流式
    if (!reply) {
        return;
    }

    // 连接数据接收信号
    connect(reply, &QIODevice::readyRead, this, &OpenAIStreamClient::onReadyRead);
    connect(reply, &QNetworkReply::finished, this, &OpenAIStreamClient::onRequestFinished);
}

void OpenAIStreamClient::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    QByteArray newData = reply->readAll();
    parseSSEData(newData);
}

void OpenAIStreamClient::onRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QByteArray data = reply->readAll();
        const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString errorMsg = parseError(data, httpStatus);
        emit error(errorMsg);
    } else {
        // 处理剩余的缓冲区数据
        if (!m_sseBuffer.isEmpty()) {
            parseSSEData(QByteArray());
        }
        
        // 发送最终完整结果
        emit finished(m_accumulated);
    }
    
    clearReply();
}

void OpenAIStreamClient::parseSSEData(const QByteArray& data)
{
    // 将新数据添加到缓冲区
    m_sseBuffer.append(data);
    
    // 按行解析SSE数据
    int lineEnd;
    while ((lineEnd = m_sseBuffer.indexOf('\n')) >= 0) {
        QByteArray line = m_sseBuffer.left(lineEnd);
        m_sseBuffer.remove(0, lineEnd + 1);
        
        // 处理回车符
        if (!line.isEmpty() && line.endsWith('\r')) {
            line.chop(1);
        }
        
        // 跳过空行
        if (line.isEmpty()) {
            continue;
        }
        
        // 处理data:开头的行
        if (line.startsWith("data:")) {
            QByteArray payload = line.mid(5).trimmed(); // 去掉"data:"前缀
            
            // 检查结束标志
            if (payload == "[DONE]") {
                emit finished(m_accumulated);
                clearReply();
                return;
            }
            
            // 处理JSON数据
            processSSEEvent(payload);
        }
    }
}

void OpenAIStreamClient::processSSEEvent(const QByteArray& eventData)
{
    if (eventData.isEmpty()) {
        return;
    }
    
    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(eventData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "SSE JSON解析错误:" << parseError.errorString();
        return;
    }
    
    if (!doc.isObject()) {
        return;
    }
    
    QJsonObject obj = doc.object();
    QJsonArray choices = obj.value(QS("choices")).toArray();
    
    if (choices.isEmpty()) {
        return;
    }
    
    QJsonObject firstChoice = choices[0].toObject();
    QJsonObject delta = firstChoice.value(QS("delta")).toObject();
    
    // 提取增量内容
    QString content = delta.value(QS("content")).toString();
    if (!content.isEmpty()) {
        m_accumulated += content;
        emit chunk(content);
    }
    
    // 处理其他可能的增量字段（如role、tool_calls等）
    // 这里可以根据需要扩展
}
