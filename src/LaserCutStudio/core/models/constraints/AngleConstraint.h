#ifndef ANGLECONSTRAINT_H
#define ANGLECONSTRAINT_H

#include "core/models/constraints/IConstraint.h"
#include "core/models/geometry/GeometricSegment.h"
#include "core/models/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Contrainte d'angle entre deux segments
 *
 * Maintient un angle fixe entre deux segments géométriques.
 * L'angle est mesuré du segment1 vers le segment2 (sens trigonométrique).
 *
 * ## Exemple : Angle droit
 *
 * @code
 * AngleConstraint* perp = new AngleConstraint(seg1, seg2, 90.0, true);
 * // Force seg1 et seg2 perpendiculaires
 * @endcode
 */
class AngleConstraint : public IConstraint,
                        protected Patterns::PropertyMixin<AngleConstraint>
{
    Q_OBJECT

    Q_PROPERTY(GeometricSegment* segment1 READ segment1 WRITE setSegment1 NOTIFY segment1Changed)
    Q_PROPERTY(GeometricSegment* segment2 READ segment2 WRITE setSegment2 NOTIFY segment2Changed)
    Q_PROPERTY(double angle READ angle WRITE setAngle NOTIFY angleChanged)

signals:
    void segment1Changed(GeometricSegment* newSegment1);
    void segment2Changed(GeometricSegment* newSegment2);
    void angleChanged(double newAngle);

public:
    AngleConstraint();
    AngleConstraint(GeometricSegment* segment1, GeometricSegment* segment2, double angleDegrees, bool locked = false);
    AngleConstraint(const AngleConstraint& other);
    ~AngleConstraint() override;

    IConstraint* clone() const override;
    DECLARE_TYPE_NAME(AngleConstraint)

    bool isSatisfied(double tolerance = 1e-6) const override;
    double error() const override;
    void apply() override;
    QList<GeometricPoint*> affectedPoints() const override;

    GeometricSegment* segment1() const { return m_segment1; }
    GeometricSegment* segment2() const { return m_segment2; }
    double angle() const { return m_angleDegrees; }
    bool isValid() const { return m_segment1 != nullptr && m_segment1->isValid() && m_segment2 != nullptr && m_segment2->isValid(); }

    void setSegment1(GeometricSegment* segment);
    void setSegment2(GeometricSegment* segment);
    void setAngle(double angleDegrees);

private:
    GeometricSegment* m_segment1 = nullptr;
    GeometricSegment* m_segment2 = nullptr;
    double m_angleDegrees = 0.0;

    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // ANGLECONSTRAINT_H
