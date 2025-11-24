/**
 * @file Triangle.cpp
 * @brief Implémentation de la classe Triangle
 */

#include "Triangle.h"
#include "core/models/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include "core/services/geometry/GeometryUtils.h"
#include <QtMath>
#include <QDebug>

namespace LaserCutStudio {
namespace Core {

// Enregistrement automatique dans le Factory Pattern
namespace {
    Patterns::FactoryMixin<IShape>::AutoRegister<Triangle> g_triangleReg;
}

// ===== Constructeurs =====

Triangle::Triangle()
    : IShape()
    , m_centerX(0.0)
    , m_centerY(0.0)
    , m_size(0.0)
{
    qCDebug(logShapes()) << "Triangle created (default)";
}

Triangle::Triangle(double centerX, double centerY, double size)
    : IShape()
    , m_centerX(centerX)
    , m_centerY(centerY)
    , m_size(size)
{
    qCDebug(logShapes()) << "Triangle created at" << centerX << "," << centerY
                         << "with size" << size;
}

Triangle::Triangle(const Triangle& other)
    : IShape(other)
    , m_centerX(other.m_centerX)
    , m_centerY(other.m_centerY)
    , m_size(other.m_size)
{
    qCDebug(logShapes()) << "Triangle copied";
}

// ===== Clone =====

IMPLEMENT_CLONE(Triangle, IShape)

// ===== Setters =====

void Triangle::setCenterX(double centerX)
{
    updateProperty(m_centerX, centerX, &Triangle::centerXChanged, &Triangle::geometryChanged);
}

void Triangle::setCenterY(double centerY)
{
    updateProperty(m_centerY, centerY, &Triangle::centerYChanged, &Triangle::geometryChanged);
}

void Triangle::setSize(double size)
{
    updateProperty(m_size, size, &Triangle::sizeChanged, &Triangle::geometryChanged);
}

// ===== Géométrie =====

double Triangle::getArea() const
{
    // Aire d'un triangle équilatéral: (sqrt(3) / 4) * côté^2
    return (qSqrt(3.0) / 4.0) * m_size * m_size;
}

QRectF Triangle::getBoundingBox() const
{
    // Calculer les 3 sommets pour trouver les min/max
    QList<Point2D> points = getPoints();
    if (points.isEmpty()) {
        return QRectF(m_centerX, m_centerY, 0, 0);
    }

    double minX = points[0].x;
    double maxX = points[0].x;
    double minY = points[0].y;
    double maxY = points[0].y;

    for (const Point2D& p : points) {
        minX = qMin(minX, p.x);
        maxX = qMax(maxX, p.x);
        minY = qMin(minY, p.y);
        maxY = qMax(maxY, p.y);
    }

    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

QList<Point2D> Triangle::getPoints() const
{
    QList<Point2D> points;

    // Triangle équilatéral avec un sommet vers le haut
    // Sommet du haut
    double height = m_size * qSqrt(3.0) / 2.0;
    double halfSize = m_size / 2.0;

    Point2D top(m_centerX, m_centerY - height * 2.0 / 3.0);
    Point2D bottomLeft(m_centerX - halfSize, m_centerY + height / 3.0);
    Point2D bottomRight(m_centerX + halfSize, m_centerY + height / 3.0);

    points.append(top);
    points.append(bottomLeft);
    points.append(bottomRight);

    return points;
}

bool Triangle::containsPoint(const Point2D& point) const
{
    QList<Point2D> vertices = getPoints();
    if (vertices.size() != 3) {
        return false;
    }

    // Méthode des aires: un point est dans le triangle si
    // aire(P,A,B) + aire(P,B,C) + aire(P,C,A) = aire(A,B,C)
    auto triangleArea = [](const Point2D& p1, const Point2D& p2, const Point2D& p3) {
        return qAbs((p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0);
    };

    double totalArea = getArea();
    double area1 = triangleArea(point, vertices[0], vertices[1]);
    double area2 = triangleArea(point, vertices[1], vertices[2]);
    double area3 = triangleArea(point, vertices[2], vertices[0]);

    return qFuzzyCompare(area1 + area2 + area3, totalArea);
}

// ===== Transformations =====

void Triangle::translate(double dx, double dy)
{
    setCenterX(m_centerX + dx);
    setCenterY(m_centerY + dy);
}

void Triangle::rotate(double angle, const Point2D& center)
{
    // Rotation du centre du triangle
    Point2D triangleCenter(m_centerX, m_centerY);
    Utils::RotationMatrix rot = Utils::RotationMatrix::fromDegrees(angle);
    Point2D rotated = Utils::rotatePoint(triangleCenter, center, rot);

    setCenterX(rotated.x);
    setCenterY(rotated.y);

    // Note: Pour un triangle équilatéral, la rotation n'affecte que la position du centre
    // car la forme reste la même après rotation de 120° ou 240°
}

void Triangle::scale(double factorX, double factorY, const Point2D& center)
{
    // Mise à l'échelle du centre
    Point2D triangleCenter(m_centerX, m_centerY);
    Point2D scaled = Utils::scalePoint(triangleCenter, center, factorX, factorY);

    setCenterX(scaled.x);
    setCenterY(scaled.y);

    // Mise à l'échelle de la taille (moyenne des deux facteurs pour conserver l'aspect)
    double avgFactor = (factorX + factorY) / 2.0;
    setSize(m_size * avgFactor);
}

} // namespace Core
} // namespace LaserCutStudio
