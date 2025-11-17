#include "TestShapes.h"
#include <cmath>

void TestShapes::initTestCase()
{
    // Nettoie la liste avant les tests
    IShape::clearAllShapes();
}

void TestShapes::cleanupTestCase()
{
    // Nettoie après les tests
    IShape::clearAllShapes();
}

// Tests pour Rectangle
void TestShapes::testRectangleConstruction()
{
    Rectangle rect(10.0, 20.0, 30.0, 40.0);
    QCOMPARE(rect.getX(), 10.0);
    QCOMPARE(rect.getY(), 20.0);
    QCOMPARE(rect.getWidth(), 30.0);
    QCOMPARE(rect.getHeight(), 40.0);
}

void TestShapes::testRectangleArea()
{
    Rectangle rect(0.0, 0.0, 10.0, 20.0);
    QCOMPARE(rect.getArea(), 200.0);
}

void TestShapes::testRectangleBoundingBox()
{
    Rectangle rect(5.0, 10.0, 15.0, 20.0);
    QRectF bbox = rect.getBoundingBox();

    QCOMPARE(bbox.x(), 5.0);
    QCOMPARE(bbox.y(), 10.0);
    QCOMPARE(bbox.width(), 15.0);
    QCOMPARE(bbox.height(), 20.0);
}

void TestShapes::testRectangleContainsPoint()
{
    Rectangle rect(0.0, 0.0, 10.0, 10.0);

    Point2D inside(5.0, 5.0);
    QVERIFY(rect.containsPoint(inside));

    Point2D outside(15.0, 15.0);
    QVERIFY(!rect.containsPoint(outside));

    Point2D edge(10.0, 10.0);
    QVERIFY(rect.containsPoint(edge));
}

void TestShapes::testRectangleTranslate()
{
    Rectangle rect(0.0, 0.0, 10.0, 10.0);
    rect.translate(5.0, 3.0);

    QCOMPARE(rect.getX(), 5.0);
    QCOMPARE(rect.getY(), 3.0);
}

void TestShapes::testRectangleScale()
{
    Rectangle rect(0.0, 0.0, 10.0, 20.0);
    Point2D center(5.0, 10.0);
    rect.scale(2.0, 2.0, center);

    QCOMPARE(rect.getWidth(), 20.0);
    QCOMPARE(rect.getHeight(), 40.0);
}

void TestShapes::testRectangleClone()
{
    Rectangle* rect1 = new Rectangle(10.0, 20.0, 30.0, 40.0);
    IShape* rect2 = rect1->clone();

    Rectangle* rect2Casted = dynamic_cast<Rectangle*>(rect2);
    QVERIFY(rect2Casted != nullptr);

    QCOMPARE(rect2Casted->getX(), 10.0);
    QCOMPARE(rect2Casted->getY(), 20.0);
    QCOMPARE(rect2Casted->getWidth(), 30.0);
    QCOMPARE(rect2Casted->getHeight(), 40.0);

    // Les IDs doivent être différents (pattern Prototype)
    QVERIFY(rect1->getId() != rect2->getId());

    delete rect1;
    delete rect2;
}

// Tests pour Circle
void TestShapes::testCircleConstruction()
{
    Circle circle(10.0, 20.0, 5.0);
    QCOMPARE(circle.getCenterX(), 10.0);
    QCOMPARE(circle.getCenterY(), 20.0);
    QCOMPARE(circle.getRadius(), 5.0);
}

void TestShapes::testCircleArea()
{
    Circle circle(0.0, 0.0, 5.0);
    double expectedArea = M_PI * 25.0;
    QCOMPARE(circle.getArea(), expectedArea);
}

void TestShapes::testCircleBoundingBox()
{
    Circle circle(10.0, 10.0, 5.0);
    QRectF bbox = circle.getBoundingBox();

    QCOMPARE(bbox.x(), 5.0);
    QCOMPARE(bbox.y(), 5.0);
    QCOMPARE(bbox.width(), 10.0);
    QCOMPARE(bbox.height(), 10.0);
}

void TestShapes::testCircleContainsPoint()
{
    Circle circle(0.0, 0.0, 5.0);

    Point2D inside(3.0, 0.0);
    QVERIFY(circle.containsPoint(inside));

    Point2D outside(10.0, 0.0);
    QVERIFY(!circle.containsPoint(outside));

    Point2D edge(5.0, 0.0);
    QVERIFY(circle.containsPoint(edge));
}

void TestShapes::testCircleTranslate()
{
    Circle circle(0.0, 0.0, 5.0);
    circle.translate(10.0, 15.0);

    QCOMPARE(circle.getCenterX(), 10.0);
    QCOMPARE(circle.getCenterY(), 15.0);
    QCOMPARE(circle.getRadius(), 5.0);
}

void TestShapes::testCircleClone()
{
    Circle* circle1 = new Circle(10.0, 20.0, 5.0);
    IShape* circle2 = circle1->clone();

    Circle* circle2Casted = dynamic_cast<Circle*>(circle2);
    QVERIFY(circle2Casted != nullptr);

    QCOMPARE(circle2Casted->getCenterX(), 10.0);
    QCOMPARE(circle2Casted->getCenterY(), 20.0);
    QCOMPARE(circle2Casted->getRadius(), 5.0);

    QVERIFY(circle1->getId() != circle2->getId());

    delete circle1;
    delete circle2;
}

// Tests pour la liste statique
void TestShapes::testShapeStaticList()
{
    IShape::clearAllShapes();

    int initialCount = IShape::getAllShapes().count();

    Rectangle* rect = new Rectangle(0.0, 0.0, 10.0, 10.0);
    Circle* circle = new Circle(0.0, 0.0, 5.0);

    QCOMPARE(IShape::getAllShapes().count(), initialCount + 2);

    delete rect;
    QCOMPARE(IShape::getAllShapes().count(), initialCount + 1);

    delete circle;
    QCOMPARE(IShape::getAllShapes().count(), initialCount);
}
