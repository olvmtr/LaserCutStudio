#include "DistanceConstraint.h"
#include "core/models/geometry/IGeometricPoint.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;
class GeometricArc;

const bool DistanceConstraint::s_registered = IConstraint::registerFactory<DistanceConstraint>();

DistanceConstraint::DistanceConstraint()
    : IConstraint(false, 1.0),
      m_point1(nullptr),
      m_point2(nullptr),
      m_distance(0.0)
{
}

DistanceConstraint::DistanceConstraint(IGeometricPoint* point1, IGeometricPoint* point2, double distance, bool locked)
    : IConstraint(locked, 1.0),
      m_point1(nullptr),
      m_point2(nullptr),
      m_distance(distance)
{
    setPoint1(point1);
    setPoint2(point2);
}

DistanceConstraint::DistanceConstraint(const DistanceConstraint& other)
    : IConstraint(other.m_locked, other.m_priority),
      m_point1(nullptr),
      m_point2(nullptr),
      m_distance(other.m_distance)
{
    // Clone les points
    if (other.m_point1) {
        m_point1 = static_cast<GeometricPoint*>(other.m_point1->clone());
        connectToPoint(m_point1);
    }
    if (other.m_point2) {
        m_point2 = static_cast<GeometricPoint*>(other.m_point2->clone());
        connectToPoint(m_point2);
    }
}

DistanceConstraint::~DistanceConstraint()
{
    disconnectFromPoint(m_point1);
    disconnectFromPoint(m_point2);
}

IConstraint* DistanceConstraint::clone() const
{
    return new DistanceConstraint(*this);
}

bool DistanceConstraint::isValid() const
{
    GeometricPoint* pt1 = qobject_cast<GeometricPoint*>(m_point1);
    GeometricPoint* pt2 = qobject_cast<GeometricPoint*>(m_point2);
    return pt1 != nullptr && pt2 != nullptr;
}

bool DistanceConstraint::isSatisfied(double tolerance) const
{
    return error() < tolerance;
}

double DistanceConstraint::error() const
{
    if (!isValid()) {
        return 0.0;
    }

    GeometricPoint* pt1 = qobject_cast<GeometricPoint*>(m_point1);
    GeometricPoint* pt2 = qobject_cast<GeometricPoint*>(m_point2);

    double currentDistance = pt1->distance(*pt2);
    return std::abs(currentDistance - m_distance);
}

void DistanceConstraint::apply()
{
    if (!isValid()) {
        return;
    }

    GeometricPoint* pt1 = qobject_cast<GeometricPoint*>(m_point1);
    GeometricPoint* pt2 = qobject_cast<GeometricPoint*>(m_point2);

    // Calcul de la distance actuelle
    double currentDist = pt1->distance(*pt2);

    // Si distance déjà correcte, rien à faire
    if (std::abs(currentDist - m_distance) < 1e-9) {
        return;
    }

    // Direction point1 → point2
    double dx = pt2->x() - pt1->x();
    double dy = pt2->y() - pt1->y();

    // Normaliser
    if (currentDist > 1e-9) {
        dx /= currentDist;
        dy /= currentDist;
    } else {
        // Points superposés : direction arbitraire
        dx = 1.0;
        dy = 0.0;
    }

    // Erreur à corriger
    double errorValue = currentDist - m_distance;

    // Nombre de points libres
    int freePoints = 0;
    if (!pt1->isLocked()) freePoints++;
    if (!pt2->isLocked()) freePoints++;

    if (freePoints == 0) {
        // Aucun point libre : contrainte non satisfaisable
        return;
    }

    // Correction par point
    double correction = errorValue / freePoints;

    // Appliquer la correction
    if (!pt1->isLocked()) {
        pt1->setX(pt1->x() + dx * correction);
        pt1->setY(pt1->y() + dy * correction);
    }

    if (!pt2->isLocked()) {
        pt2->setX(pt2->x() - dx * correction);
        pt2->setY(pt2->y() - dy * correction);
    }
}

QList<GeometricPoint*> DistanceConstraint::affectedPoints() const
{
    QList<GeometricPoint*> points;
    if (m_point1) {
        GeometricPoint* pt1 = qobject_cast<GeometricPoint*>(m_point1);
        if (pt1) points << pt1;
    }
    if (m_point2) {
        GeometricPoint* pt2 = qobject_cast<GeometricPoint*>(m_point2);
        if (pt2) points << pt2;
    }
    return points;
}

void DistanceConstraint::setPoint1(IGeometricPoint* point)
{
    if (m_point1 == point) {
        return;
    }

    disconnectFromPoint(m_point1);
    m_point1 = point;
    connectToPoint(m_point1);

    emit point1Changed(point);
    emit constraintChanged();
}

void DistanceConstraint::setPoint2(IGeometricPoint* point)
{
    if (m_point2 == point) {
        return;
    }

    disconnectFromPoint(m_point2);
    m_point2 = point;
    connectToPoint(m_point2);

    emit point2Changed(point);
    emit constraintChanged();
}

void DistanceConstraint::setDistance(double distance)
{
    updateProperty(m_distance, distance, &DistanceConstraint::distanceChanged, &DistanceConstraint::constraintChanged);
}

void DistanceConstraint::connectToPoint(IGeometricPoint* point)
{
    if (point) {
        connect(point, &GeometricPoint::geometryChanged,
                this, &DistanceConstraint::constraintChanged);

        connect(point, &Interface::aboutToBeDestroyed,
                this, [this, point](Interface* destroyed) {
            if (m_point1 == destroyed) {
                m_point1 = nullptr;
            }
            if (m_point2 == destroyed) {
                m_point2 = nullptr;
            }
        });
    }
}

void DistanceConstraint::disconnectFromPoint(IGeometricPoint* point)
{
    if (point) {
        disconnect(point, nullptr, this, nullptr);
    }
}

} // namespace Core
} // namespace LaserCutStudio
