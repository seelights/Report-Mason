/*
 * @Author: seelights
 * @Date: 2025-09-14 21:30:00
 * @LastEditTime: 2025-09-14 21:30:00
 * @LastEditors: seelights
 * @Description: KZip工具类实现
 * @FilePath: \ReportMason\src\KZipUtils.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#include "KZipUtils.h"
#include "kzip.h"
#include "karchivedirectory.h"
#include "karchivefile.h"
#include <QFileInfo>
#include <QDebug>

bool KZipUtils::readFileFromZip(const QString& zipPath, const QString& internalPath,
                                QByteArray& content)
{
    KZip zip(zipPath);
    if (!zip.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开ZIP文件:" << zipPath;
        return false;
    }

    const KArchiveDirectory* rootDir = zip.directory();
    if (!rootDir) {
        qDebug() << "ZIP文件格式错误:" << zipPath;
        zip.close();
        return false;
    }

    const KArchiveFile* file = rootDir->file(internalPath);
    if (!file) {
        qDebug() << "无法在ZIP中找到文件:" << internalPath;
        zip.close();
        return false;
    }

    content = file->data();
    zip.close();
    return true;
}

QStringList KZipUtils::getFileList(const QString& zipPath)
{
    QStringList fileList;

    KZip zip(zipPath);
    if (!zip.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开ZIP文件:" << zipPath;
        return fileList;
    }

    const KArchiveDirectory* rootDir = zip.directory();
    if (!rootDir) {
        qDebug() << "ZIP文件格式错误:" << zipPath;
        zip.close();
        return fileList;
    }

    fileList = getFilesRecursive(rootDir);
    zip.close();
    return fileList;
}

bool KZipUtils::fileExists(const QString& zipPath, const QString& internalPath)
{
    KZip zip(zipPath);
    if (!zip.open(QIODevice::ReadOnly)) {
        return false;
    }

    const KArchiveDirectory* rootDir = zip.directory();
    if (!rootDir) {
        zip.close();
        return false;
    }

    const KArchiveEntry* entry = rootDir->entry(internalPath);
    bool exists = (entry != nullptr && entry->isFile());

    zip.close();
    return exists;
}

bool KZipUtils::createZip(const QString& zipPath, const QMap<QString, QByteArray>& files)
{
    KZip zip(zipPath);
    if (!zip.open(QIODevice::WriteOnly)) {
        qDebug() << "无法创建ZIP文件:" << zipPath;
        return false;
    }

    for (auto it = files.begin(); it != files.end(); ++it) {
        if (!zip.writeFile(it.key(), it.value())) {
            qDebug() << "无法写入文件到ZIP:" << it.key();
            zip.close();
            return false;
        }
    }

    zip.close();
    return true;
}

bool KZipUtils::copyZipWithReplacements(const QString& sourcePath, const QString& targetPath,
                                        const QMap<QString, QByteArray>& replacements)
{
    // 读取源ZIP文件
    KZip sourceZip(sourcePath);
    if (!sourceZip.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开源ZIP文件:" << sourcePath;
        return false;
    }

    const KArchiveDirectory* sourceRoot = sourceZip.directory();
    if (!sourceRoot) {
        qDebug() << "源ZIP文件格式错误:" << sourcePath;
        sourceZip.close();
        return false;
    }

    // 创建目标ZIP文件
    KZip targetZip(targetPath);
    if (!targetZip.open(QIODevice::WriteOnly)) {
        qDebug() << "无法创建目标ZIP文件:" << targetPath;
        sourceZip.close();
        return false;
    }

    // 复制所有文件，替换指定的文件
    const QStringList entries = sourceRoot->entries();
    for (const QString& entryName : entries) {
        const KArchiveEntry* entry = sourceRoot->entry(entryName);
        if (entry->isFile()) {
            const KArchiveFile* file = static_cast<const KArchiveFile*>(entry);

            if (replacements.contains(entryName)) {
                // 使用替换内容
                if (!targetZip.writeFile(entryName, replacements[entryName])) {
                    qDebug() << "无法写入替换文件:" << entryName;
                    sourceZip.close();
                    targetZip.close();
                    return false;
                }
            } else {
                // 复制原文件
                QByteArray fileData = file->data();
                if (!targetZip.writeFile(entryName, fileData)) {
                    qDebug() << "无法复制文件:" << entryName;
                    sourceZip.close();
                    targetZip.close();
                    return false;
                }
            }
        }
    }

    sourceZip.close();
    targetZip.close();
    return true;
}

bool KZipUtils::isValidZip(const QString& zipPath)
{
    QFileInfo fileInfo(zipPath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return false;
    }

    KZip zip(zipPath);
    bool valid = zip.open(QIODevice::ReadOnly);
    if (valid) {
        zip.close();
    }
    return valid;
}

QMap<QString, qint64> KZipUtils::getZipInfo(const QString& zipPath)
{
    QMap<QString, qint64> info;

    KZip zip(zipPath);
    if (!zip.open(QIODevice::ReadOnly)) {
        return info;
    }

    const KArchiveDirectory* rootDir = zip.directory();
    if (!rootDir) {
        zip.close();
        return info;
    }

    const QStringList entries = rootDir->entries();
    for (const QString& entryName : entries) {
        const KArchiveEntry* entry = rootDir->entry(entryName);
        if (entry->isFile()) {
            const KArchiveFile* file = static_cast<const KArchiveFile*>(entry);
            info[entryName] = file->size();
        }
    }

    zip.close();
    return info;
}

QStringList KZipUtils::getFilesRecursive(const KArchiveDirectory* dir, const QString& prefix)
{
    QStringList files;
    const QStringList entries = dir->entries();

    for (const QString& entryName : entries) {
        const KArchiveEntry* entry = dir->entry(entryName);
        QString fullPath = prefix.isEmpty() ? entryName : prefix + "/" + entryName;

        if (entry->isFile()) {
            files.append(fullPath);
        } else if (entry->isDirectory()) {
            const KArchiveDirectory* subDir = static_cast<const KArchiveDirectory*>(entry);
            files.append(getFilesRecursive(subDir, fullPath));
        }
    }

    return files;
}
