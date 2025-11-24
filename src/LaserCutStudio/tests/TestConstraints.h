#ifndef TESTCONSTRAINTS_H
#define TESTCONSTRAINTS_H

#include <QObject>
#include <QtTest>

namespace LaserCutStudio {
namespace Core {
namespace Tests {

/**
 * @brief Tests unitaires pour les contraintes géométriques
 */
class TestConstraints : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // DistanceConstraint
    void testDistanceConstraintConstruction();
    void testDistanceConstraintError();
    void testDistanceConstraintApply();
    void testDistanceConstraintLocked();

    // LengthConstraint
    void testLengthConstraintConstruction();
    void testLengthConstraintError();
    void testLengthConstraintApply();

    // AngleConstraint
    void testAngleConstraintConstruction();
    void testAngleConstraintError();
    void testAngleConstraintApply();

    // FixedPointConstraint
    void testFixedPointConstraintConstruction();
    void testFixedPointConstraintError();
    void testFixedPointConstraintApply();

    // ParallelConstraint
    void testParallelConstraintConstruction();
    void testParallelConstraintError();
    void testParallelConstraintApply();

    // PerpendicularConstraint
    void testPerpendicularConstraintConstruction();
    void testPerpendicularConstraintError();
    void testPerpendicularConstraintApply();

    // EqualLengthConstraint
    void testEqualLengthConstraintConstruction();
    void testEqualLengthConstraintError();
    void testEqualLengthConstraintApply();

    // CoincidentConstraint
    void testCoincidentConstraintConstruction();
    void testCoincidentConstraintError();
    void testCoincidentConstraintApply();
};

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio

#endif // TESTCONSTRAINTS_H
