/*
 * @Author: seelights
 * @Date: 2025-01-27
 * @Description: Poppler Qt6 ABI兼容性包装器实现
 */

#include "PopplerCompat.h"
#include "QtCompat.h"
#include <QDebug>
#include <QLibrary>
#include <QCoreApplication>

// 静态成员变量定义
QString PopplerCompat::s_lastError;
bool PopplerCompat::s_initialized = false;
bool PopplerCompat::s_popplerAvailable = false;

bool PopplerCompat::isPopplerAvailable()
{
    if (!s_initialized) {
        initializeCompatibility();
    }
    return s_popplerAvailable;
}

QString PopplerCompat::getPopplerVersion()
{
    if (!isPopplerAvailable()) {
        return QS("Poppler不可用");
    }

    // 尝试获取Poppler版本信息
    QLibrary popplerLib(QS("poppler-qt6-3"));
    if (popplerLib.load()) {
        return QS("Poppler Qt6 (版本3)");
    }

    return QS("未知版本");
}

bool PopplerCompat::checkABICompatibility()
{
    if (!isPopplerAvailable()) {
        s_lastError = QS("Poppler库不可用");
        return false;
    }

    // 检查关键的Qt符号是否可用
    QLibrary popplerLib(QS("poppler-qt6-3"));
    if (!popplerLib.load()) {
        s_lastError = QS("无法加载Poppler Qt6库");
        return false;
    }

    // 检查Qt 6.8.2的关键符号
    // 注意：这里我们检查符号是否存在，而不是调用它们
    // 这样可以避免ABI不兼容导致的崩溃

    // 尝试解析一些基本的Poppler符号
    auto* symbol1 = popplerLib.resolve("poppler_document_new_from_file");
    auto* symbol2 = popplerLib.resolve("poppler_document_get_n_pages");

    if (!symbol1 || !symbol2) {
        s_lastError = QS("Poppler库符号解析失败，可能存在ABI不兼容");
        popplerLib.unload();
        return false;
    }

    popplerLib.unload();
    return true;
}

bool PopplerCompat::initializeCompatibility()
{
    if (s_initialized) {
        return s_popplerAvailable;
    }

    s_initialized = true;

    // 检查Poppler库文件是否存在
    QStringList popplerPaths = {
        QStringLiteral("C:/msys64/mingw64/bin/libpoppler-qt6-3.dll"),
        QStringLiteral("libpoppler-qt6-3.dll"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/libpoppler-qt6-3.dll")};

    bool found = false;
    for (const QString& path : popplerPaths) {
        QLibrary lib(path);
        if (lib.load()) {
            found = true;
            lib.unload();
            break;
        }
    }

    if (!found) {
        s_lastError = QStringLiteral("找不到Poppler Qt6库文件");
        s_popplerAvailable = false;
        return false;
    }

    // 检查ABI兼容性
    if (!checkABICompatibility()) {
        s_popplerAvailable = false;
        return false;
    }

    s_popplerAvailable = true;
    qDebug() << "PopplerCompat: Poppler库初始化成功";
    return true;
}

void PopplerCompat::cleanupCompatibility()
{
    s_initialized = false;
    s_popplerAvailable = false;
    s_lastError.clear();
}

QString PopplerCompat::getLastError() { return s_lastError; }
