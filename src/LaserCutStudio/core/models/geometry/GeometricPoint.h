#ifndef GEOMETRICPOINT_H
#define GEOMETRICPOINT_H

#include "core/models/geometry/IGeometricElement.h"
#include "core/infrastructure/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un point géométrique 2D avec contraintes
 *
 * GeometricPoint est l'élément fondamental du système de géométrie contrainte.
 * Tous les autres éléments (segments, arcs) sont définis par des points.
 *
 * ## Degrés de Liberté
 *
 * Un point peut avoir différents degrés de liberté :
 * - **0 DOF** : Point fixe (x et y verrouillés)
 * - **1 DOF** : Point contraint à une ligne/courbe
 * - **2 DOF** : Point libre (x et y modifiables)
 *
 * ## Verrouillage
 *
 * Le verrouillage empêche le solveur de modifier le point :
 * - `locked = true` : Point fixe (x, y non modifiables)
 * - `locked = false` : Point libre (ajustable par le solveur)
 *
 * @note Utilise PropertyMixin pour setters avec signaux automatiques
 */
class GeometricPoint : public IGeometricElement,
                       protected Patterns::PropertyMixin<GeometricPoint>
{
    Q_OBJECT

    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(bool locked READ isLocked WRITE setLocked NOTIFY lockedChanged)

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Geometry")
    Q_CLASSINFO("Description", "Geometric 2D point with constraints support")

signals:
    void xChanged(double newX);
    void yChanged(double newY);
    void lockedChanged(bool locked);

public:
    /**
     * @brief Constructeur par défaut (origine)
     */
    GeometricPoint();

    /**
     * @brief Constructeur avec coordonnées
     * @param x Coordonnée X
     * @param y Coordonnée Y
     * @param locked État de verrouillage (défaut : false)
     */
    GeometricPoint(double x, double y, bool locked = false);

    /**
     * @brief Constructeur de copie
     */
    GeometricPoint(const GeometricPoint& other);

    /**
     * @brief Destructeur
     */
    ~GeometricPoint() override = default;

    /**
     * @brief Clone le point
     */
    IGeometricElement* clone() const override;

    /**
     * @brief Déclare automatiquement staticTypeName() et getTypeName()
     */
    DECLARE_TYPE_NAME(GeometricPoint)

    /**
     * @brief Calcule le rectangle englobant (point unique)
     */
    QRectF getBoundingBox() const override;

    /**
     * @brief Vérifie si un point est proche (pour sélection)
     */
    bool isNear(const Point2D& point, double tolerance = 5.0) const override;

    /**
     * @brief Convertit en QPainterPath (petit cercle)
     */
    QPainterPath toPainterPath() const override;

    /**
     * @brief Obtient les points (un seul point)
     */
    QList<Point2D> getPoints(int resolution = 32) const override;

    // Getters
    double x() const { return m_x; }
    double y() const { return m_y; }
    bool isLocked() const { return m_locked; }

    /**
     * @brief Obtient la position comme Point2D
     */
    Point2D position() const { return Point2D(m_x, m_y); }

    /**
     * @brief Calcule la distance avec un autre point
     */
    double distance(const GeometricPoint& other) const;

    /**
     * @brief Calcule la distance avec un Point2D
     */
    double distance(const Point2D& other) const;

    // Setters
    /**
     * @brief Définit la coordonnée X
     * @param x Nouvelle coordonnée X
     */
    void setX(double x);

    /**
     * @brief Définit la coordonnée Y
     * @param y Nouvelle coordonnée Y
     */
    void setY(double y);

    /**
     * @brief Définit la position (x, y)
     * @param x Nouvelle coordonnée X
     * @param y Nouvelle coordonnée Y
     */
    void setPosition(double x, double y);

    /**
     * @brief Définit la position depuis un Point2D
     * @param pos Nouvelle position
     */
    void setPosition(const Point2D& pos);

    /**
     * @brief Définit l'état de verrouillage
     * @param locked true pour verrouiller (empêcher modifications par solveur)
     */
    void setLocked(bool locked);

    /**
     * @brief Translate le point
     * @param dx Déplacement en X
     * @param dy Déplacement en Y
     */
    void translate(double dx, double dy);

private:
    double m_x = 0.0;       ///< Coordonnée X
    double m_y = 0.0;       ///< Coordonnée Y
    bool m_locked = false;  ///< État de verrouillage

    // Enregistrement automatique dans le Factory
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // GEOMETRICPOINT_H
