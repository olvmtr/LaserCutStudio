#include "TestJoint.h"
#include "../core/shapes/Rectangle.h"

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
