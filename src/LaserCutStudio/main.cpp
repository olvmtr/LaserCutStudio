#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "core/ui/canvas/ConstraintCanvas2DView.h"
#include "core/models/sketch/ConstraintSketch.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Enregistrer les types Qt pour QML
    qmlRegisterType<LaserCutStudio::Core::UI::ConstraintCanvas2DView>(
        "LaserCutStudio", 1, 0, "ConstraintCanvas2DView");

    qmlRegisterType<LaserCutStudio::Core::ConstraintSketch>(
        "LaserCutStudio", 1, 0, "ConstraintSketch");

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/LaserCutStudio/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
