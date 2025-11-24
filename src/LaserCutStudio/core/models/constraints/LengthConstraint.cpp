#include "LengthConstraint.h"
#include "core/models/geometry/IGeometricSegment.h"
#include "core/models/geometry/IGeometricPoint.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;

const bool LengthConstraint::s_registered = IConstraint::registerFactory<LengthConstraint>();

LengthConstraint::LengthConstraint()
    : IConstraint(false, 1.0), m_segment(nullptr), m_length(0.0)
{
}

LengthConstraint::LengthConstraint(IGeometricSegment* segment, double length, bool locked)
    : IConstraint(locked, 1.0), m_segment(segment), m_length(length)
{
}

LengthConstraint::LengthConstraint(const LengthConstraint& other)
    : IConstraint(other.m_locked, other.m_priority), m_segment(nullptr), m_length(other.m_length)
{
    if (other.m_segment) {
        m_segment = other.m_segment->clone();
    }
}

LengthConstraint::~LengthConstraint()
{
}

IConstraint* LengthConstraint::clone() const
{
    return new LengthConstraint(*this);
}

bool LengthConstraint::isSatisfied(double tolerance) const
{
    return error() < tolerance;
}

bool LengthConstraint::isValid() const
{
    GeometricSegment* seg = qobject_cast<GeometricSegment*>(m_segment);
    return seg != nullptr && seg->isValid();
}

double LengthConstraint::error() const
{
    if (!isValid()) return 0.0;
    GeometricSegment* seg = qobject_cast<GeometricSegment*>(m_segment);
    return std::abs(seg->length() - m_length);
}

void LengthConstraint::apply()
{
    if (!isValid()) return;

    GeometricSegment* seg = qobject_cast<GeometricSegment*>(m_segment);
    if (!seg) return;

    IGeometricPoint* start = seg->startPoint();
    IGeometricPoint* end = seg->endPoint();

    if (!start || !end) return;

    double currentLen = start->distance(*end);
    if (std::abs(currentLen - m_length) < 1e-9) return;

    double dx = end->x() - start->x();
    double dy = end->y() - start->y();

    if (currentLen > 1e-9) {
        dx /= currentLen;
        dy /= currentLen;
    } else {
        dx = 1.0;
        dy = 0.0;
    }

    double errorValue = currentLen - m_length;

    int freePoints = 0;
    if (!start->isLocked()) freePoints++;
    if (!end->isLocked()) freePoints++;

    if (freePoints == 0) return;

    double correction = errorValue / freePoints;

    if (!start->isLocked()) {
        start->setX(start->x() + dx * correction);
        start->setY(start->y() + dy * correction);
    }

    if (!end->isLocked()) {
        end->setX(end->x() - dx * correction);
        end->setY(end->y() - dy * correction);
    }
}

QList<GeometricPoint*> LengthConstraint::affectedPoints() const
{
    QList<GeometricPoint*> points;
    if (isValid()) {
        GeometricSegment* seg = qobject_cast<GeometricSegment*>(m_segment);
        if (seg) {
            IGeometricPoint* start = seg->startPoint();
            IGeometricPoint* end = seg->endPoint();
            if (start) points << qobject_cast<GeometricPoint*>(start);
            if (end) points << qobject_cast<GeometricPoint*>(end);
        }
    }
    return points;
}

void LengthConstraint::setSegment(IGeometricSegment* segment)
{
    if (m_segment == segment) return;
    m_segment = segment;
    emit segmentChanged(segment);
    emit constraintChanged();
}

void LengthConstraint::setLength(double length)
{
    updateProperty(m_length, length, &LengthConstraint::lengthChanged, &LengthConstraint::constraintChanged);
}

} // namespace Core
} // namespace LaserCutStudio
