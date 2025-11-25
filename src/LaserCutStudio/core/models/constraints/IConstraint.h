#ifndef ICONSTRAINT_H
#define ICONSTRAINT_H

#include <LibInterface/Interface.h>
#include <LibInterface/Patterns/FactoryMixin.h>
#include <LibInterface/Patterns/ListManagerMixin.h>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class IGeometricPoint;

/**
 * @brief Interface de base pour toutes les contraintes géométriques
 *
 * Les contraintes définissent des relations géométriques entre éléments :
 * - Distance entre points
 * - Angle entre segments
 * - Longueur de segment
 * - Parallélisme, perpendicularité
 * - Coïncidence de points
 * - Point fixe
 *
 * ## Patterns Architecturaux
 *
 * - **Prototype Pattern** : Hérite de Interface pour clonage polymorphe
 * - **Factory Pattern** : Utilise FactoryMixin pour création depuis QVariantMap
 * - **List Manager** : Utilise ListManagerMixin pour liste statique de toutes les instances
 * - **Signals/Slots** : Hérite de QObject pour notifications de changements
 *
 * ## Solveur de Contraintes
 *
 * Chaque contrainte peut être :
 * - **Satisfaite** : `isSatisfied()` retourne true
 * - **Violée** : `error()` > tolérance
 * - **Verrouillée** : `isLocked()` = true (prioritaire)
 *
 * Le solveur appelle `apply()` pour ajuster les points libres.
 */
class IConstraint : public Interface,
                    protected Patterns::FactoryMixin<IConstraint>,
                    protected Patterns::ListManagerMixin<IConstraint>
{
    Q_OBJECT

    Q_PROPERTY(bool locked READ isLocked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(double priority READ priority WRITE setPriority NOTIFY priorityChanged)

signals:
    /**
     * @brief Signal émis lorsque la contrainte change
     */
    void constraintChanged();

    /**
     * @brief Signal émis lorsque l'état de verrouillage change
     */
    void lockedChanged(bool locked);

    /**
     * @brief Signal émis lorsque la priorité change
     */
    void priorityChanged(double priority);

    /**
     * @brief Signal émis lorsque la contrainte est satisfaite
     */
    void satisfied();

    /**
     * @brief Signal émis lorsque la contrainte est violée
     */
    void violated();

public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IConstraint();

    /**
     * @brief Clone la contrainte (Pattern Prototype)
     * @return Pointeur vers une nouvelle contrainte clonée
     */
    virtual IConstraint* clone() const override = 0;

    /**
     * @brief Obtient le type de la contrainte
     * @return Nom du type (Distance, Angle, Length, etc.)
     */
    virtual QString getTypeName() const override = 0;

    /**
     * @brief Vérifie si la contrainte est satisfaite
     * @param tolerance Tolérance d'erreur (défaut : 1e-6)
     * @return true si l'erreur est < tolérance
     */
    virtual bool isSatisfied(double tolerance = 1e-6) const = 0;

    /**
     * @brief Calcule l'erreur de la contrainte
     *
     * L'erreur mesure à quel point la contrainte est violée :
     * - 0.0 = contrainte parfaitement satisfaite
     * - > 0 = contrainte violée
     *
     * @return Erreur absolue (toujours >= 0)
     */
    virtual double error() const = 0;

    /**
     * @brief Applique la contrainte en ajustant les points libres
     *
     * Cette méthode est appelée par le solveur à chaque itération.
     * Elle ajuste les positions des points pour réduire l'erreur.
     *
     * @note Ne modifie que les points non-verrouillés
     */
    virtual void apply() = 0;

    /**
     * @brief Obtient la liste des points affectés par cette contrainte
     * @return Liste des points (pour calcul des degrés de liberté)
     */
    virtual QList<IGeometricPoint*> affectedPoints() const = 0;

    /**
     * @brief Vérifie si la contrainte est verrouillée (prioritaire)
     * @return true si verrouillée
     */
    virtual bool isLocked() const { return m_locked; }

    /**
     * @brief Définit l'état de verrouillage
     * @param locked true pour verrouiller
     */
    virtual void setLocked(bool locked);

    /**
     * @brief Obtient la priorité de la contrainte
     *
     * Priorité plus élevée = appliquée en premier par le solveur
     * - Priorité par défaut : 1.0
     * - Contraintes verrouillées : priorité infinie
     *
     * @return Priorité (>= 0)
     */
    virtual double priority() const { return m_priority; }

    /**
     * @brief Définit la priorité de la contrainte
     * @param priority Nouvelle priorité (doit être >= 0)
     */
    virtual void setPriority(double priority);

    // Factory Pattern fourni par FactoryMixin
    using FactoryMixin<IConstraint>::create;
    using FactoryMixin<IConstraint>::availableTypes;
    using FactoryMixin<IConstraint>::registerFactory;

    // Gestion de la liste statique (fournie par ListManagerMixin)
    using ListManagerMixin<IConstraint>::getAllInstances;
    using ListManagerMixin<IConstraint>::clearAllInstances;
    using ListManagerMixin<IConstraint>::instanceCount;

protected:
    /**
     * @brief Constructeur
     * @param locked État de verrouillage initial
     * @param priority Priorité initiale
     */
    IConstraint(bool locked = false, double priority = 1.0);

    bool m_locked;      ///< État de verrouillage
    double m_priority;  ///< Priorité (pour ordre de résolution)
};

} // namespace Core
} // namespace LaserCutStudio

#endif // ICONSTRAINT_H
