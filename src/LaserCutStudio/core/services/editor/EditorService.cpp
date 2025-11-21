/**
 * @file EditorService.cpp
 * @brief Implémentation du service éditeur 2D
 */

#include "core/services/editor/EditorService.h"
#include "core/models/editor/implementations/SelectionManager.h"
#include "core/models/editor/implementations/CreateShapeCommand.h"
#include "core/models/editor/implementations/DeleteShapeCommand.h"
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

bool EditorService::activateToolByShapeType(const QString& shapeType)
{
    // Chercher un outil dont le nom contient le type de forme
    // Ex: "Create Rectangle" pour shapeType "Rectangle"
    for (Editor::ITool* tool : m_availableTools) {
        if (tool && tool->getName().contains(shapeType, Qt::CaseInsensitive)) {
            setActiveTool(tool);
            qCInfo(logCore()) << "Activated tool:" << tool->getName() << "for shape type:" << shapeType;
            return true;
        }
    }

    qCWarning(logCore()) << "No tool found for shape type:" << shapeType;
    return false;
}

// ===== Gestion des formes =====

void EditorService::addShape(IShape* shape, bool createCommand)
{
    if (!shape) {
        qCWarning(logCore()) << "Cannot add null shape";
        return;
    }

    if (createCommand) {
        // Sérialiser la forme et créer une commande Undo/Redo
        QVariantMap shapeParams = shape->toVariant();
        auto* command = new Editor::CreateShapeCommand(shapeParams, this);

        // Supprimer la forme passée car CreateShapeCommand va en créer une nouvelle
        delete shape;

        pushCommand(command);
    } else {
        // Ajout direct sans commande
        addShapeDirect(shape);
    }
}

void EditorService::addShapeDirect(IShape* shape)
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
}

void EditorService::removeShape(IShape* shape, bool createCommand)
{
    if (!shape) {
        qCWarning(logCore()) << "Cannot remove null shape";
        return;
    }

    if (createCommand) {
        // Créer une commande de suppression avec Undo/Redo
        QVector<IShape*> shapes = { shape };
        auto* cmd = new Editor::DeleteShapeCommand(shapes, this);
        pushCommand(cmd);
    } else {
        removeShapeDirect(shape);
    }
}

void EditorService::removeShapeDirect(IShape* shape)
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

void EditorService::deleteSelectedShapes()
{
    if (!m_selection) {
        qCWarning(logCore()) << "Selection manager is null, cannot delete";
        return;
    }

    QVector<IShape*> selectedShapes = m_selection->getSelectedShapes();

    if (selectedShapes.isEmpty()) {
        qCDebug(logCore()) << "No shapes selected to delete";
        return;
    }

    qCInfo(logCore()) << "Deleting" << selectedShapes.size() << "selected shape(s)";

    // Créer une commande de suppression avec Undo/Redo
    auto* cmd = new Editor::DeleteShapeCommand(selectedShapes, this);
    pushCommand(cmd);

    // Désélectionner après suppression
    m_selection->clear();
}

int EditorService::copySelectedShapes()
{
    if (!m_selection) {
        qCWarning(logCore()) << "Selection manager is null, cannot copy";
        return 0;
    }

    QVector<IShape*> selectedShapes = m_selection->getSelectedShapes();

    if (selectedShapes.isEmpty()) {
        qCDebug(logCore()) << "No shapes selected to copy";
        return 0;
    }

    // Vider le clipboard
    m_clipboard.clear();

    // Sérialiser toutes les formes sélectionnées
    for (IShape* shape : selectedShapes) {
        if (shape) {
            m_clipboard.append(shape->toVariant());
        }
    }

    qCInfo(logCore()) << "Copied" << m_clipboard.size() << "shape(s) to clipboard";
    return m_clipboard.size();
}

int EditorService::pasteShapes()
{
    if (m_clipboard.isEmpty()) {
        qCDebug(logCore()) << "Clipboard is empty, nothing to paste";
        return 0;
    }

    // Désérialiser toutes les formes du clipboard
    QVector<IShape*> newShapes;
    for (const QVariantMap& data : m_clipboard) {
        IShape* shape = IShape::create(data);
        if (shape) {
            // Décaler légèrement pour voir la nouvelle forme
            QRectF bbox = shape->getBoundingBox();
            shape->translate(10.0, 10.0);
            newShapes.append(shape);
        }
    }

    if (newShapes.isEmpty()) {
        qCWarning(logCore()) << "Failed to create shapes from clipboard";
        return 0;
    }

    // Ajouter toutes les formes avec une seule commande
    // TODO: Créer PasteCommand pour gérer le batch
    for (IShape* shape : newShapes) {
        addShape(shape, true);
    }

    qCInfo(logCore()) << "Pasted" << newShapes.size() << "shape(s) from clipboard";
    return newShapes.size();
}

bool EditorService::hasClipboardData() const
{
    return !m_clipboard.isEmpty();
}

QVariantMap EditorService::getSelectionProperties() const
{
    if (!m_selection) {
        return QVariantMap();
    }

    QVector<IShape*> selectedShapes = m_selection->getSelectedShapes();

    // Retourner les propriétés uniquement si une seule forme est sélectionnée
    if (selectedShapes.size() != 1) {
        return QVariantMap();
    }

    IShape* shape = selectedShapes.first();
    if (!shape) {
        return QVariantMap();
    }

    // Utiliser toVariant() pour obtenir toutes les propriétés
    QVariantMap props = shape->toVariant();

    // Ajouter le bounding box pour faciliter l'édition
    QRectF bbox = shape->getBoundingBox();
    props["x"] = bbox.x();
    props["y"] = bbox.y();
    props["width"] = bbox.width();
    props["height"] = bbox.height();

    return props;
}

bool EditorService::setSelectionProperty(const QString& propertyName, const QVariant& value)
{
    if (!m_selection) {
        qCWarning(logCore()) << "Selection manager is null";
        return false;
    }

    QVector<IShape*> selectedShapes = m_selection->getSelectedShapes();

    // Modifier uniquement si une seule forme est sélectionnée
    if (selectedShapes.size() != 1) {
        qCWarning(logCore()) << "Can only edit properties of single selection";
        return false;
    }

    IShape* shape = selectedShapes.first();
    if (!shape) {
        return false;
    }

    // Utiliser le système de propriétés Qt pour modifier
    const QMetaObject* metaObj = shape->metaObject();
    int propIndex = metaObj->indexOfProperty(propertyName.toLatin1().constData());

    if (propIndex == -1) {
        qCWarning(logCore()) << "Property not found:" << propertyName;
        return false;
    }

    QMetaProperty metaProp = metaObj->property(propIndex);
    bool success = metaProp.write(shape, value);

    if (success) {
        qCDebug(logCore()) << "Property" << propertyName << "set to" << value;
        // TODO: Créer une commande Undo/Redo pour cette modification
    } else {
        qCWarning(logCore()) << "Failed to set property" << propertyName;
    }

    return success;
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

QStringList EditorService::getAvailableShapeTypes() const
{
    // Récupérer les types enregistrés depuis le Factory Pattern d'IShape
    QVector<QString> types = IShape::availableTypes();

    // Convertir en QStringList pour QML
    QStringList result;
    for (const QString& type : types) {
        result.append(type);
    }

    qCDebug(logCore()) << "Available shape types:" << result;
    return result;
}

} // namespace Services
} // namespace Core
} // namespace LaserCutStudio
