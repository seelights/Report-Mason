/*
 * @Author: seelights
 * @Date: 2025-09-14 21:30:00
 * @LastEditTime: 2025-09-14 22:06:51
 * @LastEditors: seelights
 * @Description: KZip工具类，提供ZIP文件操作的便捷方法
 * @FilePath: \ReportMason\include\KZipUtils.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QMap>

class KZip;
class KArchiveDirectory;
class KArchiveFile;

/**
 * @brief KZip工具类
 * 提供ZIP文件操作的便捷方法，封装KArchive的复杂操作
 */
class KZipUtils
{
public:
    /**
     * @brief 从ZIP文件中读取指定文件的内容
     * @param zipPath ZIP文件路径
     * @param internalPath ZIP内部文件路径
     * @param content 输出内容
     * @return 是否成功
     */
    static bool readFileFromZip(const QString &zipPath, 
                               const QString &internalPath, 
                               QByteArray &content);

    /**
     * @brief 获取ZIP文件中的所有文件列表
     * @param zipPath ZIP文件路径
     * @return 文件路径列表
     */
    static QStringList getFileList(const QString &zipPath);

    /**
     * @brief 检查ZIP文件中是否存在指定文件
     * @param zipPath ZIP文件路径
     * @param internalPath ZIP内部文件路径
     * @return 是否存在
     */
    static bool fileExists(const QString &zipPath, const QString &internalPath);

    /**
     * @brief 创建新的ZIP文件
     * @param zipPath 输出ZIP文件路径
     * @param files 文件映射 (内部路径 -> 内容)
     * @return 是否成功
     */
    static bool createZip(const QString &zipPath, 
                         const QMap<QString, QByteArray> &files);

    /**
     * @brief 复制ZIP文件并替换指定文件
     * @param sourcePath 源ZIP文件路径
     * @param targetPath 目标ZIP文件路径
     * @param replacements 替换文件映射 (内部路径 -> 新内容)
     * @return 是否成功
     */
    static bool copyZipWithReplacements(const QString &sourcePath,
                                       const QString &targetPath,
                                       const QMap<QString, QByteArray> &replacements);

    /**
     * @brief 验证ZIP文件是否有效
     * @param zipPath ZIP文件路径
     * @return 是否有效
     */
    static bool isValidZip(const QString &zipPath);

    /**
     * @brief 获取ZIP文件信息
     * @param zipPath ZIP文件路径
     * @return 文件信息映射 (文件名 -> 大小)
     */
    static QMap<QString, qint64> getZipInfo(const QString &zipPath);

private:
    /**
     * @brief 递归获取目录中的所有文件
     * @param dir 目录
     * @param prefix 路径前缀
     * @return 文件路径列表
     */
    static QStringList getFilesRecursive(const KArchiveDirectory *dir, 
                                        const QString &prefix = QString());
};

