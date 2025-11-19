#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDebug>
#include <functional>
#include <memory>
#include <typeinfo>

namespace LibInterface {

namespace DI {

/**
 * @class ServiceLocator
 * @brief Pattern Service Locator pour Dependency Injection
 *
 * Singleton responsable de :
 * - Enregistrer des services (factories ou instances)
 * - Résoudre des dépendances (créer ou récupérer des instances)
 * - Gérer le cycle de vie (Singleton, Transient, Scoped)
 *
 * Usage :
 * @code
 * // Enregistrer un service Singleton
 * ServiceLocator::instance().registerSingleton<IConfig>([]() {
 *     return new ConfigManager();
 * });
 *
 * // Enregistrer un service Transient (nouvelle instance à chaque fois)
 * ServiceLocator::instance().registerTransient<IShape>([]() {
 *     return new Rectangle();
 * });
 *
 * // Résoudre une dépendance
 * IConfig* config = ServiceLocator::instance().resolve<IConfig>();
 * @endcode
 *
 * @note Thread-safe (Singleton avec Meyers)
 */
class ServiceLocator : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Cycle de vie d'un service
     */
    enum class Lifetime {
        Singleton,   ///< Une seule instance pour toute l'application
        Transient,   ///< Nouvelle instance à chaque résolution
        Scoped       ///< Une instance par scope (non implémenté)
    };

    /**
     * @brief Accès à l'instance unique (Singleton)
     */
    static ServiceLocator& instance();

    // Désactiver copie et assignation
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    /**
     * @brief Enregistre un service Singleton
     *
     * @tparam T Type du service
     * @param factory Fonction qui crée une instance du service
     * @param interfaceName Nom de l'interface (optionnel, sinon utilise typeid)
     *
     * @note Le service sera créé lors de la première résolution
     */
    template<typename T>
    void registerSingleton(std::function<T*()> factory, const QString& interfaceName = QString())
    {
        QString key = interfaceName.isEmpty() ? typeid(T).name() : interfaceName;

        if (m_factories.contains(key)) {
            qWarning() << "LibInterface::ServiceLocator: Service already registered:" << key;
            return;
        }

        ServiceInfo info;
        info.lifetime = Lifetime::Singleton;
        info.factory = [factory]() -> QObject* {
            return dynamic_cast<QObject*>(factory());
        };
        info.typeName = typeid(T).name();

        m_factories[key] = info;

        #if DEBUG
            qDebug() << "LibInterface::ServiceLocator: Registered Singleton service:" << key;
        #endif
    }

    /**
     * @brief Enregistre un service Transient
     *
     * @tparam T Type du service
     * @param factory Fonction qui crée une instance du service
     * @param interfaceName Nom de l'interface (optionnel)
     *
     * @note Une nouvelle instance sera créée à chaque résolution
     */
    template<typename T>
    void registerTransient(std::function<T*()> factory, const QString& interfaceName = QString())
    {
        QString key = interfaceName.isEmpty() ? typeid(T).name() : interfaceName;

        if (m_factories.contains(key)) {
            qWarning() << "LibInterface::ServiceLocator: Service already registered:" << key;
            return;
        }

        ServiceInfo info;
        info.lifetime = Lifetime::Transient;
        info.factory = [factory]() -> QObject* {
            return dynamic_cast<QObject*>(factory());
        };
        info.typeName = typeid(T).name();

        m_factories[key] = info;

        #if DEBUG
            qDebug() << "LibInterface::ServiceLocator: Registered Transient service:" << key;
        #endif
    }

    /**
     * @brief Enregistre une instance de service (Singleton)
     *
     * @tparam T Type du service
     * @param instance Instance du service (le ServiceLocator prend ownership)
     * @param interfaceName Nom de l'interface (optionnel)
     */
    template<typename T>
    void registerInstance(T* instance, const QString& interfaceName = QString())
    {
        QString key = interfaceName.isEmpty() ? typeid(T).name() : interfaceName;

        if (m_instances.contains(key)) {
            qWarning() << "LibInterface::ServiceLocator: Instance already registered:" << key;
            return;
        }

        m_instances[key] = instance;
        instance->setParent(this);  // ServiceLocator prend ownership

        #if DEBUG
            qDebug() << "LibInterface::ServiceLocator: Registered instance:" << key;
        #endif
    }

    /**
     * @brief Résout une dépendance
     *
     * @tparam T Type du service
     * @param interfaceName Nom de l'interface (optionnel)
     * @return Pointeur vers l'instance, ou nullptr si non trouvé
     *
     * @note Pour Singleton : retourne toujours la même instance
     * @note Pour Transient : crée une nouvelle instance à chaque appel
     */
    template<typename T>
    T* resolve(const QString& interfaceName = QString())
    {
        QString key = interfaceName.isEmpty() ? typeid(T).name() : interfaceName;

        // Vérifier si instance déjà créée
        if (m_instances.contains(key)) {
            QObject* obj = m_instances[key];
            T* result = dynamic_cast<T*>(obj);
            if (!result) {
                qCritical() << "LibInterface::ServiceLocator: Failed to cast instance for:" << key;
                return nullptr;
            }
            return result;
        }

        // Vérifier si factory existe
        if (!m_factories.contains(key)) {
            qWarning() << "LibInterface::ServiceLocator: Service not registered:" << key;
            return nullptr;
        }

        ServiceInfo& info = m_factories[key];

        // Singleton : créer et stocker l'instance
        if (info.lifetime == Lifetime::Singleton) {
            QObject* obj = info.factory();
            if (!obj) {
                qCritical() << "LibInterface::ServiceLocator: Factory returned nullptr for:" << key;
                return nullptr;
            }

            T* result = dynamic_cast<T*>(obj);
            if (!result) {
                qCritical() << "LibInterface::ServiceLocator: Failed to cast service for:" << key;
                delete obj;
                return nullptr;
            }

            m_instances[key] = obj;
            obj->setParent(this);  // ServiceLocator prend ownership

            #if DEBUG
                qDebug() << "LibInterface::ServiceLocator: Created Singleton instance for:" << key;
            #endif

            return result;
        }

        // Transient : créer une nouvelle instance
        if (info.lifetime == Lifetime::Transient) {
            QObject* obj = info.factory();
            if (!obj) {
                qCritical() << "LibInterface::ServiceLocator: Factory returned nullptr for:" << key;
                return nullptr;
            }

            T* result = dynamic_cast<T*>(obj);
            if (!result) {
                qCritical() << "LibInterface::ServiceLocator: Failed to cast service for:" << key;
                delete obj;
                return nullptr;
            }

            #if DEBUG
                qDebug() << "LibInterface::ServiceLocator: Created Transient instance for:" << key;
            #endif

            // Note: L'appelant prend ownership de l'instance Transient
            return result;
        }

        return nullptr;
    }

    /**
     * @brief Vérifie si un service est enregistré
     *
     * @tparam T Type du service
     * @param interfaceName Nom de l'interface (optionnel)
     * @return true si enregistré, false sinon
     */
    template<typename T>
    bool isRegistered(const QString& interfaceName = QString()) const
    {
        QString key = interfaceName.isEmpty() ? typeid(T).name() : interfaceName;
        return m_factories.contains(key) || m_instances.contains(key);
    }

    /**
     * @brief Retire un service du locator
     *
     * @tparam T Type du service
     * @param interfaceName Nom de l'interface (optionnel)
     *
     * @note Détruit l'instance si elle existe
     */
    template<typename T>
    void unregister(const QString& interfaceName = QString())
    {
        QString key = interfaceName.isEmpty() ? typeid(T).name() : interfaceName;

        if (m_instances.contains(key)) {
            QObject* obj = m_instances[key];
            m_instances.remove(key);
            delete obj;  // Détruit l'instance
        }

        if (m_factories.contains(key)) {
            m_factories.remove(key);
        }

        #if DEBUG
            qDebug() << "LibInterface::ServiceLocator: Unregistered service:" << key;
        #endif
    }

    /**
     * @brief Nettoie tous les services
     *
     * @warning Détruit toutes les instances
     */
    void clear();

    /**
     * @brief Retourne le nombre de services enregistrés
     */
    int serviceCount() const { return m_factories.size(); }

    /**
     * @brief Retourne le nombre d'instances créées
     */
    int instanceCount() const { return m_instances.size(); }

private:
    ServiceLocator();
    ~ServiceLocator();

    struct ServiceInfo {
        Lifetime lifetime;
        std::function<QObject*()> factory;
        QString typeName;
    };

    QMap<QString, ServiceInfo> m_factories;   ///< Factories enregistrées
    QMap<QString, QObject*> m_instances;      ///< Instances créées (Singletons)
};

} // namespace DI

} // namespace LibInterface

#endif // SERVICELOCATOR_H
