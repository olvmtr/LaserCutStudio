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

MoveCommand::MoveCommand(const QVariantMap& params, QObject* parent)
    : MoveCommand(resolveShapesFromVariant(params),
                  params.value("dx").toDouble(),
                  params.value("dy").toDouble(),
                  parent)
{
    // Le constructeur délègue au constructeur principal
}

MoveCommand::~MoveCommand()
{
    qCDebug(logCore()) << "MoveCommand destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(MoveCommand, IEditorCommand)

// ===== Helper privé pour résolution des shapes =====

QVector<IShape*> MoveCommand::resolveShapesFromVariant(const QVariantMap& params)
{
    // Extraire les IDs des formes
    QStringList shapeIds = params.value("shapeIds").toStringList();

    // Résoudre les UUIDs en pointeurs IShape*
    QVector<IShape*> shapes;
    QList<IShape*> allShapes = IShape::getAllInstances();

    for (const QString& id : shapeIds) {
        // Chercher la forme avec cet UUID
        QUuid uuid(id);  // Convertir QString en QUuid
        for (IShape* shape : allShapes) {
            if (shape && shape->getId() == uuid) {
                shapes.append(shape);
                break;
            }
        }
    }

    // Log si aucune forme trouvée
    if (shapes.isEmpty()) {
        qCWarning(logCore()) << "MoveCommand: No shapes found for IDs:" << shapeIds;
    }

    return shapes;
}

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

// ===== Enregistrement automatique dans Factory =====

// Enregistre MoveCommand dans le Factory de ITransformationCommand avec factory personnalisée
const bool MoveCommand::s_registered = ITransformationCommand::registerCustomFactory(
    "MoveCommand",
    [](const QVariantMap& params) -> ITransformationCommand* {
        return new MoveCommand(params);
    }
);

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
