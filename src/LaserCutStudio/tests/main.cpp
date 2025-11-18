#include <QTest>
#include <QCoreApplication>

#include "TestTypes.h"
#include "TestShapes.h"
#include "TestPart.h"
#include "TestJoint.h"
#include "TestProject.h"
#include "TestConfigManager.h"
#include "TestLogging.h"
#include "TestPluginManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    int status = 0;

    // Exécute chaque suite de tests
    {
        TestTypes testTypes;
        status |= QTest::qExec(&testTypes, argc, argv);
    }

    {
        TestShapes testShapes;
        status |= QTest::qExec(&testShapes, argc, argv);
    }

    {
        TestPart testPart;
        status |= QTest::qExec(&testPart, argc, argv);
    }

    {
        TestJoint testJoint;
        status |= QTest::qExec(&testJoint, argc, argv);
    }

    {
        TestProject testProject;
        status |= QTest::qExec(&testProject, argc, argv);
    }

    {
        LaserCutStudio::Tests::TestConfigManager testConfigManager;
        status |= QTest::qExec(&testConfigManager, argc, argv);
    }

    {
        LaserCutStudio::Tests::TestLogging testLogging;
        status |= QTest::qExec(&testLogging, argc, argv);
    }

    {
        TestPluginManager testPluginManager;
        status |= QTest::qExec(&testPluginManager, argc, argv);
    }

    return status;
}
