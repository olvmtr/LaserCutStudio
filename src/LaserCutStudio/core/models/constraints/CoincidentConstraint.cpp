#include "CoincidentConstraint.h"
#include "../geometry/IGeometricSegment.h"
#include "../geometry/IGeometricPoint.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;
class GeometricArc;

// Enregistrement automatique dans le Factory Pattern
const bool CoincidentConstraint::s_registered =
    IConstraint::registerFactory<CoincidentConstraint>();

CoincidentConstraint::CoincidentConstraint(IGeometricPoint* point1,
                                           IGeometricPoint* point2,
                                           bool locked,
                                           QObject* parent)
    : IConstraint(parent)
    , m_point1(nullptr)
    , m_point2(nullptr)
{
    setLocked(locked);
    setPoint1(point1);
    setPoint2(point2);
}

CoincidentConstraint::~CoincidentConstraint()
{
    disconnectFromPoint(m_point1);
    disconnectFromPoint(m_point2);
}

void CoincidentConstraint::setPoint1(IGeometricPoint* point)
{
    if (m_point1 == point) return;

    disconnectFromPoint(m_point1);
    m_point1 = point;
    connectToPoint(m_point1);

    emit point1Changed(point);
    emit constraintChanged();
}

void CoincidentConstraint::setPoint2(IGeometricPoint* point)
{
    if (m_point2 == point) return;

    disconnectFromPoint(m_point2);
    m_point2 = point;
    connectToPoint(m_point2);

    emit point2Changed(point);
    emit constraintChanged();
}

void CoincidentConstraint::connectToPoint(IGeometricPoint* point)
{
    if (!point) return;

    connect(point, &IGeometricElement::geometryChanged,
            this, &CoincidentConstraint::constraintChanged);
    connect(point, &Interface::aboutToBeDestroyed,
            this, [this](Interface* destroyedPoint) {
        if (m_point1 == destroyedPoint) m_point1 = nullptr;
        if (m_point2 == destroyedPoint) m_point2 = nullptr;
    });
}

void CoincidentConstraint::disconnectFromPoint(IGeometricPoint* point)
{
    if (!point) return;
    QObject::disconnect(point, nullptr, this, nullptr);
}

bool CoincidentConstraint::isValid() const
{
    return m_point1 && m_point2;
}

bool CoincidentConstraint::isSatisfied(double tolerance) const
{
    return error() < tolerance;
}

double CoincidentConstraint::error() const
{
    if (!isValid()) return 0.0;

    // L'erreur est la distance entre les deux points
    return m_point1->distance(*m_point2);
}

void CoincidentConstraint::apply()
{
    if (!isValid()) return;

    // Si les deux points sont verrouillés, on ne peut rien faire
    if (m_point1->isLocked() && m_point2->isLocked()) return;

    double correctionFactor = isLocked() ? 1.0 : 0.5;

    if (!m_point1->isLocked() && !m_point2->isLocked()) {
        // Les deux points sont libres : les déplacer vers leur milieu
        double midX = (m_point1->x() + m_point2->x()) / 2.0;
        double midY = (m_point1->y() + m_point2->y()) / 2.0;

        double dx1 = (midX - m_point1->x()) * correctionFactor;
        double dy1 = (midY - m_point1->y()) * correctionFactor;
        m_point1->setX(m_point1->x() + dx1);
        m_point1->setY(m_point1->y() + dy1);

        double dx2 = (midX - m_point2->x()) * correctionFactor;
        double dy2 = (midY - m_point2->y()) * correctionFactor;
        m_point2->setX(m_point2->x() + dx2);
        m_point2->setY(m_point2->y() + dy2);
    } else if (!m_point1->isLocked()) {
        // Seul point1 est libre : le déplacer vers point2
        double dx = (m_point2->x() - m_point1->x()) * correctionFactor;
        double dy = (m_point2->y() - m_point1->y()) * correctionFactor;
        m_point1->setX(m_point1->x() + dx);
        m_point1->setY(m_point1->y() + dy);
    } else if (!m_point2->isLocked()) {
        // Seul point2 est libre : le déplacer vers point1
        double dx = (m_point1->x() - m_point2->x()) * correctionFactor;
        double dy = (m_point1->y() - m_point2->y()) * correctionFactor;
        m_point2->setX(m_point2->x() + dx);
        m_point2->setY(m_point2->y() + dy);
    }
}

QList<IGeometricPoint*> CoincidentConstraint::affectedPoints() const
{
    QList<IGeometricPoint*> points;
    if (m_point1 && !m_point1->isLocked()) {
        points << m_point1;
    }
    if (m_point2 && !m_point2->isLocked()) {
        points << m_point2;
    }
    return points;
}

IConstraint* CoincidentConstraint::clone() const
{
    return new CoincidentConstraint(m_point1, m_point2, isLocked());
}

} // namespace Core
} // namespace LaserCutStudio
