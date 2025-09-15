/*
 * main.cpp
 * ReportMason 主程序
 *
 * 功能说明：
 * 1. 启动Qt应用程序
 * 2. 显示主界面
 * 3. 提供文档转换和字段提取功能
 * 4. 支持多种文件格式处理
 */

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "Widget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("ReportMason");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ReportMason");

    qDebug() << "=== ReportMason 启动 ===";
    qDebug() << "应用程序名称:" << app.applicationName();
    qDebug() << "版本:" << app.applicationVersion();
    qDebug() << "Qt版本:" << QT_VERSION_STR;

    // 创建主窗口
    Widget window;
    window.show();

    qDebug() << "主窗口已显示";

    return app.exec();
}
