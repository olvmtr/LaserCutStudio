#include "GeometricPoint.h"
#include <QPainterPath>
#include <cmath>

namespace LaserCutStudio {
namespace Core {

// Enregistrement automatique dans le Factory
const bool GeometricPoint::s_registered = IGeometricElement::registerFactory<GeometricPoint>();

GeometricPoint::GeometricPoint()
    : IGeometricPoint(),
      m_x(0.0),
      m_y(0.0),
      m_locked(false)
{
}

GeometricPoint::GeometricPoint(double x, double y, bool locked)
    : IGeometricPoint(),
      m_x(x),
      m_y(y),
      m_locked(locked)
{
}

GeometricPoint::GeometricPoint(const GeometricPoint& other)
    : IGeometricPoint(),
      m_x(other.m_x),
      m_y(other.m_y),
      m_locked(other.m_locked)
{
}

IGeometricPoint* GeometricPoint::clone() const
{
    return new GeometricPoint(*this);
}

QRectF GeometricPoint::getBoundingBox() const
{
    // Rectangle centré sur le point (5×5 pixels pour visibilité)
    return QRectF(m_x - 2.5, m_y - 2.5, 5.0, 5.0);
}

bool GeometricPoint::isNear(const Point2D& point, double tolerance) const
{
    return distance(point) <= tolerance;
}

QPainterPath GeometricPoint::toPainterPath() const
{
    QPainterPath path;

    // Dessiner un petit cercle pour représenter le point
    const double radius = 3.0;  // Rayon visuel
    path.addEllipse(QPointF(m_x, m_y), radius, radius);

    return path;
}

QList<Point2D> GeometricPoint::getPoints(int resolution) const
{
    Q_UNUSED(resolution);
    return QList<Point2D>() << Point2D(m_x, m_y);
}

double GeometricPoint::distance(const IGeometricPoint& other) const
{
    double dx = m_x - other.x();
    double dy = m_y - other.y();
    return std::sqrt(dx * dx + dy * dy);
}

double GeometricPoint::distance(const Point2D& other) const
{
    double dx = m_x - other.x;
    double dy = m_y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

void GeometricPoint::setX(double x)
{
    updateProperty(m_x, x, &GeometricPoint::xChanged, &GeometricPoint::geometryChanged);
}

void GeometricPoint::setY(double y)
{
    updateProperty(m_y, y, &GeometricPoint::yChanged, &GeometricPoint::geometryChanged);
}

void GeometricPoint::setPosition(double x, double y)
{
    setX(x);
    setY(y);
}

void GeometricPoint::setPosition(const Point2D& pos)
{
    setPosition(pos.x, pos.y);
}

void GeometricPoint::setLocked(bool locked)
{
    if (m_locked != locked) {
        m_locked = locked;
        emit lockedChanged(locked);
        emit geometryChanged();
    }
}

void GeometricPoint::translate(double dx, double dy)
{
    setX(m_x + dx);
    setY(m_y + dy);
}

} // namespace Core
} // namespace LaserCutStudio
