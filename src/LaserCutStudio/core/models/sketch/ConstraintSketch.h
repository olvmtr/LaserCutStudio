#ifndef CONSTRAINTSKETCH_H
#define CONSTRAINTSKETCH_H

#include <LibInterface/Interface.h>
#include "core/models/geometry/IGeometricElement.h"
#include "core/models/geometry/IGeometricPoint.h"
#include "core/models/geometry/IGeometricSegment.h"
#include "core/models/geometry/IGeometricArc.h"
#include "core/models/constraints/IConstraint.h"

// Forward declarations
namespace LaserCutStudio {
namespace Core {
    class GeometricPoint;
    class GeometricSegment;
    class GeometricArc;
}
}
#include "core/models/sketch/ConstraintSolver.h"
#include <LibInterface/Patterns/FactoryMixin.h>
#include <LibInterface/Patterns/PropertyMixin.h>
#include <QString>
#include <QList>
#include <QPainterPath>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Sketch de géométrie contrainte
 *
 * ConstraintSketch est le conteneur principal qui regroupe :
 * - Éléments géométriques (points, segments, arcs)
 * - Contraintes géométriques (distance, angle, longueur, etc.)
 * - Solveur de contraintes (résolution itérative)
 *
 * ## Exemple d'utilisation : Rectangle 10cm × 5cm
 *
 * @code
 * ConstraintSketch* sketch = new ConstraintSketch("Pièce laser");
 * sketch->setUnit(ConstraintSketch::Centimeters);
 *
 * // Créer les 4 coins
 * GeometricPoint* p1 = sketch->addPoint(0, 0, true);   // Origine fixe
 * GeometricPoint* p2 = sketch->addPoint(100, 0);
 * GeometricPoint* p3 = sketch->addPoint(100, 50);
 * GeometricPoint* p4 = sketch->addPoint(0, 50);
 *
 * // Créer les segments
 * GeometricSegment* s1 = sketch->addSegment(p1, p2);
 * GeometricSegment* s2 = sketch->addSegment(p2, p3);
 * GeometricSegment* s3 = sketch->addSegment(p3, p4);
 * GeometricSegment* s4 = sketch->addSegment(p4, p1);
 *
 * // Contraintes de longueur
 * sketch->addLengthConstraint(s1, 10.0, true);  // 10 cm (verrouillé)
 * sketch->addLengthConstraint(s2, 5.0, true);   // 5 cm (verrouillé)
 * sketch->addLengthConstraint(s3, 10.0, true);
 * sketch->addLengthConstraint(s4, 5.0, true);
 *
 * // Contraintes d'angle (90°)
 * sketch->addAngleConstraint(s1, s2, 90.0, true);
 * sketch->addAngleConstraint(s2, s3, 90.0, true);
 * sketch->addAngleConstraint(s3, s4, 90.0, true);
 *
 * // Résoudre
 * if (sketch->solve()) {
 *     qDebug() << "Rectangle créé avec succès !";
 *
 *     // Export pour découpe laser
 *     QList<Point2D> polyline = sketch->toPolyline();
 *     QPainterPath path = sketch->toPainterPath();
 * }
 * @endcode
 */
class ConstraintSketch : public Interface,
                         protected Patterns::FactoryMixin<ConstraintSketch>,
                         protected Patterns::PropertyMixin<ConstraintSketch>
{
    Q_OBJECT

public:
    /**
     * @brief Unités de mesure
     */
    enum MeasurementUnit {
        Millimeters,
        Centimeters,
        Inches
    };
    Q_ENUM(MeasurementUnit)

private:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(MeasurementUnit unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(bool autoSolve READ autoSolve WRITE setAutoSolve NOTIFY autoSolveChanged)

signals:
    void nameChanged(const QString& newName);
    void unitChanged(MeasurementUnit newUnit);
    void autoSolveChanged(bool newAutoSolve);

    void elementAdded(IGeometricElement* element);
    void elementRemoved(IGeometricElement* element);
    void constraintAdded(IConstraint* constraint);
    void constraintRemoved(IConstraint* constraint);

    void solveCompleted(bool success);
    void geometryChanged();

public:

    /**
     * @brief Constructeur par défaut
     */
    ConstraintSketch();

    /**
     * @brief Constructeur avec nom
     * @param name Nom du sketch
     */
    explicit ConstraintSketch(const QString& name);

    /**
     * @brief Destructeur
     */
    ~ConstraintSketch() override;

    /**
     * @brief Clone le sketch
     */
    ConstraintSketch* clone() const override;

    DECLARE_TYPE_NAME(ConstraintSketch)

    // Getters
    QString name() const { return m_name; }
    MeasurementUnit unit() const { return m_unit; }
    bool autoSolve() const { return m_autoSolve; }

    QList<IGeometricElement*> elements() const { return m_elements; }
    QList<IConstraint*> constraints() const { return m_constraints; }

    ConstraintSolver* solver() const { return m_solver; }

    // Setters
    void setName(const QString& name);
    void setUnit(MeasurementUnit unit);
    void setAutoSolve(bool autoSolve);

    // Gestion des éléments géométriques
    IGeometricPoint* addPoint(double x, double y, bool locked = false);
    IGeometricSegment* addSegment(IGeometricPoint* start, IGeometricPoint* end);
    IGeometricArc* addArc(IGeometricPoint* center, double radius, double startAngle = 0.0, double endAngle = 360.0);

    /**
     * @brief Crée un segment avec points automatiques (style KSP)
     *
     * Cherche des points existants près des coordonnées. Si aucun point
     * n'existe, crée automatiquement les points nécessaires.
     *
     * @param x1 X du premier point
     * @param y1 Y du premier point
     * @param x2 X du second point
     * @param y2 Y du second point
     * @param tolerance Distance de recherche pour points existants
     * @return Structure contenant le segment et les points créés
     */
    struct SegmentWithPoints {
        IGeometricSegment* segment;
        IGeometricPoint* startPoint;
        IGeometricPoint* endPoint;
        bool startPointCreated;  // true si le point de départ a été créé
        bool endPointCreated;    // true si le point d'arrivée a été créé
    };

    SegmentWithPoints addSegmentWithAutoPoints(double x1, double y1, double x2, double y2, double tolerance = 10.0);

    void addElement(IGeometricElement* element);
    void removeElement(IGeometricElement* element);
    void clearElements();

    // Gestion des contraintes
    void addConstraint(IConstraint* constraint);
    void removeConstraint(IConstraint* constraint);
    void clearConstraints();

    // Helpers pour créer des contraintes
    IConstraint* addDistanceConstraint(IGeometricPoint* p1, IGeometricPoint* p2, double distance, bool locked = false);
    IConstraint* addLengthConstraint(IGeometricSegment* segment, double length, bool locked = false);
    IConstraint* addAngleConstraint(IGeometricSegment* s1, IGeometricSegment* s2, double angleDegrees, bool locked = false);
    IConstraint* addFixedPointConstraint(IGeometricPoint* point, double x, double y, bool locked = true);

    // Résolution
    bool solve();

    // Export
    QList<Point2D> toPolyline() const;
    QPainterPath toPainterPath() const;
    QRectF getBoundingBox() const;

    // Conversion d'unités
    Q_INVOKABLE double convertToUnit(double valueInMm, MeasurementUnit targetUnit) const;
    Q_INVOKABLE double convertFromUnit(double value, MeasurementUnit sourceUnit) const;
    Q_INVOKABLE QString formatValue(double valueInMm, MeasurementUnit targetUnit) const;
    Q_INVOKABLE QString unitSuffix(MeasurementUnit unit) const;

    // Factory Pattern
    using FactoryMixin<ConstraintSketch>::create;
    using FactoryMixin<ConstraintSketch>::availableTypes;
    using FactoryMixin<ConstraintSketch>::registerFactory;

private:
    QString m_name;
    MeasurementUnit m_unit = Millimeters;
    bool m_autoSolve = false;

    QList<IGeometricElement*> m_elements;
    QList<IConstraint*> m_constraints;

    ConstraintSolver* m_solver;

    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // CONSTRAINTSKETCH_H
