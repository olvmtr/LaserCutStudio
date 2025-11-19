#include "PluginManager.h"
#include "core/models/shapes/interfaces/IShape.h"
#include "core/models/joints/interfaces/IJoint.h"
#include <QCoreApplication>
#include <QDirIterator>
#include <QStandardPaths>
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Plugins {

PluginManager& PluginManager::instance()
{
    static PluginManager instance;
    return instance;
}

PluginManager::PluginManager()
{
    initializeDefaultPaths();
}

PluginManager::~PluginManager()
{
    unloadAllPlugins();
}

void PluginManager::initializeDefaultPaths()
{
    // Répertoire plugins/ relatif à l'exécutable
    QString appDir = QCoreApplication::applicationDirPath();
    m_pluginPaths.append(appDir + "/plugins");

    // Répertoire utilisateur
    QString userPluginDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!userPluginDir.isEmpty()) {
        m_pluginPaths.append(userPluginDir + "/plugins");
    }

    #if DEBUG
        qCDebug(logCore) << "Default plugin paths initialized:" << m_pluginPaths;
    #endif
}

void PluginManager::addPluginPath(const QString& path)
{
    if (!m_pluginPaths.contains(path)) {
        m_pluginPaths.append(path);
        qCInfo(logCore) << "Added plugin path:" << path;
    }
}

QStringList PluginManager::pluginPaths() const
{
    return m_pluginPaths;
}

int PluginManager::loadAllPlugins()
{
    int loadedCount = 0;

    for (const QString& path : m_pluginPaths) {
        QDir dir(path);
        if (!dir.exists()) {
            #if DEBUG
                qCDebug(logCore) << "Plugin directory does not exist:" << path;
            #endif
            continue;
        }

        qCInfo(logCore) << "Scanning plugin directory:" << path;

        // Chercher tous les fichiers .so (Linux), .dylib (macOS), .dll (Windows)
        QStringList filters;
        #if defined(Q_OS_WIN)
            filters << "*.dll";
        #elif defined(Q_OS_MACOS)
            filters << "*.dylib";
        #else
            filters << "*.so";
        #endif

        QDirIterator it(path, filters, QDir::Files);
        while (it.hasNext()) {
            QString filePath = it.next();
            if (loadPlugin(filePath)) {
                loadedCount++;
            }
        }
    }

    qCInfo(logCore) << "Loaded" << loadedCount << "plugins total"
                    << "(" << m_shapePlugins.size() << "shapes,"
                    << m_jointPlugins.size() << "joints)";

    return loadedCount;
}

bool PluginManager::loadPlugin(const QString& filePath)
{
    #if DEBUG
        qCDebug(logCore) << "Attempting to load plugin:" << filePath;
    #endif

    // Vérifier si déjà chargé
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();

    for (const auto& info : m_pluginInfos) {
        if (info.fileName == fileName) {
            qCWarning(logCore) << "Plugin already loaded:" << fileName;
            return false;
        }
    }

    // Créer le loader
    QPluginLoader* loader = new QPluginLoader(filePath, this);

    if (!loader->load()) {
        QString errorMsg = loader->errorString();
        qCWarning(logCore) << "Failed to load plugin:" << filePath << "-" << errorMsg;
        emit pluginLoadFailed(filePath, errorMsg);
        delete loader;
        return false;
    }

    // Essayer de charger comme IShapePlugin
    if (tryLoadShapePlugin(loader)) {
        m_loaders.append(loader);
        return true;
    }

    // Essayer de charger comme IJointPlugin
    if (tryLoadJointPlugin(loader)) {
        m_loaders.append(loader);
        return true;
    }

    // Aucune interface reconnue
    qCWarning(logCore) << "Plugin does not implement IShapePlugin or IJointPlugin:" << filePath;
    loader->unload();
    delete loader;
    return false;
}

bool PluginManager::tryLoadShapePlugin(QPluginLoader* loader)
{
    QObject* instance = loader->instance();
    if (!instance) {
        return false;
    }

    IShapePlugin* plugin = qobject_cast<IShapePlugin*>(instance);
    if (!plugin) {
        return false;
    }

    // Vérifier que le nom n'est pas déjà pris
    QString name = plugin->shapeName();
    if (isPluginLoaded(name)) {
        qCWarning(logCore) << "Shape plugin name already in use:" << name;
        return false;
    }

    // Enregistrer dans la Factory
    if (!registerShapePlugin(plugin)) {
        qCWarning(logCore) << "Failed to register shape plugin:" << name;
        return false;
    }

    // Sauvegarder les infos
    PluginInfo info;
    info.fileName = QFileInfo(loader->fileName()).fileName();
    info.pluginName = name;
    info.description = plugin->shapeDescription();
    info.version = plugin->version();
    info.author = plugin->author();
    info.type = "Shape";
    info.isLoaded = true;

    m_shapePlugins.append(plugin);
    m_pluginInfos.append(info);

    qCInfo(logCore) << "Loaded shape plugin:" << name << "v" << info.version
                    << "by" << info.author;

    emit pluginLoaded(info);
    return true;
}

bool PluginManager::tryLoadJointPlugin(QPluginLoader* loader)
{
    QObject* instance = loader->instance();
    if (!instance) {
        return false;
    }

    IJointPlugin* plugin = qobject_cast<IJointPlugin*>(instance);
    if (!plugin) {
        return false;
    }

    // Vérifier que le nom n'est pas déjà pris
    QString name = plugin->jointName();
    if (isPluginLoaded(name)) {
        qCWarning(logCore) << "Joint plugin name already in use:" << name;
        return false;
    }

    // Enregistrer dans la Factory
    if (!registerJointPlugin(plugin)) {
        qCWarning(logCore) << "Failed to register joint plugin:" << name;
        return false;
    }

    // Sauvegarder les infos
    PluginInfo info;
    info.fileName = QFileInfo(loader->fileName()).fileName();
    info.pluginName = name;
    info.description = plugin->jointDescription();
    info.version = plugin->version();
    info.author = plugin->author();
    info.type = "Joint";
    info.isLoaded = true;

    m_jointPlugins.append(plugin);
    m_pluginInfos.append(info);

    qCInfo(logCore) << "Loaded joint plugin:" << name << "v" << info.version
                    << "by" << info.author;

    emit pluginLoaded(info);
    return true;
}

bool PluginManager::registerShapePlugin(IShapePlugin* plugin)
{
    QString name = plugin->shapeName();

    // Vérifier si le nom est déjà enregistré
    if (IShape::s_factories.contains(name)) {
        return false;
    }

    // Créer une lambda factory qui utilise le plugin
    // Note: IShape::FactoryFunc attend QVariantMap, pas QVariant
    IShape::FactoryFunc factory = [plugin](const QVariantMap& params) -> IShape* {
        QVariant paramsVariant = params;
        return plugin->createShape(paramsVariant);
    };

    // Enregistrer directement dans la map de Factory
    IShape::s_factories[name] = factory;
    return true;
}

bool PluginManager::registerJointPlugin(IJointPlugin* plugin)
{
    QString name = plugin->jointName();

    // Vérifier si le nom est déjà enregistré
    if (IJoint::s_factories.contains(name)) {
        return false;
    }

    // Créer une lambda factory qui utilise le plugin
    // Note: IJoint::FactoryFunc attend QVariantMap, pas QVariant
    IJoint::FactoryFunc factory = [plugin](const QVariantMap& params) -> IJoint* {
        QVariant paramsVariant = params;
        return plugin->createJoint(paramsVariant);
    };

    // Enregistrer directement dans la map de Factory
    IJoint::s_factories[name] = factory;
    return true;
}

void PluginManager::unloadAllPlugins()
{
    qCInfo(logCore) << "Unloading all plugins...";

    // Émettre les signaux de déchargement
    for (const auto& info : m_pluginInfos) {
        emit pluginUnloaded(info.pluginName);
    }

    // Nettoyer les listes
    m_shapePlugins.clear();
    m_jointPlugins.clear();
    m_pluginInfos.clear();

    // Décharger et détruire les loaders
    for (QPluginLoader* loader : m_loaders) {
        loader->unload();
        delete loader;
    }
    m_loaders.clear();

    qCInfo(logCore) << "All plugins unloaded";
}

QVector<PluginInfo> PluginManager::loadedPlugins() const
{
    return m_pluginInfos;
}

bool PluginManager::isPluginLoaded(const QString& pluginName) const
{
    for (const auto& info : m_pluginInfos) {
        if (info.pluginName == pluginName) {
            return true;
        }
    }
    return false;
}

PluginInfo PluginManager::pluginInfo(const QString& pluginName) const
{
    for (const auto& info : m_pluginInfos) {
        if (info.pluginName == pluginName) {
            return info;
        }
    }
    return PluginInfo();  // Retourne info vide si introuvable
}

} // namespace Plugins
} // namespace Core
} // namespace LaserCutStudio
