#ifndef TESTPLUGINMANAGER_H
#define TESTPLUGINMANAGER_H

#include <QObject>
#include <QtTest>
#include "core/infrastructure/plugins/core/PluginManager.h"
#include "core/infrastructure/plugins/interfaces/IShapePlugin.h"
#include "core/infrastructure/plugins/interfaces/IJointPlugin.h"
#include "core/models/shapes/interfaces/IShape.h"
#include "core/models/joints/interfaces/IJoint.h"

using namespace LaserCutStudio::Core;
using namespace LaserCutStudio::Core::Plugins;

/**
 * @class TestPluginManager
 * @brief Tests unitaires pour le PluginManager
 */
class TestPluginManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    // Tests de base
    void testSingleton();
    void testDefaultPaths();
    void testAddPluginPath();

    // Tests de chargement
    void testLoadAllPlugins_noPlugins();
    void testLoadPlugin_invalidFile();
    void testPluginLoadFailed_signal();

    // Tests d'état
    void testLoadedPlugins_empty();
    void testShapePluginCount_zero();
    void testJointPluginCount_zero();
    void testIsPluginLoaded_false();
    void testPluginInfo_notFound();

    // Tests de nettoyage
    void testUnloadAllPlugins();

private:
    int m_pluginLoadedCount = 0;
    int m_pluginFailedCount = 0;
    int m_pluginUnloadedCount = 0;

    void onPluginLoaded(const PluginInfo&) { m_pluginLoadedCount++; }
    void onPluginFailed(const QString&, const QString&) { m_pluginFailedCount++; }
    void onPluginUnloaded(const QString&) { m_pluginUnloadedCount++; }
};

#endif // TESTPLUGINMANAGER_H
