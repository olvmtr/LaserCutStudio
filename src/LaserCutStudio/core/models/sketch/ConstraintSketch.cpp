#include "ConstraintSketch.h"
#include "core/models/constraints/DistanceConstraint.h"
#include "core/models/constraints/LengthConstraint.h"
#include "core/models/constraints/AngleConstraint.h"
#include "core/models/constraints/FixedPointConstraint.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {

const bool ConstraintSketch::s_registered = ConstraintSketch::registerFactory<ConstraintSketch>();

ConstraintSketch::ConstraintSketch()
    : Interface(),
      m_name("Unnamed Sketch"),
      m_unit(Millimeters),
      m_autoSolve(false),
      m_solver(new ConstraintSolver(this))
{
}

ConstraintSketch::ConstraintSketch(const QString& name)
    : Interface(),
      m_name(name),
      m_unit(Millimeters),
      m_autoSolve(false),
      m_solver(new ConstraintSolver(this))
{
}

ConstraintSketch::~ConstraintSketch()
{
    clearConstraints();
    clearElements();
}

ConstraintSketch* ConstraintSketch::clone() const
{
    ConstraintSketch* cloned = new ConstraintSketch(m_name + " (copy)");
    cloned->setUnit(m_unit);
    cloned->setAutoSolve(m_autoSolve);

    // Clone elements
    for (IGeometricElement* elem : m_elements) {
        cloned->addElement(static_cast<IGeometricElement*>(elem->clone()));
    }

    // Clone constraints
    for (IConstraint* constraint : m_constraints) {
        cloned->addConstraint(static_cast<IConstraint*>(constraint->clone()));
    }

    return cloned;
}

void ConstraintSketch::setName(const QString& name)
{
    updateProperty(m_name, name, &ConstraintSketch::nameChanged);
}

void ConstraintSketch::setUnit(MeasurementUnit unit)
{
    if (m_unit != unit) {
        m_unit = unit;
        emit unitChanged(unit);
    }
}

void ConstraintSketch::setAutoSolve(bool autoSolve)
{
    if (m_autoSolve != autoSolve) {
        m_autoSolve = autoSolve;
        emit autoSolveChanged(autoSolve);
    }
}

GeometricPoint* ConstraintSketch::addPoint(double x, double y, bool locked)
{
    GeometricPoint* point = new GeometricPoint(x, y, locked);
    addElement(point);
    return point;
}

GeometricSegment* ConstraintSketch::addSegment(GeometricPoint* start, GeometricPoint* end)
{
    GeometricSegment* segment = new GeometricSegment(start, end);
    addElement(segment);
    return segment;
}

GeometricArc* ConstraintSketch::addArc(GeometricPoint* center, double radius, double startAngle, double endAngle)
{
    GeometricArc* arc = new GeometricArc(center, radius, startAngle, endAngle);
    addElement(arc);
    return arc;
}

ConstraintSketch::SegmentWithPoints ConstraintSketch::addSegmentWithAutoPoints(double x1, double y1, double x2, double y2, double tolerance)
{
    SegmentWithPoints result;
    result.segment = nullptr;
    result.startPoint = nullptr;
    result.endPoint = nullptr;
    result.startPointCreated = false;
    result.endPointCreated = false;

    // Chercher points existants près des coordonnées
    for (IGeometricElement* elem : m_elements) {
        if (elem->getTypeName() == "GeometricPoint") {
            GeometricPoint* point = static_cast<GeometricPoint*>(elem);

            // Point proche du départ?
            double dist1 = std::sqrt(std::pow(point->x() - x1, 2) + std::pow(point->y() - y1, 2));
            if (!result.startPoint && dist1 < tolerance) {
                result.startPoint = point;
            }

            // Point proche de l'arrivée?
            double dist2 = std::sqrt(std::pow(point->x() - x2, 2) + std::pow(point->y() - y2, 2));
            if (!result.endPoint && dist2 < tolerance) {
                result.endPoint = point;
            }

            // Si les deux points sont trouvés, arrêter la recherche
            if (result.startPoint && result.endPoint) {
                break;
            }
        }
    }

    // Créer les points manquants
    if (!result.startPoint) {
        result.startPoint = addPoint(x1, y1);
        result.startPointCreated = true;
    }

    if (!result.endPoint) {
        result.endPoint = addPoint(x2, y2);
        result.endPointCreated = true;
    }

    // Créer le segment
    if (result.startPoint && result.endPoint && result.startPoint != result.endPoint) {
        result.segment = addSegment(result.startPoint, result.endPoint);
    }

    return result;
}

void ConstraintSketch::addElement(IGeometricElement* element)
{
    if (!element) return;

    m_elements.append(element);

    connect(element, &IGeometricElement::geometryChanged,
            this, &ConstraintSketch::geometryChanged);

    emit elementAdded(element);

    if (m_autoSolve) {
        solve();
    }
}

void ConstraintSketch::removeElement(IGeometricElement* element)
{
    if (!element) return;

    m_elements.removeOne(element);
    disconnect(element, nullptr, this, nullptr);

    emit elementRemoved(element);
}

void ConstraintSketch::clearElements()
{
    for (IGeometricElement* elem : m_elements) {
        disconnect(elem, nullptr, this, nullptr);
        delete elem;
    }
    m_elements.clear();
}

void ConstraintSketch::addConstraint(IConstraint* constraint)
{
    if (!constraint) return;

    m_constraints.append(constraint);

    connect(constraint, &IConstraint::constraintChanged,
            this, &ConstraintSketch::geometryChanged);

    emit constraintAdded(constraint);

    if (m_autoSolve) {
        solve();
    }
}

void ConstraintSketch::removeConstraint(IConstraint* constraint)
{
    if (!constraint) return;

    m_constraints.removeOne(constraint);
    disconnect(constraint, nullptr, this, nullptr);

    emit constraintRemoved(constraint);
}

void ConstraintSketch::clearConstraints()
{
    for (IConstraint* constraint : m_constraints) {
        disconnect(constraint, nullptr, this, nullptr);
        delete constraint;
    }
    m_constraints.clear();
}

IConstraint* ConstraintSketch::addDistanceConstraint(GeometricPoint* p1, GeometricPoint* p2, double distance, bool locked)
{
    DistanceConstraint* constraint = new DistanceConstraint(p1, p2, distance, locked);
    addConstraint(constraint);
    return constraint;
}

IConstraint* ConstraintSketch::addLengthConstraint(GeometricSegment* segment, double length, bool locked)
{
    LengthConstraint* constraint = new LengthConstraint(segment, length, locked);
    addConstraint(constraint);
    return constraint;
}

IConstraint* ConstraintSketch::addAngleConstraint(GeometricSegment* s1, GeometricSegment* s2, double angleDegrees, bool locked)
{
    AngleConstraint* constraint = new AngleConstraint(s1, s2, angleDegrees, locked);
    addConstraint(constraint);
    return constraint;
}

IConstraint* ConstraintSketch::addFixedPointConstraint(GeometricPoint* point, double x, double y, bool locked)
{
    FixedPointConstraint* constraint = new FixedPointConstraint(point, x, y, locked);
    addConstraint(constraint);
    return constraint;
}

bool ConstraintSketch::solve()
{
    bool success = m_solver->solve(m_constraints);
    emit solveCompleted(success);
    return success;
}

QList<Point2D> ConstraintSketch::toPolyline() const
{
    QList<Point2D> polyline;

    for (IGeometricElement* elem : m_elements) {
        polyline.append(elem->getPoints());
    }

    return polyline;
}

QPainterPath ConstraintSketch::toPainterPath() const
{
    QPainterPath path;

    for (IGeometricElement* elem : m_elements) {
        path.addPath(elem->toPainterPath());
    }

    return path;
}

QRectF ConstraintSketch::getBoundingBox() const
{
    if (m_elements.isEmpty()) {
        return QRectF();
    }

    QRectF bbox = m_elements.first()->getBoundingBox();

    for (IGeometricElement* elem : m_elements) {
        bbox = bbox.united(elem->getBoundingBox());
    }

    return bbox;
}

// ===== Conversion d'unités =====

double ConstraintSketch::convertToUnit(double valueInMm, MeasurementUnit targetUnit) const
{
    switch (targetUnit) {
    case Millimeters:
        return valueInMm;
    case Centimeters:
        return valueInMm / 10.0;
    case Inches:
        return valueInMm / 25.4;
    default:
        return valueInMm;
    }
}

double ConstraintSketch::convertFromUnit(double value, MeasurementUnit sourceUnit) const
{
    switch (sourceUnit) {
    case Millimeters:
        return value;
    case Centimeters:
        return value * 10.0;
    case Inches:
        return value * 25.4;
    default:
        return value;
    }
}

QString ConstraintSketch::formatValue(double valueInMm, MeasurementUnit targetUnit) const
{
    double converted = convertToUnit(valueInMm, targetUnit);
    QString suffix = unitSuffix(targetUnit);

    // Format avec 2 décimales sauf si très petit
    if (converted < 0.01) {
        return QString::number(converted, 'f', 4) + " " + suffix;
    } else if (converted < 1.0) {
        return QString::number(converted, 'f', 3) + " " + suffix;
    } else {
        return QString::number(converted, 'f', 2) + " " + suffix;
    }
}

QString ConstraintSketch::unitSuffix(MeasurementUnit unit) const
{
    switch (unit) {
    case Millimeters:
        return "mm";
    case Centimeters:
        return "cm";
    case Inches:
        return "in";
    default:
        return "mm";
    }
}

} // namespace Core
} // namespace LaserCutStudio
