#ifndef LENGTHCONSTRAINT_H
#define LENGTHCONSTRAINT_H

#include "core/models/constraints/IConstraint.h"
#include "core/models/geometry/GeometricSegment.h"
#include "core/models/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Contrainte de longueur pour un segment
 *
 * Maintient une longueur fixe pour un segment géométrique.
 * Équivalent à DistanceConstraint mais sur un segment.
 */
class LengthConstraint : public IConstraint,
                         protected Patterns::PropertyMixin<LengthConstraint>
{
    Q_OBJECT

    Q_PROPERTY(GeometricSegment* segment READ segment WRITE setSegment NOTIFY segmentChanged)
    Q_PROPERTY(double length READ length WRITE setLength NOTIFY lengthChanged)

signals:
    void segmentChanged(GeometricSegment* newSegment);
    void lengthChanged(double newLength);

public:
    LengthConstraint();
    LengthConstraint(GeometricSegment* segment, double length, bool locked = false);
    LengthConstraint(const LengthConstraint& other);
    ~LengthConstraint() override;

    IConstraint* clone() const override;
    DECLARE_TYPE_NAME(LengthConstraint)

    bool isSatisfied(double tolerance = 1e-6) const override;
    double error() const override;
    void apply() override;
    QList<GeometricPoint*> affectedPoints() const override;

    GeometricSegment* segment() const { return m_segment; }
    double length() const { return m_length; }
    bool isValid() const { return m_segment != nullptr && m_segment->isValid(); }

    void setSegment(GeometricSegment* segment);
    void setLength(double length);

private:
    GeometricSegment* m_segment = nullptr;
    double m_length = 0.0;

    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // LENGTHCONSTRAINT_H
