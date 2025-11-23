#ifndef TESTSHAPES_H
#define TESTSHAPES_H

#include <QObject>
#include <QTest>
#include "core/models/shapes/Rectangle.h"
#include "core/models/shapes/Circle.h"

using namespace LaserCutStudio::Core;

class TestShapes : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Tests pour Rectangle
    void testRectangleConstruction();
    void testRectangleArea();
    void testRectangleBoundingBox();
    void testRectangleContainsPoint();
    void testRectangleTranslate();
    void testRectangleScale();
    void testRectangleClone();

    // Tests pour Circle
    void testCircleConstruction();
    void testCircleArea();
    void testCircleBoundingBox();
    void testCircleContainsPoint();
    void testCircleTranslate();
    void testCircleClone();

    // Tests pour la liste statique
    void testShapeStaticList();

    // Tests du Factory Pattern
    void testFactoryAvailableTypes();
    void testFactoryCreate();
    void testFactoryToVariant();
    void testFactoryRoundtrip();
};

#endif // TESTSHAPES_H
