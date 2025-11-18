#ifndef TESTSERVICELOCATOR_H
#define TESTSERVICELOCATOR_H

#include <QObject>
#include <QtTest>
#include "../core/di/ServiceLocator.h"

using namespace LaserCutStudio::Core::DI;

// Classes de test

/**
 * @brief Interface de test
 */
class ITestService
{
public:
    virtual ~ITestService() = default;
    virtual QString getName() const = 0;
    virtual int getCallCount() const = 0;
};

/**
 * @brief Service de test concret
 */
class TestService : public QObject, public ITestService
{
    Q_OBJECT

public:
    TestService(const QString& name = "TestService")
        : m_name(name), m_callCount(0) {}

    QString getName() const override {
        const_cast<TestService*>(this)->m_callCount++;
        return m_name;
    }

    int getCallCount() const override { return m_callCount; }

private:
    QString m_name;
    int m_callCount;
};

/**
 * @brief Autre service de test
 */
class AnotherService : public QObject
{
    Q_OBJECT

public:
    AnotherService(int value = 42) : m_value(value) {}
    int getValue() const { return m_value; }
    void setValue(int value) { m_value = value; }

private:
    int m_value;
};

/**
 * @class TestServiceLocator
 * @brief Tests unitaires pour le ServiceLocator
 */
class TestServiceLocator : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    // Tests de base
    void testSingleton();
    void testClear();

    // Tests d'enregistrement
    void testRegisterSingleton();
    void testRegisterTransient();
    void testRegisterInstance();
    void testRegisterDuplicate();

    // Tests de résolution
    void testResolveSingleton();
    void testResolveTransient();
    void testResolveInstance();
    void testResolveNotRegistered();
    void testResolveSingletonSameInstance();
    void testResolveTransientDifferentInstances();

    // Tests d'état
    void testIsRegistered();
    void testServiceCount();
    void testInstanceCount();

    // Tests de nettoyage
    void testUnregister();
    void testClearDestroysInstances();

    // Tests avec noms d'interface
    void testRegisterWithInterfaceName();
    void testResolveWithInterfaceName();

    // Tests de cast
    void testResolveCastFailure();
};

#endif // TESTSERVICELOCATOR_H
