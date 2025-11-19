#ifndef IJOINTPLUGIN_H
#define IJOINTPLUGIN_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "core/models/joints/interfaces/IJoint.h"

namespace LaserCutStudio {
namespace Core {
namespace Plugins {

/**
 * @interface IJointPlugin
 * @brief Interface pour les plugins de connexions d'assemblage personnalisées
 *
 * Cette interface permet d'étendre le système avec de nouveaux types
 * de connexions (joints) sans recompiler l'application principale.
 *
 * Les plugins doivent :
 * - Hériter de QObject et IJointPlugin
 * - Utiliser Q_INTERFACES(LaserCutStudio::Core::Plugins::IJointPlugin)
 * - Utiliser Q_PLUGIN_METADATA(IID "com.lasercutstudio.IJointPlugin")
 * - Implémenter toutes les méthodes virtuelles pures
 *
 * @see IJoint
 * @see QPluginLoader
 */
class IJointPlugin
{
public:
    virtual ~IJointPlugin() = default;

    /**
     * @brief Retourne le nom unique du type de joint
     *
     * Ce nom est utilisé pour l'enregistrement dans la Factory.
     * Doit être unique parmi tous les joints (built-in + plugins).
     *
     * @return Nom du type (ex: "DoveTail", "Mortise", "LapJoint")
     */
    virtual QString jointName() const = 0;

    /**
     * @brief Retourne une description lisible du joint
     *
     * @return Description (ex: "Queue d'aronde", "Mortaise et tenon")
     */
    virtual QString jointDescription() const = 0;

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
     * @brief Crée une instance du joint avec les paramètres fournis
     *
     * Les paramètres dépendent du type de joint et sont passés via QVariant.
     * Le plugin doit valider les paramètres et retourner nullptr si invalides.
     *
     * @param params Paramètres de création (QVariantMap)
     * @return Nouvelle instance de IJoint, ou nullptr si échec
     *
     * @note Le appelant prend ownership de l'objet retourné
     */
    virtual IJoint* createJoint(const QVariant& params) const = 0;

    /**
     * @brief Retourne les paramètres par défaut pour ce joint
     *
     * Utilisé pour créer un joint avec valeurs par défaut.
     *
     * @return QVariantMap avec paramètres par défaut
     */
    virtual QVariant defaultParameters() const = 0;

    /**
     * @brief Retourne le schéma de validation des paramètres
     *
     * Format JSON Schema (simplifié) décrivant les paramètres requis.
     *
     * Exemple pour une queue d'aronde :
     * @code{.cpp}
     * {
     *   "angle": { "type": "double", "min": 5.0, "max": 15.0 },
     *   "count": { "type": "int", "min": 2, "max": 20 }
     * }
     * @endcode
     *
     * @return Schéma de validation (QVariantMap)
     */
    virtual QVariant parameterSchema() const = 0;

    /**
     * @brief Indique si ce joint est compatible avec un matériau donné
     *
     * Certains joints fonctionnent mieux avec certains matériaux.
     *
     * @param material Nom du matériau à tester
     * @return true si compatible, false sinon
     */
    virtual bool isCompatibleWithMaterial(const QString& material) const = 0;

    /**
     * @brief Retourne la force estimée de ce type de joint
     *
     * Valeur indicative de 0.0 (faible) à 1.0 (très fort).
     * Utilisé pour recommandations à l'utilisateur.
     *
     * @return Force estimée (0.0 - 1.0)
     */
    virtual double estimatedStrength() const = 0;
};

} // namespace Plugins
} // namespace Core
} // namespace LaserCutStudio

// Déclaration de l'interface pour Qt
Q_DECLARE_INTERFACE(LaserCutStudio::Core::Plugins::IJointPlugin,
                    "com.lasercutstudio.IJointPlugin/1.0")

#endif // IJOINTPLUGIN_H
