#ifndef IGEOMETRICSEGMENT_H
#define IGEOMETRICSEGMENT_H

#include "core/models/geometry/IGeometricElement.h"
#include "core/models/geometry/IGeometricPoint.h"

namespace LaserCutStudio {
namespace Core {

// Forward declaration
class IGeometricPoint;

/**
 * @brief Interface spécifique pour les segments géométriques
 *
 * Définit les opérations spécifiques aux segments :
 * - Points de départ et d'arrivée
 * - Longueur
 * - Angle
 * - Point milieu, direction
 * - Distance à un point
 *
 * Cette interface permet aux contraintes de dépendre d'une abstraction
 * spécifique aux segments, sans dépendre de l'implémentation concrète.
 *
 * ## Principe SOLID
 *
 * Interface Segregation Principle : Les clients qui n'ont besoin que de
 * segments ne doivent pas dépendre des méthodes spécifiques aux points/arcs.
 */
class IGeometricSegment : public IGeometricElement
{
    Q_OBJECT

signals:
    void startPointChanged(IGeometricPoint* newStartPoint);
    void endPointChanged(IGeometricPoint* newEndPoint);

public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IGeometricSegment() = default;

    /**
     * @brief Clone le segment
     */
    virtual IGeometricSegment* clone() const override = 0;

    // Getters
    /**
     * @brief Obtient le point de départ
     */
    virtual IGeometricPoint* startPoint() const = 0;

    /**
     * @brief Obtient le point d'arrivée
     */
    virtual IGeometricPoint* endPoint() const = 0;

    /**
     * @brief Calcule la longueur du segment
     */
    virtual double length() const = 0;

    /**
     * @brief Calcule l'angle du segment par rapport à l'axe X
     * @return Angle en degrés [0, 360)
     */
    virtual double angle() const = 0;

    /**
     * @brief Calcule le point milieu du segment
     */
    virtual Point2D midpoint() const = 0;

    /**
     * @brief Calcule le vecteur directeur normalisé
     */
    virtual Point2D direction() const = 0;

    /**
     * @brief Vérifie si le segment est valide (points non-null)
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Calcule la distance d'un point au segment
     */
    virtual double distanceToPoint(const Point2D& point) const = 0;

    // Setters
    /**
     * @brief Définit le point de départ
     */
    virtual void setStartPoint(IGeometricPoint* start) = 0;

    /**
     * @brief Définit le point d'arrivée
     */
    virtual void setEndPoint(IGeometricPoint* end) = 0;

protected:
    /**
     * @brief Constructeur protégé
     */
    IGeometricSegment() = default;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IGEOMETRICSEGMENT_H
