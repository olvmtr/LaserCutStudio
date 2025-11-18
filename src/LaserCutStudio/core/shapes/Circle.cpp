#include "Circle.h"
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

IShape* Circle::clone() const
{
    return new Circle(*this);
}

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
    // Rotation du centre du cercle autour du point
    double rad = angle * M_PI / 180.0;
    double cosAngle = std::cos(rad);
    double sinAngle = std::sin(rad);

    double tx = m_centerX - center.x;
    double ty = m_centerY - center.y;

    m_centerX = tx * cosAngle - ty * sinAngle + center.x;
    m_centerY = tx * sinAngle + ty * cosAngle + center.y;
}

void Circle::scale(double scaleX, double scaleY, const Point2D& center)
{
    // Translation du centre
    double tx = m_centerX - center.x;
    double ty = m_centerY - center.y;

    // Mise à l'échelle
    tx *= scaleX;
    ty *= scaleY;
    m_centerX = tx + center.x;
    m_centerY = ty + center.y;

    // Mise à l'échelle du rayon (moyenne des deux échelles)
    m_radius *= (scaleX + scaleY) / 2.0;
}

void Circle::setCenterX(double x)
{
    if (!qFuzzyCompare(m_centerX, x)) {
        m_centerX = x;
        emit centerXChanged(x);
        emit geometryChanged();
    }
}

void Circle::setCenterY(double y)
{
    if (!qFuzzyCompare(m_centerY, y)) {
        m_centerY = y;
        emit centerYChanged(y);
        emit geometryChanged();
    }
}

void Circle::setRadius(double radius)
{
    if (!qFuzzyCompare(m_radius, radius)) {
        m_radius = radius;
        emit radiusChanged(radius);
        emit geometryChanged();
    }
}

// Auto-enregistrement dans le Factory Pattern
const bool Circle::s_registered = IShape::registerFactory<Circle>();

} // namespace Core
} // namespace LaserCutStudio
