/**
 * @file EditorService.cpp
 * @brief Implémentation du service éditeur 2D
 */

#include "core/services/editor/EditorService.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Services {

EditorService::EditorService(QObject* parent)
    : QObject(parent)
    , m_activeTool(nullptr)
    , m_selection(nullptr)
{
    qCInfo(logCore()) << "EditorService created";

    // TODO: Créer une implémentation concrète de ISelection
    // m_selection = new SelectionManager(this);
}

EditorService::~EditorService()
{
    qCInfo(logCore()) << "EditorService destroyed";

    // Nettoyer les stacks Undo/Redo
    clearUndoStack();

    // Les formes sont gérées par leur parent Qt
    // Les outils sont gérés par leur parent Qt
}

// ===== Gestion des outils =====

void EditorService::setActiveTool(Editor::ITool* tool)
{
    if (m_activeTool == tool) {
        return;  // Pas de changement
    }

    // Désactiver l'outil précédent
    if (m_activeTool) {
        m_activeTool->deactivate();
        m_activeTool->setActive(false);
    }

    m_activeTool = tool;

    // Activer le nouvel outil
    if (m_activeTool) {
        m_activeTool->activate();
        m_activeTool->setActive(true);
        qCInfo(logCore()) << "Active tool changed to:" << m_activeTool->getName();
    } else {
        qCInfo(logCore()) << "Active tool cleared";
    }

    emit activeToolChanged(m_activeTool);
}

void EditorService::registerTool(Editor::ITool* tool)
{
    if (!tool) {
        qCWarning(logCore()) << "Cannot register null tool";
        return;
    }

    if (m_availableTools.contains(tool)) {
        qCWarning(logCore()) << "Tool already registered:" << tool->getName();
        return;
    }

    m_availableTools.append(tool);
    qCInfo(logCore()) << "Tool registered:" << tool->getName();
}

// ===== Gestion des formes =====

void EditorService::addShape(IShape* shape, bool createCommand)
{
    if (!shape) {
        qCWarning(logCore()) << "Cannot add null shape";
        return;
    }

    if (m_shapes.contains(shape)) {
        qCWarning(logCore()) << "Shape already in editor";
        return;
    }

    m_shapes.append(shape);
    qCInfo(logShapes()) << "Shape added to editor:" << shape->getTypeName();

    emit shapeAdded(shape);
    emit shapeCountChanged(m_shapes.size());

    // TODO: Créer CreateShapeCommand si createCommand == true
}

void EditorService::removeShape(IShape* shape, bool createCommand)
{
    if (!shape) {
        qCWarning(logCore()) << "Cannot remove null shape";
        return;
    }

    int index = m_shapes.indexOf(shape);
    if (index == -1) {
        qCWarning(logCore()) << "Shape not found in editor";
        return;
    }

    m_shapes.removeAt(index);
    qCInfo(logShapes()) << "Shape removed from editor:" << shape->getTypeName();

    emit shapeRemoved(shape);
    emit shapeCountChanged(m_shapes.size());

    // TODO: Créer DeleteShapeCommand si createCommand == true
}

void EditorService::clear()
{
    int count = m_shapes.size();
    m_shapes.clear();

    qCInfo(logCore()) << "Editor cleared," << count << "shapes removed";
    emit shapeCountChanged(0);
}

// ===== Undo/Redo =====

bool EditorService::canUndo() const
{
    return !m_undoStack.isEmpty();
}

bool EditorService::canRedo() const
{
    return !m_redoStack.isEmpty();
}

void EditorService::undo()
{
    if (!canUndo()) {
        qCWarning(logCore()) << "Cannot undo: stack is empty";
        return;
    }

    Editor::IEditorCommand* command = m_undoStack.pop();

    if (command->isObsolete()) {
        qCWarning(logCore()) << "Command is obsolete, skipping:" << command->getText();
        delete command;

        // Nettoyer les commandes obsolètes et réessayer
        cleanObsoleteCommands();
        if (canUndo()) {
            undo();  // Récursion pour trouver une commande valide
        }
        return;
    }

    command->undo();
    m_redoStack.push(command);

    qCInfo(logCore()) << "Undo:" << command->getText();

    emit undone();
    emit undoStackChanged();
}

void EditorService::redo()
{
    if (!canRedo()) {
        qCWarning(logCore()) << "Cannot redo: stack is empty";
        return;
    }

    Editor::IEditorCommand* command = m_redoStack.pop();

    if (command->isObsolete()) {
        qCWarning(logCore()) << "Command is obsolete, skipping:" << command->getText();
        delete command;

        // Nettoyer et réessayer
        cleanObsoleteCommands();
        if (canRedo()) {
            redo();  // Récursion
        }
        return;
    }

    command->redo();
    m_undoStack.push(command);

    qCInfo(logCore()) << "Redo:" << command->getText();

    emit redone();
    emit undoStackChanged();
}

void EditorService::pushCommand(Editor::IEditorCommand* command)
{
    if (!command) {
        qCWarning(logCore()) << "Cannot push null command";
        return;
    }

    // Exécuter la commande
    command->execute();

    // Vider la pile Redo (nouvelle branche d'historique)
    qDeleteAll(m_redoStack);
    m_redoStack.clear();

    // Tentative de fusion avec la dernière commande
    if (!m_undoStack.isEmpty()) {
        Editor::IEditorCommand* lastCommand = m_undoStack.top();

        if (lastCommand->canMerge() && command->canMerge() &&
            lastCommand->getCommandId() == command->getCommandId()) {

            if (lastCommand->mergeWith(command)) {
                qCInfo(logCore()) << "Command merged with previous:" << command->getText();
                delete command;  // Fusion réussie, supprimer la commande
                emit undoStackChanged();
                return;
            }
        }
    }

    // Ajouter au stack
    m_undoStack.push(command);
    qCInfo(logCore()) << "Command pushed:" << command->getText();

    // Limiter la taille du stack
    while (m_undoStack.size() > m_undoLimit) {
        Editor::IEditorCommand* oldest = m_undoStack.first();
        m_undoStack.removeFirst();
        delete oldest;
    }

    emit undoStackChanged();
}

void EditorService::clearUndoStack()
{
    qDeleteAll(m_undoStack);
    m_undoStack.clear();

    qDeleteAll(m_redoStack);
    m_redoStack.clear();

    qCInfo(logCore()) << "Undo/Redo stacks cleared";
    emit undoStackChanged();
}

void EditorService::setUndoLimit(int limit)
{
    if (limit < 0) {
        qCWarning(logCore()) << "Invalid undo limit:" << limit;
        return;
    }

    m_undoLimit = limit;

    // Nettoyer si nécessaire
    while (m_undoStack.size() > m_undoLimit) {
        Editor::IEditorCommand* oldest = m_undoStack.first();
        m_undoStack.removeFirst();
        delete oldest;
    }
}

void EditorService::cleanObsoleteCommands()
{
    // Nettoyer le stack Undo
    for (int i = m_undoStack.size() - 1; i >= 0; --i) {
        if (m_undoStack[i]->isObsolete()) {
            qCInfo(logCore()) << "Removing obsolete command from undo stack:"
                              << m_undoStack[i]->getText();
            delete m_undoStack[i];
            m_undoStack.removeAt(i);
        }
    }

    // Nettoyer le stack Redo
    for (int i = m_redoStack.size() - 1; i >= 0; --i) {
        if (m_redoStack[i]->isObsolete()) {
            qCInfo(logCore()) << "Removing obsolete command from redo stack:"
                              << m_redoStack[i]->getText();
            delete m_redoStack[i];
            m_redoStack.removeAt(i);
        }
    }
}

} // namespace Services
} // namespace Core
} // namespace LaserCutStudio
