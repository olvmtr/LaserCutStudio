#include "TestConfigManager.h"
#include "../core/config/ConfigManager.h"
#include "../core/types/Material.h"
#include <QSignalSpy>
#include <QSettings>

using namespace LaserCutStudio::Core;

namespace LaserCutStudio {
namespace Tests {

void TestConfigManager::initTestCase()
{
    // Utiliser un fichier de configuration de test
    QCoreApplication::setOrganizationName("LaserCutStudioTest");
    QCoreApplication::setApplicationName("LaserCutStudioTest");
}

void TestConfigManager::cleanupTestCase()
{
    // Nettoyer la configuration de test
    QSettings settings("LaserCutStudioTest", "LaserCutStudioTest");
    settings.clear();
}

void TestConfigManager::init()
{
    // Réinitialiser avant chaque test
    ConfigManager::instance().resetToDefaults();
}

void TestConfigManager::cleanup()
{
    // Nettoyage après chaque test
}

void TestConfigManager::testSingleton()
{
    // Vérifier que l'instance est toujours la même
    ConfigManager& instance1 = ConfigManager::instance();
    ConfigManager& instance2 = ConfigManager::instance();

    QCOMPARE(&instance1, &instance2);
}

void TestConfigManager::testDefaultValues()
{
    ConfigManager& config = ConfigManager::instance();

    // Vérifier les valeurs par défaut
    QCOMPARE(config.getDefaultUnit(), ConfigManager::Unit::Millimeters);
    QCOMPARE(config.getDisplayPrecision(), 2);
    QCOMPARE(config.getDefaultThickness(), 3.0);
    QCOMPARE(config.getDefaultMaterial(), QString("Plywood"));

    // Vérifier que les matériaux par défaut sont présents
    QList<Material> materials = config.getMaterials();
    QCOMPARE(materials.size(), 5);

    QStringList materialNames;
    for (const Material& mat : materials) {
        materialNames.append(mat.getName());
    }

    QVERIFY(materialNames.contains("Wood"));
    QVERIFY(materialNames.contains("Plywood"));
    QVERIFY(materialNames.contains("MDF"));
    QVERIFY(materialNames.contains("Acrylic"));
    QVERIFY(materialNames.contains("Cardboard"));
}

void TestConfigManager::testLoadSave()
{
    ConfigManager& config = ConfigManager::instance();

    // Modifier des valeurs
    config.setDefaultUnit(ConfigManager::Unit::Inches);
    config.setDisplayPrecision(3);
    config.setDefaultThickness(6.0);
    config.setDefaultMaterial("Wood");

    // Sauvegarder
    config.save();

    // Réinitialiser
    config.resetToDefaults();
    QCOMPARE(config.getDefaultUnit(), ConfigManager::Unit::Millimeters);

    // Charger
    config.load();

    // Vérifier que les valeurs sont restaurées
    QCOMPARE(config.getDefaultUnit(), ConfigManager::Unit::Inches);
    QCOMPARE(config.getDisplayPrecision(), 3);
    QCOMPARE(config.getDefaultThickness(), 6.0);
    QCOMPARE(config.getDefaultMaterial(), QString("Wood"));
}

void TestConfigManager::testResetToDefaults()
{
    ConfigManager& config = ConfigManager::instance();

    // Modifier des valeurs
    config.setDefaultUnit(ConfigManager::Unit::Centimeters);
    config.setDisplayPrecision(4);

    // Réinitialiser
    config.resetToDefaults();

    // Vérifier que les valeurs par défaut sont restaurées
    QCOMPARE(config.getDefaultUnit(), ConfigManager::Unit::Millimeters);
    QCOMPARE(config.getDisplayPrecision(), 2);
}

void TestConfigManager::testGetMaterials()
{
    ConfigManager& config = ConfigManager::instance();
    QList<Material> materials = config.getMaterials();

    QCOMPARE(materials.size(), 5);
    QCOMPARE(materials[0].getName(), QString("Wood"));
}

void TestConfigManager::testGetMaterial()
{
    ConfigManager& config = ConfigManager::instance();

    Material plywood = config.getMaterial("Plywood");
    QCOMPARE(plywood.getName(), QString("Plywood"));
    QCOMPARE(plywood.getDensity(), 550.0);

    // Matériau inexistant
    Material unknown = config.getMaterial("NonExistent");
    QCOMPARE(unknown.getName(), QString("Unknown"));
}

void TestConfigManager::testSetMaterial()
{
    ConfigManager& config = ConfigManager::instance();

    // Ajouter un nouveau matériau
    Material custom("Custom", 800.0, QColor(255, 0, 0));
    config.setMaterial(custom);

    QList<Material> materials = config.getMaterials();
    QCOMPARE(materials.size(), 6);

    Material retrieved = config.getMaterial("Custom");
    QCOMPARE(retrieved.getName(), QString("Custom"));
    QCOMPARE(retrieved.getDensity(), 800.0);
    QCOMPARE(retrieved.getColor(), QColor(255, 0, 0));
}

void TestConfigManager::testUpdateMaterial()
{
    ConfigManager& config = ConfigManager::instance();

    // Modifier un matériau existant
    Material modifiedPlywood("Plywood", 600.0, QColor(255, 255, 0));
    config.setMaterial(modifiedPlywood);

    // Vérifier que la taille n'a pas changé
    QList<Material> materials = config.getMaterials();
    QCOMPARE(materials.size(), 5);

    // Vérifier que les valeurs sont mises à jour
    Material retrieved = config.getMaterial("Plywood");
    QCOMPARE(retrieved.getDensity(), 600.0);
    QCOMPARE(retrieved.getColor(), QColor(255, 255, 0));
}

void TestConfigManager::testRemoveMaterial()
{
    ConfigManager& config = ConfigManager::instance();

    // Supprimer un matériau
    config.removeMaterial("Cardboard");

    QList<Material> materials = config.getMaterials();
    QCOMPARE(materials.size(), 4);

    Material retrieved = config.getMaterial("Cardboard");
    QCOMPARE(retrieved.getName(), QString("Unknown"));
}

void TestConfigManager::testDefaultUnit()
{
    ConfigManager& config = ConfigManager::instance();

    config.setDefaultUnit(ConfigManager::Unit::Centimeters);
    QCOMPARE(config.getDefaultUnit(), ConfigManager::Unit::Centimeters);

    config.setDefaultUnit(ConfigManager::Unit::Inches);
    QCOMPARE(config.getDefaultUnit(), ConfigManager::Unit::Inches);
}

void TestConfigManager::testDisplayPrecision()
{
    ConfigManager& config = ConfigManager::instance();

    config.setDisplayPrecision(4);
    QCOMPARE(config.getDisplayPrecision(), 4);

    config.setDisplayPrecision(1);
    QCOMPARE(config.getDisplayPrecision(), 1);
}

void TestConfigManager::testDefaultThickness()
{
    ConfigManager& config = ConfigManager::instance();

    config.setDefaultThickness(5.5);
    QCOMPARE(config.getDefaultThickness(), 5.5);

    config.setDefaultThickness(10.0);
    QCOMPARE(config.getDefaultThickness(), 10.0);
}

void TestConfigManager::testDefaultMaterial()
{
    ConfigManager& config = ConfigManager::instance();

    config.setDefaultMaterial("Acrylic");
    QCOMPARE(config.getDefaultMaterial(), QString("Acrylic"));

    config.setDefaultMaterial("Wood");
    QCOMPARE(config.getDefaultMaterial(), QString("Wood"));
}

void TestConfigManager::testLastExportDirectory()
{
    ConfigManager& config = ConfigManager::instance();

    QString testDir = "/tmp/test_export";
    config.setLastExportDirectory(testDir);
    QCOMPARE(config.getLastExportDirectory(), testDir);
}

void TestConfigManager::testLastProjectDirectory()
{
    ConfigManager& config = ConfigManager::instance();

    QString testDir = "/tmp/test_project";
    config.setLastProjectDirectory(testDir);
    QCOMPARE(config.getLastProjectDirectory(), testDir);
}

void TestConfigManager::testConfigurationChangedSignal()
{
    ConfigManager& config = ConfigManager::instance();
    QSignalSpy spy(&config, &ConfigManager::configurationChanged);

    config.setDefaultUnit(ConfigManager::Unit::Inches);
    QCOMPARE(spy.count(), 1);

    config.setDisplayPrecision(5);
    QCOMPARE(spy.count(), 2);

    // Définir la même valeur ne doit pas émettre de signal
    config.setDisplayPrecision(5);
    QCOMPARE(spy.count(), 2);
}

void TestConfigManager::testMaterialsChangedSignal()
{
    ConfigManager& config = ConfigManager::instance();
    QSignalSpy spy(&config, &ConfigManager::materialsChanged);

    Material custom("Custom", 800.0, QColor(255, 0, 0));
    config.setMaterial(custom);
    QCOMPARE(spy.count(), 1);

    config.removeMaterial("Custom");
    QCOMPARE(spy.count(), 2);
}

} // namespace Tests
} // namespace LaserCutStudio
