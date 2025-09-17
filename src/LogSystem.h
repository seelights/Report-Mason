#pragma once

#include <QObject>
#include <QTextEdit>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class LogSystem : public QObject
{
    Q_OBJECT

public:
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

    explicit LogSystem(QObject *parent = nullptr);
    ~LogSystem();

    // 设置日志输出控件
    void setLogWidget(QTextEdit *widget);
    
    // 设置日志文件
    void setLogFile(const QString &filePath);
    
    // 设置日志级别
    void setLogLevel(LogLevel level);
    
    // 日志输出方法
    void log(LogLevel level, const QString &message);
    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);
    void critical(const QString &message);
    
    // 清空日志
    void clear();
    
    // 保存日志到文件
    void saveToFile(const QString &filePath = QString());

signals:
    void logMessage(const QString &message);

private:
    QTextEdit *m_logWidget;
    QString m_logFilePath;
    LogLevel m_currentLevel;
    QMutex m_mutex;
    
    QString formatMessage(LogLevel level, const QString &message);
    QString levelToString(LogLevel level);
    QString getCurrentTimestamp();
};

