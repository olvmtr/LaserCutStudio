#ifndef TESTPROJECT_H
#define TESTPROJECT_H

#include <QObject>
#include <QTest>
#include "core/models/projects/implementations/Project.h"
#include "core/models/parts/implementations/Part.h"
#include "core/models/shapes/implementations/Rectangle.h"

using namespace LaserCutStudio::Core;

class TestProject : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testProjectConstruction();
    void testProjectGettersSetters();
    void testProjectAddRemovePart();
    void testProjectTotalVolume();
    void testProjectTotalMass();
    void testProjectClone();
    void testProjectStaticList();

    // Tests du Factory Pattern
    void testFactoryAvailableTypes();
    void testFactoryCreate();
    void testFactoryToVariant();
};

#endif // TESTPROJECT_H
