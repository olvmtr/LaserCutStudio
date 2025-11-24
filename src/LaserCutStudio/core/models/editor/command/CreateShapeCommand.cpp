/**
 * @file CreateShapeCommand.cpp
 * @brief Implémentation de la commande de création de forme
 */

#include "core/models/editor/command/CreateShapeCommand.h"
#include "core/models/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

// Note: CreateShapeCommand n'est PAS auto-enregistré dans le Factory Pattern
// car il nécessite une EditorService qui ne peut pas être fournie automatiquement.
// Les commandes sont créées manuellement par le code qui a accès à EditorService.

CreateShapeCommand::CreateShapeCommand(const QVariantMap& shapeParams,
                                       Services::EditorService* editorService,
                                       QObject* parent)
    : IEditorCommand(parent)
    , m_shapeParams(shapeParams)
    , m_editorService(editorService)
    , m_shape(nullptr)
    , m_executed(false)
{
    if (!m_editorService) {
        qCWarning(logCore()) << "CreateShapeCommand: EditorService is null!";
    }

    QString shapeType = m_shapeParams.value("type").toString();
    qCDebug(logCore()) << "CreateShapeCommand created for type:" << shapeType;
}

CreateShapeCommand::~CreateShapeCommand()
{
    // Si la forme est en notre possession (après undo), la supprimer
    if (m_shape && !m_executed) {
        qCDebug(logCore()) << "CreateShapeCommand: Deleting owned shape";
        delete m_shape;
        m_shape = nullptr;
    }

    qCDebug(logCore()) << "CreateShapeCommand destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(CreateShapeCommand, IEditorCommand)

// ===== IEditorCommand interface =====

QString CreateShapeCommand::getText() const
{
    QString shapeType = m_shapeParams.value("type").toString();
    return QString("Create %1").arg(shapeType);
}

void CreateShapeCommand::execute()
{
    if (m_executed) {
        qCWarning(logCore()) << "CreateShapeCommand: Already executed, use redo() instead";
        return;
    }

    if (!m_editorService) {
        qCCritical(logCore()) << "CreateShapeCommand: EditorService is null, cannot execute!";
        return;
    }

    // Créer la forme via Factory Pattern
    m_shape = IShape::create(m_shapeParams);

    if (!m_shape) {
        QString shapeType = m_shapeParams.value("type").toString();
        qCCritical(logCore()) << "CreateShapeCommand: Failed to create shape:" << shapeType;
        qCCritical(logCore()) << "Available types:" << IShape::availableTypes();
        return;
    }

    // Ajouter à l'éditeur directement (pas de commande pour éviter récursion)
    m_editorService->addShapeDirect(m_shape);

    m_executed = true;

    qCInfo(logShapes()) << "CreateShapeCommand: Shape created:" << m_shape->getTypeName();
    emit executed();
}

void CreateShapeCommand::undo()
{
    if (!m_executed) {
        qCWarning(logCore()) << "CreateShapeCommand: Not executed, cannot undo";
        return;
    }

    if (!m_shape) {
        qCWarning(logCore()) << "CreateShapeCommand: Shape is null, command may be obsolete";
        return;
    }

    if (!m_editorService) {
        qCCritical(logCore()) << "CreateShapeCommand: EditorService is null, cannot undo!";
        return;
    }

    // Retirer la forme de l'éditeur directement (pas de commande)
    m_editorService->removeShapeDirect(m_shape);

    // On garde la forme en vie mais on prend ownership
    // (elle n'est plus dans l'éditeur mais existe encore en mémoire)
    m_executed = false;

    qCInfo(logCore()) << "CreateShapeCommand: Undone, shape removed from editor";
    emit undone();
}

void CreateShapeCommand::redo()
{
    if (m_executed) {
        qCWarning(logCore()) << "CreateShapeCommand: Already executed, use undo() first";
        return;
    }

    if (!m_shape) {
        qCWarning(logCore()) << "CreateShapeCommand: Shape is null, cannot redo";
        return;
    }

    if (!m_editorService) {
        qCCritical(logCore()) << "CreateShapeCommand: EditorService is null, cannot redo!";
        return;
    }

    // Remettre la forme dans l'éditeur directement (pas de commande)
    m_editorService->addShapeDirect(m_shape);

    m_executed = true;

    qCInfo(logCore()) << "CreateShapeCommand: Redone, shape added back to editor";
    emit redone();
}

bool CreateShapeCommand::isObsolete() const
{
    // La commande est obsolète si :
    // 1. La forme a été détruite (nullptr)
    // 2. L'EditorService est nul
    if (!m_editorService) {
        qCDebug(logCore()) << "CreateShapeCommand: Obsolete (EditorService is null)";
        return true;
    }

    if (m_executed && !m_shape) {
        qCDebug(logCore()) << "CreateShapeCommand: Obsolete (Shape was destroyed)";
        return true;
    }

    return false;
}

bool CreateShapeCommand::mergeWith(IEditorCommand* other)
{
    Q_UNUSED(other)
    // CreateShapeCommand ne supporte pas la fusion
    return false;
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
