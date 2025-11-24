#ifndef FIXEDPOINTCONSTRAINT_H
#define FIXEDPOINTCONSTRAINT_H

#include "core/models/constraints/IConstraint.h"
#include "core/models/geometry/GeometricPoint.h"
#include "core/infrastructure/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Contrainte de point fixe
 *
 * Maintient un point à une position (x, y) fixe.
 * Équivalent à verrouiller le point, mais plus explicite.
 */
class FixedPointConstraint : public IConstraint,
                              protected Patterns::PropertyMixin<FixedPointConstraint>
{
    Q_OBJECT

    Q_PROPERTY(GeometricPoint* point READ point WRITE setPoint NOTIFY pointChanged)
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)

signals:
    void pointChanged(GeometricPoint* newPoint);
    void xChanged(double newX);
    void yChanged(double newY);

public:
    FixedPointConstraint();
    FixedPointConstraint(GeometricPoint* point, double x, double y, bool locked = true);
    FixedPointConstraint(const FixedPointConstraint& other);
    ~FixedPointConstraint() override;

    IConstraint* clone() const override;
    DECLARE_TYPE_NAME(FixedPointConstraint)

    bool isSatisfied(double tolerance = 1e-6) const override;
    double error() const override;
    void apply() override;
    QList<GeometricPoint*> affectedPoints() const override;

    GeometricPoint* point() const { return m_point; }
    double x() const { return m_x; }
    double y() const { return m_y; }
    bool isValid() const { return m_point != nullptr; }

    void setPoint(GeometricPoint* point);
    void setX(double x);
    void setY(double y);
    void setPosition(double x, double y);

private:
    GeometricPoint* m_point = nullptr;
    double m_x = 0.0;
    double m_y = 0.0;

    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // FIXEDPOINTCONSTRAINT_H
