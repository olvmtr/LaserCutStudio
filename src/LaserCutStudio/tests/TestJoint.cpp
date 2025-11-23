#include "TestJoint.h"
#include "core/models/shapes/Rectangle.h"

void TestJoint::initTestCase()
{
    IJoint::clearAllJoints();
    IPart::clearAllParts();
    IShape::clearAllShapes();
}

void TestJoint::cleanupTestCase()
{
    IJoint::clearAllJoints();
    IPart::clearAllParts();
    IShape::clearAllShapes();
}

void TestJoint::testTabJointConstruction()
{
    Rectangle* shapeA = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeB = new Rectangle(0, 0, 10, 10);
    Part* partA = new Part("PartA", shapeA, 3.0, Material::Wood());
    Part* partB = new Part("PartB", shapeB, 3.0, Material::Wood());

    TabJoint joint(partA, partB, Point3D(0, 0, 0), 90.0, 10.0, 3.0);

    QCOMPARE(joint.getType(), JointType::TAB);
    QCOMPARE(joint.getPartA(), partA);
    QCOMPARE(joint.getPartB(), partB);
    QCOMPARE(joint.getAngle(), 90.0);
    QCOMPARE(joint.getTabWidth(), 10.0);
    QCOMPARE(joint.getTabDepth(), 3.0);

    delete partA;
    delete partB;
    delete shapeA;
    delete shapeB;
}

void TestJoint::testTabJointGettersSetters()
{
    TabJoint joint;

    joint.setTabWidth(15.0);
    QCOMPARE(joint.getTabWidth(), 15.0);

    joint.setTabDepth(5.0);
    QCOMPARE(joint.getTabDepth(), 5.0);

    joint.setAngle(45.0);
    QCOMPARE(joint.getAngle(), 45.0);
}

void TestJoint::testTabJointConnect()
{
    Rectangle* shapeA = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeB = new Rectangle(0, 0, 10, 10);
    Part* partA = new Part("PartA", shapeA, 3.0, Material::Wood());
    Part* partB = new Part("PartB", shapeB, 3.0, Material::Wood());

    TabJoint* joint = new TabJoint();

    joint->connect(partA, partB);

    QCOMPARE(joint->getPartA(), partA);
    QCOMPARE(joint->getPartB(), partB);
    QVERIFY(joint->isValid());

    // Vérifie que le joint est ajouté aux pièces
    QVERIFY(partA->getJoints().contains(joint));
    QVERIFY(partB->getJoints().contains(joint));

    delete joint;
    delete partA;
    delete partB;
    delete shapeA;
    delete shapeB;
}

void TestJoint::testTabJointClone()
{
    Rectangle* shapeA = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeB = new Rectangle(0, 0, 10, 10);
    Part* partA = new Part("PartA", shapeA, 3.0, Material::Wood());
    Part* partB = new Part("PartB", shapeB, 3.0, Material::Wood());

    TabJoint* joint1 = new TabJoint(partA, partB, Point3D(0, 0, 0), 90.0, 10.0, 3.0);
    IJoint* joint2 = joint1->clone();

    TabJoint* joint2Casted = dynamic_cast<TabJoint*>(joint2);
    QVERIFY(joint2Casted != nullptr);

    QCOMPARE(joint2Casted->getTabWidth(), 10.0);
    QCOMPARE(joint2Casted->getTabDepth(), 3.0);
    QVERIFY(joint1->getId() != joint2->getId());

    delete joint1;
    delete joint2;
    delete partA;
    delete partB;
    delete shapeA;
    delete shapeB;
}

void TestJoint::testFingerJointConstruction()
{
    Rectangle* shapeA = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeB = new Rectangle(0, 0, 10, 10);
    Part* partA = new Part("PartA", shapeA, 3.0, Material::Wood());
    Part* partB = new Part("PartB", shapeB, 3.0, Material::Wood());

    FingerJoint joint(partA, partB, Point3D(0, 0, 0), 90.0, 5, 5.0);

    QCOMPARE(joint.getType(), JointType::FINGER);
    QCOMPARE(joint.getFingerCount(), 5);
    QCOMPARE(joint.getFingerWidth(), 5.0);

    delete partA;
    delete partB;
    delete shapeA;
    delete shapeB;
}

void TestJoint::testFingerJointGettersSetters()
{
    FingerJoint joint;

    joint.setFingerCount(7);
    QCOMPARE(joint.getFingerCount(), 7);

    joint.setFingerWidth(3.0);
    QCOMPARE(joint.getFingerWidth(), 3.0);
}

void TestJoint::testJointStaticList()
{
    IJoint::clearAllJoints();

    int initialCount = IJoint::getAllJoints().count();

    TabJoint* joint1 = new TabJoint();
    FingerJoint* joint2 = new FingerJoint();

    QCOMPARE(IJoint::getAllJoints().count(), initialCount + 2);

    delete joint1;
    QCOMPARE(IJoint::getAllJoints().count(), initialCount + 1);

    delete joint2;
    QCOMPARE(IJoint::getAllJoints().count(), initialCount);
}

// ===== Tests du Factory Pattern =====

void TestJoint::testFactoryAvailableTypes()
{
    QStringList types = IJoint::availableTypes();

    // Vérifie que les deux types concrets sont enregistrés
    QVERIFY(types.contains("TabJoint"));
    QVERIFY(types.contains("FingerJoint"));
    QCOMPARE(types.size(), 2);
}

void TestJoint::testFactoryCreate()
{
    // Teste la création d'un TabJoint
    QVariantMap tabConfig;
    tabConfig["type"] = "TabJoint";
    tabConfig["tabWidth"] = 12.5;
    tabConfig["tabDepth"] = 4.0;

    IJoint* tabJoint = IJoint::create(tabConfig);
    QVERIFY(tabJoint != nullptr);
    QCOMPARE(tabJoint->getTypeName(), QString("TabJoint"));

    TabJoint* tab = dynamic_cast<TabJoint*>(tabJoint);
    QVERIFY(tab != nullptr);
    QCOMPARE(tab->getTabWidth(), 12.5);
    QCOMPARE(tab->getTabDepth(), 4.0);

    delete tabJoint;

    // Teste la création d'un FingerJoint
    QVariantMap fingerConfig;
    fingerConfig["type"] = "FingerJoint";
    fingerConfig["fingerCount"] = 8;
    fingerConfig["fingerWidth"] = 6.0;

    IJoint* fingerJoint = IJoint::create(fingerConfig);
    QVERIFY(fingerJoint != nullptr);
    QCOMPARE(fingerJoint->getTypeName(), QString("FingerJoint"));

    FingerJoint* finger = dynamic_cast<FingerJoint*>(fingerJoint);
    QVERIFY(finger != nullptr);
    QCOMPARE(finger->getFingerCount(), 8);
    QCOMPARE(finger->getFingerWidth(), 6.0);

    delete fingerJoint;

    // Teste un type inconnu
    QVariantMap unknownConfig;
    unknownConfig["type"] = "UnknownJoint";

    IJoint* unknownJoint = IJoint::create(unknownConfig);
    QVERIFY(unknownJoint == nullptr);
}

void TestJoint::testFactoryToVariant()
{
    // Teste la sérialisation d'un TabJoint
    TabJoint tab;
    tab.setTabWidth(15.0);
    tab.setTabDepth(5.0);

    QVariantMap tabMap = tab.toVariant();

    QCOMPARE(tabMap["type"].toString(), QString("TabJoint"));
    QCOMPARE(tabMap["tabWidth"].toDouble(), 15.0);
    QCOMPARE(tabMap["tabDepth"].toDouble(), 5.0);

    // Teste la sérialisation d'un FingerJoint
    FingerJoint finger;
    finger.setFingerCount(10);
    finger.setFingerWidth(7.5);

    QVariantMap fingerMap = finger.toVariant();

    QCOMPARE(fingerMap["type"].toString(), QString("FingerJoint"));
    QCOMPARE(fingerMap["fingerCount"].toInt(), 10);
    QCOMPARE(fingerMap["fingerWidth"].toDouble(), 7.5);
}

void TestJoint::testFactoryRoundtrip()
{
    // Teste un cycle complet : création -> sérialisation -> désérialisation
    TabJoint* original = new TabJoint();
    original->setTabWidth(20.0);
    original->setTabDepth(8.0);

    // Sérialise
    QVariantMap serialized = original->toVariant();

    // Désérialise
    IJoint* restored = IJoint::create(serialized);
    QVERIFY(restored != nullptr);

    TabJoint* restoredTab = dynamic_cast<TabJoint*>(restored);
    QVERIFY(restoredTab != nullptr);

    // Vérifie que les valeurs sont restaurées
    QCOMPARE(restoredTab->getTabWidth(), 20.0);
    QCOMPARE(restoredTab->getTabDepth(), 8.0);

    // Vérifie que ce sont deux objets différents
    QVERIFY(original->getId() != restored->getId());

    delete original;
    delete restored;
}

// ===== Tests du système Signals/Slots =====

void TestJoint::testSignalsAutoCleanup()
{
    // Teste le nettoyage automatique via Signals/Slots
    Rectangle* shapeA = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeB = new Rectangle(0, 0, 10, 10);
    Part* partA = new Part("PartA", shapeA, 3.0, Material::Wood());
    Part* partB = new Part("PartB", shapeB, 3.0, Material::Wood());

    TabJoint* joint = new TabJoint(partA, partB, Point3D(0, 0, 0), 90.0, 10.0, 3.0);

    // Vérifie que le joint est bien connecté
    QCOMPARE(joint->getPartA(), partA);
    QCOMPARE(joint->getPartB(), partB);
    QVERIFY(joint->isValid());

    // Détruit partA - le signal aboutToBeDestroyed() devrait être émis
    // et le joint devrait automatiquement mettre m_partA à nullptr
    delete partA;
    delete shapeA;

    // Vérifie que le joint a bien été nettoyé automatiquement
    QVERIFY(joint->getPartA() == nullptr);
    QCOMPARE(joint->getPartB(), partB);
    QVERIFY(!joint->isValid()); // Le joint n'est plus valide car partA est nullptr

    // Détruit partB
    delete partB;
    delete shapeB;

    // Vérifie que partB a aussi été nettoyé
    QVERIFY(joint->getPartA() == nullptr);
    QVERIFY(joint->getPartB() == nullptr);

    delete joint;
}

void TestJoint::testSignalsDisconnect()
{
    // Teste que disconnect() déconnecte bien les signaux
    Rectangle* shapeA = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeB = new Rectangle(0, 0, 10, 10);
    Part* partA = new Part("PartA", shapeA, 3.0, Material::Wood());
    Part* partB = new Part("PartB", shapeB, 3.0, Material::Wood());

    TabJoint* joint = new TabJoint(partA, partB, Point3D(0, 0, 0), 90.0, 10.0, 3.0);

    // Vérifie la connexion initiale
    QCOMPARE(joint->getPartA(), partA);
    QCOMPARE(joint->getPartB(), partB);

    // Appelle disconnect()
    joint->disconnect();

    // Vérifie que les pointeurs sont à nullptr
    QVERIFY(joint->getPartA() == nullptr);
    QVERIFY(joint->getPartB() == nullptr);

    // Maintenant, détruire les parts ne devrait pas causer de problème
    // car les signaux ont été déconnectés
    delete partA;
    delete partB;
    delete shapeA;
    delete shapeB;

    // Le joint devrait toujours être OK (pointeurs déjà à nullptr)
    QVERIFY(joint->getPartA() == nullptr);
    QVERIFY(joint->getPartB() == nullptr);

    delete joint;
}

void TestJoint::testSignalsReconnect()
{
    // Teste la reconnexion à de nouveaux Parts
    Rectangle* shapeA = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeB = new Rectangle(0, 0, 10, 10);
    Rectangle* shapeC = new Rectangle(0, 0, 15, 15);
    Rectangle* shapeD = new Rectangle(0, 0, 20, 20);

    Part* partA = new Part("PartA", shapeA, 3.0, Material::Wood());
    Part* partB = new Part("PartB", shapeB, 3.0, Material::Wood());
    Part* partC = new Part("PartC", shapeC, 3.0, Material::Wood());
    Part* partD = new Part("PartD", shapeD, 3.0, Material::Wood());

    TabJoint* joint = new TabJoint(partA, partB, Point3D(0, 0, 0), 90.0, 10.0, 3.0);

    // Vérifie la connexion initiale
    QCOMPARE(joint->getPartA(), partA);
    QCOMPARE(joint->getPartB(), partB);

    // Reconnecte à de nouveaux parts
    joint->connect(partC, partD);

    // Vérifie la nouvelle connexion
    QCOMPARE(joint->getPartA(), partC);
    QCOMPARE(joint->getPartB(), partD);

    // Détruit les anciens parts - ne devrait pas affecter le joint
    // car les signaux ont été déconnectés lors du connect()
    delete partA;
    delete partB;
    delete shapeA;
    delete shapeB;

    // Le joint devrait toujours pointer vers C et D
    QCOMPARE(joint->getPartA(), partC);
    QCOMPARE(joint->getPartB(), partD);

    // Détruit les nouveaux parts - devrait déclencher le cleanup
    delete partC;
    delete shapeC;

    QVERIFY(joint->getPartA() == nullptr);
    QCOMPARE(joint->getPartB(), partD);

    delete partD;
    delete shapeD;
    delete joint;
}
