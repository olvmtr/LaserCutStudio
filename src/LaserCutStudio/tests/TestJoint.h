#ifndef TESTJOINT_H
#define TESTJOINT_H

#include <QObject>
#include <QTest>
#include "core/models/joints/implementations/TabJoint.h"
#include "core/models/joints/implementations/FingerJoint.h"
#include "core/models/parts/implementations/Part.h"

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

    // Tests du système Signals/Slots (logique métier uniquement)
    void testSignalsAutoCleanup();
    void testSignalsDisconnect();
    void testSignalsReconnect();
};

#endif // TESTJOINT_H
