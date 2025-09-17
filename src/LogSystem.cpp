#include "QtCompat.h"
#include "LogSystem.h"
#include <QApplication>
#include <QScrollBar>

LogSystem::LogSystem(QObject *parent)
    : QObject(parent)
    , m_logWidget(nullptr)
    , m_currentLevel(INFO)
{
}

LogSystem::~LogSystem()
{
    if (!m_logFilePath.isEmpty()) {
        saveToFile();
    }
}

void LogSystem::setLogWidget(QTextEdit *widget)
{
    m_logWidget = widget;
}

void LogSystem::setLogFile(const QString &filePath)
{
    m_logFilePath = filePath;
}

void LogSystem::setLogLevel(LogLevel level)
{
    m_currentLevel = level;
}

void LogSystem::log(LogLevel level, const QString &message)
{
    if (level < m_currentLevel) {
        return;
    }

    QMutexLocker locker(&m_mutex);
    
    QString formattedMessage = formatMessage(level, message);
    
    // 输出到控件
    if (m_logWidget) {
        m_logWidget->append(formattedMessage);
        
        // 自动滚动到底部
        QScrollBar *scrollBar = m_logWidget->verticalScrollBar();
        if (scrollBar) {
            scrollBar->setValue(scrollBar->maximum());
        }
    }
    
    // 输出到文件
    if (!m_logFilePath.isEmpty()) {
        QFile file(m_logFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream stream(&file);
            // Qt 6中QTextStream默认使用UTF-8编码，不需要设置setCodec
            stream << formattedMessage << "\n";
            file.close();
        }
    }
    
    // 发送信号
    emit logMessage(formattedMessage);
}

void LogSystem::debug(const QString &message)
{
    log(DEBUG, message);
}

void LogSystem::info(const QString &message)
{
    log(INFO, message);
}

void LogSystem::warning(const QString &message)
{
    log(WARNING, message);
}

void LogSystem::error(const QString &message)
{
    log(ERROR, message);
}

void LogSystem::critical(const QString &message)
{
    log(CRITICAL, message);
}

void LogSystem::clear()
{
    if (m_logWidget) {
        m_logWidget->clear();
    }
}

void LogSystem::saveToFile(const QString &filePath)
{
    QString targetPath = filePath.isEmpty() ? m_logFilePath : filePath;
    if (targetPath.isEmpty()) {
        return;
    }
    
    if (m_logWidget) {
        QFile file(targetPath);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            // Qt 6中QTextStream默认使用UTF-8编码，不需要设置setCodec
            stream << m_logWidget->toPlainText();
            file.close();
        }
    }
}

QString LogSystem::formatMessage(LogLevel level, const QString &message)
{
    QString timestamp = getCurrentTimestamp();
    QString levelStr = levelToString(level);
    return QStringLiteral("[%1] [%2] %3").arg(timestamp, levelStr, message);
}

QString LogSystem::levelToString(LogLevel level)
{
    switch (level) {
        case DEBUG: return QStringLiteral("DEBUG");
        case INFO: return QStringLiteral("INFO");
        case WARNING: return QStringLiteral("WARNING");
        case ERROR: return QStringLiteral("ERROR");
        case CRITICAL: return QStringLiteral("CRITICAL");
        default: return QStringLiteral("UNKNOWN");
    }
}

QString LogSystem::getCurrentTimestamp()
{
    return QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz"));
}
