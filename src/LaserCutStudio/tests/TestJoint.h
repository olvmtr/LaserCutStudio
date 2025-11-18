#ifndef TESTJOINT_H
#define TESTJOINT_H

#include <QObject>
#include <QTest>
#include "../core/joints/TabJoint.h"
#include "../core/joints/FingerJoint.h"
#include "../core/parts/Part.h"

using namespace LaserCutStudio::Core;

class TestJoint : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testTabJointConstruction();
    void testTabJointGettersSetters();
    void testTabJointConnect();
    void testTabJointClone();

    void testFingerJointConstruction();
    void testFingerJointGettersSetters();

    void testJointStaticList();

    // Tests du Factory Pattern
    void testFactoryAvailableTypes();
    void testFactoryCreate();
    void testFactoryToVariant();
    void testFactoryRoundtrip();

    // Tests du système Signals/Slots
    void testSignalsAutoCleanup();
};

#endif // TESTJOINT_H
