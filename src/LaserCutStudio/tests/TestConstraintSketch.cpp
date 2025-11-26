#include "TestConstraintSketch.h"
#include "core/models/sketch/ConstraintSolver.h"
#include "core/models/sketch/ConstraintSketch.h"
#include "core/models/geometry/implementation/GeometricPoint.h"
#include "core/models/geometry/implementation/GeometricSegment.h"
#include "core/models/geometry/implementation/GeometricArc.h"
#include "core/models/constraints/DistanceConstraint.h"
#include "core/models/constraints/LengthConstraint.h"
#include "core/models/constraints/FixedPointConstraint.h"

namespace LaserCutStudio {
namespace Core {
namespace Tests {

void TestConstraintSketch::initTestCase()
{
    IConstraint::clearAllInstances();
    IGeometricElement::clearAllInstances();
}

void TestConstraintSketch::cleanupTestCase()
{
    IConstraint::clearAllInstances();
    IGeometricElement::clearAllInstances();
}

// ============================================================================
// ConstraintSolver Tests
// ============================================================================

void TestConstraintSketch::testSolverConstruction()
{
    ConstraintSolver solver;

    QCOMPARE(solver.maxIterations(), 100);
    QCOMPARE(solver.tolerance(), 1e-6);
    QCOMPARE(solver.converged(), false);
    QCOMPARE(solver.iterationCount(), 0);
}

void TestConstraintSketch::testSolverSimpleDistance()
{
    ConstraintSolver solver;

    IGeometricPoint* p1 = new GeometricPoint(0, 0, true);   // Fixed
    IGeometricPoint* p2 = new GeometricPoint(5, 0, false);  // Free

    DistanceConstraint* constraint = new DistanceConstraint(p1, p2, 10.0, false);

    QList<IConstraint*> constraints;
    constraints << constraint;

    bool success = solver.solve(constraints);

    QVERIFY(success);
    QVERIFY(solver.converged());
    QVERIFY(solver.iterationCount() > 0);
    QVERIFY(solver.residualError() < 1e-6);

    // Check distance is correct
    QVERIFY(qAbs(p1->distance(*p2) - 10.0) < 1e-3);

    delete constraint;
    delete p2;
    delete p1;
}

void TestConstraintSketch::testSolverMultipleConstraints()
{
    ConstraintSolver solver;

    // Triangle with 3 fixed side lengths
    IGeometricPoint* p1 = new GeometricPoint(0, 0, true);
    IGeometricPoint* p2 = new GeometricPoint(10, 0, false);
    IGeometricPoint* p3 = new GeometricPoint(5, 5, false);

    DistanceConstraint* c1 = new DistanceConstraint(p1, p2, 10.0, false);
    DistanceConstraint* c2 = new DistanceConstraint(p2, p3, 8.0, false);
    DistanceConstraint* c3 = new DistanceConstraint(p3, p1, 8.0, false);

    QList<IConstraint*> constraints;
    constraints << c1 << c2 << c3;

    bool success = solver.solve(constraints);

    QVERIFY(success);
    QVERIFY(solver.converged());

    delete c3;
    delete c2;
    delete c1;
    delete p3;
    delete p2;
    delete p1;
}

void TestConstraintSketch::testSolverConvergence()
{
    ConstraintSolver solver;
    solver.setMaxIterations(5);  // Very few iterations

    IGeometricPoint* p1 = new GeometricPoint(0, 0, true);
    IGeometricPoint* p2 = new GeometricPoint(1, 1, false);

    DistanceConstraint* constraint = new DistanceConstraint(p1, p2, 100.0, false);

    QList<IConstraint*> constraints;
    constraints << constraint;

    bool success = solver.solve(constraints);

    // Should stop after max iterations OR converge early
    QVERIFY(solver.iterationCount() <= 5);
    QVERIFY(solver.iterationCount() > 0);

    delete constraint;
    delete p2;
    delete p1;
}

// ============================================================================
// ConstraintSketch Tests
// ============================================================================

void TestConstraintSketch::testSketchConstruction()
{
    ConstraintSketch sketch("Test Sketch");

    QCOMPARE(sketch.name(), QString("Test Sketch"));
    QCOMPARE(sketch.unit(), ConstraintSketch::Millimeters);
    QCOMPARE(sketch.autoSolve(), false);
    QVERIFY(sketch.elements().isEmpty());
    QVERIFY(sketch.constraints().isEmpty());
    QVERIFY(sketch.solver() != nullptr);
}

void TestConstraintSketch::testSketchAddElements()
{
    ConstraintSketch sketch;

    IGeometricPoint* p1 = sketch.addPoint(0, 0, true);
    IGeometricPoint* p2 = sketch.addPoint(10, 0);

    QCOMPARE(sketch.elements().size(), 2);
    QVERIFY(p1->isLocked());
    QVERIFY(!p2->isLocked());

    IGeometricSegment* seg = sketch.addSegment(p1, p2);

    QCOMPARE(sketch.elements().size(), 3);
    QVERIFY(seg->isValid());
}

void TestConstraintSketch::testSketchAddConstraints()
{
    ConstraintSketch sketch;

    IGeometricPoint* p1 = sketch.addPoint(0, 0, true);
    IGeometricPoint* p2 = sketch.addPoint(5, 0);

    IConstraint* constraint = sketch.addDistanceConstraint(p1, p2, 10.0, false);

    QCOMPARE(sketch.constraints().size(), 1);
    QVERIFY(constraint != nullptr);
}

void TestConstraintSketch::testSketchSolveRectangle()
{
    ConstraintSketch sketch("Rectangle");

    // 4 corners
    IGeometricPoint* p1 = sketch.addPoint(0, 0, true);
    IGeometricPoint* p2 = sketch.addPoint(10, 0);
    IGeometricPoint* p3 = sketch.addPoint(10, 5);
    IGeometricPoint* p4 = sketch.addPoint(0, 5);

    // 4 sides
    IGeometricSegment* s1 = sketch.addSegment(p1, p2);
    IGeometricSegment* s2 = sketch.addSegment(p2, p3);
    IGeometricSegment* s3 = sketch.addSegment(p3, p4);
    IGeometricSegment* s4 = sketch.addSegment(p4, p1);

    // Length constraints
    sketch.addLengthConstraint(s1, 20.0, true);
    sketch.addLengthConstraint(s2, 10.0, true);
    sketch.addLengthConstraint(s3, 20.0, true);
    sketch.addLengthConstraint(s4, 10.0, true);

    // Solve
    bool success = sketch.solve();

    QVERIFY(success);
    QVERIFY(sketch.solver()->converged());

    // Check lengths
    QVERIFY(qAbs(s1->length() - 20.0) < 0.1);
    QVERIFY(qAbs(s2->length() - 10.0) < 0.1);
}

void TestConstraintSketch::testSketchUnits()
{
    ConstraintSketch sketch;

    sketch.setUnit(ConstraintSketch::Centimeters);
    QCOMPARE(sketch.unit(), ConstraintSketch::Centimeters);

    sketch.setUnit(ConstraintSketch::Inches);
    QCOMPARE(sketch.unit(), ConstraintSketch::Inches);
}

void TestConstraintSketch::testSketchExport()
{
    ConstraintSketch sketch;

    IGeometricPoint* p1 = sketch.addPoint(0, 0);
    IGeometricPoint* p2 = sketch.addPoint(10, 0);
    sketch.addSegment(p1, p2);

    QList<Point2D> polyline = sketch.toPolyline();
    QVERIFY(polyline.size() >= 2);

    QPainterPath path = sketch.toPainterPath();
    QVERIFY(!path.isEmpty());

    QRectF bbox = sketch.getBoundingBox();
    QVERIFY(!bbox.isNull());
}

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio
