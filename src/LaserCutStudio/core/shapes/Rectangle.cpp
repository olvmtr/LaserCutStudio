#include "Rectangle.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

Rectangle::Rectangle()
    : IShape()
    , m_x(0.0)
    , m_y(0.0)
    , m_width(10.0)
    , m_height(10.0)
{
}

Rectangle::Rectangle(double x, double y, double width, double height)
    : IShape()
    , m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
{
}

Rectangle::Rectangle(const Rectangle& other)
    : IShape(other)
    , m_x(other.m_x)
    , m_y(other.m_y)
    , m_width(other.m_width)
    , m_height(other.m_height)
{
}

IShape* Rectangle::clone() const
{
    return new Rectangle(*this);
}

double Rectangle::getArea() const
{
    return m_width * m_height;
}

QRectF Rectangle::getBoundingBox() const
{
    return QRectF(m_x, m_y, m_width, m_height);
}

bool Rectangle::containsPoint(const Point2D& point) const
{
    return point.x >= m_x && point.x <= m_x + m_width &&
           point.y >= m_y && point.y <= m_y + m_height;
}

QList<Point2D> Rectangle::getPoints() const
{
    QList<Point2D> points;
    points.append(Point2D(m_x, m_y));                      // Coin supérieur gauche
    points.append(Point2D(m_x + m_width, m_y));            // Coin supérieur droit
    points.append(Point2D(m_x + m_width, m_y + m_height)); // Coin inférieur droit
    points.append(Point2D(m_x, m_y + m_height));           // Coin inférieur gauche
    return points;
}

void Rectangle::translate(double dx, double dy)
{
    m_x += dx;
    m_y += dy;
}

void Rectangle::rotate(double angle, const Point2D& center)
{
    // Conversion de l'angle en radians
    double rad = angle * M_PI / 180.0;
    double cosAngle = std::cos(rad);
    double sinAngle = std::sin(rad);

    // Pour un rectangle, on fait pivoter le centre
    double centerX = m_x + m_width / 2.0;
    double centerY = m_y + m_height / 2.0;

    // Translation vers l'origine
    double tx = centerX - center.x;
    double ty = centerY - center.y;

    // Rotation
    double rotatedX = tx * cosAngle - ty * sinAngle;
    double rotatedY = tx * sinAngle + ty * cosAngle;

    // Translation retour
    centerX = rotatedX + center.x;
    centerY = rotatedY + center.y;

    // Mise à jour de la position (coin supérieur gauche)
    m_x = centerX - m_width / 2.0;
    m_y = centerY - m_height / 2.0;
}

void Rectangle::scale(double scaleX, double scaleY, const Point2D& center)
{
    // Calcul du centre du rectangle
    double centerX = m_x + m_width / 2.0;
    double centerY = m_y + m_height / 2.0;

    // Translation vers l'origine
    double tx = centerX - center.x;
    double ty = centerY - center.y;

    // Mise à l'échelle
    tx *= scaleX;
    ty *= scaleY;
    m_width *= scaleX;
    m_height *= scaleY;

    // Translation retour
    centerX = tx + center.x;
    centerY = ty + center.y;

    // Mise à jour de la position
    m_x = centerX - m_width / 2.0;
    m_y = centerY - m_height / 2.0;
}

void Rectangle::setX(double x)
{
    if (!qFuzzyCompare(m_x, x)) {
        m_x = x;
        emit xChanged(x);
        emit geometryChanged();
    }
}

void Rectangle::setY(double y)
{
    if (!qFuzzyCompare(m_y, y)) {
        m_y = y;
        emit yChanged(y);
        emit geometryChanged();
    }
}

void Rectangle::setWidth(double width)
{
    if (!qFuzzyCompare(m_width, width)) {
        m_width = width;
        emit widthChanged(width);
        emit geometryChanged();
    }
}

void Rectangle::setHeight(double height)
{
    if (!qFuzzyCompare(m_height, height)) {
        m_height = height;
        emit heightChanged(height);
        emit geometryChanged();
    }
}

// Auto-enregistrement dans le Factory Pattern
const bool Rectangle::s_registered = IShape::registerFactory<Rectangle>();

} // namespace Core
} // namespace LaserCutStudio
