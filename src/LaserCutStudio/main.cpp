#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "core/ui/canvas/Canvas2DView.h"
#include "core/ui/viewmodels/SelectionPropertiesViewModel.h"
#include "core/services/editor/EditorService.h"
#include "core/models/editor/implementations/ShapeCreationTool.h"
#include "core/models/editor/implementations/SelectionTool.h"
#include "core/models/shapes/interfaces/IShape.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Enregistrer les types Qt pour QML
    qmlRegisterType<LaserCutStudio::Core::UI::Canvas2DView>(
        "LaserCutStudio", 1, 0, "Canvas2DView");

    qmlRegisterType<LaserCutStudio::Core::UI::SelectionPropertiesViewModel>(
        "LaserCutStudio", 1, 0, "SelectionPropertiesViewModel");

    qmlRegisterUncreatableType<LaserCutStudio::Core::Services::EditorService>(
        "LaserCutStudio", 1, 0, "EditorService",
        "EditorService cannot be created from QML");

    // Créer EditorService et outils
    auto* editorService = new LaserCutStudio::Core::Services::EditorService(&app);

    // Créer automatiquement un outil pour chaque type de forme enregistré dans le Factory
    QVector<QString> availableShapeTypes = LaserCutStudio::Core::IShape::availableTypes();
    LaserCutStudio::Core::Editor::ITool* firstTool = nullptr;

    for (const QString& shapeType : availableShapeTypes) {
        auto* tool = new LaserCutStudio::Core::Editor::ShapeCreationTool(shapeType, editorService);
        tool->setCreationMode(LaserCutStudio::Core::Editor::ShapeCreationTool::CreationMode::ClickAndDrag);
        editorService->registerTool(tool);

        // Garder le premier outil pour l'activer
        if (!firstTool) {
            firstTool = tool;
        }
    }

    // Créer et enregistrer SelectionTool
    auto* selectionTool = new LaserCutStudio::Core::Editor::SelectionTool(editorService);
    editorService->registerTool(selectionTool);

    // Activer SelectionTool par défaut
    editorService->setActiveTool(selectionTool);

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
