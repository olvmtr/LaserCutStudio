#include "Rectangle.h"
#include "../utils/GeometryUtils.h"
#include "../patterns/ClonableMixin.h"
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

IMPLEMENT_CLONE(Rectangle, IShape)

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
    // Calcul du centre du rectangle
    Point2D rectCenter(m_x + m_width / 2.0, m_y + m_height / 2.0);

    // Rotation du centre avec GeometryUtils
    Utils::RotationMatrix rotation = Utils::RotationMatrix::fromDegrees(angle);
    Point2D rotatedCenter = Utils::rotatePoint(rectCenter, center, rotation);

    // Mise à jour de la position (coin supérieur gauche)
    m_x = rotatedCenter.x - m_width / 2.0;
    m_y = rotatedCenter.y - m_height / 2.0;
}

void Rectangle::scale(double scaleX, double scaleY, const Point2D& center)
{
    // Calcul du centre du rectangle
    Point2D rectCenter(m_x + m_width / 2.0, m_y + m_height / 2.0);

    // Mise à l'échelle du centre avec GeometryUtils
    Point2D scaledCenter = Utils::scalePoint(rectCenter, center, scaleX, scaleY);

    // Mise à l'échelle des dimensions
    m_width *= scaleX;
    m_height *= scaleY;

    // Mise à jour de la position (coin supérieur gauche)
    m_x = scaledCenter.x - m_width / 2.0;
    m_y = scaledCenter.y - m_height / 2.0;
}

void Rectangle::setX(double x)
{
    updateProperty(m_x, x, &Rectangle::xChanged, &Rectangle::geometryChanged);
}

void Rectangle::setY(double y)
{
    updateProperty(m_y, y, &Rectangle::yChanged, &Rectangle::geometryChanged);
}

void Rectangle::setWidth(double width)
{
    updateProperty(m_width, width, &Rectangle::widthChanged, &Rectangle::geometryChanged);
}

void Rectangle::setHeight(double height)
{
    updateProperty(m_height, height, &Rectangle::heightChanged, &Rectangle::geometryChanged);
}

// Auto-enregistrement dans le Factory Pattern
const bool Rectangle::s_registered = IShape::registerFactory<Rectangle>();

} // namespace Core
} // namespace LaserCutStudio
