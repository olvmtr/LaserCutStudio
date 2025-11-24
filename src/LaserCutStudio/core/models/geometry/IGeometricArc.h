#ifndef IGEOMETRICARC_H
#define IGEOMETRICARC_H

#include "core/models/geometry/IGeometricElement.h"
#include "core/models/geometry/IGeometricPoint.h"

namespace LaserCutStudio {
namespace Core {

// Forward declaration
class IGeometricPoint;

/**
 * @brief Interface spécifique pour les arcs géométriques
 *
 * Définit les opérations spécifiques aux arcs de cercle :
 * - Centre (point)
 * - Rayon
 * - Angles de début et fin
 *
 * Cette interface permet aux contraintes de dépendre d'une abstraction
 * spécifique aux arcs, sans dépendre de l'implémentation concrète.
 *
 * ## Principe SOLID
 *
 * Interface Segregation Principle : Les clients qui n'ont besoin que d'arcs
 * ne doivent pas dépendre des méthodes spécifiques aux points/segments.
 */
class IGeometricArc : public IGeometricElement
{
    Q_OBJECT

signals:
    void centerChanged(IGeometricPoint* newCenter);
    void radiusChanged(double newRadius);
    void startAngleChanged(double newStartAngle);
    void endAngleChanged(double newEndAngle);

public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IGeometricArc() = default;

    /**
     * @brief Clone l'arc
     */
    virtual IGeometricArc* clone() const override = 0;

    // Getters
    /**
     * @brief Obtient le point central de l'arc
     */
    virtual IGeometricPoint* center() const = 0;

    /**
     * @brief Obtient le rayon de l'arc
     */
    virtual double radius() const = 0;

    /**
     * @brief Obtient l'angle de départ (en degrés)
     */
    virtual double startAngle() const = 0;

    /**
     * @brief Obtient l'angle de fin (en degrés)
     */
    virtual double endAngle() const = 0;

    /**
     * @brief Vérifie si l'arc est valide (centre non-null, rayon > 0)
     */
    virtual bool isValid() const = 0;

    // Setters
    /**
     * @brief Définit le point central
     */
    virtual void setCenter(IGeometricPoint* center) = 0;

    /**
     * @brief Définit le rayon
     */
    virtual void setRadius(double radius) = 0;

    /**
     * @brief Définit l'angle de départ
     */
    virtual void setStartAngle(double angle) = 0;

    /**
     * @brief Définit l'angle de fin
     */
    virtual void setEndAngle(double angle) = 0;

protected:
    /**
     * @brief Constructeur protégé
     */
    IGeometricArc() = default;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IGEOMETRICARC_H
