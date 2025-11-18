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

// ===== Tests du Factory Pattern =====

void TestPart::testFactoryAvailableTypes()
{
    QStringList types = IPart::availableTypes();

    // Vérifie que le type Part est enregistré
    QVERIFY(types.contains("Part"));
    QCOMPARE(types.size(), 1);
}

void TestPart::testFactoryCreate()
{
    // Teste la création d'un Part via Factory
    QVariantMap partConfig;
    partConfig["type"] = "Part";
    // Note: name, thickness, material ne sont pas des Q_PROPERTY dans Part
    // donc on teste juste la création

    IPart* part = IPart::create(partConfig);
    QVERIFY(part != nullptr);
    QCOMPARE(part->getTypeName(), QString("Part"));

    Part* partCasted = dynamic_cast<Part*>(part);
    QVERIFY(partCasted != nullptr);

    delete part;

    // Teste un type inconnu
    QVariantMap unknownConfig;
    unknownConfig["type"] = "UnknownPart";

    IPart* unknownPart = IPart::create(unknownConfig);
    QVERIFY(unknownPart == nullptr);
}

void TestPart::testFactoryToVariant()
{
    // Teste la sérialisation d'un Part
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    Part part("TestPart", rect, 5.0, Material::Wood());

    QVariantMap partMap = part.toVariant();

    QCOMPARE(partMap["type"].toString(), QString("Part"));
    // Note: name, thickness, material ne sont pas des Q_PROPERTY
    // donc ils ne seront pas dans le QVariantMap automatiquement
    // C'est normal car Part n'a pas de Q_PROPERTY propres

    delete rect;
}
