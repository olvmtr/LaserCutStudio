#include "TestProject.h"

void TestProject::initTestCase()
{
    IProject::clearAllProjects();
    IPart::clearAllParts();
    IShape::clearAllShapes();
}

void TestProject::cleanupTestCase()
{
    IProject::clearAllProjects();
    IPart::clearAllParts();
    IShape::clearAllShapes();
}

void TestProject::testProjectConstruction()
{
    Project project("MyProject");

    QCOMPARE(project.getName(), QString("MyProject"));
    QVERIFY(project.isEmpty());
    QCOMPARE(project.getPartCount(), 0);
}

void TestProject::testProjectGettersSetters()
{
    Project project;

    project.setName("NewProject");
    QCOMPARE(project.getName(), QString("NewProject"));

    ProjectMetadata metadata;
    metadata.author = "Test Author";
    metadata.description = "Test Description";
    metadata.version = "2.0";

    project.setMetadata(metadata);

    ProjectMetadata retrieved = project.getMetadata();
    QCOMPARE(retrieved.author, QString("Test Author"));
    QCOMPARE(retrieved.description, QString("Test Description"));
    QCOMPARE(retrieved.version, QString("2.0"));
}

void TestProject::testProjectAddRemovePart()
{
    Project project("TestProject");

    Rectangle* rect1 = new Rectangle(0, 0, 10, 10);
    Rectangle* rect2 = new Rectangle(0, 0, 20, 20);

    Part* part1 = new Part("Part1", rect1, 3.0, Material::Wood());
    Part* part2 = new Part("Part2", rect2, 3.0, Material::Plywood());

    project.addPart(part1);
    QCOMPARE(project.getPartCount(), 1);
    QVERIFY(!project.isEmpty());

    project.addPart(part2);
    QCOMPARE(project.getPartCount(), 2);

    project.removePart(part1);
    QCOMPARE(project.getPartCount(), 1);

    project.removePart(part2);
    QCOMPARE(project.getPartCount(), 0);
    QVERIFY(project.isEmpty());

    delete part1;
    delete part2;
    delete rect1;
    delete rect2;
}

void TestProject::testProjectTotalVolume()
{
    Project project("TestProject");

    Rectangle* rect1 = new Rectangle(0, 0, 100, 50); // 5000 mm²
    Rectangle* rect2 = new Rectangle(0, 0, 200, 100); // 20000 mm²

    Part* part1 = new Part("Part1", rect1, 3.0, Material::Wood()); // 15000 mm³
    Part* part2 = new Part("Part2", rect2, 3.0, Material::Wood()); // 60000 mm³

    project.addPart(part1);
    project.addPart(part2);

    double expectedVolume = 15000.0 + 60000.0; // 75000 mm³
    QCOMPARE(project.getTotalVolume(), expectedVolume);

    delete part1;
    delete part2;
    delete rect1;
    delete rect2;
}

void TestProject::testProjectTotalMass()
{
    Project project("TestProject");

    Rectangle* rect = new Rectangle(0, 0, 100, 50); // 5000 mm²
    Part* part = new Part("Part", rect, 3.0, Material::Wood()); // 15000 mm³

    project.addPart(part);

    // Volume = 15000 mm³ = 1.5e-5 m³
    // Densité Wood = 600 kg/m³
    // Masse = 1.5e-5 * 600 = 0.009 kg
    double expectedMass = (15000.0 / 1e9) * 600.0;
    QCOMPARE(project.getTotalMass(), expectedMass);

    delete part;
    delete rect;
}

void TestProject::testProjectClone()
{
    Project* project1 = new Project("TestProject");

    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    Part* part = new Part("Part1", rect, 3.0, Material::Wood());

    project1->addPart(part);

    IProject* project2 = project1->clone();

    Project* project2Casted = dynamic_cast<Project*>(project2);
    QVERIFY(project2Casted != nullptr);

    QCOMPARE(project2Casted->getName(), QString("TestProject"));
    QCOMPARE(project2Casted->getPartCount(), 1);

    // Les IDs doivent être différents
    QVERIFY(project1->getId() != project2->getId());

    delete project1;
    delete project2;
    delete part;
    delete rect;
}

void TestProject::testProjectStaticList()
{
    IProject::clearAllProjects();

    int initialCount = IProject::getAllProjects().count();

    Project* proj1 = new Project();
    Project* proj2 = new Project();

    QCOMPARE(IProject::getAllProjects().count(), initialCount + 2);

    delete proj1;
    QCOMPARE(IProject::getAllProjects().count(), initialCount + 1);

    delete proj2;
    QCOMPARE(IProject::getAllProjects().count(), initialCount);
}
