#ifndef FACTORYMIXIN_H
#define FACTORYMIXIN_H

#include <QMap>
#include <QString>
#include <QVariantMap>
#include <QStringList>
#include <QMetaObject>
#include <QMetaProperty>
#include <functional>

namespace LaserCutStudio {
namespace Core {
namespace Patterns {

/**
 * @brief Mixin CRTP pour fournir le Factory Pattern automatiquement
 *
 * Ce template élimine la duplication du code Factory Pattern
 * entre les différentes interfaces (IShape, IPart, IJoint, IProject).
 *
 * Usage:
 * @code
 * class IShape : public Interface, protected FactoryMixin<IShape> {
 * public:
 *     using FactoryMixin<IShape>::create;
 *     using FactoryMixin<IShape>::availableTypes;
 *     using FactoryMixin<IShape>::registerFactory;
 * };
 * @endcode
 *
 * @tparam Base Type de base (IShape, IPart, etc.)
 */
template<typename Base>
class FactoryMixin
{
protected:
    /**
     * @brief Type de fonction factory
     */
    using FactoryFunc = std::function<Base*(const QVariantMap&)>;

    /**
     * @brief Map des factories enregistrées (typeName -> factory function)
     */
    static QMap<QString, FactoryFunc> s_factories;

    /**
     * @brief Enregistre une classe concrète dans le Factory Pattern
     *
     * Cette méthode utilise QMetaObject pour configurer automatiquement
     * l'objet créé depuis les paramètres QVariantMap via Q_PROPERTY.
     *
     * @tparam T Type concret à enregistrer (doit hériter de Base)
     * @return true (permet l'utilisation comme initialiseur static)
     *
     * @note La classe T doit avoir une méthode statique staticTypeName()
     */
    template<typename T>
    static bool registerFactory() {
        s_factories[T::staticTypeName()] = [](const QVariantMap& params) {
            auto* obj = new T();
            // Utilise Q_PROPERTY pour configurer l'objet depuis params
            const QMetaObject* meta = obj->metaObject();
            for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
                int propIndex = meta->indexOfProperty(it.key().toUtf8().constData());
                if (propIndex >= 0) {
                    QMetaProperty prop = meta->property(propIndex);
                    if (prop.isWritable()) {
                        prop.write(obj, it.value());
                    }
                }
            }
            return obj;
        };
        return true;
    }

public:
    /**
     * @brief Crée une instance depuis une configuration QVariant
     *
     * @param config Configuration avec au minimum la clé "type"
     * @return Nouvelle instance ou nullptr si type inconnu
     */
    static Base* create(const QVariantMap& config) {
        QString type = config.value("type").toString();
        if (s_factories.contains(type)) {
            return s_factories[type](config);
        }
        return nullptr;
    }

    /**
     * @brief Liste tous les types disponibles
     * @return Liste des noms de types enregistrés
     */
    static QStringList availableTypes() {
        return s_factories.keys();
    }

    /**
     * @brief Vérifie si un type est enregistré
     * @param typeName Nom du type à vérifier
     * @return true si le type est enregistré
     */
    static bool isTypeRegistered(const QString& typeName) {
        return s_factories.contains(typeName);
    }

    /**
     * @brief Retourne le nombre de types enregistrés
     */
    static int registeredTypeCount() {
        return s_factories.size();
    }

    /**
     * @brief Vide toutes les factories enregistrées
     * @warning À utiliser avec précaution, généralement uniquement pour les tests
     */
    static void clearFactories() {
        s_factories.clear();
    }

    /**
     * @brief Helper pour auto-enregistrement au démarrage
     *
     * Cette classe template permet d'enregistrer automatiquement
     * une classe concrète dans le Factory Pattern via une variable
     * globale statique.
     *
     * Usage :
     * @code
     * // Dans Rectangle.cpp (au niveau namespace)
     * namespace {
     *     FactoryMixin<IShape>::AutoRegister<Rectangle> g_rectangleReg;
     * }
     * @endcode
     *
     * Cela remplace le pattern manuel :
     * @code
     * // Ancien pattern (plus verbeux)
     * static const bool s_registered;
     * const bool Rectangle::s_registered = IShape::registerFactory<Rectangle>();
     * @endcode
     *
     * @tparam T Type concret à enregistrer
     */
    template<typename T>
    struct AutoRegister {
        AutoRegister() {
            FactoryMixin<Base>::registerFactory<T>();
        }
    };
};

/**
 * @brief Définition du membre statique template
 *
 * Cette définition doit être dans le .h car c'est un template.
 * Chaque instanciation du template (FactoryMixin<IShape>, FactoryMixin<IPart>, etc.)
 * aura son propre s_factories statique.
 */
template<typename Base>
QMap<QString, typename FactoryMixin<Base>::FactoryFunc> FactoryMixin<Base>::s_factories;

} // namespace Patterns
} // namespace Core
} // namespace LaserCutStudio

#endif // FACTORYMIXIN_H
