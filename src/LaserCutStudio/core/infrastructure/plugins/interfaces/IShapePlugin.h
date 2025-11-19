#ifndef ISHAPEPLUGIN_H
#define ISHAPEPLUGIN_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "core/models/shapes/interfaces/IShape.h"

namespace LaserCutStudio {
namespace Core {
namespace Plugins {

/**
 * @interface IShapePlugin
 * @brief Interface pour les plugins de formes personnalisées
 *
 * Cette interface permet d'étendre le système avec de nouvelles formes
 * géométriques sans recompiler l'application principale.
 *
 * Les plugins doivent :
 * - Hériter de QObject et IShapePlugin
 * - Utiliser Q_INTERFACES(LaserCutStudio::Core::Plugins::IShapePlugin)
 * - Utiliser Q_PLUGIN_METADATA(IID "com.lasercutstudio.IShapePlugin")
 * - Implémenter toutes les méthodes virtuelles pures
 *
 * @see IShape
 * @see QPluginLoader
 */
class IShapePlugin
{
public:
    virtual ~IShapePlugin() = default;

    /**
     * @brief Retourne le nom unique du type de forme
     *
     * Ce nom est utilisé pour l'enregistrement dans la Factory.
     * Doit être unique parmi toutes les formes (built-in + plugins).
     *
     * @return Nom du type (ex: "Polygon", "Star", "Gear")
     */
    virtual QString shapeName() const = 0;

    /**
     * @brief Retourne une description lisible de la forme
     *
     * @return Description (ex: "Polygone régulier", "Étoile à N branches")
     */
    virtual QString shapeDescription() const = 0;

    /**
     * @brief Retourne la version du plugin
     *
     * Format recommandé : "1.0.0" (semantic versioning)
     *
     * @return Version du plugin
     */
    virtual QString version() const = 0;

    /**
     * @brief Retourne l'auteur du plugin
     *
     * @return Nom de l'auteur ou organisation
     */
    virtual QString author() const = 0;

    /**
     * @brief Crée une instance de la forme avec les paramètres fournis
     *
     * Les paramètres dépendent du type de forme et sont passés via QVariant.
     * Le plugin doit valider les paramètres et retourner nullptr si invalides.
     *
     * @param params Paramètres de création (QVariantMap)
     * @return Nouvelle instance de IShape, ou nullptr si échec
     *
     * @note Le appelant prend ownership de l'objet retourné
     */
    virtual IShape* createShape(const QVariant& params) const = 0;

    /**
     * @brief Retourne les paramètres par défaut pour cette forme
     *
     * Utilisé pour créer une forme avec valeurs par défaut.
     *
     * @return QVariantMap avec paramètres par défaut
     */
    virtual QVariant defaultParameters() const = 0;

    /**
     * @brief Retourne le schéma de validation des paramètres
     *
     * Format JSON Schema (simplifié) décrivant les paramètres requis.
     *
     * Exemple pour un polygone :
     * @code{.cpp}
     * {
     *   "sides": { "type": "int", "min": 3, "max": 100 },
     *   "radius": { "type": "double", "min": 0.0 }
     * }
     * @endcode
     *
     * @return Schéma de validation (QVariantMap)
     */
    virtual QVariant parameterSchema() const = 0;
};

} // namespace Plugins
} // namespace Core
} // namespace LaserCutStudio

// Déclaration de l'interface pour Qt
Q_DECLARE_INTERFACE(LaserCutStudio::Core::Plugins::IShapePlugin,
                    "com.lasercutstudio.IShapePlugin/1.0")

#endif // ISHAPEPLUGIN_H
