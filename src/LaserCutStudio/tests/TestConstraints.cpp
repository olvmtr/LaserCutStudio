#include "TestConstraints.h"
#include "core/models/constraints/DistanceConstraint.h"
#include "core/models/constraints/LengthConstraint.h"
#include "core/models/constraints/AngleConstraint.h"
#include "core/models/constraints/FixedPointConstraint.h"
#include "core/models/constraints/ParallelConstraint.h"
#include "core/models/constraints/PerpendicularConstraint.h"
#include "core/models/constraints/EqualLengthConstraint.h"
#include "core/models/constraints/CoincidentConstraint.h"
#include "core/models/geometry/implementation/GeometricPoint.h"
#include "core/models/geometry/implementation/GeometricSegment.h"

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(3, 4);

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(3, 4);  // Distance = 5

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0, true);  // Fixed
    IGeometricPoint* p2 = new GeometricPoint(5, 0, false); // Free

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(5, 0);

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);
    IGeometricSegment* seg = new GeometricSegment(p1, p2);

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);  // Length = 10
    IGeometricSegment* seg = new GeometricSegment(p1, p2);

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0, true);   // Fixed
    IGeometricPoint* p2 = new GeometricPoint(10, 0, false); // Free
    IGeometricSegment* seg = new GeometricSegment(p1, p2);

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);
    IGeometricPoint* p3 = new GeometricPoint(10, 10);

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p2, p3);

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);   // s1: 0°
    IGeometricPoint* p3 = new GeometricPoint(10, 10);  // s2: 90°

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p2, p3);

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
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);
    IGeometricPoint* p3 = new GeometricPoint(15, 5, false);  // Free

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p2, p3);

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
    IGeometricPoint* p = new GeometricPoint(5, 5);

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
    IGeometricPoint* p = new GeometricPoint(3, 4);  // Distance from origin = 5

    FixedPointConstraint* constraint = new FixedPointConstraint(p, 0.0, 0.0, true);

    // Error should be distance = 5
    QCOMPARE(constraint->error(), 5.0);

    delete constraint;
    delete p;
}

void TestConstraints::testFixedPointConstraintApply()
{
    IGeometricPoint* p = new GeometricPoint(5, 5, false);  // Free

    FixedPointConstraint* constraint = new FixedPointConstraint(p, 10.0, 20.0, true);

    constraint->apply();

    // Point should be fixed at (10, 20)
    QCOMPARE(p->x(), 10.0);
    QCOMPARE(p->y(), 20.0);

    delete constraint;
    delete p;
}

// ============================================================================
// ParallelConstraint Tests
// ============================================================================

void TestConstraints::testParallelConstraintConstruction()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(10, 5);

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    ParallelConstraint* constraint = new ParallelConstraint(s1, s2, false);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->segment1(), s1);
    QCOMPARE(constraint->segment2(), s2);
    QCOMPARE(constraint->isLocked(), false);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testParallelConstraintError()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);    // s1: horizontal (0°)
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(10, 10);   // s2: angle ~26°

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    ParallelConstraint* constraint = new ParallelConstraint(s1, s2, false);

    // Error should be non-zero (segments not parallel)
    QVERIFY(constraint->error() > 1.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testParallelConstraintApply()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);    // s1: horizontal
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(10, 8, false);  // s2: slightly off, free point

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    ParallelConstraint* constraint = new ParallelConstraint(s1, s2, false);

    // Apply multiple times
    for (int i = 0; i < 20; ++i) {
        constraint->apply();
    }

    // Segments should be closer to parallel (error reduced)
    QVERIFY(constraint->error() < 10.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

// ============================================================================
// PerpendicularConstraint Tests
// ============================================================================

void TestConstraints::testPerpendicularConstraintConstruction()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);
    IGeometricPoint* p3 = new GeometricPoint(10, 0);
    IGeometricPoint* p4 = new GeometricPoint(10, 10);

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    PerpendicularConstraint* constraint = new PerpendicularConstraint(s1, s2, false);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->segment1(), s1);
    QCOMPARE(constraint->segment2(), s2);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testPerpendicularConstraintError()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);   // s1: 0°
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(0, 15);   // s2: 90° (already perpendicular)

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    PerpendicularConstraint* constraint = new PerpendicularConstraint(s1, s2, false);

    // Error should be close to 0 (already perpendicular)
    QVERIFY(constraint->error() < 1.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testPerpendicularConstraintApply()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);   // s1: horizontal
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(5, 10, false);  // s2: slightly off 90°, free

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    PerpendicularConstraint* constraint = new PerpendicularConstraint(s1, s2, false);

    // Apply multiple times
    for (int i = 0; i < 20; ++i) {
        constraint->apply();
    }

    // Should be closer to 90° (error reduced)
    QVERIFY(constraint->error() < 20.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

// ============================================================================
// EqualLengthConstraint Tests
// ============================================================================

void TestConstraints::testEqualLengthConstraintConstruction()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(15, 5);

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);  // Length 10
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);  // Length 15

    EqualLengthConstraint* constraint = new EqualLengthConstraint(s1, s2, false);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->segment1(), s1);
    QCOMPARE(constraint->segment2(), s2);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testEqualLengthConstraintError()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);  // Length 10
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(15, 5);  // Length 15

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    EqualLengthConstraint* constraint = new EqualLengthConstraint(s1, s2, false);

    // Error should be |10 - 15| = 5
    QCOMPARE(constraint->error(), 5.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraints::testEqualLengthConstraintApply()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0);      // s1: 10
    IGeometricPoint* p3 = new GeometricPoint(0, 5);
    IGeometricPoint* p4 = new GeometricPoint(15, 5, false);  // s2: 15, free

    IGeometricSegment* s1 = new GeometricSegment(p1, p2);
    IGeometricSegment* s2 = new GeometricSegment(p3, p4);

    EqualLengthConstraint* constraint = new EqualLengthConstraint(s1, s2, false);

    // Apply multiple times
    for (int i = 0; i < 20; ++i) {
        constraint->apply();
    }

    // Lengths should be closer
    QVERIFY(qAbs(s1->length() - s2->length()) < 2.0);

    delete constraint;
    delete s2;
    delete s1;
    delete p4;
    delete p3;
    delete p2;
    delete p1;
}

// ============================================================================
// CoincidentConstraint Tests
// ============================================================================

void TestConstraints::testCoincidentConstraintConstruction()
{
    IGeometricPoint* p1 = new GeometricPoint(5, 5);
    IGeometricPoint* p2 = new GeometricPoint(10, 10);

    CoincidentConstraint* constraint = new CoincidentConstraint(p1, p2, false);

    QVERIFY(constraint->isValid());
    QCOMPARE(constraint->point1(), p1);
    QCOMPARE(constraint->point2(), p2);

    delete constraint;
    delete p2;
    delete p1;
}

void TestConstraints::testCoincidentConstraintError()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(3, 4);  // Distance 5

    CoincidentConstraint* constraint = new CoincidentConstraint(p1, p2, false);

    // Error should be distance = 5
    QCOMPARE(constraint->error(), 5.0);

    delete constraint;
    delete p2;
    delete p1;
}

void TestConstraints::testCoincidentConstraintApply()
{
    IGeometricPoint* p1 = new GeometricPoint(0, 0);
    IGeometricPoint* p2 = new GeometricPoint(10, 0, false);  // Free

    CoincidentConstraint* constraint = new CoincidentConstraint(p1, p2, false);

    // Apply multiple times
    for (int i = 0; i < 10; ++i) {
        constraint->apply();
    }

    // Points should be very close
    QVERIFY(p1->distance(*p2) < 1.0);

    delete constraint;
    delete p2;
    delete p1;
}

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio
