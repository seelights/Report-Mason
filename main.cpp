/*
 * @Author: seelights
 * @Date: 2025-09-14 19:52:21
 * @LastEditTime: 2025-09-17 18:18:56
 * @LastEditors: seelights
 * @Description:
 * @FilePath: \ReportMason\main.cpp
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */
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
#include <QTabWidget>
#include <QProcessEnvironment>

#include "src/QtCompat.h"
#include "src/widgetTest/XmlTestWidget.h"
#include "src/widgetTest/TestWidget.h"
#include "src/widgetTest/DocxContentTestWidget.h"
#include "src/widgetTest/PdfContentTestWidget.h"
#include "src/widgetTest/AiTestWidget.h" // 暂时注释掉AI模块
#include "src/widgetTest/DocxToXmlTestWidget.h"
#include "src/widgetTest/PdfToXmlTestWidget.h"
#include "src/widgetTest/LosslessConverterTestWidget.h"

int main(int argc, char* argv[])
{
    // 设置Qt插件路径环境变量
    QString appDir = QDir::currentPath();
    QString pluginPath = appDir + QS("/platforms");
    
    // 如果本地没有插件，使用MSYS2的插件路径
    if (!QDir(pluginPath).exists()) {
        pluginPath = QS("C:/msys64/mingw64/share/qt6/plugins");
    }
    
    // 使用qputenv设置环境变量
    qputenv("QT_PLUGIN_PATH", pluginPath.toLocal8Bit());
    
    // 修复PATH设置 - 使用QByteArray的正确方法
    QByteArray currentPath = qgetenv("PATH");
    QByteArray newPath = currentPath + ";C:/msys64/mingw64/bin";
    qputenv("PATH", newPath);
    
    qDebug() << "设置QT_PLUGIN_PATH为:" << pluginPath;

    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName(QS("ReportMason"));
    app.setApplicationVersion(QS("1.0.0"));
    app.setOrganizationName(QS("ReportMason"));

    qDebug() << "=== ReportMason 启动 ===";
    qDebug() << "应用程序名称:" << app.applicationName();
    qDebug() << "版本:" << app.applicationVersion();
    qDebug() << "Qt版本:" << QT_VERSION_STR;

    // 创建主窗口
    QWidget mainWindow;
    mainWindow.setWindowTitle(QS("ReportMason - 文档处理工具"));
    mainWindow.setMinimumSize(1200, 800);

    // 创建标签页
    QTabWidget* tabWidget = new QTabWidget(&mainWindow);

    // 添加XML测试工具标签页
    XmlTestWidget* xmlTestWidget = new XmlTestWidget();
    tabWidget->addTab(xmlTestWidget, QStringLiteral("XML测试工具"));

    // 添加测试工具标签页
    TestWidget* testWidget = new TestWidget();
    tabWidget->addTab(testWidget, QStringLiteral("测试工具"));

    // 添加DOCX内容提取测试标签页
    DocxContentTestWidget* docxTestWidget = new DocxContentTestWidget();
    tabWidget->addTab(docxTestWidget, QStringLiteral("DOCX内容提取"));

    // 添加PDF内容提取测试标签页
    PdfContentTestWidget* pdfTestWidget = new PdfContentTestWidget();
    tabWidget->addTab(pdfTestWidget, QStringLiteral("PDF内容提取"));
    // 添加AI功能测试标签页
    AiTestWidget* aiTestWidget = new AiTestWidget();
    tabWidget->addTab(aiTestWidget, QStringLiteral("AI功能测试"));

    // 添加DOCX无损转换测试标签页
    DocxToXmlTestWidget* docxToXmlTestWidget = new DocxToXmlTestWidget();
    tabWidget->addTab(docxToXmlTestWidget, QStringLiteral("DOCX无损转换"));

    // 添加PDF无损转换测试标签页
    PdfToXmlTestWidget* pdfToXmlTestWidget = new PdfToXmlTestWidget();
    tabWidget->addTab(pdfToXmlTestWidget, QStringLiteral("PDF无损转换"));

    // 添加无损转换器测试标签页
    LosslessConverterTestWidget* losslessTestWidget = new LosslessConverterTestWidget();
    tabWidget->addTab(losslessTestWidget, QStringLiteral("无损转换器测试"));

    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(&mainWindow);
    layout->addWidget(tabWidget);

    mainWindow.show();

    qDebug() << "主窗口已显示";

    return app.exec();
}
