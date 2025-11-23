#include "BenchmarkShapes.h"
#include "core/models/shapes/Rectangle.h"
#include "core/models/shapes/Circle.h"
#include "core/models/shapes/IShape.h"

using namespace LaserCutStudio::Core;

namespace LaserCutStudio {
namespace Benchmarks {

void BenchmarkShapes::benchmarkRectangleCreation()
{
    QBENCHMARK {
        Rectangle rect(0, 0, 100, 50);
    }
}

void BenchmarkShapes::benchmarkCircleCreation()
{
    QBENCHMARK {
        Circle circle(0, 0, 50);
    }
}

void BenchmarkShapes::benchmarkRectangleClone()
{
    Rectangle rect(0, 0, 100, 50);

    QBENCHMARK {
        Interface* clone = rect.clone();
        delete clone;
    }
}

void BenchmarkShapes::benchmarkCircleClone()
{
    Circle circle(0, 0, 50);

    QBENCHMARK {
        Interface* clone = circle.clone();
        delete clone;
    }
}

void BenchmarkShapes::benchmarkRectangleArea()
{
    Rectangle rect(0, 0, 100, 50);

    QBENCHMARK {
        double area = rect.getArea();
        Q_UNUSED(area);
    }
}

void BenchmarkShapes::benchmarkCircleArea()
{
    Circle circle(0, 0, 50);

    QBENCHMARK {
        double area = circle.getArea();
        Q_UNUSED(area);
    }
}

void BenchmarkShapes::benchmarkRectangleBoundingBox()
{
    Rectangle rect(0, 0, 100, 50);

    QBENCHMARK {
        QRectF bbox = rect.getBoundingBox();
        Q_UNUSED(bbox);
    }
}

void BenchmarkShapes::benchmarkCircleBoundingBox()
{
    Circle circle(0, 0, 50);

    QBENCHMARK {
        QRectF bbox = circle.getBoundingBox();
        Q_UNUSED(bbox);
    }
}

void BenchmarkShapes::benchmarkRectangleContainsPoint()
{
    Rectangle rect(0, 0, 100, 50);
    Point2D testPoint(25, 25);

    QBENCHMARK {
        bool contains = rect.containsPoint(testPoint);
        Q_UNUSED(contains);
    }
}

void BenchmarkShapes::benchmarkCircleContainsPoint()
{
    Circle circle(0, 0, 50);
    Point2D testPoint(25, 25);

    QBENCHMARK {
        bool contains = circle.containsPoint(testPoint);
        Q_UNUSED(contains);
    }
}

void BenchmarkShapes::benchmarkRectangleTranslate()
{
    Rectangle rect(0, 0, 100, 50);

    QBENCHMARK {
        rect.translate(10, 10);
    }
}

void BenchmarkShapes::benchmarkRectangleRotate()
{
    Rectangle rect(0, 0, 100, 50);
    Point2D center(50, 25);

    QBENCHMARK {
        rect.rotate(45, center);
    }
}

void BenchmarkShapes::benchmarkRectangleScale()
{
    Rectangle rect(0, 0, 100, 50);
    Point2D center(50, 25);

    QBENCHMARK {
        rect.scale(1.5, 1.5, center);
    }
}

void BenchmarkShapes::benchmarkRectangleToVariantMap()
{
    Rectangle rect(0, 0, 100, 50);

    QBENCHMARK {
        QVariantMap map = rect.toVariant();
        Q_UNUSED(map);
    }
}

void BenchmarkShapes::benchmarkCircleToVariantMap()
{
    Circle circle(0, 0, 50);

    QBENCHMARK {
        QVariantMap map = circle.toVariant();
        Q_UNUSED(map);
    }
}

void BenchmarkShapes::benchmarkFactoryCreate()
{
    QVariantMap rectData;
    rectData["type"] = "Rectangle";
    rectData["x"] = 0.0;
    rectData["y"] = 0.0;
    rectData["width"] = 100.0;
    rectData["height"] = 50.0;

    QBENCHMARK {
        IShape* shape = IShape::create(rectData);
        delete shape;
    }
}

void BenchmarkShapes::benchmarkFactoryRoundtrip()
{
    Rectangle rect(0, 0, 100, 50);

    QBENCHMARK {
        QVariantMap map = rect.toVariant();
        IShape* shape = IShape::create(map);
        delete shape;
    }
}

} // namespace Benchmarks
} // namespace LaserCutStudio
