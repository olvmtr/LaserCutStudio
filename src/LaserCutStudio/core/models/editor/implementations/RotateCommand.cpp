/**
 * @file RotateCommand.cpp
 * @brief Implémentation de la commande de rotation
 */

#include "core/models/editor/implementations/RotateCommand.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

RotateCommand::RotateCommand(const QVector<IShape*>& shapes,
                           double angleDegrees,
                           const Point2D& center,
                           QObject* parent)
    : IEditorCommand(parent)
    , m_shapes(shapes)
    , m_angleDegrees(angleDegrees)
    , m_center(center)
    , m_executed(false)
{
    // Enregistrer les positions initiales de toutes les formes
    for (IShape* shape : m_shapes) {
        if (shape) {
            QRectF bbox = shape->getBoundingBox();
            m_initialPositions[shape] = qMakePair(bbox.x(), bbox.y());
        }
    }

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
    if (m_executed) {
        qCWarning(logCore()) << "RotateCommand: Already executed";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "RotateCommand: No shapes to rotate";
        return;
    }

    // Appliquer la rotation à toutes les formes
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->rotate(m_angleDegrees, m_center);
        }
    }

    m_executed = true;

    qCInfo(logCore()) << "RotateCommand: Rotated" << m_shapes.size() << "shape(s) by"
                      << m_angleDegrees << "degrees";
    emit executed();
}

void RotateCommand::undo()
{
    if (!m_executed) {
        qCWarning(logCore()) << "RotateCommand: Not executed, cannot undo";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "RotateCommand: No shapes to undo";
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

    m_executed = false;

    qCInfo(logCore()) << "RotateCommand: Undone rotation of" << m_shapes.size() << "shape(s)";
    emit undone();
}

void RotateCommand::redo()
{
    if (m_executed) {
        qCWarning(logCore()) << "RotateCommand: Already executed, use undo() first";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "RotateCommand: No shapes to redo";
        return;
    }

    // Réappliquer la rotation
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->rotate(m_angleDegrees, m_center);
        }
    }

    m_executed = true;

    qCInfo(logCore()) << "RotateCommand: Redone rotation of" << m_shapes.size() << "shape(s)";
    emit redone();
}

bool RotateCommand::isObsolete() const
{
    // La commande est obsolète si toutes les formes ont été détruites
    for (IShape* shape : m_shapes) {
        if (shape) {
            return false;  // Au moins une forme existe encore
        }
    }

    qCDebug(logCore()) << "RotateCommand: Obsolete (all shapes destroyed)";
    return true;
}

bool RotateCommand::canMerge() const
{
    // Permet la fusion de rotations consécutives
    return true;
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
    if (m_shapes.size() != otherRotate->m_shapes.size()) {
        return false;
    }

    for (IShape* shape : m_shapes) {
        if (!otherRotate->m_shapes.contains(shape)) {
            return false;
        }
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
