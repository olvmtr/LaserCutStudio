#ifndef GEOMETRICSEGMENT_H
#define GEOMETRICSEGMENT_H

#include "core/models/geometry/IGeometricElement.h"
#include "core/models/geometry/GeometricPoint.h"
#include "core/models/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un segment entre deux points géométriques
 *
 * GeometricSegment est défini par deux points (start et end).
 * Le segment suit automatiquement les mouvements des points.
 *
 * ## Propriétés Calculées
 *
 * - **Longueur** : Distance entre start et end
 * - **Angle** : Angle par rapport à l'axe X (en degrés)
 * - **Point milieu** : Centre du segment
 * - **Vecteur directeur** : Direction normalisée
 *
 * ## Gestion des Points
 *
 * Le segment écoute les signaux des points pour se mettre à jour :
 * - Si un point bouge → `geometryChanged()` émis
 * - Si un point est détruit → segment invalide (pointeurs nullptr)
 *
 * @note Utilise PropertyMixin pour setters avec signaux automatiques
 */
class GeometricSegment : public IGeometricElement,
                         protected Patterns::PropertyMixin<GeometricSegment>
{
    Q_OBJECT

    Q_PROPERTY(GeometricPoint* startPoint READ startPoint WRITE setStartPoint NOTIFY startPointChanged)
    Q_PROPERTY(GeometricPoint* endPoint READ endPoint WRITE setEndPoint NOTIFY endPointChanged)
    Q_PROPERTY(double length READ length NOTIFY geometryChanged)
    Q_PROPERTY(double angle READ angle NOTIFY geometryChanged)

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Geometry")
    Q_CLASSINFO("Description", "Geometric segment between two points")

signals:
    void startPointChanged(GeometricPoint* newStartPoint);
    void endPointChanged(GeometricPoint* newEndPoint);

public:
    /**
     * @brief Constructeur par défaut (segment nul)
     */
    GeometricSegment();

    /**
     * @brief Constructeur avec points
     * @param start Point de départ
     * @param end Point d'arrivée
     */
    GeometricSegment(GeometricPoint* start, GeometricPoint* end);

    /**
     * @brief Constructeur de copie
     * @note Clone les points (nouveaux points indépendants)
     */
    GeometricSegment(const GeometricSegment& other);

    /**
     * @brief Destructeur
     */
    ~GeometricSegment() override;

    /**
     * @brief Clone le segment
     * @note Clone également les points
     */
    IGeometricElement* clone() const override;

    /**
     * @brief Déclare automatiquement staticTypeName() et getTypeName()
     */
    DECLARE_TYPE_NAME(GeometricSegment)

    /**
     * @brief Calcule le rectangle englobant
     */
    QRectF getBoundingBox() const override;

    /**
     * @brief Vérifie si un point est proche du segment (pour sélection)
     */
    bool isNear(const Point2D& point, double tolerance = 5.0) const override;

    /**
     * @brief Convertit en QPainterPath (ligne)
     */
    QPainterPath toPainterPath() const override;

    /**
     * @brief Obtient les points (start, end)
     */
    QList<Point2D> getPoints(int resolution = 32) const override;

    // Getters
    GeometricPoint* startPoint() const { return m_startPoint; }
    GeometricPoint* endPoint() const { return m_endPoint; }

    /**
     * @brief Calcule la longueur du segment
     * @return Distance entre start et end
     */
    double length() const;

    /**
     * @brief Calcule l'angle du segment par rapport à l'axe X
     * @return Angle en degrés [0, 360)
     */
    double angle() const;

    /**
     * @brief Calcule le point milieu du segment
     */
    Point2D midpoint() const;

    /**
     * @brief Calcule le vecteur directeur normalisé
     * @return Vecteur (dx, dy) de longueur 1
     */
    Point2D direction() const;

    /**
     * @brief Vérifie si le segment est valide (points non-null)
     */
    bool isValid() const { return m_startPoint != nullptr && m_endPoint != nullptr; }

    // Setters
    /**
     * @brief Définit le point de départ
     * @param start Nouveau point de départ
     */
    void setStartPoint(GeometricPoint* start);

    /**
     * @brief Définit le point d'arrivée
     * @param end Nouveau point d'arrivée
     */
    void setEndPoint(GeometricPoint* end);

    /**
     * @brief Calcule la distance d'un point au segment
     * @param point Point à tester
     * @return Distance minimale au segment
     */
    double distanceToPoint(const Point2D& point) const;

private:
    /**
     * @brief Connecte aux signaux d'un point
     */
    void connectToPoint(GeometricPoint* point);

    /**
     * @brief Déconnecte des signaux d'un point
     */
    void disconnectFromPoint(GeometricPoint* point);

    GeometricPoint* m_startPoint = nullptr;  ///< Point de départ
    GeometricPoint* m_endPoint = nullptr;    ///< Point d'arrivée

    // Enregistrement automatique dans le Factory
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // GEOMETRICSEGMENT_H
