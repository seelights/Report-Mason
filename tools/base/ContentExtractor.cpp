/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 内容提取器基类实现
 * @FilePath: \ReportMason\tools\base\ContentExtractor.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "QtCompat.h"
#include "ContentExtractor.h"
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QCryptographicHash>

ContentExtractor::ContentExtractor(QObject* parent)
    : QObject(parent),
      m_outputDirectory(QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
                        QS("/ReportMason")),
      m_idCounter(0)
{
    // 确保输出目录存在
    QDir().mkpath(m_outputDirectory);
}

ContentExtractor::~ContentExtractor() {}

QString ContentExtractor::getLastError() const { return m_lastError; }

void ContentExtractor::setOutputDirectory(const QString& outputDir)
{
    m_outputDirectory = outputDir;
    QDir().mkpath(m_outputDirectory);
}

QString ContentExtractor::getOutputDirectory() const { return m_outputDirectory; }

void ContentExtractor::setLastError(const QString& error)
{
    m_lastError = error;
    qDebug() << "ContentExtractor Error:" << error;
}

bool ContentExtractor::validateFilePath(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    return fileInfo.exists() && fileInfo.isFile() && fileInfo.isReadable();
}

QString ContentExtractor::cleanText(const QString& text) const
{
    QString cleaned = text;

    // 统一换行符
    cleaned.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    cleaned.replace(QStringLiteral("\r"), QStringLiteral("\n"));

    // 移除多余空白
    cleaned = cleaned.simplified();

    // 统一标点符号（全角转半角）
    cleaned.replace(QStringLiteral("："), QStringLiteral(":"));
    cleaned.replace(QStringLiteral("，"), QStringLiteral(","));
    cleaned.replace(QStringLiteral("。"), QStringLiteral("."));
    cleaned.replace(QStringLiteral("！"), QStringLiteral("!"));
    cleaned.replace(QStringLiteral("？"), QStringLiteral("?"));

    return cleaned;
}

QString ContentExtractor::generateUniqueId(const QString& prefix)
{
    QString timestamp = QDateTime::currentDateTime().toString(QS("yyyyMMddhhmmss"));
    QString counter = QString::number(++m_idCounter);
    return QS("%1_%2_%3").arg(prefix, timestamp, counter);
}

bool ContentExtractor::saveContentToFile(const QByteArray& content, const QString& fileName)
{
    QString fullPath = QDir(m_outputDirectory).absoluteFilePath(fileName);
    QFile file(fullPath);

    if (!file.open(QIODevice::WriteOnly)) {
        setLastError(QS("无法创建文件: %1").arg(fullPath));
        return false;
    }

    qint64 bytesWritten = file.write(content);
    file.close();

    if (bytesWritten != content.size()) {
        setLastError(QS("文件写入不完整: %1").arg(fullPath));
        return false;
    }

    return true;
}

QString ContentExtractor::encodeToBase64(const QByteArray& content) const
{
    return QString::fromUtf8(content.toBase64());
}

QByteArray ContentExtractor::decodeFromBase64(const QString& base64String) const
{
    return QByteArray::fromBase64(base64String.toUtf8());
}
