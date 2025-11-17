#include "TestPart.h"

void TestPart::initTestCase()
{
    IPart::clearAllParts();
    IShape::clearAllShapes();
}

void TestPart::cleanupTestCase()
{
    IPart::clearAllParts();
    IShape::clearAllShapes();
}

void TestPart::testPartConstruction()
{
    Rectangle* rect = new Rectangle(0.0, 0.0, 100.0, 50.0);
    Part part("TestPart", rect, 3.0, Material::Plywood());

    QCOMPARE(part.getName(), QString("TestPart"));
    QCOMPARE(part.getShape(), rect);
    QCOMPARE(part.getThickness(), 3.0);
    QCOMPARE(part.getMaterial().getName(), QString("Plywood"));

    delete rect;
}

void TestPart::testPartGettersSetters()
{
    Part part;

    part.setName("MyPart");
    QCOMPARE(part.getName(), QString("MyPart"));

    part.setThickness(5.0);
    QCOMPARE(part.getThickness(), 5.0);

    Material wood = Material::Wood();
    part.setMaterial(wood);
    QCOMPARE(part.getMaterial().getName(), QString("Wood"));
}

void TestPart::testPartVolume()
{
    // Rectangle de 100x50 mm avec épaisseur 3 mm
    Rectangle* rect = new Rectangle(0.0, 0.0, 100.0, 50.0);
    Part part("TestPart", rect, 3.0, Material::Plywood());

    double expectedVolume = 100.0 * 50.0 * 3.0; // 15000 mm³
    QCOMPARE(part.getVolume(), expectedVolume);

    delete rect;
}

void TestPart::testPartMass()
{
    // Rectangle de 100x50 mm avec épaisseur 3 mm
    Rectangle* rect = new Rectangle(0.0, 0.0, 100.0, 50.0);
    Part part("TestPart", rect, 3.0, Material::Plywood());

    // Volume = 15000 mm³ = 15000 / 1e9 m³ = 1.5e-5 m³
    // Densité Plywood = 550 kg/m³
    // Masse = 1.5e-5 * 550 = 0.00825 kg
    double expectedMass = (15000.0 / 1e9) * 550.0;
    QCOMPARE(part.getMass(), expectedMass);

    delete rect;
}

void TestPart::testPartClone()
{
    Rectangle* rect = new Rectangle(0.0, 0.0, 100.0, 50.0);
    Part* part1 = new Part("TestPart", rect, 3.0, Material::Wood());

    IPart* part2 = part1->clone();

    Part* part2Casted = dynamic_cast<Part*>(part2);
    QVERIFY(part2Casted != nullptr);

    QCOMPARE(part2Casted->getName(), QString("TestPart"));
    QCOMPARE(part2Casted->getThickness(), 3.0);
    QCOMPARE(part2Casted->getMaterial().getName(), QString("Wood"));

    // Les IDs doivent être différents
    QVERIFY(part1->getId() != part2->getId());

    delete rect;
    delete part1;
    delete part2;
}

void TestPart::testPartStaticList()
{
    IPart::clearAllParts();

    int initialCount = IPart::getAllParts().count();

    Part* part1 = new Part();
    Part* part2 = new Part();

    QCOMPARE(IPart::getAllParts().count(), initialCount + 2);

    delete part1;
    QCOMPARE(IPart::getAllParts().count(), initialCount + 1);

    delete part2;
    QCOMPARE(IPart::getAllParts().count(), initialCount);
}
