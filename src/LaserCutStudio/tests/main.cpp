#include <QTest>
#include <QCoreApplication>

// Nouveau système de géométrie contrainte uniquement
#include "TestGeometry.h"
#include "TestConstraints.h"
#include "TestConstraintSketch.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    int status = 0;

    // Tests du nouveau système de géométrie contrainte
    {
        LaserCutStudio::Core::Tests::TestGeometry testGeometry;
        status |= QTest::qExec(&testGeometry, argc, argv);
    }

    {
        LaserCutStudio::Core::Tests::TestConstraints testConstraints;
        status |= QTest::qExec(&testConstraints, argc, argv);
    }

    {
        LaserCutStudio::Core::Tests::TestConstraintSketch testConstraintSketch;
        status |= QTest::qExec(&testConstraintSketch, argc, argv);
    }

    return status;
}
