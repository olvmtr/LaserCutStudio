#include "FixedPointConstraint.h"
#include "core/models/geometry/IGeometricPoint.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;
class GeometricArc;

const bool FixedPointConstraint::s_registered = IConstraint::registerFactory<FixedPointConstraint>();

FixedPointConstraint::FixedPointConstraint()
    : IConstraint(true, 1.0), m_point(nullptr), m_x(0.0), m_y(0.0)
{
}

FixedPointConstraint::FixedPointConstraint(IGeometricPoint* point, double x, double y, bool locked)
    : IConstraint(locked, 1.0), m_point(point), m_x(x), m_y(y)
{
}

FixedPointConstraint::FixedPointConstraint(const FixedPointConstraint& other)
    : IConstraint(other.m_locked, other.m_priority), m_point(nullptr), m_x(other.m_x), m_y(other.m_y)
{
    if (other.m_point) {
        m_point = static_cast<GeometricPoint*>(other.m_point->clone());
    }
}

FixedPointConstraint::~FixedPointConstraint()
{
}

IConstraint* FixedPointConstraint::clone() const
{
    return new FixedPointConstraint(*this);
}

bool FixedPointConstraint::isSatisfied(double tolerance) const
{
    return error() < tolerance;
}

double FixedPointConstraint::error() const
{
    if (!isValid()) return 0.0;

    double dx = m_point->x() - m_x;
    double dy = m_point->y() - m_y;
    return std::sqrt(dx * dx + dy * dy);
}

void FixedPointConstraint::apply()
{
    if (!isValid() || m_point->isLocked()) return;

    // Force la position fixe
    m_point->setX(m_x);
    m_point->setY(m_y);
}

QList<GeometricPoint*> FixedPointConstraint::affectedPoints() const
{
    QList<GeometricPoint*> points;
    if (m_point) points << qobject_cast<GeometricPoint*>(m_point);
    return points;
}

void FixedPointConstraint::setPoint(IGeometricPoint* point)
{
    if (m_point == point) return;
    m_point = point;
    emit pointChanged(point);
    emit constraintChanged();
}

void FixedPointConstraint::setX(double x)
{
    updateProperty(m_x, x, &FixedPointConstraint::xChanged, &FixedPointConstraint::constraintChanged);
}

void FixedPointConstraint::setY(double y)
{
    updateProperty(m_y, y, &FixedPointConstraint::yChanged, &FixedPointConstraint::constraintChanged);
}

void FixedPointConstraint::setPosition(double x, double y)
{
    setX(x);
    setY(y);
}

} // namespace Core
} // namespace LaserCutStudio
