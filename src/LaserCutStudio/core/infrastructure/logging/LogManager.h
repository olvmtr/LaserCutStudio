#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Gestionnaire centralisé du logging
 *
 * Configure le format des messages de log et permet la redirection
 * vers fichier. Singleton thread-safe.
 */
class LogManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Format de sortie des logs
     */
    enum class Format {
        Default,        ///< Format par défaut Qt
        Detailed,       ///< Inclut timestamp, catégorie, fichier:ligne
        Compact,        ///< Format compact (niveau + message)
        Json            ///< Format JSON (pour parsing automatique)
    };
    Q_ENUM(Format)

    /**
     * @brief Obtient l'instance unique (thread-safe)
     */
    static LogManager& instance();

    /**
     * @brief Configure le format des logs
     */
    void setFormat(Format format);

    /**
     * @brief Obtient le format actuel
     */
    Format getFormat() const { return m_format; }

    /**
     * @brief Active la sortie vers fichier
     * @param filePath Chemin du fichier de log
     * @return true si succès
     */
    bool enableFileOutput(const QString& filePath);

    /**
     * @brief Désactive la sortie vers fichier
     */
    void disableFileOutput();

    /**
     * @brief Vérifie si la sortie fichier est active
     */
    bool isFileOutputEnabled() const { return m_fileOutput != nullptr; }

    /**
     * @brief Obtient le chemin du fichier de log
     */
    QString getLogFilePath() const { return m_logFilePath; }

    /**
     * @brief Configure les règles de logging via QT_LOGGING_RULES
     * @param rules Règles au format "category.level=true/false"
     *
     * Exemple:
     * @code
     * setLoggingRules("lasercutstudio.core.shapes.debug=true\n"
     *                 "lasercutstudio.core.*.info=true");
     * @endcode
     */
    void setLoggingRules(const QString& rules);

    /**
     * @brief Initialise le logging avec configuration par défaut
     */
    void initialize();

signals:
    /**
     * @brief Émis quand la configuration du logging change
     */
    void loggingConfigChanged();

private:
    LogManager();
    ~LogManager() override;

    // Interdit copie et affectation (singleton)
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    static void messageHandler(QtMsgType type,
                              const QMessageLogContext& context,
                              const QString& msg);

    static QString formatMessage(QtMsgType type,
                                const QMessageLogContext& context,
                                const QString& msg,
                                Format format);

    static QString levelToString(QtMsgType type);
    static QString timestampString();

    Format m_format;
    QString m_logFilePath;
    QFile* m_fileOutput;
    QTextStream* m_fileStream;

    static LogManager* s_instance;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // LOGMANAGER_H
