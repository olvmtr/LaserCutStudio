/**
 * @file MoveCommand.cpp
 * @brief Implémentation de la commande de déplacement
 */

#include "core/models/editor/command/transformations/MoveCommand.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

using namespace Transformations;

MoveCommand::MoveCommand(const QVector<IShape*>& shapes,
                         double dx,
                         double dy,
                         QObject* parent)
    : ITransformationCommand(shapes, parent)
    , m_dx(dx)
    , m_dy(dy)
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
    if (!validateTransformState(false)) {
        return;
    }

    // Déplacer toutes les formes
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->translate(m_dx, m_dy);
        }
    }

    markExecuted();
}

void MoveCommand::undo()
{
    if (!validateUndoState()) {
        return;
    }

    // Déplacer en sens inverse
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->translate(-m_dx, -m_dy);
        }
    }

    markUndone();
}

void MoveCommand::redo()
{
    if (!validateTransformState(true)) {
        return;
    }

    // Redéplacer dans la direction d'origine
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->translate(m_dx, m_dy);
        }
    }

    markExecuted();
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
    if (!shapesMatch(otherMove->m_shapes)) {
        return false;
    }

    // IMPORTANT : Si THIS a déjà été exécutée (usage via CommandStack::push()),
    // alors on doit exécuter OTHER avant de fusionner, sinon les formes
    // ne seront pas déplacées visuellement.
    // Si THIS n'a pas été exécutée (usage manuel de mergeWith()), on ne fait rien.
    if (m_executed && !otherMove->m_executed) {
        otherMove->execute();
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
