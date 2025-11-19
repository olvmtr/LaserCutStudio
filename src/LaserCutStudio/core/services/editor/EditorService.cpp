/**
 * @file EditorService.cpp
 * @brief Implémentation du service éditeur 2D
 */

#include "core/services/editor/EditorService.h"
#include "core/models/editor/implementations/SelectionManager.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Services {

EditorService::EditorService(QObject* parent)
    : QObject(parent)
    , m_activeTool(nullptr)
    , m_selection(nullptr)
    , m_commandStack(nullptr)
{
    qCInfo(logCore()) << "EditorService created";

    // Créer le gestionnaire de sélection
    m_selection = new Editor::SelectionManager(this);

    // Créer le CommandStack
    m_commandStack = new Editor::CommandStack(this);
    m_commandStack->setUndoLimit(100);  // Limite par défaut

    // Connecter les signaux du CommandStack
    connect(m_commandStack, &Editor::CommandStack::canUndoChanged,
            this, &EditorService::undoStackChanged);
    connect(m_commandStack, &Editor::CommandStack::canRedoChanged,
            this, &EditorService::undoStackChanged);
    connect(m_commandStack, &Editor::CommandStack::undoCountChanged,
            this, &EditorService::undoStackChanged);
    connect(m_commandStack, &Editor::CommandStack::redoCountChanged,
            this, &EditorService::undoStackChanged);
    connect(m_commandStack, &Editor::CommandStack::commandUndone,
            this, &EditorService::undone);
    connect(m_commandStack, &Editor::CommandStack::commandRedone,
            this, &EditorService::redone);

    qCInfo(logCore()) << "EditorService initialized with CommandStack and SelectionManager";
}

EditorService::~EditorService()
{
    qCInfo(logCore()) << "EditorService destroyed";

    // CommandStack et SelectionManager sont détruits automatiquement (parents Qt)
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
    return m_commandStack ? m_commandStack->canUndo() : false;
}

bool EditorService::canRedo() const
{
    return m_commandStack ? m_commandStack->canRedo() : false;
}

void EditorService::undo()
{
    if (!m_commandStack) {
        qCWarning(logCore()) << "CommandStack is null, cannot undo";
        return;
    }

    m_commandStack->undo();
}

void EditorService::redo()
{
    if (!m_commandStack) {
        qCWarning(logCore()) << "CommandStack is null, cannot redo";
        return;
    }

    m_commandStack->redo();
}

void EditorService::pushCommand(Editor::IEditorCommand* command)
{
    if (!command) {
        qCWarning(logCore()) << "Cannot push null command";
        return;
    }

    if (!m_commandStack) {
        qCWarning(logCore()) << "CommandStack is null, cannot push command";
        delete command;
        return;
    }

    m_commandStack->push(command);
}

void EditorService::clearUndoStack()
{
    if (!m_commandStack) {
        qCWarning(logCore()) << "CommandStack is null, cannot clear";
        return;
    }

    m_commandStack->clear();
}

void EditorService::setUndoLimit(int limit)
{
    if (!m_commandStack) {
        qCWarning(logCore()) << "CommandStack is null, cannot set limit";
        return;
    }

    m_commandStack->setUndoLimit(limit);
    qCInfo(logCore()) << "Undo limit set to:" << limit;
}

int EditorService::getUndoStackSize() const
{
    return m_commandStack ? m_commandStack->undoCount() : 0;
}

int EditorService::getRedoStackSize() const
{
    return m_commandStack ? m_commandStack->redoCount() : 0;
}

int EditorService::getUndoLimit() const
{
    return m_commandStack ? m_commandStack->undoLimit() : 0;
}

} // namespace Services
} // namespace Core
} // namespace LaserCutStudio
