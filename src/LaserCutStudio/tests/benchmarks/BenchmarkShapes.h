#ifndef BENCHMARKSHAPES_H
#define BENCHMARKSHAPES_H

#include <QObject>
#include <QtTest/QtTest>

namespace LaserCutStudio {
namespace Benchmarks {

/**
 * @brief Tests de performance pour les formes géométriques
 */
class BenchmarkShapes : public QObject
{
    Q_OBJECT

private slots:
    // Benchmarks création
    void benchmarkRectangleCreation();
    void benchmarkCircleCreation();

    // Benchmarks clonage
    void benchmarkRectangleClone();
    void benchmarkCircleClone();

    // Benchmarks calculs géométriques
    void benchmarkRectangleArea();
    void benchmarkCircleArea();
    void benchmarkRectangleBoundingBox();
    void benchmarkCircleBoundingBox();
    void benchmarkRectangleContainsPoint();
    void benchmarkCircleContainsPoint();

    // Benchmarks transformations
    void benchmarkRectangleTranslate();
    void benchmarkRectangleRotate();
    void benchmarkRectangleScale();

    // Benchmarks sérialisation
    void benchmarkRectangleToVariantMap();
    void benchmarkCircleToVariantMap();

    // Benchmarks Factory Pattern
    void benchmarkFactoryCreate();
    void benchmarkFactoryRoundtrip();
};

} // namespace Benchmarks
} // namespace LaserCutStudio

#endif // BENCHMARKSHAPES_H
