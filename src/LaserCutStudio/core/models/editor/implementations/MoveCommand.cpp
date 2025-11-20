/**
 * @file MoveCommand.cpp
 * @brief Implémentation de la commande de déplacement
 */

#include "core/models/editor/implementations/MoveCommand.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

MoveCommand::MoveCommand(const QVector<IShape*>& shapes,
                         double dx,
                         double dy,
                         QObject* parent)
    : IEditorCommand(parent)
    , m_shapes(shapes)
    , m_dx(dx)
    , m_dy(dy)
    , m_executed(false)
{
    qCDebug(logCore()) << "MoveCommand created:"
                       << m_shapes.size() << "shapes, delta:"
                       << m_dx << "," << m_dy;
}

MoveCommand::~MoveCommand()
{
    qCDebug(logCore()) << "MoveCommand destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(MoveCommand, IEditorCommand)

// ===== IEditorCommand interface =====

QString MoveCommand::getText() const
{
    if (m_shapes.size() == 1) {
        return QString("Move shape");
    }
    return QString("Move %1 shapes").arg(m_shapes.size());
}

void MoveCommand::execute()
{
    if (m_executed) {
        qCWarning(logCore()) << "MoveCommand: Already executed, use redo() instead";
        return;
    }

    // Déplacer toutes les formes
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->translate(m_dx, m_dy);
        }
    }

    m_executed = true;

    qCInfo(logCore()) << "MoveCommand: Moved" << m_shapes.size() << "shapes by"
                      << m_dx << "," << m_dy;
    emit executed();
}

void MoveCommand::undo()
{
    if (!m_executed) {
        qCWarning(logCore()) << "MoveCommand: Not executed, cannot undo";
        return;
    }

    // Déplacer en sens inverse
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->translate(-m_dx, -m_dy);
        }
    }

    m_executed = false;

    qCInfo(logCore()) << "MoveCommand: Undone movement";
    emit undone();
}

void MoveCommand::redo()
{
    if (m_executed) {
        qCWarning(logCore()) << "MoveCommand: Already executed, use undo() first";
        return;
    }

    // Redéplacer dans la direction d'origine
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->translate(m_dx, m_dy);
        }
    }

    m_executed = true;

    qCInfo(logCore()) << "MoveCommand: Redone movement";
    emit redone();
}

bool MoveCommand::isObsolete() const
{
    // La commande est obsolète si toutes les formes ont été détruites
    for (IShape* shape : m_shapes) {
        if (shape) {
            return false;  // Au moins une forme existe encore
        }
    }

    qCDebug(logCore()) << "MoveCommand: Obsolete (all shapes destroyed)";
    return true;
}

bool MoveCommand::canMerge() const
{
    // MoveCommand supporte la fusion
    return true;
}

QString MoveCommand::getCommandId() const
{
    return "MoveShape";
}

bool MoveCommand::mergeWith(IEditorCommand* other)
{
    // Tenter de fusionner avec une autre MoveCommand
    auto* otherMove = qobject_cast<MoveCommand*>(other);
    if (!otherMove) {
        return false;  // Pas une MoveCommand
    }

    // Vérifier que les deux commandes déplacent les mêmes formes
    if (m_shapes.size() != otherMove->m_shapes.size()) {
        return false;
    }

    for (IShape* shape : m_shapes) {
        if (!otherMove->m_shapes.contains(shape)) {
            return false;  // Pas les mêmes formes
        }
    }

    // Fusionner : additionner les deltas
    m_dx += otherMove->m_dx;
    m_dy += otherMove->m_dy;

    qCDebug(logCore()) << "MoveCommand: Merged with another, new delta:"
                       << m_dx << "," << m_dy;

    return true;
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
