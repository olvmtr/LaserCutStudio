#include "LogManager.h"
#include <QDateTime>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QtDebug>

namespace LaserCutStudio {
namespace Core {

LogManager* LogManager::s_instance = nullptr;

LogManager::LogManager()
    : QObject(nullptr)
    , m_format(Format::Default)
    , m_fileOutput(nullptr)
    , m_fileStream(nullptr)
{
    s_instance = this;
}

LogManager::~LogManager()
{
    disableFileOutput();
    s_instance = nullptr;
}

LogManager& LogManager::instance()
{
    static LogManager instance;
    return instance;
}

void LogManager::setFormat(Format format)
{
    if (m_format != format) {
        m_format = format;
        emit loggingConfigChanged();
    }
}

bool LogManager::enableFileOutput(const QString& filePath)
{
    // Désactiver sortie existante
    disableFileOutput();

    // Créer répertoire si nécessaire
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create log directory:" << dir.path();
            return false;
        }
    }

    // Ouvrir fichier
    m_fileOutput = new QFile(filePath);
    if (!m_fileOutput->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << filePath;
        delete m_fileOutput;
        m_fileOutput = nullptr;
        return false;
    }

    m_fileStream = new QTextStream(m_fileOutput);
    m_logFilePath = filePath;

    qInfo() << "Log file output enabled:" << filePath;
    return true;
}

void LogManager::disableFileOutput()
{
    if (m_fileStream) {
        delete m_fileStream;
        m_fileStream = nullptr;
    }

    if (m_fileOutput) {
        m_fileOutput->close();
        delete m_fileOutput;
        m_fileOutput = nullptr;
    }

    m_logFilePath.clear();
}

void LogManager::setLoggingRules(const QString& rules)
{
    QLoggingCategory::setFilterRules(rules);
    emit loggingConfigChanged();
}

void LogManager::initialize()
{
    // Installer le message handler personnalisé
    qInstallMessageHandler(messageHandler);

    // Configuration par défaut : afficher Info et au-dessus
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "*.info=true\n"
        "*.warning=true\n"
        "*.critical=true\n"
        "lasercutstudio.*=true"
    );

    qInfo() << "LogManager initialized";
}

void LogManager::messageHandler(QtMsgType type,
                                const QMessageLogContext& context,
                                const QString& msg)
{
    if (!s_instance) {
        // Fallback si LogManager n'est pas initialisé
        fprintf(stderr, "%s\n", msg.toLocal8Bit().constData());
        return;
    }

    QString formattedMsg = formatMessage(type, context, msg, s_instance->m_format);

    // Sortie console
    fprintf(stderr, "%s\n", formattedMsg.toLocal8Bit().constData());

    // Sortie fichier si activée
    if (s_instance->m_fileStream) {
        (*s_instance->m_fileStream) << formattedMsg << "\n";
        s_instance->m_fileStream->flush();
    }
}

QString LogManager::formatMessage(QtMsgType type,
                                  const QMessageLogContext& context,
                                  const QString& msg,
                                  Format format)
{
    switch (format) {
    case Format::Default:
        // Format Qt par défaut
        return msg;

    case Format::Detailed: {
        // [2024-11-18 22:30:45.123] [INFO] [lasercutstudio.core.shapes] Message (file.cpp:123)
        QString result;
        result += QString("[%1] ").arg(timestampString());
        result += QString("[%1] ").arg(levelToString(type));

        if (context.category) {
            result += QString("[%1] ").arg(context.category);
        }

        result += msg;

        if (context.file && context.line > 0) {
            result += QString(" (%1:%2)").arg(context.file).arg(context.line);
        }

        return result;
    }

    case Format::Compact: {
        // INFO: Message
        return QString("%1: %2").arg(levelToString(type)).arg(msg);
    }

    case Format::Json: {
        // {"timestamp":"2024-11-18T22:30:45","level":"INFO","category":"...","message":"..."}
        QJsonObject obj;
        obj["timestamp"] = timestampString();
        obj["level"] = levelToString(type);

        if (context.category) {
            obj["category"] = QString(context.category);
        }

        obj["message"] = msg;

        if (context.file && context.line > 0) {
            obj["file"] = QString(context.file);
            obj["line"] = context.line;
        }

        QJsonDocument doc(obj);
        return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    }
    }

    return msg;
}

QString LogManager::levelToString(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:    return "DEBUG";
    case QtInfoMsg:     return "INFO";
    case QtWarningMsg:  return "WARNING";
    case QtCriticalMsg: return "CRITICAL";
    case QtFatalMsg:    return "FATAL";
    default:            return "UNKNOWN";
    }
}

QString LogManager::timestampString()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
}

} // namespace Core
} // namespace LaserCutStudio
