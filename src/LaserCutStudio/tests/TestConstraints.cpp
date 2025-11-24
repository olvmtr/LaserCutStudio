#include "TestConstraints.h"
#include "core/models/constraints/DistanceConstraint.h"
#include "core/models/constraints/LengthConstraint.h"
#include "core/models/constraints/AngleConstraint.h"
#include "core/models/constraints/FixedPointConstraint.h"
#include "core/models/geometry/GeometricPoint.h"
#include "core/models/geometry/GeometricSegment.h"

namespace LaserCutStudio {
namespace Core {
namespace Tests {

void TestConstraints::initTestCase()
{
    IConstraint::clearAllInstances();
    IGeometricElement::clearAllInstances();
}

void TestConstraints::cleanupTestCase()
{
    IConstraint::clearAllInstances();
    IGeometricElement::clearAllInstances();
}

// ============================================================================
// DistanceConstraint Tests
// ============================================================================

void TestConstraints::testDistanceConstraintConstruction()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(3, 4);

    DistanceConstraint* constraint = new DistanceConstraint(p1, p2, 5.0, false);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->point1(), p1);
    QCOMPARE(constraint->point2(), p2);
    QCOMPARE(constraint->distance(), 5.0);
    QCOMPARE(constraint->isLocked(), false);

    delete constraint;
    delete p2;
    delete p1;
}

void TestConstraints::testDistanceConstraintError()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(3, 4);  // Distance = 5

    DistanceConstraint* constraint = new DistanceConstraint(p1, p2, 10.0, false);

    // Error should be |5 - 10| = 5
    QCOMPARE(constraint->error(), 5.0);
    QVERIFY(!constraint->isSatisfied(1e-6));

    delete constraint;
    delete p2;
    delete p1;
}

void TestConstraints::testDistanceConstraintApply()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0, true);  // Fixed
    GeometricPoint* p2 = new GeometricPoint(5, 0, false); // Free

    DistanceConstraint* constraint = new DistanceConstraint(p1, p2, 10.0, false);

    // Apply constraint
    constraint->apply();

    // P2 should move to distance 10 from P1
    double dist = p1->distance(*p2);
    QVERIFY(qAbs(dist - 10.0) < 0.1);

    delete constraint;
    delete p2;
    delete p1;
}

void TestConstraints::testDistanceConstraintLocked()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(5, 0);

    DistanceConstraint* constraint = new DistanceConstraint(p1, p2, 10.0, true);

    QCOMPARE(constraint->isLocked(), true);
    QCOMPARE(constraint->priority(), 1.0);

    constraint->setLocked(false);
    QCOMPARE(constraint->isLocked(), false);

    delete constraint;
    delete p2;
    delete p1;
}

// ============================================================================
// LengthConstraint Tests
// ============================================================================

void TestConstraints::testLengthConstraintConstruction()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(10, 0);
    GeometricSegment* seg = new GeometricSegment(p1, p2);

    LengthConstraint* constraint = new LengthConstraint(seg, 15.0, false);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->segment(), seg);
    QCOMPARE(constraint->length(), 15.0);

    delete constraint;
    delete seg;
    delete p2;
    delete p1;
}

void TestConstraints::testLengthConstraintError()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(10, 0);  // Length = 10
    GeometricSegment* seg = new GeometricSegment(p1, p2);

    LengthConstraint* constraint = new LengthConstraint(seg, 15.0, false);

    // Error should be |10 - 15| = 5
    QCOMPARE(constraint->error(), 5.0);

    delete constraint;
    delete seg;
    delete p2;
    delete p1;
}

void TestConstraints::testLengthConstraintApply()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0, true);   // Fixed
    GeometricPoint* p2 = new GeometricPoint(10, 0, false); // Free
    GeometricSegment* seg = new GeometricSegment(p1, p2);

    LengthConstraint* constraint = new LengthConstraint(seg, 20.0, false);

    constraint->apply();

    // Segment length should be close to 20
    QVERIFY(qAbs(seg->length() - 20.0) < 0.1);

    delete constraint;
    delete seg;
    delete p2;
    delete p1;
}

// ============================================================================
// AngleConstraint Tests
// ============================================================================

void TestConstraints::testAngleConstraintConstruction()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(10, 0);
    GeometricPoint* p3 = new GeometricPoint(10, 10);

    GeometricSegment* s1 = new GeometricSegment(p1, p2);
    GeometricSegment* s2 = new GeometricSegment(p2, p3);

    AngleConstraint* constraint = new AngleConstraint(s1, s2, 90.0, false);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->segment1(), s1);
    QCOMPARE(constraint->segment2(), s2);
    QCOMPARE(constraint->angle(), 90.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testAngleConstraintError()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(10, 0);   // s1: 0°
    GeometricPoint* p3 = new GeometricPoint(10, 10);  // s2: 90°

    GeometricSegment* s1 = new GeometricSegment(p1, p2);
    GeometricSegment* s2 = new GeometricSegment(p2, p3);

    AngleConstraint* constraint = new AngleConstraint(s1, s2, 90.0, false);

    // Angle difference should be 0 (already 90°)
    QVERIFY(constraint->error() < 1.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testAngleConstraintApply()
{
    GeometricPoint* p1 = new GeometricPoint(0, 0);
    GeometricPoint* p2 = new GeometricPoint(10, 0);
    GeometricPoint* p3 = new GeometricPoint(15, 5, false);  // Free

    GeometricSegment* s1 = new GeometricSegment(p1, p2);
    GeometricSegment* s2 = new GeometricSegment(p2, p3);

    AngleConstraint* constraint = new AngleConstraint(s1, s2, 90.0, false);

    // Apply multiple times for convergence
    for (int i = 0; i < 10; ++i) {
        constraint->apply();
    }

    // Check angle is closer to 90°
    double angle1 = s1->angle();
    double angle2 = s2->angle();
    double diff = angle2 - angle1;
    while (diff > 180.0) diff -= 360.0;
    while (diff < -180.0) diff += 360.0;

    QVERIFY(qAbs(diff - 90.0) < 20.0);  // Relaxed tolerance

    delete constraint;
    delete s2;
    delete s1;
    delete p3;
    delete p2;
    delete p1;
}

// ============================================================================
// FixedPointConstraint Tests
// ============================================================================

void TestConstraints::testFixedPointConstraintConstruction()
{
    GeometricPoint* p = new GeometricPoint(5, 5);

    FixedPointConstraint* constraint = new FixedPointConstraint(p, 10.0, 20.0, true);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->point(), p);
    QCOMPARE(constraint->x(), 10.0);
    QCOMPARE(constraint->y(), 20.0);
    QCOMPARE(constraint->isLocked(), true);

    delete constraint;
    delete p;
}

void TestConstraints::testFixedPointConstraintError()
{
    GeometricPoint* p = new GeometricPoint(3, 4);  // Distance from origin = 5

    FixedPointConstraint* constraint = new FixedPointConstraint(p, 0.0, 0.0, true);

    // Error should be distance = 5
    QCOMPARE(constraint->error(), 5.0);

    delete constraint;
    delete p;
}

void TestConstraints::testFixedPointConstraintApply()
{
    GeometricPoint* p = new GeometricPoint(5, 5, false);  // Free

    FixedPointConstraint* constraint = new FixedPointConstraint(p, 10.0, 20.0, true);

    constraint->apply();

    // Point should be fixed at (10, 20)
    QCOMPARE(p->x(), 10.0);
    QCOMPARE(p->y(), 20.0);

    delete constraint;
    delete p;
}

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio
