/*
 * @Author: seelights
 * @Date: 2025-01-27
 * @Description: Poppler Qt6 ABI兼容性包装器
 * 解决Qt 6.8.2与Qt 6.9.1编译的Poppler库的ABI不兼容问题
 */

#ifndef POPPLERCOMPAT_H
#define POPPLERCOMPAT_H

#include <QString>
#include <QByteArray>

/**
 * @brief Poppler ABI兼容性包装器
 * 
 * 这个类提供Qt 6.8.2与Qt 6.9.1编译的Poppler库之间的ABI兼容性
 */
class PopplerCompat
{
public:
    /**
     * @brief 检查Poppler库是否可用
     * @return 如果Poppler库可用返回true，否则返回false
     */
    static bool isPopplerAvailable();
    
    /**
     * @brief 获取Poppler版本信息
     * @return Poppler版本字符串
     */
    static QString getPopplerVersion();
    
    /**
     * @brief 检查ABI兼容性
     * @return 如果ABI兼容返回true，否则返回false
     */
    static bool checkABICompatibility();
    
    /**
     * @brief 初始化Poppler兼容性
     * @return 如果初始化成功返回true，否则返回false
     */
    static bool initializeCompatibility();
    
    /**
     * @brief 清理Poppler兼容性资源
     */
    static void cleanupCompatibility();
    
    /**
     * @brief 获取最后的错误信息
     * @return 错误信息字符串
     */
    static QString getLastError();

private:
    static QString s_lastError;
    static bool s_initialized;
    static bool s_popplerAvailable;
};

#endif // POPPLERCOMPAT_H
