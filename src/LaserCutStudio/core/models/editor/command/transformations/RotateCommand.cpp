/**
 * @file RotateCommand.cpp
 * @brief Implémentation de la commande de rotation
 */

#include "core/models/editor/command/transformations/RotateCommand.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

using namespace Transformations;

RotateCommand::RotateCommand(const QVector<IShape*>& shapes,
                           double angleDegrees,
                           const Point2D& center,
                           QObject* parent)
    : ITransformationCommand(shapes, parent)
    , m_angleDegrees(angleDegrees)
    , m_center(center)
{
    qCDebug(logCore()) << "RotateCommand created:" << m_shapes.size() << "shape(s),"
                       << m_angleDegrees << "degrees";
}

RotateCommand::~RotateCommand()
{
    qCDebug(logCore()) << "RotateCommand destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(RotateCommand, IEditorCommand)

// ===== IEditorCommand interface =====

QString RotateCommand::getText() const
{
    if (m_shapes.size() == 1) {
        return QString("Rotate %1").arg(m_shapes.first()->getTypeName());
    } else {
        return QString("Rotate %1 shapes").arg(m_shapes.size());
    }
}

void RotateCommand::execute()
{
    if (!validateTransformState(false)) {
        return;
    }

    // Appliquer la rotation à toutes les formes
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->rotate(m_angleDegrees, m_center);
        }
    }

    markExecuted();
}

void RotateCommand::undo()
{
    if (!validateUndoState()) {
        return;
    }

    // Restaurer les positions initiales
    for (IShape* shape : m_shapes) {
        if (shape && m_initialPositions.contains(shape)) {
            QPair<double, double> initialPos = m_initialPositions[shape];
            QRectF currentBbox = shape->getBoundingBox();

            // Calculer le déplacement nécessaire pour revenir à la position initiale
            double dx = initialPos.first - currentBbox.x();
            double dy = initialPos.second - currentBbox.y();

            shape->translate(dx, dy);
        }
    }

    markUndone();
}

void RotateCommand::redo()
{
    if (!validateTransformState(true)) {
        return;
    }

    // Réappliquer la rotation
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->rotate(m_angleDegrees, m_center);
        }
    }

    markExecuted();
}

bool RotateCommand::mergeWith(IEditorCommand* other)
{
    if (!other || other->getCommandId() != "RotateShape") {
        return false;
    }

    RotateCommand* otherRotate = qobject_cast<RotateCommand*>(other);
    if (!otherRotate) {
        return false;
    }

    // Vérifier que ce sont les mêmes formes
    if (!shapesMatch(otherRotate->m_shapes)) {
        return false;
    }

    // Vérifier que le centre de rotation est le même (à epsilon près)
    const double epsilon = 1e-6;
    if (std::abs(m_center.x - otherRotate->m_center.x) > epsilon ||
        std::abs(m_center.y - otherRotate->m_center.y) > epsilon) {
        return false;
    }

    // Fusionner : additionner les angles
    m_angleDegrees += otherRotate->m_angleDegrees;

    qCDebug(logCore()) << "RotateCommand: Merged with rotation, total angle:"
                       << m_angleDegrees << "degrees";

    return true;
}

QString RotateCommand::getCommandId() const
{
    return "RotateShape";
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
