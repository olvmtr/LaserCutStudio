#include "core/models/shapes/Circle.h"
#include "core/services/geometry/GeometryUtils.h"
#include "core/models/patterns/prototype/ClonableMixin.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace LaserCutStudio {
namespace Core {

Circle::Circle()
    : IShape()
    , m_centerX(0.0)
    , m_centerY(0.0)
    , m_radius(5.0)
{
}

Circle::Circle(double centerX, double centerY, double radius)
    : IShape()
    , m_centerX(centerX)
    , m_centerY(centerY)
    , m_radius(radius)
{
}

Circle::Circle(const Circle& other)
    : IShape(other)
    , m_centerX(other.m_centerX)
    , m_centerY(other.m_centerY)
    , m_radius(other.m_radius)
{
}

IMPLEMENT_CLONE(Circle, IShape)

double Circle::getArea() const
{
    return M_PI * m_radius * m_radius;
}

QRectF Circle::getBoundingBox() const
{
    return QRectF(m_centerX - m_radius, m_centerY - m_radius,
                  m_radius * 2.0, m_radius * 2.0);
}

bool Circle::containsPoint(const Point2D& point) const
{
    double dx = point.x - m_centerX;
    double dy = point.y - m_centerY;
    return (dx * dx + dy * dy) <= (m_radius * m_radius);
}

QList<Point2D> Circle::getPoints() const
{
    // Retourne des points échantillonnés sur le cercle (32 points)
    QList<Point2D> points;
    const int numPoints = 32;
    for (int i = 0; i < numPoints; ++i) {
        double angle = 2.0 * M_PI * i / numPoints;
        points.append(Point2D(
            m_centerX + m_radius * std::cos(angle),
            m_centerY + m_radius * std::sin(angle)
        ));
    }
    return points;
}

void Circle::translate(double dx, double dy)
{
    m_centerX += dx;
    m_centerY += dy;
}

void Circle::rotate(double angle, const Point2D& center)
{
    // Rotation du centre du cercle avec GeometryUtils
    Point2D circleCenter(m_centerX, m_centerY);
    Utils::RotationMatrix rotation = Utils::RotationMatrix::fromDegrees(angle);
    Point2D rotatedCenter = Utils::rotatePoint(circleCenter, center, rotation);

    m_centerX = rotatedCenter.x;
    m_centerY = rotatedCenter.y;
}

void Circle::scale(double scaleX, double scaleY, const Point2D& center)
{
    // Mise à l'échelle du centre avec GeometryUtils
    Point2D circleCenter(m_centerX, m_centerY);
    Point2D scaledCenter = Utils::scalePoint(circleCenter, center, scaleX, scaleY);

    m_centerX = scaledCenter.x;
    m_centerY = scaledCenter.y;

    // Mise à l'échelle du rayon (moyenne des deux échelles)
    m_radius *= (scaleX + scaleY) / 2.0;
}

void Circle::setCenterX(double x)
{
    updateProperty(m_centerX, x, &Circle::centerXChanged, &Circle::geometryChanged);
}

void Circle::setCenterY(double y)
{
    updateProperty(m_centerY, y, &Circle::centerYChanged, &Circle::geometryChanged);
}

void Circle::setRadius(double radius)
{
    updateProperty(m_radius, radius, &Circle::radiusChanged, &Circle::geometryChanged);
}

// Auto-enregistrement dans le Factory Pattern
const bool Circle::s_registered = IShape::registerFactory<Circle>();

} // namespace Core
} // namespace LaserCutStudio
