/**
 * @file TestEditorCommands.h
 * @brief Tests unitaires pour les commandes d'éditeur (Undo/Redo)
 *
 * Tests couvrant :
 * - CreateShapeCommand : Création de formes avec Undo/Redo
 * - MoveCommand : Déplacement de formes avec Undo/Redo et merge
 * - CommandStack : Gestion de la pile Undo/Redo
 * - SelectionTool : Sélection et drag & drop
 * - EditorService : Intégration complète
 */

#ifndef TESTEDITORCOMMANDS_H
#define TESTEDITORCOMMANDS_H

#include <QObject>
#include <QtTest>
#include "core/services/editor/EditorService.h"
#include "core/models/editor/implementations/CreateShapeCommand.h"
#include "core/models/editor/implementations/MoveCommand.h"
#include "core/models/editor/implementations/SelectionTool.h"
#include "core/models/editor/implementations/ShapeCreationTool.h"
#include "core/models/shapes/implementations/Rectangle.h"
#include "core/models/shapes/implementations/Circle.h"

using namespace LaserCutStudio::Core;

class TestEditorCommands : public QObject
{
    Q_OBJECT

private slots:
    // Setup et cleanup
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Tests CreateShapeCommand
    void testCreateShapeCommand_Execute();
    void testCreateShapeCommand_Undo();
    void testCreateShapeCommand_Redo();
    void testCreateShapeCommand_MultipleShapes();

    // Tests MoveCommand
    void testMoveCommand_Execute();
    void testMoveCommand_Undo();
    void testMoveCommand_Redo();
    void testMoveCommand_MultipleShapes();
    void testMoveCommand_Merge();

    // Tests CommandStack
    void testCommandStack_PushAndUndo();
    void testCommandStack_UndoRedo();
    void testCommandStack_UndoLimit();
    void testCommandStack_Clear();

    // Tests SelectionTool
    void testSelectionTool_SelectShape();
    void testSelectionTool_ClearSelection();
    void testSelectionTool_DragAndDrop();
    void testSelectionTool_SelectAll();

    // Tests d'intégration EditorService
    void testEditorService_AddShapeWithCommand();
    void testEditorService_UndoAddShape();
    void testEditorService_RedoAddShape();
    void testEditorService_CompleteWorkflow();

    // Tests de reproduction du bug
    void testBugReproduction_UndoAfterMove();
    void testBugReproduction_MultipleUndoRedo();
    void testBugFix_ConsecutiveDragsWithMerge();  // Test du bug de fusion MoveCommand

private:
    Services::EditorService* m_editorService = nullptr;
    Editor::SelectionTool* m_selectionTool = nullptr;
};

#endif // TESTEDITORCOMMANDS_H
