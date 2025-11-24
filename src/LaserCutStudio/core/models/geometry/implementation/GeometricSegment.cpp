#include "GeometricSegment.h"
#include <QPainterPath>
#include <cmath>

namespace LaserCutStudio {
namespace Core {

// Enregistrement automatique dans le Factory
const bool GeometricSegment::s_registered = IGeometricElement::registerFactory<GeometricSegment>();

GeometricSegment::GeometricSegment()
    : IGeometricElement(),
      m_startPoint(nullptr),
      m_endPoint(nullptr)
{
}

GeometricSegment::GeometricSegment(GeometricPoint* start, GeometricPoint* end)
    : IGeometricElement(),
      m_startPoint(nullptr),
      m_endPoint(nullptr)
{
    setStartPoint(start);
    setEndPoint(end);
}

GeometricSegment::GeometricSegment(const GeometricSegment& other)
    : IGeometricElement(),
      m_startPoint(nullptr),
      m_endPoint(nullptr)
{
    // Clone les points (nouveaux points indépendants)
    if (other.m_startPoint) {
        m_startPoint = static_cast<GeometricPoint*>(other.m_startPoint->clone());
        connectToPoint(m_startPoint);
    }
    if (other.m_endPoint) {
        m_endPoint = static_cast<GeometricPoint*>(other.m_endPoint->clone());
        connectToPoint(m_endPoint);
    }
}

GeometricSegment::~GeometricSegment()
{
    disconnectFromPoint(m_startPoint);
    disconnectFromPoint(m_endPoint);
}

IGeometricElement* GeometricSegment::clone() const
{
    return new GeometricSegment(*this);
}

QRectF GeometricSegment::getBoundingBox() const
{
    if (!isValid()) {
        return QRectF();
    }

    double x1 = m_startPoint->x();
    double y1 = m_startPoint->y();
    double x2 = m_endPoint->x();
    double y2 = m_endPoint->y();

    double left = std::min(x1, x2);
    double top = std::min(y1, y2);
    double right = std::max(x1, x2);
    double bottom = std::max(y1, y2);

    return QRectF(left, top, right - left, bottom - top);
}

bool GeometricSegment::isNear(const Point2D& point, double tolerance) const
{
    if (!isValid()) {
        return false;
    }

    return distanceToPoint(point) <= tolerance;
}

QPainterPath GeometricSegment::toPainterPath() const
{
    QPainterPath path;

    if (!isValid()) {
        return path;
    }

    path.moveTo(m_startPoint->x(), m_startPoint->y());
    path.lineTo(m_endPoint->x(), m_endPoint->y());

    return path;
}

QList<Point2D> GeometricSegment::getPoints(int resolution) const
{
    Q_UNUSED(resolution);

    QList<Point2D> points;
    if (!isValid()) {
        return points;
    }

    points << m_startPoint->position();
    points << m_endPoint->position();

    return points;
}

double GeometricSegment::length() const
{
    if (!isValid()) {
        return 0.0;
    }

    return m_startPoint->distance(*m_endPoint);
}

double GeometricSegment::angle() const
{
    if (!isValid()) {
        return 0.0;
    }

    double dx = m_endPoint->x() - m_startPoint->x();
    double dy = m_endPoint->y() - m_startPoint->y();

    double angleRad = std::atan2(dy, dx);
    double angleDeg = angleRad * 180.0 / M_PI;

    // Normaliser [0, 360)
    if (angleDeg < 0.0) {
        angleDeg += 360.0;
    }

    return angleDeg;
}

Point2D GeometricSegment::midpoint() const
{
    if (!isValid()) {
        return Point2D(0.0, 0.0);
    }

    double mx = (m_startPoint->x() + m_endPoint->x()) / 2.0;
    double my = (m_startPoint->y() + m_endPoint->y()) / 2.0;

    return Point2D(mx, my);
}

Point2D GeometricSegment::direction() const
{
    if (!isValid()) {
        return Point2D(1.0, 0.0);
    }

    double len = length();
    if (len < 1e-9) {
        return Point2D(1.0, 0.0);
    }

    double dx = (m_endPoint->x() - m_startPoint->x()) / len;
    double dy = (m_endPoint->y() - m_startPoint->y()) / len;

    return Point2D(dx, dy);
}

void GeometricSegment::setStartPoint(GeometricPoint* start)
{
    if (m_startPoint == start) {
        return;
    }

    disconnectFromPoint(m_startPoint);
    m_startPoint = start;
    connectToPoint(m_startPoint);

    emit startPointChanged(start);
    emit geometryChanged();
}

void GeometricSegment::setEndPoint(GeometricPoint* end)
{
    if (m_endPoint == end) {
        return;
    }

    disconnectFromPoint(m_endPoint);
    m_endPoint = end;
    connectToPoint(m_endPoint);

    emit endPointChanged(end);
    emit geometryChanged();
}

double GeometricSegment::distanceToPoint(const Point2D& point) const
{
    if (!isValid()) {
        return std::numeric_limits<double>::max();
    }

    Point2D start = m_startPoint->position();
    Point2D end = m_endPoint->position();

    // Vecteur segment
    double dx = end.x - start.x;
    double dy = end.y - start.y;

    // Longueur au carré
    double lengthSq = dx * dx + dy * dy;

    if (lengthSq < 1e-9) {
        // Segment dégénéré (point)
        return m_startPoint->distance(point);
    }

    // Projection du point sur la ligne
    double t = ((point.x - start.x) * dx + (point.y - start.y) * dy) / lengthSq;

    // Clamper t à [0, 1] pour rester sur le segment
    t = std::max(0.0, std::min(1.0, t));

    // Point le plus proche sur le segment
    Point2D closest(start.x + t * dx, start.y + t * dy);

    // Distance
    return point.distance(closest);
}

void GeometricSegment::connectToPoint(GeometricPoint* point)
{
    if (point) {
        connect(point, &GeometricPoint::geometryChanged,
                this, &GeometricSegment::geometryChanged);

        connect(point, &Interface::aboutToBeDestroyed,
                this, [this, point](Interface* destroyed) {
            if (m_startPoint == destroyed) {
                m_startPoint = nullptr;
            }
            if (m_endPoint == destroyed) {
                m_endPoint = nullptr;
            }
        });
    }
}

void GeometricSegment::disconnectFromPoint(GeometricPoint* point)
{
    if (point) {
        disconnect(point, nullptr, this, nullptr);
    }
}

} // namespace Core
} // namespace LaserCutStudio
