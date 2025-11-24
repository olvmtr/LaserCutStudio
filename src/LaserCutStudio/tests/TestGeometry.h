#ifndef TESTGEOMETRY_H
#define TESTGEOMETRY_H

#include <QObject>
#include <QtTest>

namespace LaserCutStudio {
namespace Core {
namespace Tests {

/**
 * @brief Tests unitaires pour les éléments géométriques
 *
 * Teste GeometricPoint, GeometricSegment, GeometricArc
 */
class TestGeometry : public QObject
{
    Q_OBJECT

private slots:
    // Setup/Cleanup
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // GeometricPoint tests
    void testGeometricPointConstruction();
    void testGeometricPointProperties();
    void testGeometricPointLocking();
    void testGeometricPointDistance();
    void testGeometricPointClone();
    void testGeometricPointFactoryPattern();

    // GeometricSegment tests
    void testGeometricSegmentConstruction();
    void testGeometricSegmentLength();
    void testGeometricSegmentAngle();
    void testGeometricSegmentMidpoint();
    void testGeometricSegmentDistanceToPoint();
    void testGeometricSegmentClone();
    void testGeometricSegmentFactoryPattern();

    // GeometricArc tests
    void testGeometricArcConstruction();
    void testGeometricArcBoundingBox();
    void testGeometricArcPoints();
    void testGeometricArcClone();
    void testGeometricArcFactoryPattern();
};

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio

#endif // TESTGEOMETRY_H
