#ifndef DISTANCECONSTRAINT_H
#define DISTANCECONSTRAINT_H

#include "core/models/constraints/IConstraint.h"
#include "core/models/geometry/IGeometricPoint.h"

// Forward declarations
class GeometricPoint;
#include "core/models/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Contrainte de distance entre deux points
 *
 * Cette contrainte maintient une distance fixe entre deux points géométriques.
 *
 * ## Exemple d'utilisation
 *
 * @code
 * GeometricPoint* p1 = new GeometricPoint(0, 0, true);  // Point fixe
 * GeometricPoint* p2 = new GeometricPoint(5, 0, false); // Point libre
 *
 * // Contrainte: distance = 10 cm
 * DistanceConstraint* constraint = new DistanceConstraint(p1, p2, 10.0, true);
 *
 * // p2 sera ajusté pour respecter la distance de 10 cm
 * @endcode
 *
 * ## Algorithme apply()
 *
 * La méthode `apply()` ajuste les positions des points pour satisfaire la contrainte :
 * 1. Calcule l'erreur : `currentDistance - targetDistance`
 * 2. Calcule la direction : vecteur normalisé entre les points
 * 3. Déplace chaque point libre de `error/2` dans la direction appropriée
 *
 * @note Seuls les points non-verrouillés sont modifiés
 */
class DistanceConstraint : public IConstraint,
                           protected Patterns::PropertyMixin<DistanceConstraint>
{
    Q_OBJECT

    Q_PROPERTY(IGeometricPoint* point1 READ point1 WRITE setPoint1 NOTIFY point1Changed)
    Q_PROPERTY(IGeometricPoint* point2 READ point2 WRITE setPoint2 NOTIFY point2Changed)
    Q_PROPERTY(double distance READ distance WRITE setDistance NOTIFY distanceChanged)

signals:
    void point1Changed(IGeometricPoint* newPoint1);
    void point2Changed(IGeometricPoint* newPoint2);
    void distanceChanged(double newDistance);

public:
    /**
     * @brief Constructeur par défaut
     */
    DistanceConstraint();

    /**
     * @brief Constructeur avec points et distance
     * @param point1 Premier point
     * @param point2 Second point
     * @param distance Distance cible (en unités)
     * @param locked État de verrouillage (prioritaire si true)
     */
    DistanceConstraint(IGeometricPoint* point1, IGeometricPoint* point2, double distance, bool locked = false);

    /**
     * @brief Constructeur de copie
     */
    DistanceConstraint(const DistanceConstraint& other);

    /**
     * @brief Destructeur
     */
    ~DistanceConstraint() override;

    /**
     * @brief Clone la contrainte
     */
    IConstraint* clone() const override;

    /**
     * @brief Déclare automatiquement staticTypeName() et getTypeName()
     */
    DECLARE_TYPE_NAME(DistanceConstraint)

    /**
     * @brief Vérifie si la contrainte est satisfaite
     */
    bool isSatisfied(double tolerance = 1e-6) const override;

    /**
     * @brief Calcule l'erreur (différence entre distance actuelle et cible)
     */
    double error() const override;

    /**
     * @brief Applique la contrainte (ajuste les points libres)
     */
    void apply() override;

    /**
     * @brief Obtient les points affectés
     */
    QList<GeometricPoint*> affectedPoints() const override;

    // Getters
    IGeometricPoint* point1() const { return m_point1; }
    IGeometricPoint* point2() const { return m_point2; }
    double distance() const { return m_distance; }
    bool isValid() const;

    // Setters
    void setPoint1(IGeometricPoint* point);
    void setPoint2(IGeometricPoint* point);
    void setDistance(double distance);

private:
    void connectToPoint(IGeometricPoint* point);
    void disconnectFromPoint(IGeometricPoint* point);

    IGeometricPoint* m_point1 = nullptr;  ///< Premier point
    IGeometricPoint* m_point2 = nullptr;  ///< Second point
    double m_distance = 0.0;             ///< Distance cible

    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // DISTANCECONSTRAINT_H
