/*
 * @Author: seelights
 * @Date: 2025-09-15 19:05:00
 * @LastEditTime: 2025-09-15 19:05:00
 * @LastEditors: seelights
 * @Description: 内容处理工具类实现
 * @FilePath: \ReportMason\tools\utils\ContentUtils.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

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
    cleaned.replace("\r\n", "\n");
    cleaned.replace("\r", "\n");
    
    // 移除多余空白
    cleaned = cleaned.simplified();
    
    // 统一标点符号（全角转半角）
    cleaned.replace("：", ":");
    cleaned.replace("，", ",");
    cleaned.replace("。", ".");
    cleaned.replace("！", "!");
    cleaned.replace("？", "?");
    
    return cleaned;
}

QString ContentUtils::detectFileFormat(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix().toLower();
}

QString ContentUtils::generateUniqueId(const QString &prefix)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString random = QString::number(QRandomGenerator::global()->bounded(10000));
    return QString("%1_%2_%3").arg(prefix, timestamp, random);
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
    escaped.replace("&", "&amp;");
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    escaped.replace("\"", "&quot;");
    escaped.replace("'", "&apos;");
    return escaped;
}

QString ContentUtils::escapeJson(const QString &content)
{
    QString escaped = content;
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.replace("\b", "\\b");
    escaped.replace("\f", "\\f");
    escaped.replace("\n", "\\n");
    escaped.replace("\r", "\\r");
    escaped.replace("\t", "\\t");
    return escaped;
}

QString ContentUtils::escapeCsv(const QString &content)
{
    QString escaped = content;
    
    // 如果内容包含逗号、引号或换行符，需要用引号包围
    if (escaped.contains(',') || escaped.contains('"') || escaped.contains('\n')) {
        // 转义引号
        escaped.replace('"', "\"\"");
        // 用引号包围
        escaped = '"' + escaped + '"';
    }
    
    return escaped;
}

QString ContentUtils::escapeHtml(const QString &content)
{
    QString escaped = content;
    escaped.replace("&", "&amp;");
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    escaped.replace("\"", "&quot;");
    escaped.replace("'", "&#39;");
    return escaped;
}

QString ContentUtils::detectImageFormat(const QByteArray &imageData)
{
    if (imageData.isEmpty()) {
        return QString();
    }

    // 通过文件头检测
    if (imageData.startsWith("\x89PNG")) {
        return "png";
    } else if (imageData.startsWith("\xFF\xD8\xFF")) {
        return "jpg";
    } else if (imageData.startsWith("GIF8")) {
        return "gif";
    } else if (imageData.startsWith("BM")) {
        return "bmp";
    } else if (imageData.startsWith("II*\x00") || imageData.startsWith("MM\x00*")) {
        return "tiff";
    } else if (imageData.startsWith("RIFF") && imageData.mid(8, 4) == "WEBP") {
        return "webp";
    }

    // 尝试使用QImageReader检测
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    QString format = reader.format();
    
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
    
    return doc.toJson(QJsonDocument::Indented);
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

    return hash.result().toHex();
}

QString ContentUtils::calculateDataHash(const QByteArray &data)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(data);
    return hash.result().toHex();
}
