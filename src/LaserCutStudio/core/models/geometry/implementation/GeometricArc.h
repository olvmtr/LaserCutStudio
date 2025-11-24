#ifndef GEOMETRICARC_H
#define GEOMETRICARC_H

#include "core/models/geometry/IGeometricArc.h"
#include "core/models/geometry/IGeometricPoint.h"
#include "core/models/patterns/properties/PropertyMixin.h"

// Forward declaration
namespace LaserCutStudio {
namespace Core {
    class GeometricPoint;
}
}

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un arc de cercle géométrique
 *
 * GeometricArc est défini par un centre, un rayon et deux angles.
 * Utilisé pour les courbes, arrondis, encoches circulaires, etc.
 *
 * @note Pour l'instant version simplifiée, à enrichir plus tard
 */
class GeometricArc : public IGeometricArc,
                     protected Patterns::PropertyMixin<GeometricArc>
{
    Q_OBJECT

    Q_PROPERTY(IGeometricPoint* center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(double startAngle READ startAngle WRITE setStartAngle NOTIFY startAngleChanged)
    Q_PROPERTY(double endAngle READ endAngle WRITE setEndAngle NOTIFY endAngleChanged)

signals:
    void centerChanged(IGeometricPoint* newCenter);
    void radiusChanged(double newRadius);
    void startAngleChanged(double newStartAngle);
    void endAngleChanged(double newEndAngle);

public:
    GeometricArc();
    GeometricArc(IGeometricPoint* center, double radius, double startAngle = 0.0, double endAngle = 360.0);
    GeometricArc(const GeometricArc& other);
    ~GeometricArc() override;

    IGeometricArc* clone() const override;
    DECLARE_TYPE_NAME(GeometricArc)

    QRectF getBoundingBox() const override;
    bool isNear(const Point2D& point, double tolerance = 5.0) const override;
    QPainterPath toPainterPath() const override;
    QList<Point2D> getPoints(int resolution = 32) const override;

    // Getters
    IGeometricPoint* center() const override { return m_center; }
    double radius() const override { return m_radius; }
    double startAngle() const override { return m_startAngle; }
    double endAngle() const override { return m_endAngle; }
    bool isValid() const override { return m_center != nullptr && m_radius > 0.0; }

    // Setters
    void setCenter(IGeometricPoint* center) override;
    void setRadius(double radius) override;
    void setStartAngle(double angle) override;
    void setEndAngle(double angle) override;

private:
    void connectToPoint(IGeometricPoint* point);
    void disconnectFromPoint(IGeometricPoint* point);

    IGeometricPoint* m_center = nullptr;
    double m_radius = 0.0;
    double m_startAngle = 0.0;   // Degrés
    double m_endAngle = 360.0;   // Degrés

    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // GEOMETRICARC_H
