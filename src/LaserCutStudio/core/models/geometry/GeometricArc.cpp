#include "GeometricArc.h"
#include <QPainterPath>
#include <cmath>

namespace LaserCutStudio {
namespace Core {

const bool GeometricArc::s_registered = IGeometricElement::registerFactory<GeometricArc>();

GeometricArc::GeometricArc()
    : IGeometricElement(), m_center(nullptr), m_radius(0.0), m_startAngle(0.0), m_endAngle(360.0)
{
}

GeometricArc::GeometricArc(GeometricPoint* center, double radius, double startAngle, double endAngle)
    : IGeometricElement(), m_center(nullptr), m_radius(radius), m_startAngle(startAngle), m_endAngle(endAngle)
{
    setCenter(center);
}

GeometricArc::GeometricArc(const GeometricArc& other)
    : IGeometricElement(), m_center(nullptr), m_radius(other.m_radius),
      m_startAngle(other.m_startAngle), m_endAngle(other.m_endAngle)
{
    if (other.m_center) {
        m_center = static_cast<GeometricPoint*>(other.m_center->clone());
        connectToPoint(m_center);
    }
}

GeometricArc::~GeometricArc()
{
    disconnectFromPoint(m_center);
}

IGeometricElement* GeometricArc::clone() const
{
    return new GeometricArc(*this);
}

QRectF GeometricArc::getBoundingBox() const
{
    if (!isValid()) return QRectF();
    double cx = m_center->x(), cy = m_center->y();
    return QRectF(cx - m_radius, cy - m_radius, 2 * m_radius, 2 * m_radius);
}

bool GeometricArc::isNear(const Point2D& point, double tolerance) const
{
    if (!isValid()) return false;
    double dist = m_center->distance(point);
    return std::abs(dist - m_radius) <= tolerance;
}

QPainterPath GeometricArc::toPainterPath() const
{
    QPainterPath path;
    if (!isValid()) return path;

    QRectF rect = getBoundingBox();
    path.arcMoveTo(rect, m_startAngle);
    path.arcTo(rect, m_startAngle, m_endAngle - m_startAngle);

    return path;
}

QList<Point2D> GeometricArc::getPoints(int resolution) const
{
    QList<Point2D> points;
    if (!isValid()) return points;

    double cx = m_center->x(), cy = m_center->y();
    double angleSpan = m_endAngle - m_startAngle;
    int numPoints = std::max(2, resolution);

    for (int i = 0; i <= numPoints; ++i) {
        double t = static_cast<double>(i) / numPoints;
        double angle = (m_startAngle + t * angleSpan) * M_PI / 180.0;
        points << Point2D(cx + m_radius * std::cos(angle), cy + m_radius * std::sin(angle));
    }

    return points;
}

void GeometricArc::setCenter(GeometricPoint* center)
{
    if (m_center == center) return;
    disconnectFromPoint(m_center);
    m_center = center;
    connectToPoint(m_center);
    emit centerChanged(center);
    emit geometryChanged();
}

void GeometricArc::setRadius(double radius)
{
    updateProperty(m_radius, radius, &GeometricArc::radiusChanged, &GeometricArc::geometryChanged);
}

void GeometricArc::setStartAngle(double angle)
{
    updateProperty(m_startAngle, angle, &GeometricArc::startAngleChanged, &GeometricArc::geometryChanged);
}

void GeometricArc::setEndAngle(double angle)
{
    updateProperty(m_endAngle, angle, &GeometricArc::endAngleChanged, &GeometricArc::geometryChanged);
}

void GeometricArc::connectToPoint(GeometricPoint* point)
{
    if (point) {
        connect(point, &GeometricPoint::geometryChanged, this, &GeometricArc::geometryChanged);
        connect(point, &Interface::aboutToBeDestroyed, this, [this](Interface* destroyed) {
            if (m_center == destroyed) m_center = nullptr;
        });
    }
}

void GeometricArc::disconnectFromPoint(GeometricPoint* point)
{
    if (point) disconnect(point, nullptr, this, nullptr);
}

} // namespace Core
} // namespace LaserCutStudio
