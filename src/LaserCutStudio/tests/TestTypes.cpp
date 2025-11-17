#include "TestTypes.h"

// Tests pour Point2D
void TestTypes::testPoint2DConstruction()
{
    Point2D p1;
    QCOMPARE(p1.x, 0.0);
    QCOMPARE(p1.y, 0.0);

    Point2D p2(5.0, 10.0);
    QCOMPARE(p2.x, 5.0);
    QCOMPARE(p2.y, 10.0);
}

void TestTypes::testPoint2DDistance()
{
    Point2D p1(0.0, 0.0);
    Point2D p2(3.0, 4.0);

    double dist = p1.distance(p2);
    QCOMPARE(dist, 5.0);
}

void TestTypes::testPoint2DOperators()
{
    Point2D p1(1.0, 2.0);
    Point2D p2(3.0, 4.0);

    // Test addition
    Point2D p3 = p1 + p2;
    QCOMPARE(p3.x, 4.0);
    QCOMPARE(p3.y, 6.0);

    // Test soustraction
    Point2D p4 = p2 - p1;
    QCOMPARE(p4.x, 2.0);
    QCOMPARE(p4.y, 2.0);

    // Test multiplication par scalaire
    Point2D p5 = p1 * 2.0;
    QCOMPARE(p5.x, 2.0);
    QCOMPARE(p5.y, 4.0);

    // Test égalité
    Point2D p6(1.0, 2.0);
    QVERIFY(p1 == p6);
}

// Tests pour Point3D
void TestTypes::testPoint3DConstruction()
{
    Point3D p1;
    QCOMPARE(p1.x, 0.0);
    QCOMPARE(p1.y, 0.0);
    QCOMPARE(p1.z, 0.0);

    Point3D p2(5.0, 10.0, 15.0);
    QCOMPARE(p2.x, 5.0);
    QCOMPARE(p2.y, 10.0);
    QCOMPARE(p2.z, 15.0);
}

void TestTypes::testPoint3DDistance()
{
    Point3D p1(0.0, 0.0, 0.0);
    Point3D p2(3.0, 4.0, 0.0);

    double dist = p1.distance(p2);
    QCOMPARE(dist, 5.0);
}

void TestTypes::testPoint3DOperators()
{
    Point3D p1(1.0, 2.0, 3.0);
    Point3D p2(4.0, 5.0, 6.0);

    // Test addition
    Point3D p3 = p1 + p2;
    QCOMPARE(p3.x, 5.0);
    QCOMPARE(p3.y, 7.0);
    QCOMPARE(p3.z, 9.0);

    // Test soustraction
    Point3D p4 = p2 - p1;
    QCOMPARE(p4.x, 3.0);
    QCOMPARE(p4.y, 3.0);
    QCOMPARE(p4.z, 3.0);

    // Test multiplication
    Point3D p5 = p1 * 2.0;
    QCOMPARE(p5.x, 2.0);
    QCOMPARE(p5.y, 4.0);
    QCOMPARE(p5.z, 6.0);

    // Test égalité
    Point3D p6(1.0, 2.0, 3.0);
    QVERIFY(p1 == p6);
}

// Tests pour Material
void TestTypes::testMaterialConstruction()
{
    Material m1;
    QCOMPARE(m1.getName(), QString("Unknown"));
    QCOMPARE(m1.getDensity(), 0.0);

    Material m2("TestMaterial", 500.0, QColor(Qt::red));
    QCOMPARE(m2.getName(), QString("TestMaterial"));
    QCOMPARE(m2.getDensity(), 500.0);
    QCOMPARE(m2.getColor(), QColor(Qt::red));
}

void TestTypes::testMaterialGettersSetters()
{
    Material m;

    m.setName("Wood");
    QCOMPARE(m.getName(), QString("Wood"));

    m.setDensity(600.0);
    QCOMPARE(m.getDensity(), 600.0);

    m.setColor(QColor(Qt::blue));
    QCOMPARE(m.getColor(), QColor(Qt::blue));
}

void TestTypes::testMaterialPredefined()
{
    Material wood = Material::Wood();
    QCOMPARE(wood.getName(), QString("Wood"));
    QCOMPARE(wood.getDensity(), 600.0);

    Material plywood = Material::Plywood();
    QCOMPARE(plywood.getName(), QString("Plywood"));
    QCOMPARE(plywood.getDensity(), 550.0);

    Material mdf = Material::MDF();
    QCOMPARE(mdf.getName(), QString("MDF"));
    QCOMPARE(mdf.getDensity(), 750.0);
}
