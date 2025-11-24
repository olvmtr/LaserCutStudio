#include "LengthConstraint.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

const bool LengthConstraint::s_registered = IConstraint::registerFactory<LengthConstraint>();

LengthConstraint::LengthConstraint()
    : IConstraint(false, 1.0), m_segment(nullptr), m_length(0.0)
{
}

LengthConstraint::LengthConstraint(GeometricSegment* segment, double length, bool locked)
    : IConstraint(locked, 1.0), m_segment(segment), m_length(length)
{
}

LengthConstraint::LengthConstraint(const LengthConstraint& other)
    : IConstraint(other.m_locked, other.m_priority), m_segment(nullptr), m_length(other.m_length)
{
    if (other.m_segment) {
        m_segment = static_cast<GeometricSegment*>(other.m_segment->clone());
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

double LengthConstraint::error() const
{
    if (!isValid()) return 0.0;
    return std::abs(m_segment->length() - m_length);
}

void LengthConstraint::apply()
{
    if (!isValid()) return;

    GeometricPoint* start = m_segment->startPoint();
    GeometricPoint* end = m_segment->endPoint();

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
        points << m_segment->startPoint() << m_segment->endPoint();
    }
    return points;
}

void LengthConstraint::setSegment(GeometricSegment* segment)
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
