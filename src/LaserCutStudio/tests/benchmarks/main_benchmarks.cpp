#include <QCoreApplication>
#include <QtTest/QtTest>
#include "BenchmarkShapes.h"

/**
 * @brief Point d'entrée pour les benchmarks
 *
 * Usage:
 * ./LaserCutStudioBenchmarks                    # Tous les benchmarks
 * ./LaserCutStudioBenchmarks -iterations 10000   # Plus d'itérations
 * ./LaserCutStudioBenchmarks -csv                # Sortie CSV
 */
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    int status = 0;

    qInfo() << "=== LaserCutStudio Performance Benchmarks ===";
    qInfo() << "";

    // Benchmarks Shapes
    {
        qInfo() << "Running BenchmarkShapes...";
        LaserCutStudio::Benchmarks::BenchmarkShapes benchShapes;
        status |= QTest::qExec(&benchShapes, argc, argv);
        qInfo() << "";
    }

    qInfo() << "=== Benchmarks completed ===";

    return status;
}
