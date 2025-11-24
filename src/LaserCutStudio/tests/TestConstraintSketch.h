#ifndef TESTCONSTRAINTSKETCH_H
#define TESTCONSTRAINTSKETCH_H

#include <QObject>
#include <QtTest>

namespace LaserCutStudio {
namespace Core {
namespace Tests {

/**
 * @brief Tests unitaires pour ConstraintSolver et ConstraintSketch
 */
class TestConstraintSketch : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // ConstraintSolver
    void testSolverConstruction();
    void testSolverSimpleDistance();
    void testSolverMultipleConstraints();
    void testSolverConvergence();

    // ConstraintSketch
    void testSketchConstruction();
    void testSketchAddElements();
    void testSketchAddConstraints();
    void testSketchSolveRectangle();
    void testSketchUnits();
    void testSketchExport();
};

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio

#endif // TESTCONSTRAINTSKETCH_H
