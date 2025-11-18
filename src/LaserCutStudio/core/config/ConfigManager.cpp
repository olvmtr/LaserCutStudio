#include "ConfigManager.h"
#include <QStandardPaths>
#include <QDir>

namespace LaserCutStudio {
namespace Core {

ConfigManager::ConfigManager()
    : QObject(nullptr)
    , m_settings("LaserCutStudio", "LaserCutStudio")
{
    loadDefaults();
}

ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

void ConfigManager::load()
{
    // Charger préférences générales
    m_defaultUnit = static_cast<Unit>(
        m_settings.value("general/defaultUnit", static_cast<int>(Unit::Millimeters)).toInt()
    );
    m_displayPrecision = m_settings.value("general/displayPrecision", 2).toInt();
    m_defaultThickness = m_settings.value("general/defaultThickness", 3.0).toDouble();
    m_defaultMaterial = m_settings.value("general/defaultMaterial", "Plywood").toString();

    // Charger chemins
    m_lastExportDirectory = m_settings.value(
        "paths/lastExportDirectory",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    ).toString();

    m_lastProjectDirectory = m_settings.value(
        "paths/lastProjectDirectory",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    ).toString();

    // Charger matériaux
    loadMaterials();

    emit configurationChanged();
}

void ConfigManager::save()
{
    // Sauvegarder préférences générales
    m_settings.setValue("general/defaultUnit", static_cast<int>(m_defaultUnit));
    m_settings.setValue("general/displayPrecision", m_displayPrecision);
    m_settings.setValue("general/defaultThickness", m_defaultThickness);
    m_settings.setValue("general/defaultMaterial", m_defaultMaterial);

    // Sauvegarder chemins
    m_settings.setValue("paths/lastExportDirectory", m_lastExportDirectory);
    m_settings.setValue("paths/lastProjectDirectory", m_lastProjectDirectory);

    // Sauvegarder matériaux
    saveMaterials();

    m_settings.sync();
}

void ConfigManager::resetToDefaults()
{
    m_settings.clear();
    loadDefaults();
    emit configurationChanged();
    emit materialsChanged();
}

void ConfigManager::loadDefaults()
{
    // Valeurs par défaut
    m_defaultUnit = Unit::Millimeters;
    m_displayPrecision = 2;
    m_defaultThickness = 3.0;
    m_defaultMaterial = "Plywood";

    m_lastExportDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_lastProjectDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    // Matériaux par défaut
    m_materials.clear();
    m_materials.append(Material::Wood());
    m_materials.append(Material::Plywood());
    m_materials.append(Material::MDF());
    m_materials.append(Material::Acrylic());
    m_materials.append(Material::Cardboard());
}

void ConfigManager::loadMaterials()
{
    m_materials.clear();

    int size = m_settings.beginReadArray("materials");

    if (size == 0) {
        // Pas de matériaux sauvegardés, utiliser les défauts
        m_settings.endArray();
        loadDefaults();
        return;
    }

    for (int i = 0; i < size; ++i) {
        m_settings.setArrayIndex(i);

        QString name = m_settings.value("name").toString();
        double density = m_settings.value("density").toDouble();
        QColor color = m_settings.value("color").value<QColor>();

        m_materials.append(Material(name, density, color));
    }

    m_settings.endArray();
    emit materialsChanged();
}

void ConfigManager::saveMaterials()
{
    m_settings.beginWriteArray("materials");

    for (int i = 0; i < m_materials.size(); ++i) {
        m_settings.setArrayIndex(i);

        const Material& mat = m_materials[i];
        m_settings.setValue("name", mat.getName());
        m_settings.setValue("density", mat.getDensity());
        m_settings.setValue("color", mat.getColor());
    }

    m_settings.endArray();
}

QList<Material> ConfigManager::getMaterials() const
{
    return m_materials;
}

Material ConfigManager::getMaterial(const QString& name) const
{
    for (const Material& mat : m_materials) {
        if (mat.getName() == name) {
            return mat;
        }
    }

    // Retourner matériau par défaut si non trouvé
    return Material();
}

void ConfigManager::setMaterial(const Material& material)
{
    // Chercher si le matériau existe déjà
    for (int i = 0; i < m_materials.size(); ++i) {
        if (m_materials[i].getName() == material.getName()) {
            m_materials[i] = material;
            emit materialsChanged();
            return;
        }
    }

    // Ajouter nouveau matériau
    m_materials.append(material);
    emit materialsChanged();
}

void ConfigManager::removeMaterial(const QString& name)
{
    for (int i = 0; i < m_materials.size(); ++i) {
        if (m_materials[i].getName() == name) {
            m_materials.removeAt(i);
            emit materialsChanged();
            return;
        }
    }
}

void ConfigManager::setDefaultUnit(Unit unit)
{
    if (m_defaultUnit != unit) {
        m_defaultUnit = unit;
        emit configurationChanged();
    }
}

void ConfigManager::setDisplayPrecision(int precision)
{
    if (m_displayPrecision != precision) {
        m_displayPrecision = precision;
        emit configurationChanged();
    }
}

void ConfigManager::setDefaultThickness(double thickness)
{
    if (m_defaultThickness != thickness) {
        m_defaultThickness = thickness;
        emit configurationChanged();
    }
}

void ConfigManager::setDefaultMaterial(const QString& material)
{
    if (m_defaultMaterial != material) {
        m_defaultMaterial = material;
        emit configurationChanged();
    }
}

void ConfigManager::setLastExportDirectory(const QString& dir)
{
    if (m_lastExportDirectory != dir) {
        m_lastExportDirectory = dir;
        // Pas besoin d'émettre configurationChanged pour les chemins
    }
}

void ConfigManager::setLastProjectDirectory(const QString& dir)
{
    if (m_lastProjectDirectory != dir) {
        m_lastProjectDirectory = dir;
    }
}

} // namespace Core
} // namespace LaserCutStudio
