#ifndef TESTPART_H
#define TESTPART_H

#include <QObject>
#include <QTest>
#include "core/models/parts/implementations/Part.h"
#include "core/models/shapes/implementations/Rectangle.h"

using namespace LaserCutStudio::Core;

class TestPart : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testPartConstruction();
    void testPartGettersSetters();
    void testPartVolume();
    void testPartMass();
    void testPartClone();
    void testPartStaticList();

    // Tests du Factory Pattern
    void testFactoryAvailableTypes();
    void testFactoryCreate();
    void testFactoryToVariant();
};

#endif // TESTPART_H
