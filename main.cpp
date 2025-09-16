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

#include "src/XmlTestWidget.h"
#include "src/TestWidget.h"
#include "src/DocxContentTestWidget.h"
#include "src/PdfContentTestWidget.h"

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
    QWidget mainWindow;
    mainWindow.setWindowTitle(QStringLiteral("ReportMason - 文档处理工具"));
    mainWindow.setMinimumSize(1200, 800);
    
    // 创建标签页
    QTabWidget *tabWidget = new QTabWidget(&mainWindow);
    
    // 添加XML测试工具标签页
    XmlTestWidget *xmlTestWidget = new XmlTestWidget();
    tabWidget->addTab(xmlTestWidget, QStringLiteral("XML测试工具"));
    
    // 添加测试工具标签页
    TestWidget *testWidget = new TestWidget();
    tabWidget->addTab(testWidget, QStringLiteral("测试工具"));
    
    // 添加DOCX内容提取测试标签页
    DocxContentTestWidget *docxTestWidget = new DocxContentTestWidget();
    tabWidget->addTab(docxTestWidget, QStringLiteral("DOCX内容提取"));
    
    // 添加PDF内容提取测试标签页
    PdfContentTestWidget *pdfTestWidget = new PdfContentTestWidget();
    tabWidget->addTab(pdfTestWidget, QStringLiteral("PDF内容提取"));
    
    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout(&mainWindow);
    layout->addWidget(tabWidget);
    
    mainWindow.show();

    qDebug() << "主窗口已显示";

    return app.exec();
}
