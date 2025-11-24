#include "ParallelConstraint.h"
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
const bool ParallelConstraint::s_registered =
    IConstraint::registerFactory<ParallelConstraint>();

ParallelConstraint::ParallelConstraint(IGeometricSegment* segment1,
                                       IGeometricSegment* segment2,
                                       bool locked,
                                       QObject* parent)
    : IConstraint(parent)
    , m_segment1(nullptr)
    , m_segment2(nullptr)
{
    setLocked(locked);
    setSegment1(segment1);
    setSegment2(segment2);
}

ParallelConstraint::~ParallelConstraint()
{
    disconnectFromSegment(m_segment1);
    disconnectFromSegment(m_segment2);
}

void ParallelConstraint::setSegment1(IGeometricSegment* segment)
{
    if (m_segment1 == segment) return;

    disconnectFromSegment(m_segment1);
    m_segment1 = segment;
    connectToSegment(m_segment1);

    emit segment1Changed(segment);
    emit constraintChanged();
}

void ParallelConstraint::setSegment2(IGeometricSegment* segment)
{
    if (m_segment2 == segment) return;

    disconnectFromSegment(m_segment2);
    m_segment2 = segment;
    connectToSegment(m_segment2);

    emit segment2Changed(segment);
    emit constraintChanged();
}

void ParallelConstraint::connectToSegment(IGeometricSegment* segment)
{
    if (!segment) return;

    connect(segment, &IGeometricElement::geometryChanged,
            this, &ParallelConstraint::constraintChanged);
    connect(segment, &Interface::aboutToBeDestroyed,
            this, [this](Interface* destroyedSegment) {
        if (m_segment1 == destroyedSegment) m_segment1 = nullptr;
        if (m_segment2 == destroyedSegment) m_segment2 = nullptr;
    });
}

void ParallelConstraint::disconnectFromSegment(IGeometricSegment* segment)
{
    if (!segment) return;
    QObject::disconnect(segment, nullptr, this, nullptr);
}

bool ParallelConstraint::isValid() const
{
    return m_segment1 && m_segment2 &&
           m_segment1->isValid() && m_segment2->isValid();
}

bool ParallelConstraint::isSatisfied(double tolerance) const
{
    return error() < tolerance;
}

double ParallelConstraint::error() const
{
    if (!isValid()) return 0.0;

    double angle1 = m_segment1->angle();
    double angle2 = m_segment2->angle();

    // Calculer la différence d'angle (en degrés)
    double diff = std::fmod(std::abs(angle2 - angle1), 180.0);

    // Les segments sont parallèles si diff ≈ 0° ou diff ≈ 180°
    if (diff > 90.0) {
        diff = 180.0 - diff;
    }

    return diff;
}

void ParallelConstraint::apply()
{
    if (!isValid()) return;

    double targetAngle = m_segment1->angle();
    double currentAngle = m_segment2->angle();

    // Calculer la différence d'angle
    double angleDiff = targetAngle - currentAngle;

    // Normaliser la différence d'angle dans [-180, 180]
    while (angleDiff > 180.0) angleDiff -= 360.0;
    while (angleDiff < -180.0) angleDiff += 360.0;

    // Ajuster le signe si nécessaire (on cherche le plus court chemin)
    if (std::abs(angleDiff) > 90.0) {
        angleDiff = angleDiff > 0 ? angleDiff - 180.0 : angleDiff + 180.0;
    }

    // Trouver les points libres du segment2
    IGeometricPoint* start = m_segment2->startPoint();
    IGeometricPoint* end = m_segment2->endPoint();

    if (!start || !end) return;

    // Si les deux points sont verrouillés, on ne peut rien faire
    if (start->isLocked() && end->isLocked()) return;

    // Calculer le centre de rotation (milieu du segment)
    Point2D center = m_segment2->midpoint();

    // Convertir en radians
    double angleRad = angleDiff * M_PI / 180.0;
    double cosAngle = std::cos(angleRad);
    double sinAngle = std::sin(angleRad);

    // Rotation des points libres autour du centre
    double correctionFactor = isLocked() ? 1.0 : 0.5;

    if (!start->isLocked()) {
        double dx = start->x() - center.x;
        double dy = start->y() - center.y;
        double newX = center.x + (dx * cosAngle - dy * sinAngle);
        double newY = center.y + (dx * sinAngle + dy * cosAngle);

        start->setX(start->x() + (newX - start->x()) * correctionFactor);
        start->setY(start->y() + (newY - start->y()) * correctionFactor);
    }

    if (!end->isLocked()) {
        double dx = end->x() - center.x;
        double dy = end->y() - center.y;
        double newX = center.x + (dx * cosAngle - dy * sinAngle);
        double newY = center.y + (dx * sinAngle + dy * cosAngle);

        end->setX(end->x() + (newX - end->x()) * correctionFactor);
        end->setY(end->y() + (newY - end->y()) * correctionFactor);
    }
}

QList<GeometricPoint*> ParallelConstraint::affectedPoints() const
{
    QList<GeometricPoint*> points;
    if (m_segment2 && m_segment2->isValid()) {
        IGeometricPoint* start = m_segment2->startPoint();
        IGeometricPoint* end = m_segment2->endPoint();
        if (start && !start->isLocked()) {
            points << qobject_cast<GeometricPoint*>(start);
        }
        if (end && !end->isLocked()) {
            points << qobject_cast<GeometricPoint*>(end);
        }
    }
    return points;
}

IConstraint* ParallelConstraint::clone() const
{
    return new ParallelConstraint(m_segment1, m_segment2, isLocked());
}

} // namespace Core
} // namespace LaserCutStudio
