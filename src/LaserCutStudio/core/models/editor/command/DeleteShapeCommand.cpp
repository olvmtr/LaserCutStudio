/**
 * @file DeleteShapeCommand.cpp
 * @brief Implémentation de la commande de suppression de formes
 */

#include "core/models/editor/command/DeleteShapeCommand.h"
#include "core/models/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

DeleteShapeCommand::DeleteShapeCommand(const QVector<IShape*>& shapes,
                                       Services::EditorService* editorService,
                                       QObject* parent)
    : IEditorCommand(parent)
    , m_shapes(shapes)
    , m_editorService(editorService)
    , m_executed(false)
{
    if (!m_editorService) {
        qCWarning(logCore()) << "DeleteShapeCommand: EditorService is null!";
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "DeleteShapeCommand: No shapes to delete!";
    } else {
        qCDebug(logCore()) << "DeleteShapeCommand created for" << m_shapes.size() << "shape(s)";
    }
}

DeleteShapeCommand::~DeleteShapeCommand()
{
    // Si les formes sont en notre possession (après execute), les supprimer
    if (m_executed) {
        qCDebug(logCore()) << "DeleteShapeCommand: Deleting" << m_shapes.size() << "owned shape(s)";
        for (IShape* shape : m_shapes) {
            delete shape;
        }
        m_shapes.clear();
    }

    qCDebug(logCore()) << "DeleteShapeCommand destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(DeleteShapeCommand, IEditorCommand)

// ===== IEditorCommand interface =====

QString DeleteShapeCommand::getText() const
{
    if (m_shapes.size() == 1) {
        return QString("Delete %1").arg(m_shapes.first()->getTypeName());
    } else {
        return QString("Delete %1 shapes").arg(m_shapes.size());
    }
}

void DeleteShapeCommand::execute()
{
    if (m_executed) {
        qCWarning(logCore()) << "DeleteShapeCommand: Already executed, use redo() instead";
        return;
    }

    if (!m_editorService) {
        qCCritical(logCore()) << "DeleteShapeCommand: EditorService is null, cannot execute!";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "DeleteShapeCommand: No shapes to delete";
        return;
    }

    // Retirer toutes les formes de l'éditeur directement (pas de commande pour éviter récursion)
    for (IShape* shape : m_shapes) {
        if (shape) {
            m_editorService->removeShapeDirect(shape);
        }
    }

    // On garde les formes en vie mais on prend ownership
    // (elles ne sont plus dans l'éditeur mais existent encore en mémoire)
    m_executed = true;

    qCInfo(logCore()) << "DeleteShapeCommand: Removed" << m_shapes.size() << "shape(s) from editor";
    emit executed();
}

void DeleteShapeCommand::undo()
{
    if (!m_executed) {
        qCWarning(logCore()) << "DeleteShapeCommand: Not executed, cannot undo";
        return;
    }

    if (!m_editorService) {
        qCCritical(logCore()) << "DeleteShapeCommand: EditorService is null, cannot undo!";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "DeleteShapeCommand: No shapes to restore";
        return;
    }

    // Remettre toutes les formes dans l'éditeur directement (pas de commande)
    for (IShape* shape : m_shapes) {
        if (shape) {
            m_editorService->addShapeDirect(shape);
        }
    }

    // Les formes sont maintenant dans l'éditeur, on relâche ownership
    m_executed = false;

    qCInfo(logCore()) << "DeleteShapeCommand: Restored" << m_shapes.size() << "shape(s) to editor";
    emit undone();
}

void DeleteShapeCommand::redo()
{
    if (m_executed) {
        qCWarning(logCore()) << "DeleteShapeCommand: Already executed, use undo() first";
        return;
    }

    if (!m_editorService) {
        qCCritical(logCore()) << "DeleteShapeCommand: EditorService is null, cannot redo!";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "DeleteShapeCommand: No shapes to delete";
        return;
    }

    // Retirer à nouveau toutes les formes de l'éditeur directement (pas de commande)
    for (IShape* shape : m_shapes) {
        if (shape) {
            m_editorService->removeShapeDirect(shape);
        }
    }

    // On reprend ownership
    m_executed = true;

    qCInfo(logCore()) << "DeleteShapeCommand: Removed" << m_shapes.size() << "shape(s) from editor (redo)";
    emit redone();
}

bool DeleteShapeCommand::isObsolete() const
{
    // La commande est obsolète si :
    // 1. L'EditorService est nul
    // 2. Aucune forme valide (toutes nulles)
    if (!m_editorService) {
        qCDebug(logCore()) << "DeleteShapeCommand: Obsolete (EditorService is null)";
        return true;
    }

    // Vérifier si au moins une forme est valide
    bool hasValidShape = false;
    for (IShape* shape : m_shapes) {
        if (shape) {
            hasValidShape = true;
            break;
        }
    }

    if (!hasValidShape) {
        qCDebug(logCore()) << "DeleteShapeCommand: Obsolete (All shapes are null)";
        return true;
    }

    return false;
}

bool DeleteShapeCommand::mergeWith(IEditorCommand* other)
{
    Q_UNUSED(other)
    // DeleteShapeCommand ne supporte pas la fusion
    return false;
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
