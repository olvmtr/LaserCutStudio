#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QPluginLoader>
#include <QDir>
#include "core/infrastructure/plugins/interfaces/IShapePlugin.h"
#include "core/infrastructure/plugins/interfaces/IJointPlugin.h"
#include "core/infrastructure/logging/LogCategories.h"

namespace LaserCutStudio {
namespace Core {
namespace Plugins {

/**
 * @struct PluginInfo
 * @brief Informations sur un plugin chargé
 */
struct PluginInfo
{
    QString fileName;       ///< Nom du fichier du plugin
    QString pluginName;     ///< Nom du plugin (shapeName ou jointName)
    QString description;    ///< Description du plugin
    QString version;        ///< Version du plugin
    QString author;         ///< Auteur du plugin
    QString type;           ///< Type: "Shape" ou "Joint"
    bool isLoaded;          ///< État de chargement

    /**
     * @brief Convertit les informations en QVariantMap
     */
    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        map["fileName"] = fileName;
        map["pluginName"] = pluginName;
        map["description"] = description;
        map["version"] = version;
        map["author"] = author;
        map["type"] = type;
        map["isLoaded"] = isLoaded;
        return map;
    }
};

/**
 * @class PluginManager
 * @brief Gestionnaire de plugins pour formes et joints
 *
 * Singleton responsable de :
 * - Découvrir les plugins dans les répertoires configurés
 * - Charger et décharger les plugins dynamiquement
 * - Enregistrer les plugins dans les Factories appropriées
 * - Gérer les erreurs de chargement
 *
 * Répertoires de plugins par défaut :
 * - ./plugins/ (relatif à l'exécutable)
 * - ~/.config/LaserCutStudio/plugins/ (Linux/macOS)
 * - %APPDATA%/LaserCutStudio/plugins/ (Windows)
 *
 * @note Thread-safe (Singleton avec Meyers)
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Accès à l'instance unique (Singleton)
     */
    static PluginManager& instance();

    // Désactiver copie et assignation
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    /**
     * @brief Ajoute un répertoire de recherche de plugins
     *
     * @param path Chemin absolu ou relatif du répertoire
     */
    void addPluginPath(const QString& path);

    /**
     * @brief Retourne la liste des répertoires de recherche
     */
    QStringList pluginPaths() const;

    /**
     * @brief Découvre et charge tous les plugins disponibles
     *
     * Parcourt tous les répertoires configurés et charge les plugins trouvés.
     *
     * @return Nombre de plugins chargés avec succès
     */
    int loadAllPlugins();

    /**
     * @brief Charge un plugin spécifique
     *
     * @param filePath Chemin complet vers le fichier du plugin
     * @return true si chargement réussi, false sinon
     */
    bool loadPlugin(const QString& filePath);

    /**
     * @brief Décharge tous les plugins
     *
     * @warning Les instances créées par les plugins deviennent invalides
     */
    void unloadAllPlugins();

    /**
     * @brief Retourne la liste des plugins chargés
     */
    QVector<PluginInfo> loadedPlugins() const;

    /**
     * @brief Retourne le nombre de plugins de formes chargés
     */
    int shapePluginCount() const { return m_shapePlugins.size(); }

    /**
     * @brief Retourne le nombre de plugins de joints chargés
     */
    int jointPluginCount() const { return m_jointPlugins.size(); }

    /**
     * @brief Vérifie si un plugin est chargé
     *
     * @param pluginName Nom du plugin (shapeName ou jointName)
     * @return true si chargé, false sinon
     */
    bool isPluginLoaded(const QString& pluginName) const;

    /**
     * @brief Retourne les informations d'un plugin
     *
     * @param pluginName Nom du plugin
     * @return Informations du plugin, ou PluginInfo vide si introuvable
     */
    PluginInfo pluginInfo(const QString& pluginName) const;

signals:
    /**
     * @brief Émis quand un plugin est chargé avec succès
     *
     * @param info Informations sur le plugin chargé
     */
    void pluginLoaded(const PluginInfo& info);

    /**
     * @brief Émis quand le chargement d'un plugin échoue
     *
     * @param filePath Chemin du plugin
     * @param errorString Message d'erreur
     */
    void pluginLoadFailed(const QString& filePath, const QString& errorString);

    /**
     * @brief Émis quand un plugin est déchargé
     *
     * @param pluginName Nom du plugin déchargé
     */
    void pluginUnloaded(const QString& pluginName);

private:
    PluginManager();
    ~PluginManager();

    /**
     * @brief Tente de charger un plugin comme IShapePlugin
     *
     * @param loader QPluginLoader avec le fichier chargé
     * @return true si succès, false sinon
     */
    bool tryLoadShapePlugin(QPluginLoader* loader);

    /**
     * @brief Tente de charger un plugin comme IJointPlugin
     *
     * @param loader QPluginLoader avec le fichier chargé
     * @return true si succès, false sinon
     */
    bool tryLoadJointPlugin(QPluginLoader* loader);

    /**
     * @brief Enregistre un plugin de forme dans la Factory
     *
     * @param plugin Plugin à enregistrer
     * @return true si succès, false si nom déjà pris
     */
    bool registerShapePlugin(IShapePlugin* plugin);

    /**
     * @brief Enregistre un plugin de joint dans la Factory
     *
     * @param plugin Plugin à enregistrer
     * @return true si succès, false si nom déjà pris
     */
    bool registerJointPlugin(IJointPlugin* plugin);

    /**
     * @brief Initialise les chemins de plugins par défaut
     */
    void initializeDefaultPaths();

    QStringList m_pluginPaths;                      ///< Répertoires de recherche
    QVector<QPluginLoader*> m_loaders;              ///< Loaders actifs
    QVector<IShapePlugin*> m_shapePlugins;          ///< Plugins de formes chargés
    QVector<IJointPlugin*> m_jointPlugins;          ///< Plugins de joints chargés
    QVector<PluginInfo> m_pluginInfos;              ///< Informations des plugins
};

} // namespace Plugins
} // namespace Core
} // namespace LaserCutStudio

#endif // PLUGINMANAGER_H
