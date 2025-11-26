#include "EqualLengthConstraint.h"
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
const bool EqualLengthConstraint::s_registered =
    IConstraint::registerFactory<EqualLengthConstraint>();

EqualLengthConstraint::EqualLengthConstraint(IGeometricSegment* segment1,
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

EqualLengthConstraint::~EqualLengthConstraint()
{
    disconnectFromSegment(m_segment1);
    disconnectFromSegment(m_segment2);
}

void EqualLengthConstraint::setSegment1(IGeometricSegment* segment)
{
    if (m_segment1 == segment) return;

    disconnectFromSegment(m_segment1);
    m_segment1 = segment;
    connectToSegment(m_segment1);

    emit segment1Changed(segment);
    emit constraintChanged();
}

void EqualLengthConstraint::setSegment2(IGeometricSegment* segment)
{
    if (m_segment2 == segment) return;

    disconnectFromSegment(m_segment2);
    m_segment2 = segment;
    connectToSegment(m_segment2);

    emit segment2Changed(segment);
    emit constraintChanged();
}

void EqualLengthConstraint::connectToSegment(IGeometricSegment* segment)
{
    if (!segment) return;

    connect(segment, &IGeometricElement::geometryChanged,
            this, &EqualLengthConstraint::constraintChanged);
    connect(segment, &Interface::aboutToBeDestroyed,
            this, [this](Interface* destroyedSegment) {
        if (m_segment1 == destroyedSegment) m_segment1 = nullptr;
        if (m_segment2 == destroyedSegment) m_segment2 = nullptr;
    });
}

void EqualLengthConstraint::disconnectFromSegment(IGeometricSegment* segment)
{
    if (!segment) return;
    QObject::disconnect(segment, nullptr, this, nullptr);
}

bool EqualLengthConstraint::isValid() const
{
    return m_segment1 && m_segment2 &&
           m_segment1->isValid() && m_segment2->isValid();
}

bool EqualLengthConstraint::isSatisfied(double tolerance) const
{
    return error() < tolerance;
}

double EqualLengthConstraint::error() const
{
    if (!isValid()) return 0.0;

    double length1 = m_segment1->length();
    double length2 = m_segment2->length();

    return std::abs(length1 - length2);
}

void EqualLengthConstraint::apply()
{
    if (!isValid()) return;

    double targetLength = m_segment1->length();
    double currentLength = m_segment2->length();

    if (currentLength < 1e-9) return;  // Éviter division par zéro

    double errorValue = targetLength - currentLength;

    // Trouver les points du segment2
    IGeometricPoint* start = m_segment2->startPoint();
    IGeometricPoint* end = m_segment2->endPoint();

    if (!start || !end) return;

    // Si les deux points sont verrouillés, on ne peut rien faire
    if (start->isLocked() && end->isLocked()) return;

    // Calculer la direction du segment
    Point2D dir = m_segment2->direction();

    // Calculer le facteur de correction
    double correctionFactor = isLocked() ? 1.0 : 0.5;
    double scaleFactor = (targetLength / currentLength - 1.0) * correctionFactor;

    // Calculer le centre du segment (point fixe pour le scaling)
    Point2D center = m_segment2->midpoint();

    // Ajuster les points libres
    if (!start->isLocked() && !end->isLocked()) {
        // Les deux points sont libres : les éloigner/rapprocher du centre
        double dx = (start->x() - center.x) * scaleFactor;
        double dy = (start->y() - center.y) * scaleFactor;
        start->setX(start->x() + dx);
        start->setY(start->y() + dy);

        dx = (end->x() - center.x) * scaleFactor;
        dy = (end->y() - center.y) * scaleFactor;
        end->setX(end->x() + dx);
        end->setY(end->y() + dy);
    } else if (!start->isLocked()) {
        // Seul le point de départ est libre : le déplacer le long de la direction
        start->setX(start->x() - dir.x * errorValue * correctionFactor);
        start->setY(start->y() - dir.y * errorValue * correctionFactor);
    } else if (!end->isLocked()) {
        // Seul le point de fin est libre : le déplacer le long de la direction
        end->setX(end->x() + dir.x * errorValue * correctionFactor);
        end->setY(end->y() + dir.y * errorValue * correctionFactor);
    }
}

QList<IGeometricPoint*> EqualLengthConstraint::affectedPoints() const
{
    QList<IGeometricPoint*> points;
    if (m_segment2 && m_segment2->isValid()) {
        IGeometricPoint* start = m_segment2->startPoint();
        IGeometricPoint* end = m_segment2->endPoint();
        if (start && !start->isLocked()) {
            points << start;
        }
        if (end && !end->isLocked()) {
            points << end;
        }
    }
    return points;
}

IConstraint* EqualLengthConstraint::clone() const
{
    return new EqualLengthConstraint(m_segment1, m_segment2, isLocked());
}

} // namespace Core
} // namespace LaserCutStudio
