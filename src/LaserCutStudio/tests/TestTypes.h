#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QObject>
#include <QTest>
#include "core/models/base/types/Point2D.h"
#include "core/models/base/types/Point3D.h"
#include "core/models/base/types/Material.h"

using namespace LaserCutStudio::Core;

class TestTypes : public QObject
{
    Q_OBJECT

private slots:
    // Tests pour Point2D
    void testPoint2DConstruction();
    void testPoint2DDistance();
    void testPoint2DOperators();

    // Tests pour Point3D
    void testPoint3DConstruction();
    void testPoint3DDistance();
    void testPoint3DOperators();

    // Tests pour Material
    void testMaterialConstruction();
    void testMaterialGettersSetters();
    void testMaterialPredefined();
};

#endif // TESTTYPES_H
