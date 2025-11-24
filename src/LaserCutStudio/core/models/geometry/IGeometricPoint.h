#ifndef IGEOMETRICPOINT_H
#define IGEOMETRICPOINT_H

#include "core/models/geometry/IGeometricElement.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Interface spécifique pour les points géométriques
 *
 * Définit les opérations spécifiques aux points :
 * - Coordonnées x, y
 * - Verrouillage (locked/unlocked)
 * - Calcul de distance
 * - Translation
 *
 * Cette interface permet aux contraintes de dépendre d'une abstraction
 * spécifique aux points, sans dépendre de l'implémentation concrète.
 *
 * ## Principe SOLID
 *
 * Interface Segregation Principle : Les clients qui n'ont besoin que de
 * points ne doivent pas dépendre des méthodes spécifiques aux segments/arcs.
 */
class IGeometricPoint : public IGeometricElement
{
    Q_OBJECT

signals:
    void xChanged(double newX);
    void yChanged(double newY);
    void lockedChanged(bool locked);

public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IGeometricPoint() = default;

    /**
     * @brief Clone le point
     */
    virtual IGeometricPoint* clone() const override = 0;

    // Getters
    /**
     * @brief Obtient la coordonnée X
     */
    virtual double x() const = 0;

    /**
     * @brief Obtient la coordonnée Y
     */
    virtual double y() const = 0;

    /**
     * @brief Vérifie si le point est verrouillé
     */
    virtual bool isLocked() const = 0;

    /**
     * @brief Obtient la position comme Point2D
     */
    virtual Point2D position() const = 0;

    /**
     * @brief Calcule la distance avec un autre point
     */
    virtual double distance(const IGeometricPoint& other) const = 0;

    /**
     * @brief Calcule la distance avec un Point2D
     */
    virtual double distance(const Point2D& other) const = 0;

    // Setters
    /**
     * @brief Définit la coordonnée X
     */
    virtual void setX(double x) = 0;

    /**
     * @brief Définit la coordonnée Y
     */
    virtual void setY(double y) = 0;

    /**
     * @brief Définit la position (x, y)
     */
    virtual void setPosition(double x, double y) = 0;

    /**
     * @brief Définit la position depuis un Point2D
     */
    virtual void setPosition(const Point2D& pos) = 0;

    /**
     * @brief Définit l'état de verrouillage
     */
    virtual void setLocked(bool locked) = 0;

    /**
     * @brief Translate le point
     */
    virtual void translate(double dx, double dy) = 0;

protected:
    /**
     * @brief Constructeur protégé
     */
    IGeometricPoint() = default;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IGEOMETRICPOINT_H
