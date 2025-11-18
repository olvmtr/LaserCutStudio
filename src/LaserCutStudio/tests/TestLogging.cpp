#include "TestLogging.h"
#include "../core/logging/LogManager.h"
#include "../core/logging/LogCategories.h"
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

using namespace LaserCutStudio::Core;

namespace LaserCutStudio {
namespace Tests {

void TestLogging::initTestCase()
{
    // Initialiser le LogManager
    LogManager::instance().initialize();
}

void TestLogging::cleanupTestCase()
{
    LogManager::instance().disableFileOutput();
}

void TestLogging::init()
{
    // Réinitialiser avant chaque test
    LogManager::instance().setFormat(LogManager::Format::Default);
    LogManager::instance().disableFileOutput();
}

void TestLogging::cleanup()
{
    // Nettoyage après chaque test
}

void TestLogging::testSingleton()
{
    LogManager& instance1 = LogManager::instance();
    LogManager& instance2 = LogManager::instance();

    QCOMPARE(&instance1, &instance2);
}

void TestLogging::testFormatSelection()
{
    LogManager& manager = LogManager::instance();

    // Tester chaque format
    manager.setFormat(LogManager::Format::Default);
    QCOMPARE(manager.getFormat(), LogManager::Format::Default);

    manager.setFormat(LogManager::Format::Detailed);
    QCOMPARE(manager.getFormat(), LogManager::Format::Detailed);

    manager.setFormat(LogManager::Format::Compact);
    QCOMPARE(manager.getFormat(), LogManager::Format::Compact);

    manager.setFormat(LogManager::Format::Json);
    QCOMPARE(manager.getFormat(), LogManager::Format::Json);
}

void TestLogging::testFileOutput()
{
    LogManager& manager = LogManager::instance();

    // Créer un répertoire temporaire
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString logPath = tempDir.path() + "/test.log";

    // Activer la sortie fichier
    QVERIFY(manager.enableFileOutput(logPath));
    QVERIFY(manager.isFileOutputEnabled());
    QCOMPARE(manager.getLogFilePath(), logPath);

    // Écrire un message
    qInfo() << "Test message to file";

    // Désactiver et vérifier que le fichier existe
    manager.disableFileOutput();
    QVERIFY(!manager.isFileOutputEnabled());

    QFile logFile(logPath);
    QVERIFY(logFile.exists());
    QVERIFY(logFile.size() > 0);

    // Vérifier le contenu
    QVERIFY(logFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = QTextStream(&logFile).readAll();
    QVERIFY(content.contains("Test message to file"));
    logFile.close();
}

void TestLogging::testLoggingRules()
{
    LogManager& manager = LogManager::instance();

    // Configurer les règles
    QString rules = "lasercutstudio.core.shapes.debug=true\n"
                    "lasercutstudio.core.parts.info=true";

    manager.setLoggingRules(rules);

    // Vérifier que les catégories sont activées
    QVERIFY(logShapes().isDebugEnabled());
    QVERIFY(logParts().isInfoEnabled());
}

void TestLogging::testMessageFormatting()
{
    LogManager& manager = LogManager::instance();

    // Créer un répertoire temporaire pour les logs
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    // Tester format Detailed
    QString logPathDetailed = tempDir.path() + "/detailed.log";
    manager.setFormat(LogManager::Format::Detailed);
    manager.enableFileOutput(logPathDetailed);

    qInfo() << "Detailed format test";

    manager.disableFileOutput();

    QFile fileDetailed(logPathDetailed);
    QVERIFY(fileDetailed.open(QIODevice::ReadOnly | QIODevice::Text));
    QString contentDetailed = QTextStream(&fileDetailed).readAll();
    QVERIFY(contentDetailed.contains("[INFO]"));
    QVERIFY(contentDetailed.contains("Detailed format test"));
    fileDetailed.close();

    // Tester format Compact
    QString logPathCompact = tempDir.path() + "/compact.log";
    manager.setFormat(LogManager::Format::Compact);
    manager.enableFileOutput(logPathCompact);

    qInfo() << "Compact format test";

    manager.disableFileOutput();

    QFile fileCompact(logPathCompact);
    QVERIFY(fileCompact.open(QIODevice::ReadOnly | QIODevice::Text));
    QString contentCompact = QTextStream(&fileCompact).readAll();
    QVERIFY(contentCompact.contains("INFO:"));
    QVERIFY(contentCompact.contains("Compact format test"));
    fileCompact.close();

    // Tester format JSON
    QString logPathJson = tempDir.path() + "/json.log";
    manager.setFormat(LogManager::Format::Json);
    manager.enableFileOutput(logPathJson);

    qInfo() << "JSON format test";

    manager.disableFileOutput();

    QFile fileJson(logPathJson);
    QVERIFY(fileJson.open(QIODevice::ReadOnly | QIODevice::Text));
    QString contentJson = QTextStream(&fileJson).readAll();
    QVERIFY(contentJson.contains("\"level\":\"INFO\""));
    QVERIFY(contentJson.contains("\"message\":\"JSON format test\""));
    fileJson.close();
}

void TestLogging::testCategories()
{
    // Vérifier que les catégories sont définies
    QVERIFY(!QString(logCore().categoryName()).isEmpty());
    QVERIFY(!QString(logShapes().categoryName()).isEmpty());
    QVERIFY(!QString(logParts().categoryName()).isEmpty());
    QVERIFY(!QString(logJoints().categoryName()).isEmpty());
    QVERIFY(!QString(logProjects().categoryName()).isEmpty());
    QVERIFY(!QString(logConfig().categoryName()).isEmpty());
    QVERIFY(!QString(logPerformance().categoryName()).isEmpty());
}

void TestLogging::testCategoryFiltering()
{
    // Activer uniquement les messages de shapes
    LogManager::instance().setLoggingRules(
        "*.debug=false\n"
        "lasercutstudio.core.shapes.debug=true\n"
        "lasercutstudio.core.parts.debug=false"
    );

    // Vérifier que seul shapes.debug est activé
    QVERIFY(logShapes().isDebugEnabled());
    QVERIFY(!logParts().isDebugEnabled());

    // Les niveaux info/warning/critical sont toujours actifs par défaut
    QVERIFY(logParts().isInfoEnabled());
    QVERIFY(logParts().isWarningEnabled());
}

} // namespace Tests
} // namespace LaserCutStudio
