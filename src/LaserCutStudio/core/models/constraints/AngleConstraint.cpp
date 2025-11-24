#include "AngleConstraint.h"
#include "core/models/geometry/IGeometricSegment.h"
#include "core/models/geometry/IGeometricPoint.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;
class GeometricArc;

const bool AngleConstraint::s_registered = IConstraint::registerFactory<AngleConstraint>();

AngleConstraint::AngleConstraint()
    : IConstraint(false, 1.0), m_segment1(nullptr), m_segment2(nullptr), m_angleDegrees(0.0)
{
}

AngleConstraint::AngleConstraint(IGeometricSegment* segment1, IGeometricSegment* segment2, double angleDegrees, bool locked)
    : IConstraint(locked, 1.0), m_segment1(segment1), m_segment2(segment2), m_angleDegrees(angleDegrees)
{
}

AngleConstraint::AngleConstraint(const AngleConstraint& other)
    : IConstraint(other.m_locked, other.m_priority), m_segment1(nullptr), m_segment2(nullptr), m_angleDegrees(other.m_angleDegrees)
{
    if (other.m_segment1) {
        m_segment1 = static_cast<GeometricSegment*>(other.m_segment1->clone());
    }
    if (other.m_segment2) {
        m_segment2 = static_cast<GeometricSegment*>(other.m_segment2->clone());
    }
}

AngleConstraint::~AngleConstraint()
{
}

IConstraint* AngleConstraint::clone() const
{
    return new AngleConstraint(*this);
}

bool AngleConstraint::isValid() const
{
    GeometricSegment* seg1 = qobject_cast<GeometricSegment*>(m_segment1);
    GeometricSegment* seg2 = qobject_cast<GeometricSegment*>(m_segment2);
    return seg1 && seg2 && seg1->isValid() && seg2->isValid();
}

bool AngleConstraint::isSatisfied(double tolerance) const
{
    return error() < tolerance;
}

double AngleConstraint::error() const
{
    if (!isValid()) return 0.0;

    double angle1 = m_segment1->angle();
    double angle2 = m_segment2->angle();

    // Différence d'angle
    double diff = angle2 - angle1;

    // Normaliser [-180, 180]
    while (diff > 180.0) diff -= 360.0;
    while (diff < -180.0) diff += 360.0;

    return std::abs(diff - m_angleDegrees);
}

void AngleConstraint::apply()
{
    if (!isValid()) return;

    GeometricSegment* seg1 = qobject_cast<GeometricSegment*>(m_segment1);
    GeometricSegment* seg2 = qobject_cast<GeometricSegment*>(m_segment2);
    if (!seg1 || !seg2) return;

    double angle1 = seg1->angle();
    double angle2 = seg2->angle();

    // Différence actuelle
    double currentDiff = angle2 - angle1;
    while (currentDiff > 180.0) currentDiff -= 360.0;
    while (currentDiff < -180.0) currentDiff += 360.0;

    // Erreur angulaire
    double errorAngle = currentDiff - m_angleDegrees;

    if (std::abs(errorAngle) < 1e-6) return;

    // Correction : rotation des points libres
    // On va rotate les extrémités des segments pour corriger l'angle

    // Pour simplifier, on fait une correction partielle (relaxation)
    double correctionAngle = -errorAngle * 0.5;  // Convergence progressive

    // Rotation du second segment autour de son point de départ
    IGeometricPoint* start2 = seg2->startPoint();
    IGeometricPoint* end2 = seg2->endPoint();

    if (start2 && end2 && !end2->isLocked()) {
        double angleRad = correctionAngle * M_PI / 180.0;
        double cosA = std::cos(angleRad);
        double sinA = std::sin(angleRad);

        // Vecteur du segment
        double dx = end2->x() - start2->x();
        double dy = end2->y() - start2->y();

        // Rotation
        double newDx = dx * cosA - dy * sinA;
        double newDy = dx * sinA + dy * cosA;

        // Nouvelle position
        end2->setX(start2->x() + newDx);
        end2->setY(start2->y() + newDy);
    }
}

QList<GeometricPoint*> AngleConstraint::affectedPoints() const
{
    QList<GeometricPoint*> points;
    if (m_segment1) {
        GeometricSegment* seg1 = qobject_cast<GeometricSegment*>(m_segment1);
        if (seg1 && seg1->isValid()) {
            IGeometricPoint* start = seg1->startPoint();
            IGeometricPoint* end = seg1->endPoint();
            if (start) points << qobject_cast<GeometricPoint*>(start);
            if (end) points << qobject_cast<GeometricPoint*>(end);
        }
    }
    if (m_segment2) {
        GeometricSegment* seg2 = qobject_cast<GeometricSegment*>(m_segment2);
        if (seg2 && seg2->isValid()) {
            IGeometricPoint* start = seg2->startPoint();
            IGeometricPoint* end = seg2->endPoint();
            if (start) points << qobject_cast<GeometricPoint*>(start);
            if (end) points << qobject_cast<GeometricPoint*>(end);
        }
    }
    return points;
}

void AngleConstraint::setSegment1(IGeometricSegment* segment)
{
    if (m_segment1 == segment) return;
    m_segment1 = segment;
    emit segment1Changed(segment);
    emit constraintChanged();
}

void AngleConstraint::setSegment2(IGeometricSegment* segment)
{
    if (m_segment2 == segment) return;
    m_segment2 = segment;
    emit segment2Changed(segment);
    emit constraintChanged();
}

void AngleConstraint::setAngle(double angleDegrees)
{
    updateProperty(m_angleDegrees, angleDegrees, &AngleConstraint::angleChanged, &AngleConstraint::constraintChanged);
}

} // namespace Core
} // namespace LaserCutStudio
