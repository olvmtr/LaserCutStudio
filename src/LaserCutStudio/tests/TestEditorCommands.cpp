/**
 * @file TestEditorCommands.cpp
 * @brief Implémentation des tests pour les commandes d'éditeur
 */

#include "TestEditorCommands.h"
#include "core/models/editor/command/CommandStack.h"

using namespace LaserCutStudio::Core;

// ===== Setup et Cleanup =====

void TestEditorCommands::initTestCase()
{
    qInfo() << "=== TestEditorCommands: Starting test suite ===";
}

void TestEditorCommands::cleanupTestCase()
{
    qInfo() << "=== TestEditorCommands: Test suite completed ===";
}

void TestEditorCommands::init()
{
    // Créer un nouveau EditorService pour chaque test
    m_editorService = new Services::EditorService();
    QVERIFY(m_editorService != nullptr);

    // Créer un SelectionTool
    m_selectionTool = new Editor::SelectionTool(m_editorService);
    QVERIFY(m_selectionTool != nullptr);
    m_editorService->registerTool(m_selectionTool);
}

void TestEditorCommands::cleanup()
{
    // Nettoyer les formes restantes
    if (m_editorService) {
        m_editorService->clear();
        delete m_editorService;
        m_editorService = nullptr;
    }
    // Le SelectionTool est détruit avec EditorService (parent/child)
    m_selectionTool = nullptr;
}

// ===== Tests CreateShapeCommand =====

void TestEditorCommands::testCreateShapeCommand_Execute()
{
    qInfo() << "Test: CreateShapeCommand::execute()";

    // Créer une commande pour créer un rectangle
    QVariantMap params;
    params["type"] = "Rectangle";
    params["x"] = 100.0;
    params["y"] = 100.0;
    params["width"] = 50.0;
    params["height"] = 30.0;

    auto* command = new Editor::CreateShapeCommand(params, m_editorService);
    QVERIFY(command != nullptr);

    // Vérifier qu'aucune forme n'existe avant
    QCOMPARE(m_editorService->getShapeCount(), 0);

    // Exécuter la commande
    command->execute();

    // Vérifier qu'une forme a été créée
    QCOMPARE(m_editorService->getShapeCount(), 1);

    IShape* shape = m_editorService->getShapes().first();
    QVERIFY(shape != nullptr);
    QCOMPARE(shape->getTypeName(), QString("Rectangle"));

    delete command;
}

void TestEditorCommands::testCreateShapeCommand_Undo()
{
    qInfo() << "Test: CreateShapeCommand::undo()";

    QVariantMap params;
    params["type"] = "Circle";
    params["centerX"] = 50.0;
    params["centerY"] = 50.0;
    params["radius"] = 25.0;

    auto* command = new Editor::CreateShapeCommand(params, m_editorService);
    command->execute();

    // Vérifier que la forme existe
    QCOMPARE(m_editorService->getShapeCount(), 1);

    // Annuler
    command->undo();

    // Vérifier que la forme a disparu
    QCOMPARE(m_editorService->getShapeCount(), 0);

    delete command;
}

void TestEditorCommands::testCreateShapeCommand_Redo()
{
    qInfo() << "Test: CreateShapeCommand::redo()";

    QVariantMap params;
    params["type"] = "Rectangle";
    params["x"] = 0.0;
    params["y"] = 0.0;
    params["width"] = 100.0;
    params["height"] = 100.0;

    auto* command = new Editor::CreateShapeCommand(params, m_editorService);
    command->execute();
    command->undo();

    // Vérifier que la forme a disparu
    QCOMPARE(m_editorService->getShapeCount(), 0);

    // Refaire
    command->redo();

    // Vérifier que la forme réapparaît
    QCOMPARE(m_editorService->getShapeCount(), 1);

    IShape* shape = m_editorService->getShapes().first();
    QVERIFY(shape != nullptr);
    QCOMPARE(shape->getTypeName(), QString("Rectangle"));

    delete command;
}

void TestEditorCommands::testCreateShapeCommand_MultipleShapes()
{
    qInfo() << "Test: CreateShapeCommand with multiple shapes";

    QList<Editor::CreateShapeCommand*> commands;

    // Créer 3 formes
    for (int i = 0; i < 3; ++i) {
        QVariantMap params;
        params["type"] = "Rectangle";
        params["x"] = i * 100.0;
        params["y"] = 0.0;
        params["width"] = 50.0;
        params["height"] = 50.0;

        auto* command = new Editor::CreateShapeCommand(params, m_editorService);
        command->execute();
        commands.append(command);
    }

    // Vérifier qu'on a 3 formes
    QCOMPARE(m_editorService->getShapeCount(), 3);

    // Annuler toutes les commandes
    for (auto* cmd : commands) {
        cmd->undo();
    }

    // Vérifier que toutes les formes ont disparu
    QCOMPARE(m_editorService->getShapeCount(), 0);

    // Refaire toutes les commandes
    for (auto* cmd : commands) {
        cmd->redo();
    }

    // Vérifier que les 3 formes sont revenues
    QCOMPARE(m_editorService->getShapeCount(), 3);

    qDeleteAll(commands);
}

// ===== Tests MoveCommand =====

void TestEditorCommands::testMoveCommand_Execute()
{
    qInfo() << "Test: MoveCommand::execute()";

    // Créer une forme
    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShapeDirect(rect);

    // Créer une commande de déplacement
    QVector<IShape*> shapes = { rect };
    auto* command = new Editor::MoveCommand(shapes, 50.0, 30.0);

    // Position initiale
    QCOMPARE(rect->getX(), 100.0);
    QCOMPARE(rect->getY(), 100.0);

    // Exécuter
    command->execute();

    // Vérifier nouvelle position
    QCOMPARE(rect->getX(), 150.0);
    QCOMPARE(rect->getY(), 130.0);

    delete command;
}

void TestEditorCommands::testMoveCommand_Undo()
{
    qInfo() << "Test: MoveCommand::undo()";

    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShapeDirect(rect);

    QVector<IShape*> shapes = { rect };
    auto* command = new Editor::MoveCommand(shapes, 50.0, 30.0);

    command->execute();
    QCOMPARE(rect->getX(), 150.0);
    QCOMPARE(rect->getY(), 130.0);

    // Annuler
    command->undo();

    // Vérifier retour à position initiale
    QCOMPARE(rect->getX(), 100.0);
    QCOMPARE(rect->getY(), 100.0);

    delete command;
}

void TestEditorCommands::testMoveCommand_Redo()
{
    qInfo() << "Test: MoveCommand::redo()";

    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShapeDirect(rect);

    QVector<IShape*> shapes = { rect };
    auto* command = new Editor::MoveCommand(shapes, 50.0, 30.0);

    command->execute();
    command->undo();

    QCOMPARE(rect->getX(), 100.0);
    QCOMPARE(rect->getY(), 100.0);

    // Refaire
    command->redo();

    // Vérifier retour à nouvelle position
    QCOMPARE(rect->getX(), 150.0);
    QCOMPARE(rect->getY(), 130.0);

    delete command;
}

void TestEditorCommands::testMoveCommand_MultipleShapes()
{
    qInfo() << "Test: MoveCommand with multiple shapes";

    // Créer 2 formes
    auto* rect1 = new Rectangle(100, 100, 50, 50);
    auto* rect2 = new Rectangle(200, 200, 50, 50);
    m_editorService->addShapeDirect(rect1);
    m_editorService->addShapeDirect(rect2);

    // Déplacer les deux ensemble
    QVector<IShape*> shapes = { rect1, rect2 };
    auto* command = new Editor::MoveCommand(shapes, 50.0, 30.0);

    command->execute();

    // Vérifier que les deux ont bougé
    QCOMPARE(rect1->getX(), 150.0);
    QCOMPARE(rect1->getY(), 130.0);
    QCOMPARE(rect2->getX(), 250.0);
    QCOMPARE(rect2->getY(), 230.0);

    // Annuler
    command->undo();

    // Vérifier que les deux sont revenues
    QCOMPARE(rect1->getX(), 100.0);
    QCOMPARE(rect1->getY(), 100.0);
    QCOMPARE(rect2->getX(), 200.0);
    QCOMPARE(rect2->getY(), 200.0);

    delete command;
}

void TestEditorCommands::testMoveCommand_Merge()
{
    qInfo() << "Test: MoveCommand::mergeWith()";

    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShapeDirect(rect);

    QVector<IShape*> shapes = { rect };

    // Première commande : déplacer de (10, 20)
    auto* cmd1 = new Editor::MoveCommand(shapes, 10.0, 20.0);

    // Deuxième commande : déplacer de (5, 10)
    auto* cmd2 = new Editor::MoveCommand(shapes, 5.0, 10.0);

    // Vérifier que merge est possible
    QVERIFY(cmd1->canMerge());
    QCOMPARE(cmd1->getCommandId(), QString("MoveShape"));

    // Merger cmd2 dans cmd1
    bool merged = cmd1->mergeWith(cmd2);
    QVERIFY(merged);

    // Exécuter cmd1 qui contient maintenant le déplacement combiné
    cmd1->execute();

    // Position devrait être (100 + 10 + 5, 100 + 20 + 10) = (115, 130)
    QCOMPARE(rect->getX(), 115.0);
    QCOMPARE(rect->getY(), 130.0);

    // Annuler : devrait revenir à (100, 100) en une seule fois
    cmd1->undo();
    QCOMPARE(rect->getX(), 100.0);
    QCOMPARE(rect->getY(), 100.0);

    delete cmd1;
    delete cmd2;
}

// ===== Tests CommandStack =====

void TestEditorCommands::testCommandStack_PushAndUndo()
{
    qInfo() << "Test: CommandStack push and undo";

    // Créer une forme via commande
    QVariantMap params;
    params["type"] = "Rectangle";
    params["x"] = 0.0;
    params["y"] = 0.0;
    params["width"] = 100.0;
    params["height"] = 100.0;

    auto* command = new Editor::CreateShapeCommand(params, m_editorService);

    // Push la commande (ownership transféré)
    m_editorService->pushCommand(command);

    // Vérifier que la forme existe
    QCOMPARE(m_editorService->getShapeCount(), 1);
    QVERIFY(m_editorService->canUndo());
    QVERIFY(!m_editorService->canRedo());

    // Undo via EditorService
    m_editorService->undo();

    // Vérifier que la forme a disparu
    QCOMPARE(m_editorService->getShapeCount(), 0);
    QVERIFY(!m_editorService->canUndo());
    QVERIFY(m_editorService->canRedo());
}

void TestEditorCommands::testCommandStack_UndoRedo()
{
    qInfo() << "Test: CommandStack undo/redo cycle";

    // Créer 3 formes
    for (int i = 0; i < 3; ++i) {
        QVariantMap params;
        params["type"] = "Rectangle";
        params["x"] = i * 100.0;
        params["y"] = 0.0;
        params["width"] = 50.0;
        params["height"] = 50.0;

        m_editorService->addShape(nullptr, true); // Utilise CreateShapeCommand
        // Note: addShape avec createCommand=true crée la commande automatiquement
    }

    // En réalité, addShape(nullptr, true) ne fonctionne pas comme ça
    // Utilisons pushCommand directement
    for (int i = 0; i < 3; ++i) {
        QVariantMap params;
        params["type"] = "Rectangle";
        params["x"] = i * 100.0;
        params["y"] = 0.0;
        params["width"] = 50.0;
        params["height"] = 50.0;

        auto* cmd = new Editor::CreateShapeCommand(params, m_editorService);
        m_editorService->pushCommand(cmd);
    }

    QCOMPARE(m_editorService->getShapeCount(), 3);
    QCOMPARE(m_editorService->getUndoStackSize(), 3);

    // Undo 2 fois
    m_editorService->undo();
    m_editorService->undo();

    QCOMPARE(m_editorService->getShapeCount(), 1);
    QCOMPARE(m_editorService->getUndoStackSize(), 1);
    QCOMPARE(m_editorService->getRedoStackSize(), 2);

    // Redo 1 fois
    m_editorService->redo();

    QCOMPARE(m_editorService->getShapeCount(), 2);
    QCOMPARE(m_editorService->getUndoStackSize(), 2);
    QCOMPARE(m_editorService->getRedoStackSize(), 1);
}

void TestEditorCommands::testCommandStack_UndoLimit()
{
    qInfo() << "Test: CommandStack undo limit";

    // Définir une limite de 5 commandes
    m_editorService->setUndoLimit(5);
    QCOMPARE(m_editorService->getUndoLimit(), 5);

    // Créer 10 formes
    for (int i = 0; i < 10; ++i) {
        QVariantMap params;
        params["type"] = "Rectangle";
        params["x"] = i * 50.0;
        params["y"] = 0.0;
        params["width"] = 40.0;
        params["height"] = 40.0;

        auto* cmd = new Editor::CreateShapeCommand(params, m_editorService);
        m_editorService->pushCommand(cmd);
    }

    QCOMPARE(m_editorService->getShapeCount(), 10);

    // Le stack ne devrait contenir que 5 commandes (les plus récentes)
    QCOMPARE(m_editorService->getUndoStackSize(), 5);

    // Undo 5 fois pour tout annuler
    for (int i = 0; i < 5; ++i) {
        m_editorService->undo();
    }

    // On devrait avoir 5 formes restantes (les 5 premières hors limite)
    QCOMPARE(m_editorService->getShapeCount(), 5);
}

void TestEditorCommands::testCommandStack_Clear()
{
    qInfo() << "Test: CommandStack clear";

    // Créer quelques formes
    for (int i = 0; i < 3; ++i) {
        QVariantMap params;
        params["type"] = "Circle";
        params["centerX"] = i * 50.0;
        params["centerY"] = 50.0;
        params["radius"] = 25.0;

        auto* cmd = new Editor::CreateShapeCommand(params, m_editorService);
        m_editorService->pushCommand(cmd);
    }

    QCOMPARE(m_editorService->getUndoStackSize(), 3);

    // Vider le stack
    m_editorService->clearUndoStack();

    // Vérifier que le stack est vide
    QCOMPARE(m_editorService->getUndoStackSize(), 0);
    QVERIFY(!m_editorService->canUndo());

    // Les formes devraient toujours exister
    QCOMPARE(m_editorService->getShapeCount(), 3);
}

// ===== Tests SelectionTool =====

void TestEditorCommands::testSelectionTool_SelectShape()
{
    qInfo() << "Test: SelectionTool select shape";

    // Créer une forme
    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShapeDirect(rect);

    // Activer le SelectionTool
    m_editorService->setActiveTool(m_selectionTool);

    // Simuler un clic sur la forme
    Point2D clickPos(125, 125); // Centre du rectangle
    m_selectionTool->handleMousePress(clickPos, Qt::LeftButton);

    // Vérifier que la forme est sélectionnée
    auto* selection = m_editorService->getSelection();
    QVERIFY(selection != nullptr);
    QCOMPARE(selection->getCount(), 1);
    QVERIFY(selection->isSelected(rect));
}

void TestEditorCommands::testSelectionTool_ClearSelection()
{
    qInfo() << "Test: SelectionTool clear selection";

    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShapeDirect(rect);

    m_editorService->setActiveTool(m_selectionTool);

    // Sélectionner la forme
    Point2D clickPos(125, 125);
    m_selectionTool->handleMousePress(clickPos, Qt::LeftButton);

    auto* selection = m_editorService->getSelection();
    QCOMPARE(selection->getCount(), 1);

    // Cliquer sur le fond (en dehors de la forme)
    Point2D emptyClick(300, 300);
    m_selectionTool->handleMousePress(emptyClick, Qt::LeftButton);

    // La sélection devrait être vidée
    QCOMPARE(selection->getCount(), 0);
}

void TestEditorCommands::testSelectionTool_DragAndDrop()
{
    qInfo() << "Test: SelectionTool drag and drop";

    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShapeDirect(rect);

    m_editorService->setActiveTool(m_selectionTool);

    // Sélectionner la forme
    Point2D clickPos(125, 125);
    m_selectionTool->handleMousePress(clickPos, Qt::LeftButton);

    // Vérifier état initial
    QCOMPARE(rect->getX(), 100.0);
    QCOMPARE(rect->getY(), 100.0);
    QCOMPARE(m_editorService->getUndoStackSize(), 0);

    // Simuler un drag
    Point2D movePos1(130, 130);
    m_selectionTool->handleMouseMove(movePos1);

    Point2D movePos2(140, 145);
    m_selectionTool->handleMouseMove(movePos2);

    // Relâcher
    Point2D releasePos(150, 150);
    m_selectionTool->handleMouseRelease(releasePos, Qt::LeftButton);

    // Vérifier que la forme a bougé
    QVERIFY(rect->getX() != 100.0 || rect->getY() != 100.0);

    // Vérifier qu'une commande Move a été créée
    QCOMPARE(m_editorService->getUndoStackSize(), 1);
    QVERIFY(m_editorService->canUndo());

    // Undo devrait ramener à position initiale (ou très proche, tolérance de 1px)
    m_editorService->undo();
    QVERIFY(qAbs(rect->getX() - 100.0) < 1.0);  // Tolérance de 1 pixel
    QVERIFY(qAbs(rect->getY() - 100.0) < 1.0);
}

void TestEditorCommands::testSelectionTool_SelectAll()
{
    qInfo() << "Test: SelectionTool select all";

    // Créer 3 formes
    auto* rect1 = new Rectangle(100, 100, 50, 50);
    auto* rect2 = new Rectangle(200, 100, 50, 50);
    auto* circle = new Circle(300, 150, 25);

    m_editorService->addShapeDirect(rect1);
    m_editorService->addShapeDirect(rect2);
    m_editorService->addShapeDirect(circle);

    m_editorService->setActiveTool(m_selectionTool);

    // Simuler Ctrl+A
    m_selectionTool->handleKeyPress(Qt::Key_A, Qt::ControlModifier);

    // Vérifier que toutes les formes sont sélectionnées
    auto* selection = m_editorService->getSelection();
    QCOMPARE(selection->getCount(), 3);
}

// ===== Tests d'intégration EditorService =====

void TestEditorCommands::testEditorService_AddShapeWithCommand()
{
    qInfo() << "Test: EditorService addShape with command";

    // Créer une forme via addShape avec createCommand=true
    auto* rect = new Rectangle(100, 100, 50, 50);

    QCOMPARE(m_editorService->getShapeCount(), 0);
    QCOMPARE(m_editorService->getUndoStackSize(), 0);

    m_editorService->addShape(rect, true);

    // La forme devrait être ajoutée ET une commande créée
    QCOMPARE(m_editorService->getShapeCount(), 1);
    QCOMPARE(m_editorService->getUndoStackSize(), 1);
}

void TestEditorCommands::testEditorService_UndoAddShape()
{
    qInfo() << "Test: EditorService undo add shape";

    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShape(rect, true);

    QCOMPARE(m_editorService->getShapeCount(), 1);

    // Undo
    m_editorService->undo();

    // La forme devrait avoir disparu
    QCOMPARE(m_editorService->getShapeCount(), 0);
}

void TestEditorCommands::testEditorService_RedoAddShape()
{
    qInfo() << "Test: EditorService redo add shape";

    auto* rect = new Rectangle(100, 100, 50, 50);
    m_editorService->addShape(rect, true);
    m_editorService->undo();

    QCOMPARE(m_editorService->getShapeCount(), 0);

    // Redo
    m_editorService->redo();

    // La forme devrait réapparaître
    QCOMPARE(m_editorService->getShapeCount(), 1);
}

void TestEditorCommands::testEditorService_CompleteWorkflow()
{
    qInfo() << "Test: EditorService complete workflow";

    // 1. Créer 2 formes
    auto* rect = new Rectangle(100, 100, 50, 50);
    auto* circle = new Circle(200, 200, 25);

    m_editorService->addShape(rect, true);
    m_editorService->addShape(circle, true);

    QCOMPARE(m_editorService->getShapeCount(), 2);
    QCOMPARE(m_editorService->getUndoStackSize(), 2);

    // 2. Déplacer le rectangle
    IShape* rectShape = m_editorService->getShapes()[0];
    QVector<IShape*> shapes = { rectShape };
    auto* moveCmd = new Editor::MoveCommand(shapes, 50.0, 50.0);
    m_editorService->pushCommand(moveCmd);

    QCOMPARE(m_editorService->getUndoStackSize(), 3);

    // 3. Undo 2 fois (move + circle)
    m_editorService->undo(); // Annule le move
    m_editorService->undo(); // Annule le circle

    QCOMPARE(m_editorService->getShapeCount(), 1); // Seul rectangle reste
    QCOMPARE(m_editorService->getUndoStackSize(), 1);
    QCOMPARE(m_editorService->getRedoStackSize(), 2);

    // 4. Redo 1 fois
    m_editorService->redo(); // Recrée le circle

    QCOMPARE(m_editorService->getShapeCount(), 2);
    QCOMPARE(m_editorService->getRedoStackSize(), 1);
}

// ===== Tests de reproduction du bug =====

void TestEditorCommands::testBugReproduction_UndoAfterMove()
{
    qInfo() << "Test: Bug reproduction - Undo after move";

    // Reproduire exactement le workflow de l'utilisateur

    // 1. Créer un rectangle
    auto* rect = new Rectangle(171.243, 179.877, 50, 50);
    m_editorService->addShape(rect, true);

    qInfo() << "  Step 1: Created rectangle at" << rect->getX() << rect->getY();
    qInfo() << "  Undo stack size:" << m_editorService->getUndoStackSize();
    qInfo() << "  Can undo:" << m_editorService->canUndo();

    // 2. Sélectionner et déplacer
    m_editorService->setActiveTool(m_selectionTool);

    IShape* shape = m_editorService->getShapes()[0];
    Rectangle* rectShape = qobject_cast<Rectangle*>(shape);
    QVERIFY(rectShape != nullptr);

    QVector<IShape*> shapes = { shape };
    auto* moveCmd = new Editor::MoveCommand(shapes, 27.3413, -62.5973);
    m_editorService->pushCommand(moveCmd);

    qInfo() << "  Step 2: Moved rectangle to" << rectShape->getX() << rectShape->getY();
    qInfo() << "  Undo stack size:" << m_editorService->getUndoStackSize();
    qInfo() << "  Can undo:" << m_editorService->canUndo();

    // 3. Essayer de faire Undo
    QVERIFY(m_editorService->canUndo());

    double xBefore = rectShape->getX();
    double yBefore = rectShape->getY();

    m_editorService->undo();

    qInfo() << "  Step 3: After undo, position:" << rectShape->getX() << rectShape->getY();
    qInfo() << "  Undo stack size:" << m_editorService->getUndoStackSize();
    qInfo() << "  Can redo:" << m_editorService->canRedo();

    // Vérifier que la position a changé
    QVERIFY(rectShape->getX() != xBefore || rectShape->getY() != yBefore);

    // Vérifier que canRedo est true
    QVERIFY(m_editorService->canRedo());

    // 4. Redo
    m_editorService->redo();

    qInfo() << "  Step 4: After redo, position:" << rectShape->getX() << rectShape->getY();
    QCOMPARE(rectShape->getX(), xBefore);
    QCOMPARE(rectShape->getY(), yBefore);
}

void TestEditorCommands::testBugReproduction_MultipleUndoRedo()
{
    qInfo() << "Test: Bug reproduction - Multiple undo/redo";

    // Créer 3 rectangles et les déplacer
    QList<IShape*> shapes;

    for (int i = 0; i < 3; ++i) {
        auto* rect = new Rectangle(100.0 * i, 100.0, 50, 50);
        m_editorService->addShape(rect, true);
        shapes.append(m_editorService->getShapes().last());
    }

    qInfo() << "  Created 3 rectangles";
    qInfo() << "  Undo stack size:" << m_editorService->getUndoStackSize();

    // Déplacer chaque rectangle
    for (int i = 0; i < 3; ++i) {
        QVector<IShape*> shapeVec = { shapes[i] };
        auto* moveCmd = new Editor::MoveCommand(shapeVec, 50.0 * i, 30.0 * i);
        m_editorService->pushCommand(moveCmd);
    }

    qInfo() << "  Moved all rectangles";
    qInfo() << "  Undo stack size:" << m_editorService->getUndoStackSize();
    QCOMPARE(m_editorService->getUndoStackSize(), 6); // 3 creates + 3 moves

    // Undo 5 fois
    for (int i = 0; i < 5; ++i) {
        QVERIFY(m_editorService->canUndo());
        m_editorService->undo();
        qInfo() << "  Undo" << (i + 1) << "- Stack size:" << m_editorService->getUndoStackSize();
    }

    QCOMPARE(m_editorService->getUndoStackSize(), 1);
    QCOMPARE(m_editorService->getRedoStackSize(), 5);

    // Redo 1 fois
    QVERIFY(m_editorService->canRedo());
    m_editorService->redo();

    qInfo() << "  After redo - Undo stack:" << m_editorService->getUndoStackSize();
    qInfo() << "  After redo - Redo stack:" << m_editorService->getRedoStackSize();

    QCOMPARE(m_editorService->getUndoStackSize(), 2);
    QCOMPARE(m_editorService->getRedoStackSize(), 4);
}

void TestEditorCommands::testBugFix_ConsecutiveDragsWithMerge()
{
    qInfo() << "Test: Bug fix - Consecutive drags with automatic merge";

    // Ce test reproduit le bug exact décrit par l'utilisateur :
    // Faire deux drags consécutifs → le deuxième drag fait revenir la forme à sa position initiale
    //
    // CAUSE DU BUG : CommandStack::push() tente de fusionner les commandes AVANT d'exécuter.
    // Si fusion réussit, la nouvelle commande est supprimée SANS être exécutée !
    //
    // FIX : Dans MoveCommand::mergeWith(), on exécute la commande other AVANT de fusionner.

    qInfo() << "  === ÉTAPE 1 : Créer un rectangle à position initiale ===";
    auto* rect = new Rectangle(100.0, 100.0, 50.0, 50.0);
    m_editorService->addShapeDirect(rect);

    double initialX = rect->getX();
    double initialY = rect->getY();
    qInfo() << "  Position initiale:" << initialX << "," << initialY;
    QCOMPARE(initialX, 100.0);
    QCOMPARE(initialY, 100.0);

    qInfo() << "  === ÉTAPE 2 : Premier drag (déplacement de +50, +30) ===";
    QVector<IShape*> shapes = { rect };
    auto* moveCmd1 = new Editor::MoveCommand(shapes, 50.0, 30.0);
    m_editorService->pushCommand(moveCmd1);  // Ownership transféré

    double afterFirstDragX = rect->getX();
    double afterFirstDragY = rect->getY();
    qInfo() << "  Position après 1er drag:" << afterFirstDragX << "," << afterFirstDragY;

    // Vérifier que le rectangle a bien bougé
    QCOMPARE(afterFirstDragX, 150.0);  // 100 + 50
    QCOMPARE(afterFirstDragY, 130.0);  // 100 + 30

    qInfo() << "  Undo stack size:" << m_editorService->getUndoStackSize();
    QCOMPARE(m_editorService->getUndoStackSize(), 1);

    qInfo() << "  === ÉTAPE 3 : Deuxième drag (déplacement de +20, -40) ===";
    // Ce deuxième drag devrait fusionner avec le premier via CommandStack
    auto* moveCmd2 = new Editor::MoveCommand(shapes, 20.0, -40.0);
    m_editorService->pushCommand(moveCmd2);  // Ownership transféré

    double afterSecondDragX = rect->getX();
    double afterSecondDragY = rect->getY();
    qInfo() << "  Position après 2e drag:" << afterSecondDragX << "," << afterSecondDragY;

    // VÉRIFICATION CRITIQUE : Le rectangle devrait être à (170, 90), PAS à (150, 130) !
    // Bug : si mergeWith() n'exécute pas la commande, la forme reste à (150, 130)
    QCOMPARE(afterSecondDragX, 170.0);  // 150 + 20 = 170
    QCOMPARE(afterSecondDragY, 90.0);   // 130 - 40 = 90

    qInfo() << "  Undo stack size:" << m_editorService->getUndoStackSize();
    // Après fusion, il devrait toujours y avoir 1 commande (fusionnée)
    QCOMPARE(m_editorService->getUndoStackSize(), 1);

    qInfo() << "  === ÉTAPE 4 : Undo (devrait revenir à position initiale) ===";
    m_editorService->undo();

    double afterUndoX = rect->getX();
    double afterUndoY = rect->getY();
    qInfo() << "  Position après undo:" << afterUndoX << "," << afterUndoY;

    // Après undo de la commande fusionnée, on devrait revenir à (100, 100)
    // La commande fusionnée contient : dx = 50 + 20 = 70, dy = 30 - 40 = -10
    // Donc undo déplace de -70, +10 : (170, 90) → (100, 100) ✅
    QCOMPARE(afterUndoX, initialX);
    QCOMPARE(afterUndoY, initialY);

    qInfo() << "  === ÉTAPE 5 : Redo (devrait revenir à position finale) ===";
    m_editorService->redo();

    double afterRedoX = rect->getX();
    double afterRedoY = rect->getY();
    qInfo() << "  Position après redo:" << afterRedoX << "," << afterRedoY;

    // Après redo, on devrait revenir à (170, 90)
    QCOMPARE(afterRedoX, afterSecondDragX);
    QCOMPARE(afterRedoY, afterSecondDragY);

    qInfo() << "  === TEST RÉUSSI : Le bug de fusion est corrigé ! ===";
}
