/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 内容处理工具类实现
 * @FilePath: \ReportMason\tools\utils\ContentUtils.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "QtCompat.h"
#include "ContentUtils.h"
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QCryptographicHash>
#include <QImageReader>
#include <QImageWriter>
#include <QBuffer>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QDebug>

QString ContentUtils::cleanText(const QString &text)
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

QString ContentUtils::detectFileFormat(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix().toLower();
}

QString ContentUtils::generateUniqueId(const QString &prefix)
{
    QString timestamp = QDateTime::currentDateTime().toString(QS("yyyyMMddhhmmss"));
    QString random = QString::number(QRandomGenerator::global()->bounded(10000));
    return QString(QS("%1_%2_%3")).arg(prefix, timestamp, random);
}

QString ContentUtils::encodeToBase64(const QByteArray &data)
{
    return QString::fromUtf8(data.toBase64());
}

QByteArray ContentUtils::decodeFromBase64(const QString &base64String)
{
    return QByteArray::fromBase64(base64String.toUtf8());
}

bool ContentUtils::validateFilePath(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.exists() && fileInfo.isFile() && fileInfo.isReadable();
}

bool ContentUtils::createDirectory(const QString &dirPath)
{
    QDir dir;
    return dir.mkpath(dirPath);
}

QString ContentUtils::getFileExtension(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix().toLower();
}

QString ContentUtils::getFileNameWithoutExtension(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.baseName();
}

QString ContentUtils::joinPath(const QString &basePath, const QString &relativePath)
{
    return QDir(basePath).absoluteFilePath(relativePath);
}

QString ContentUtils::escapeXml(const QString &content)
{
    QString escaped = content;
    escaped.replace(QStringLiteral("&"), QStringLiteral("&amp;"));
    escaped.replace(QStringLiteral("<"), QStringLiteral("&lt;"));
    escaped.replace(QStringLiteral(">"), QStringLiteral("&gt;"));
    escaped.replace(QStringLiteral("\""), QStringLiteral("&quot;"));
    escaped.replace(QStringLiteral("'"), QStringLiteral("&apos;"));
    return escaped;
}

QString ContentUtils::escapeJson(const QString &content)
{
    QString escaped = content;
    escaped.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    escaped.replace(QStringLiteral("\""), QStringLiteral("\\\""));
    escaped.replace(QStringLiteral("\b"), QStringLiteral("\\b"));
    escaped.replace(QStringLiteral("\f"), QStringLiteral("\\f"));
    escaped.replace(QStringLiteral("\n"), QStringLiteral("\\n"));
    escaped.replace(QStringLiteral("\r"), QStringLiteral("\\r"));
    escaped.replace(QStringLiteral("\t"), QStringLiteral("\\t"));
    return escaped;
}

QString ContentUtils::escapeCsv(const QString &content)
{
    QString escaped = content;
    
    // 如果内容包含逗号、引号或换行符，需要用引号包围
    if (escaped.contains(QS(",")) || escaped.contains(QS("\"")) || escaped.contains(QS("\n"))) {
        // 转义引号
        escaped.replace(QS("\""), QS("\"\""));
        // 用引号包围
        escaped = QS("\"") + escaped + QS("\"");
    }
    
    return escaped;
}

QString ContentUtils::escapeHtml(const QString &content)
{
    QString escaped = content;
    escaped.replace(QStringLiteral("&"), QStringLiteral("&amp;"));
    escaped.replace(QStringLiteral("<"), QStringLiteral("&lt;"));
    escaped.replace(QStringLiteral(">"), QStringLiteral("&gt;"));
    escaped.replace(QStringLiteral("\""), QStringLiteral("&quot;"));
    escaped.replace(QStringLiteral("'"), QStringLiteral("&#39;"));
    return escaped;
}

QString ContentUtils::detectImageFormat(const QByteArray &imageData)
{
    if (imageData.isEmpty()) {
        return QString();
    }

    // 通过文件头检测
    if (imageData.startsWith("\x89PNG")) {
        return QS("png");
    } else if (imageData.startsWith("\xFF\xD8\xFF")) {
        return QS("jpg");
    } else if (imageData.startsWith("GIF8")) {
        return QS("gif");
    } else if (imageData.startsWith("BM")) {
        return QS("bmp");
    } else if (imageData.startsWith("II*\x00") || imageData.startsWith("MM\x00*")) {
        return QS("tiff");
    } else if (imageData.startsWith("RIFF") && imageData.mid(8, 4) == "WEBP") {
        return QS("webp");
    }

    // 尝试使用QImageReader检测
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    QString format = QString::fromUtf8(reader.format());
    
    return format.toLower();
}

QSize ContentUtils::getImageSize(const QByteArray &imageData)
{
    if (imageData.isEmpty()) {
        return QSize();
    }

    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    return reader.size();
}

QByteArray ContentUtils::compressImage(const QByteArray &imageData, int quality)
{
    if (imageData.isEmpty()) {
        return QByteArray();
    }

    QBuffer inputBuffer;
    inputBuffer.setData(imageData);
    inputBuffer.open(QIODevice::ReadOnly);

    QImageReader reader(&inputBuffer);
    QImage image = reader.read();
    
    if (image.isNull()) {
        return QByteArray();
    }

    QBuffer outputBuffer;
    outputBuffer.open(QIODevice::WriteOnly);

    QImageWriter writer(&outputBuffer, "JPEG");
    writer.setQuality(quality);
    writer.write(image);

    return outputBuffer.data();
}

QByteArray ContentUtils::resizeImage(const QByteArray &imageData, const QSize &size)
{
    if (imageData.isEmpty() || size.isEmpty()) {
        return QByteArray();
    }

    QBuffer inputBuffer;
    inputBuffer.setData(imageData);
    inputBuffer.open(QIODevice::ReadOnly);

    QImageReader reader(&inputBuffer);
    QImage image = reader.read();
    
    if (image.isNull()) {
        return QByteArray();
    }

    QImage scaledImage = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QBuffer outputBuffer;
    outputBuffer.open(QIODevice::WriteOnly);

    QImageWriter writer(&outputBuffer, "PNG");
    writer.write(scaledImage);

    return outputBuffer.data();
}

bool ContentUtils::validateJson(const QString &jsonString)
{
    QJsonParseError error;
    QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    return error.error == QJsonParseError::NoError;
}

QString ContentUtils::formatJson(const QString &jsonString)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        return jsonString; // 返回原始字符串
    }
    
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

QString ContentUtils::calculateFileHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(&file);
    file.close();

    return QString::fromUtf8(hash.result().toHex());
}

QString ContentUtils::calculateDataHash(const QByteArray &data)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(data);
    return QString::fromUtf8(hash.result().toHex());
}
