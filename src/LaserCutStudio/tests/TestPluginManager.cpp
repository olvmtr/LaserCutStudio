#include "TestPluginManager.h"

void TestPluginManager::initTestCase()
{
    // Connecter les signaux pour tester
    PluginManager& manager = PluginManager::instance();
    connect(&manager, &PluginManager::pluginLoaded,
            this, &TestPluginManager::onPluginLoaded);
    connect(&manager, &PluginManager::pluginLoadFailed,
            this, &TestPluginManager::onPluginFailed);
    connect(&manager, &PluginManager::pluginUnloaded,
            this, &TestPluginManager::onPluginUnloaded);
}

void TestPluginManager::cleanupTestCase()
{
    // Décharger tous les plugins
    PluginManager::instance().unloadAllPlugins();
}

void TestPluginManager::cleanup()
{
    // Réinitialiser les compteurs après chaque test
    m_pluginLoadedCount = 0;
    m_pluginFailedCount = 0;
    m_pluginUnloadedCount = 0;

    // Décharger tous les plugins
    PluginManager::instance().unloadAllPlugins();
}

void TestPluginManager::testSingleton()
{
    PluginManager& manager1 = PluginManager::instance();
    PluginManager& manager2 = PluginManager::instance();

    QCOMPARE(&manager1, &manager2);
}

void TestPluginManager::testDefaultPaths()
{
    PluginManager& manager = PluginManager::instance();
    QStringList paths = manager.pluginPaths();

    QVERIFY(!paths.isEmpty());
    QVERIFY(paths.size() >= 1);  // Au moins ./plugins/

    // Vérifier que les chemins contiennent "plugins"
    for (const QString& path : paths) {
        QVERIFY(path.contains("plugins"));
    }
}

void TestPluginManager::testAddPluginPath()
{
    PluginManager& manager = PluginManager::instance();

    int initialCount = manager.pluginPaths().size();
    manager.addPluginPath("/custom/path/to/plugins");

    QStringList paths = manager.pluginPaths();
    QCOMPARE(paths.size(), initialCount + 1);
    QVERIFY(paths.contains("/custom/path/to/plugins"));

    // Ajouter le même chemin ne doit pas le dupliquer
    manager.addPluginPath("/custom/path/to/plugins");
    QCOMPARE(manager.pluginPaths().size(), initialCount + 1);
}

void TestPluginManager::testLoadAllPlugins_noPlugins()
{
    PluginManager& manager = PluginManager::instance();

    // Les répertoires par défaut n'ont probablement pas de plugins
    int loaded = manager.loadAllPlugins();

    // Peut être 0 si pas de plugins, c'est normal
    QVERIFY(loaded >= 0);
    QCOMPARE(loaded, manager.shapePluginCount() + manager.jointPluginCount());
}

void TestPluginManager::testLoadPlugin_invalidFile()
{
    PluginManager& manager = PluginManager::instance();

    bool result = manager.loadPlugin("/invalid/path/to/plugin.so");

    QVERIFY(!result);
    QVERIFY(m_pluginFailedCount > 0);  // Signal pluginLoadFailed émis
}

void TestPluginManager::testPluginLoadFailed_signal()
{
    PluginManager& manager = PluginManager::instance();

    m_pluginFailedCount = 0;
    manager.loadPlugin("/nonexistent/plugin.so");

    QVERIFY(m_pluginFailedCount > 0);
}

void TestPluginManager::testLoadedPlugins_empty()
{
    PluginManager& manager = PluginManager::instance();

    QVector<PluginInfo> plugins = manager.loadedPlugins();

    QCOMPARE(plugins.size(), 0);
}

void TestPluginManager::testShapePluginCount_zero()
{
    PluginManager& manager = PluginManager::instance();

    QCOMPARE(manager.shapePluginCount(), 0);
}

void TestPluginManager::testJointPluginCount_zero()
{
    PluginManager& manager = PluginManager::instance();

    QCOMPARE(manager.jointPluginCount(), 0);
}

void TestPluginManager::testIsPluginLoaded_false()
{
    PluginManager& manager = PluginManager::instance();

    QVERIFY(!manager.isPluginLoaded("NonExistentPlugin"));
}

void TestPluginManager::testPluginInfo_notFound()
{
    PluginManager& manager = PluginManager::instance();

    PluginInfo info = manager.pluginInfo("NonExistent");

    QVERIFY(info.pluginName.isEmpty());
    QVERIFY(!info.isLoaded);
}

void TestPluginManager::testUnloadAllPlugins()
{
    PluginManager& manager = PluginManager::instance();

    // Tenter de charger des plugins (peut échouer si pas de plugins)
    manager.loadAllPlugins();

    m_pluginUnloadedCount = 0;
    manager.unloadAllPlugins();

    QCOMPARE(manager.shapePluginCount(), 0);
    QCOMPARE(manager.jointPluginCount(), 0);
    QCOMPARE(manager.loadedPlugins().size(), 0);
}
