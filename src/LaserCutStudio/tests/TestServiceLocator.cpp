#include "TestServiceLocator.h"

void TestServiceLocator::initTestCase()
{
    // Initialisation une fois avant tous les tests
}

void TestServiceLocator::cleanupTestCase()
{
    // Nettoyage final
    ServiceLocator::instance().clear();
}

void TestServiceLocator::cleanup()
{
    // Nettoyer après chaque test
    ServiceLocator::instance().clear();
}

void TestServiceLocator::testSingleton()
{
    ServiceLocator& locator1 = ServiceLocator::instance();
    ServiceLocator& locator2 = ServiceLocator::instance();

    QCOMPARE(&locator1, &locator2);
}

void TestServiceLocator::testClear()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<TestService>([]() { return new TestService(); });
    locator.registerTransient<AnotherService>([]() { return new AnotherService(); });

    QCOMPARE(locator.serviceCount(), 2);

    locator.clear();

    QCOMPARE(locator.serviceCount(), 0);
    QCOMPARE(locator.instanceCount(), 0);
}

void TestServiceLocator::testRegisterSingleton()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<TestService>([]() {
        return new TestService("MySingleton");
    });

    QVERIFY(locator.isRegistered<TestService>());
    QCOMPARE(locator.serviceCount(), 1);
}

void TestServiceLocator::testRegisterTransient()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerTransient<TestService>([]() {
        return new TestService("MyTransient");
    });

    QVERIFY(locator.isRegistered<TestService>());
    QCOMPARE(locator.serviceCount(), 1);
}

void TestServiceLocator::testRegisterInstance()
{
    ServiceLocator& locator = ServiceLocator::instance();

    TestService* service = new TestService("MyInstance");
    locator.registerInstance<TestService>(service);

    QVERIFY(locator.isRegistered<TestService>());
    QCOMPARE(locator.instanceCount(), 1);
}

void TestServiceLocator::testRegisterDuplicate()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<TestService>([]() { return new TestService("First"); });
    locator.registerSingleton<TestService>([]() { return new TestService("Second"); });

    // Le deuxième enregistrement devrait être ignoré
    QCOMPARE(locator.serviceCount(), 1);

    // Vérifier que c'est bien le premier qui est utilisé
    TestService* service = locator.resolve<TestService>();
    QVERIFY(service != nullptr);
    QCOMPARE(service->getName(), QString("First"));
}

void TestServiceLocator::testResolveSingleton()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<TestService>([]() {
        return new TestService("MySingleton");
    });

    TestService* service = locator.resolve<TestService>();

    QVERIFY(service != nullptr);
    QCOMPARE(service->getName(), QString("MySingleton"));
    QCOMPARE(locator.instanceCount(), 1);
}

void TestServiceLocator::testResolveTransient()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerTransient<TestService>([]() {
        return new TestService("MyTransient");
    });

    TestService* service = locator.resolve<TestService>();

    QVERIFY(service != nullptr);
    QCOMPARE(service->getName(), QString("MyTransient"));

    // Transient ne stocke pas l'instance
    QCOMPARE(locator.instanceCount(), 0);

    // Nettoyer l'instance transient
    delete service;
}

void TestServiceLocator::testResolveInstance()
{
    ServiceLocator& locator = ServiceLocator::instance();

    TestService* original = new TestService("MyInstance");
    locator.registerInstance<TestService>(original);

    TestService* resolved = locator.resolve<TestService>();

    QVERIFY(resolved != nullptr);
    QCOMPARE(resolved, original);
    QCOMPARE(resolved->getName(), QString("MyInstance"));
}

void TestServiceLocator::testResolveNotRegistered()
{
    ServiceLocator& locator = ServiceLocator::instance();

    TestService* service = locator.resolve<TestService>();

    QVERIFY(service == nullptr);
}

void TestServiceLocator::testResolveSingletonSameInstance()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<TestService>([]() {
        return new TestService("Singleton");
    });

    TestService* first = locator.resolve<TestService>();
    TestService* second = locator.resolve<TestService>();

    QVERIFY(first != nullptr);
    QVERIFY(second != nullptr);
    QCOMPARE(first, second);  // Même instance

    // Vérifier le compteur d'appels
    QCOMPARE(first->getCallCount(), 0);
    first->getName();
    QCOMPARE(first->getCallCount(), 1);
    second->getName();
    QCOMPARE(second->getCallCount(), 2);  // Même objet
}

void TestServiceLocator::testResolveTransientDifferentInstances()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerTransient<TestService>([]() {
        return new TestService("Transient");
    });

    TestService* first = locator.resolve<TestService>();
    TestService* second = locator.resolve<TestService>();

    QVERIFY(first != nullptr);
    QVERIFY(second != nullptr);
    QVERIFY(first != second);  // Instances différentes

    // Vérifier les compteurs indépendants
    first->getName();
    QCOMPARE(first->getCallCount(), 1);
    QCOMPARE(second->getCallCount(), 0);

    // Nettoyer
    delete first;
    delete second;
}

void TestServiceLocator::testIsRegistered()
{
    ServiceLocator& locator = ServiceLocator::instance();

    QVERIFY(!locator.isRegistered<TestService>());

    locator.registerSingleton<TestService>([]() { return new TestService(); });

    QVERIFY(locator.isRegistered<TestService>());
}

void TestServiceLocator::testServiceCount()
{
    ServiceLocator& locator = ServiceLocator::instance();

    QCOMPARE(locator.serviceCount(), 0);

    locator.registerSingleton<TestService>([]() { return new TestService(); });
    QCOMPARE(locator.serviceCount(), 1);

    locator.registerTransient<AnotherService>([]() { return new AnotherService(); });
    QCOMPARE(locator.serviceCount(), 2);
}

void TestServiceLocator::testInstanceCount()
{
    ServiceLocator& locator = ServiceLocator::instance();

    QCOMPARE(locator.instanceCount(), 0);

    // Enregistrer sans résoudre
    locator.registerSingleton<TestService>([]() { return new TestService(); });
    QCOMPARE(locator.instanceCount(), 0);

    // Résoudre crée l'instance
    TestService* service = locator.resolve<TestService>();
    QVERIFY(service != nullptr);
    QCOMPARE(locator.instanceCount(), 1);
}

void TestServiceLocator::testUnregister()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<TestService>([]() { return new TestService(); });
    TestService* service = locator.resolve<TestService>();

    QVERIFY(service != nullptr);
    QVERIFY(locator.isRegistered<TestService>());
    QCOMPARE(locator.instanceCount(), 1);

    locator.unregister<TestService>();

    QVERIFY(!locator.isRegistered<TestService>());
    QCOMPARE(locator.instanceCount(), 0);
}

void TestServiceLocator::testClearDestroysInstances()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<TestService>([]() { return new TestService(); });
    locator.registerSingleton<AnotherService>([]() { return new AnotherService(); });

    // Résoudre pour créer les instances
    TestService* service1 = locator.resolve<TestService>();
    AnotherService* service2 = locator.resolve<AnotherService>();

    QVERIFY(service1 != nullptr);
    QVERIFY(service2 != nullptr);
    QCOMPARE(locator.instanceCount(), 2);

    // Clear devrait détruire les instances
    locator.clear();

    QCOMPARE(locator.instanceCount(), 0);
    QCOMPARE(locator.serviceCount(), 0);
}

void TestServiceLocator::testRegisterWithInterfaceName()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<ITestService>(
        []() { return new TestService("WithName"); },
        "ITestService"
    );

    QVERIFY(locator.isRegistered<ITestService>("ITestService"));
    QCOMPARE(locator.serviceCount(), 1);
}

void TestServiceLocator::testResolveWithInterfaceName()
{
    ServiceLocator& locator = ServiceLocator::instance();

    locator.registerSingleton<ITestService>(
        []() { return new TestService("NamedService"); },
        "ITestService"
    );

    ITestService* service = locator.resolve<ITestService>("ITestService");

    QVERIFY(service != nullptr);
    QCOMPARE(service->getName(), QString("NamedService"));
}

void TestServiceLocator::testResolveCastFailure()
{
    ServiceLocator& locator = ServiceLocator::instance();

    // Enregistrer avec un type, essayer de résoudre avec un autre
    locator.registerSingleton<TestService>([]() { return new TestService(); });

    // Ceci devrait échouer car AnotherService n'est pas compatible avec TestService
    AnotherService* service = locator.resolve<AnotherService>();

    QVERIFY(service == nullptr);
}
