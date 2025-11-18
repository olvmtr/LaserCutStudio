#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariantMap>
#include <QList>
#include "../types/Material.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Gestionnaire de configuration centralisé pour l'application
 *
 * Utilise QSettings pour persister la configuration utilisateur.
 * Singleton thread-safe pour accès global.
 */
class ConfigManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Unités de mesure supportées
     */
    enum class Unit {
        Millimeters,
        Centimeters,
        Inches
    };
    Q_ENUM(Unit)

    /**
     * @brief Obtient l'instance unique (thread-safe)
     */
    static ConfigManager& instance();

    /**
     * @brief Charge la configuration depuis QSettings
     */
    void load();

    /**
     * @brief Sauvegarde la configuration vers QSettings
     */
    void save();

    /**
     * @brief Réinitialise aux valeurs par défaut
     */
    void resetToDefaults();

    // === Matériaux ===

    /**
     * @brief Obtient tous les matériaux disponibles
     */
    QList<Material> getMaterials() const;

    /**
     * @brief Obtient un matériau par nom
     */
    Material getMaterial(const QString& name) const;

    /**
     * @brief Ajoute ou met à jour un matériau
     */
    void setMaterial(const Material& material);

    /**
     * @brief Supprime un matériau personnalisé
     */
    void removeMaterial(const QString& name);

    // === Préférences générales ===

    /**
     * @brief Obtient l'unité par défaut
     */
    Unit getDefaultUnit() const { return m_defaultUnit; }

    /**
     * @brief Définit l'unité par défaut
     */
    void setDefaultUnit(Unit unit);

    /**
     * @brief Obtient la précision d'affichage (nombre de décimales)
     */
    int getDisplayPrecision() const { return m_displayPrecision; }

    /**
     * @brief Définit la précision d'affichage
     */
    void setDisplayPrecision(int precision);

    /**
     * @brief Obtient l'épaisseur par défaut (mm)
     */
    double getDefaultThickness() const { return m_defaultThickness; }

    /**
     * @brief Définit l'épaisseur par défaut
     */
    void setDefaultThickness(double thickness);

    /**
     * @brief Obtient le matériau par défaut
     */
    QString getDefaultMaterial() const { return m_defaultMaterial; }

    /**
     * @brief Définit le matériau par défaut
     */
    void setDefaultMaterial(const QString& material);

    // === Chemins ===

    /**
     * @brief Obtient le dernier répertoire d'export utilisé
     */
    QString getLastExportDirectory() const { return m_lastExportDirectory; }

    /**
     * @brief Définit le dernier répertoire d'export
     */
    void setLastExportDirectory(const QString& dir);

    /**
     * @brief Obtient le dernier répertoire de projet utilisé
     */
    QString getLastProjectDirectory() const { return m_lastProjectDirectory; }

    /**
     * @brief Définit le dernier répertoire de projet
     */
    void setLastProjectDirectory(const QString& dir);

signals:
    /**
     * @brief Émis quand la configuration change
     */
    void configurationChanged();

    /**
     * @brief Émis quand les matériaux changent
     */
    void materialsChanged();

private:
    ConfigManager();
    ~ConfigManager() override = default;

    // Interdit copie et affectation (singleton)
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void loadDefaults();
    void loadMaterials();
    void saveMaterials();

    QSettings m_settings;
    QList<Material> m_materials;

    // Préférences
    Unit m_defaultUnit;
    int m_displayPrecision;
    double m_defaultThickness;
    QString m_defaultMaterial;
    QString m_lastExportDirectory;
    QString m_lastProjectDirectory;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // CONFIGMANAGER_H
