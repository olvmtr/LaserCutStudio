/**
 * @file ITransformationCommand.cpp
 * @brief Implémentation de l'interface pour commandes de transformation
 */

#include "core/models/editor/command/transformations/ITransformationCommand.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {
namespace Transformations {  // ⭐ Nouveau sous-namespace

ITransformationCommand::ITransformationCommand(const QVector<IShape*>& shapes,
                                               QObject* parent)
    : IEditorCommand(parent)
    , m_shapes(shapes)
    , m_executed(false)
{
    // Enregistrer automatiquement cette commande dans la liste via ListManagerMixin
    registerInstance(this);

    // Enregistrer les positions initiales de toutes les formes
    recordInitialPositions();
}

ITransformationCommand::~ITransformationCommand()
{
    // Désenregistrer automatiquement cette commande de la liste via ListManagerMixin
    unregisterInstance(this);
}

// ===== IEditorCommand interface (implémentation commune) =====

bool ITransformationCommand::isObsolete() const
{
    // La commande est obsolète si toutes les formes ont été détruites
    for (IShape* shape : m_shapes) {
        if (shape) {
            return false;  // Au moins une forme existe encore
        }
    }

    qCDebug(logCore()) << getTypeName() << ": Obsolete (all shapes destroyed)";
    return true;
}

// ===== Helpers de validation =====

bool ITransformationCommand::validateTransformState(bool isRedo) const
{
    // Vérifier l'état d'exécution
    if (isRedo) {
        // Pour redo(), la commande doit avoir été undone
        if (m_executed) {
            qCWarning(logCore()) << getTypeName()
                                << ": Already executed, use undo() first";
            return false;
        }
    } else {
        // Pour execute(), la commande ne doit pas avoir été exécutée
        if (m_executed) {
            qCWarning(logCore()) << getTypeName()
                                << ": Already executed, use redo() instead";
            return false;
        }
    }

    // Vérifier que la liste n'est pas vide
    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << getTypeName() << ": No shapes to transform";
        return false;
    }

    return true;
}

bool ITransformationCommand::validateUndoState() const
{
    // Vérifier que la commande a été exécutée
    if (!m_executed) {
        qCWarning(logCore()) << getTypeName()
                            << ": Not executed, cannot undo";
        return false;
    }

    // Vérifier que la liste n'est pas vide
    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << getTypeName() << ": No shapes to undo";
        return false;
    }

    return true;
}

// ===== Helpers de comparaison =====

bool ITransformationCommand::shapesMatch(const QVector<IShape*>& other) const
{
    // Vérifier la taille
    if (m_shapes.size() != other.size()) {
        return false;
    }

    // Vérifier que chaque forme de m_shapes est dans other
    for (IShape* shape : m_shapes) {
        if (!other.contains(shape)) {
            return false;  // Pas les mêmes formes
        }
    }

    return true;
}

// ===== Helpers d'enregistrement =====

void ITransformationCommand::recordInitialPositions()
{
    m_initialPositions.clear();

    for (IShape* shape : m_shapes) {
        if (shape) {
            QRectF bbox = shape->getBoundingBox();
            m_initialPositions[shape] = qMakePair(bbox.x(), bbox.y());
        }
    }
}

void ITransformationCommand::markExecuted()
{
    m_executed = true;
    qCInfo(logCore()) << getTypeName() << ": Transformation applied to"
                     << m_shapes.size() << "shape(s)";
    emit executed();
}

void ITransformationCommand::markUndone()
{
    m_executed = false;
    qCInfo(logCore()) << getTypeName() << ": Undone transformation of"
                     << m_shapes.size() << "shape(s)";
    emit undone();
}

} // namespace Transformations
} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
