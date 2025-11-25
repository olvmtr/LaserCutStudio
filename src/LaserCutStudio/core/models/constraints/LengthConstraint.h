#ifndef LENGTHCONSTRAINT_H
#define LENGTHCONSTRAINT_H

#include "core/models/constraints/IConstraint.h"
#include "core/models/geometry/IGeometricSegment.h"
#include <LibInterface/Patterns/PropertyMixin.h>

namespace LaserCutStudio {
namespace Core {

// Forward declaration
class GeometricSegment;
class GeometricPoint;

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

    Q_PROPERTY(IGeometricSegment* segment READ segment WRITE setSegment NOTIFY segmentChanged)
    Q_PROPERTY(double length READ length WRITE setLength NOTIFY lengthChanged)

signals:
    void segmentChanged(IGeometricSegment* newSegment);
    void lengthChanged(double newLength);

public:
    LengthConstraint();
    LengthConstraint(IGeometricSegment* segment, double length, bool locked = false);
    LengthConstraint(const LengthConstraint& other);
    ~LengthConstraint() override;

    IConstraint* clone() const override;
    DECLARE_TYPE_NAME(LengthConstraint)

    bool isSatisfied(double tolerance = 1e-6) const override;
    double error() const override;
    void apply() override;
    QList<IGeometricPoint*> affectedPoints() const override;

    IGeometricSegment* segment() const { return m_segment; }
    double length() const { return m_length; }
    bool isValid() const;

    void setSegment(IGeometricSegment* segment);
    void setLength(double length);

private:
    IGeometricSegment* m_segment = nullptr;
    double m_length = 0.0;

    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // LENGTHCONSTRAINT_H
