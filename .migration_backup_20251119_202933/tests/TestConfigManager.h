#ifndef TESTCONFIGMANAGER_H
#define TESTCONFIGMANAGER_H

#include <QObject>
#include <QtTest/QtTest>

namespace LaserCutStudio {
namespace Tests {

/**
 * @brief Tests pour ConfigManager
 */
class TestConfigManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Tests configuration de base
    void testSingleton();
    void testDefaultValues();
    void testLoadSave();
    void testResetToDefaults();

    // Tests matériaux
    void testGetMaterials();
    void testGetMaterial();
    void testSetMaterial();
    void testUpdateMaterial();
    void testRemoveMaterial();

    // Tests préférences
    void testDefaultUnit();
    void testDisplayPrecision();
    void testDefaultThickness();
    void testDefaultMaterial();

    // Tests chemins
    void testLastExportDirectory();
    void testLastProjectDirectory();

    // Tests signaux
    void testConfigurationChangedSignal();
    void testMaterialsChangedSignal();
};

} // namespace Tests
} // namespace LaserCutStudio

#endif // TESTCONFIGMANAGER_H
