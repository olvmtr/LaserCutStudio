#include "TestGeometry.h"
#include "core/models/geometry/implementation/GeometricPoint.h"
#include "core/models/geometry/implementation/GeometricSegment.h"
#include "core/models/geometry/implementation/GeometricArc.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {
namespace Tests {

void TestGeometry::initTestCase()
{
    // Clear all instances before tests
    IGeometricElement::clearAllInstances();
}

void TestGeometry::cleanupTestCase()
{
    // Clean up after all tests
    IGeometricElement::clearAllInstances();
}

void TestGeometry::init()
{
    // Setup before each test
}

void TestGeometry::cleanup()
{
    // Cleanup after each test
}

// ============================================================================
// GeometricPoint Tests
// ============================================================================

void TestGeometry::testGeometricPointConstruction()
{
    IGeometricPoint* p1 = new GeometricPoint();
    QCOMPARE(p1->x(), 0.0);
    QCOMPARE(p1->y(), 0.0);
    QCOMPARE(p1->isLocked(), false);

    IGeometricPoint* p2 = new GeometricPoint(10.0, 20.0, true);
    QCOMPARE(p2->x(), 10.0);
    QCOMPARE(p2->y(), 20.0);
    QCOMPARE(p2->isLocked(), true);

    delete p1;
    delete p2;
}

void TestGeometry::testGeometricPointProperties()
{
    IGeometricPoint* p = new GeometricPoint(5.0, 10.0);

    // Test setters
    p->setX(15.0);
    QCOMPARE(p->x(), 15.0);

    p->setY(25.0);
    QCOMPARE(p->y(), 25.0);

    p->setPosition(30.0, 40.0);
    QCOMPARE(p->x(), 30.0);
    QCOMPARE(p->y(), 40.0);

    Point2D pos(50.0, 60.0);
    p->setPosition(pos);
    QCOMPARE(p->x(), 50.0);
    QCOMPARE(p->y(), 60.0);

    delete p;
}

void TestGeometry::testGeometricPointLocking()
{
    IGeometricPoint* p = new GeometricPoint(10.0, 20.0, false);
    QCOMPARE(p->isLocked(), false);

    p->setLocked(true);
    QCOMPARE(p->isLocked(), true);

    p->setLocked(false);
    QCOMPARE(p->isLocked(), false);

    delete p;
}

void TestGeometry::testGeometricPointDistance()
{
    IGeometricPoint* p1 = new GeometricPoint(0.0, 0.0);
    IGeometricPoint* p2 = new GeometricPoint(3.0, 4.0);

    // Distance should be 5.0 (Pythagorean triple 3-4-5)
    QCOMPARE(p1->distance(*p2), 5.0);
    QCOMPARE(p2->distance(*p1), 5.0);

    Point2D p3(6.0, 8.0);
    QCOMPARE(p1->distance(p3), 10.0);

    delete p1;
    delete p2;
}

void TestGeometry::testGeometricPointClone()
{
    IGeometricPoint* original = new GeometricPoint(10.0, 20.0, true);
    IGeometricPoint* cloned = static_cast<IGeometricPoint*>(original->clone());

    QVERIFY(cloned != nullptr);
    QVERIFY(cloned != original);
    QCOMPARE(cloned->x(), original->x());
    QCOMPARE(cloned->y(), original->y());
    QCOMPARE(cloned->isLocked(), original->isLocked());

    delete original;
    delete cloned;
}

void TestGeometry::testGeometricPointFactoryPattern()
{
    QCOMPARE(GeometricPoint::staticTypeName(), QString("GeometricPoint"));
    QVERIFY(IGeometricElement::availableTypes().contains("GeometricPoint"));

    QVariantMap data;
    data["type"] = "GeometricPoint";
    data["x"] = 15.0;
    data["y"] = 25.0;
    data["locked"] = true;

    IGeometricElement* element = IGeometricElement::create(data);
    QVERIFY(element != nullptr);

    IGeometricPoint* point = dynamic_cast<IGeometricPoint*>(element);
    QVERIFY(point != nullptr);
    QCOMPARE(point->x(), 15.0);
    QCOMPARE(point->y(), 25.0);
    QCOMPARE(point->isLocked(), true);

    delete element;
}

// ============================================================================
// GeometricSegment Tests
// ============================================================================

void TestGeometry::testGeometricSegmentConstruction()
{
    IGeometricPoint* p1 = new GeometricPoint(0.0, 0.0);
    IGeometricPoint* p2 = new GeometricPoint(10.0, 0.0);

    IGeometricSegment* seg = new GeometricSegment(p1, p2);
    QVERIFY(seg->isValid());
    QCOMPARE(seg->startPoint(), p1);
    QCOMPARE(seg->endPoint(), p2);

    delete seg;
    delete p2;
    delete p1;
}

void TestGeometry::testGeometricSegmentLength()
{
    IGeometricPoint* p1 = new GeometricPoint(0.0, 0.0);
    IGeometricPoint* p2 = new GeometricPoint(3.0, 4.0);

    IGeometricSegment* seg = new GeometricSegment(p1, p2);
    QCOMPARE(seg->length(), 5.0);  // 3-4-5 triangle

    delete seg;
    delete p2;
    delete p1;
}

void TestGeometry::testGeometricSegmentAngle()
{
    IGeometricPoint* p1 = new GeometricPoint(0.0, 0.0);
    IGeometricPoint* p2 = new GeometricPoint(1.0, 0.0);  // Horizontal right

    IGeometricSegment* seg = new GeometricSegment(p1, p2);
    QCOMPARE(seg->angle(), 0.0);  // 0° (East)

    p2->setPosition(0.0, 1.0);  // Vertical up
    QCOMPARE(seg->angle(), 90.0);  // 90° (North)

    p2->setPosition(-1.0, 0.0);  // Horizontal left
    QCOMPARE(seg->angle(), 180.0);  // 180° (West)

    delete seg;
    delete p2;
    delete p1;
}

void TestGeometry::testGeometricSegmentMidpoint()
{
    IGeometricPoint* p1 = new GeometricPoint(0.0, 0.0);
    IGeometricPoint* p2 = new GeometricPoint(10.0, 20.0);

    IGeometricSegment* seg = new GeometricSegment(p1, p2);
    Point2D mid = seg->midpoint();

    QCOMPARE(mid.x, 5.0);
    QCOMPARE(mid.y, 10.0);

    delete seg;
    delete p2;
    delete p1;
}

void TestGeometry::testGeometricSegmentDistanceToPoint()
{
    IGeometricPoint* p1 = new GeometricPoint(0.0, 0.0);
    IGeometricPoint* p2 = new GeometricPoint(10.0, 0.0);  // Horizontal segment

    IGeometricSegment* seg = new GeometricSegment(p1, p2);

    // Point on the segment
    Point2D onSegment(5.0, 0.0);
    QVERIFY(seg->distanceToPoint(onSegment) < 1e-6);

    // Point above the segment
    Point2D above(5.0, 3.0);
    QCOMPARE(seg->distanceToPoint(above), 3.0);

    delete seg;
    delete p2;
    delete p1;
}

void TestGeometry::testGeometricSegmentClone()
{
    IGeometricPoint* p1 = new GeometricPoint(0.0, 0.0);
    IGeometricPoint* p2 = new GeometricPoint(10.0, 20.0);

    IGeometricSegment* original = new GeometricSegment(p1, p2);
    IGeometricSegment* cloned = static_cast<IGeometricSegment*>(original->clone());

    QVERIFY(cloned != nullptr);
    QVERIFY(cloned != original);
    QVERIFY(cloned->isValid());

    // Cloned segment should have cloned points (not same pointers)
    QVERIFY(cloned->startPoint() != original->startPoint());
    QVERIFY(cloned->endPoint() != original->endPoint());

    // But same values
    QCOMPARE(cloned->startPoint()->x(), original->startPoint()->x());
    QCOMPARE(cloned->startPoint()->y(), original->startPoint()->y());
    QCOMPARE(cloned->length(), original->length());

    delete original;
    delete cloned;
    delete p2;
    delete p1;
}

void TestGeometry::testGeometricSegmentFactoryPattern()
{
    QCOMPARE(GeometricSegment::staticTypeName(), QString("GeometricSegment"));
    QVERIFY(IGeometricElement::availableTypes().contains("GeometricSegment"));
}

// ============================================================================
// GeometricArc Tests
// ============================================================================

void TestGeometry::testGeometricArcConstruction()
{
    IGeometricPoint* center = new GeometricPoint(10.0, 10.0);
    IGeometricArc* arc = new GeometricArc(center, 5.0, 0.0, 90.0);

    QVERIFY(arc->isValid());
    QCOMPARE(arc->center(), center);
    QCOMPARE(arc->radius(), 5.0);
    QCOMPARE(arc->startAngle(), 0.0);
    QCOMPARE(arc->endAngle(), 90.0);

    delete arc;
    delete center;
}

void TestGeometry::testGeometricArcBoundingBox()
{
    IGeometricPoint* center = new GeometricPoint(10.0, 10.0);
    IGeometricArc* arc = new GeometricArc(center, 5.0, 0.0, 360.0);

    QRectF bbox = arc->getBoundingBox();
    QCOMPARE(bbox.x(), 5.0);
    QCOMPARE(bbox.y(), 5.0);
    QCOMPARE(bbox.width(), 10.0);
    QCOMPARE(bbox.height(), 10.0);

    delete arc;
    delete center;
}

void TestGeometry::testGeometricArcPoints()
{
    IGeometricPoint* center = new GeometricPoint(0.0, 0.0);
    IGeometricArc* arc = new GeometricArc(center, 10.0, 0.0, 90.0);

    QList<Point2D> points = arc->getPoints(4);  // 5 points (0, 25%, 50%, 75%, 100%)
    QVERIFY(points.size() >= 2);

    // First point should be at 0° (10, 0)
    QVERIFY(qAbs(points.first().x - 10.0) < 0.1);
    QVERIFY(qAbs(points.first().y - 0.0) < 0.1);

    // Last point should be at 90° (0, 10)
    QVERIFY(qAbs(points.last().x - 0.0) < 0.1);
    QVERIFY(qAbs(points.last().y - 10.0) < 0.1);

    delete arc;
    delete center;
}

void TestGeometry::testGeometricArcClone()
{
    IGeometricPoint* center = new GeometricPoint(5.0, 5.0);
    IGeometricArc* original = new GeometricArc(center, 3.0, 0.0, 180.0);

    IGeometricArc* cloned = static_cast<IGeometricArc*>(original->clone());

    QVERIFY(cloned != nullptr);
    QVERIFY(cloned != original);
    QVERIFY(cloned->isValid());

    // Cloned arc should have cloned center (not same pointer)
    QVERIFY(cloned->center() != original->center());

    // But same values
    QCOMPARE(cloned->center()->x(), original->center()->x());
    QCOMPARE(cloned->center()->y(), original->center()->y());
    QCOMPARE(cloned->radius(), original->radius());
    QCOMPARE(cloned->startAngle(), original->startAngle());
    QCOMPARE(cloned->endAngle(), original->endAngle());

    delete original;
    delete cloned;
    delete center;
}

void TestGeometry::testGeometricArcFactoryPattern()
{
    QCOMPARE(GeometricArc::staticTypeName(), QString("GeometricArc"));
    QVERIFY(IGeometricElement::availableTypes().contains("GeometricArc"));
}

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio
