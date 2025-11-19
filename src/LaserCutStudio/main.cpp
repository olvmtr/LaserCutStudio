#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "core/ui/canvas/Canvas2DView.h"
#include "core/services/editor/EditorService.h"
#include "core/models/editor/implementations/ShapeCreationTool.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Enregistrer les types Qt pour QML
    qmlRegisterType<LaserCutStudio::Core::UI::Canvas2DView>(
        "LaserCutStudio", 1, 0, "Canvas2DView");

    qmlRegisterUncreatableType<LaserCutStudio::Core::Services::EditorService>(
        "LaserCutStudio", 1, 0, "EditorService",
        "EditorService cannot be created from QML");

    // Créer EditorService et outils
    auto* editorService = new LaserCutStudio::Core::Services::EditorService(&app);

    // Créer un outil Rectangle par défaut
    auto* rectangleTool = new LaserCutStudio::Core::Editor::ShapeCreationTool("Rectangle", editorService);
    rectangleTool->setCreationMode(LaserCutStudio::Core::Editor::ShapeCreationTool::CreationMode::ClickAndDrag);
    editorService->registerTool(rectangleTool);
    editorService->setActiveTool(rectangleTool);

    QQmlApplicationEngine engine;

    // Exposer EditorService au contexte QML
    engine.rootContext()->setContextProperty("editorService", editorService);

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
