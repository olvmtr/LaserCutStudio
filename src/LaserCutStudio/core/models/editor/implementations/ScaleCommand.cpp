/**
 * @file ScaleCommand.cpp
 * @brief Implémentation de la commande de mise à l'échelle
 */

#include "core/models/editor/implementations/ScaleCommand.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

ScaleCommand::ScaleCommand(const QVector<IShape*>& shapes,
                         double scaleX,
                         double scaleY,
                         const Point2D& center,
                         QObject* parent)
    : IEditorCommand(parent)
    , m_shapes(shapes)
    , m_scaleX(scaleX)
    , m_scaleY(scaleY)
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

    qCDebug(logCore()) << "ScaleCommand created:" << m_shapes.size() << "shape(s),"
                       << "scale(" << m_scaleX << "," << m_scaleY << ")";
}

ScaleCommand::~ScaleCommand()
{
    qCDebug(logCore()) << "ScaleCommand destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(ScaleCommand, IEditorCommand)

// ===== IEditorCommand interface =====

QString ScaleCommand::getText() const
{
    if (m_shapes.size() == 1) {
        return QString("Scale %1").arg(m_shapes.first()->getTypeName());
    } else {
        return QString("Scale %1 shapes").arg(m_shapes.size());
    }
}

void ScaleCommand::execute()
{
    if (m_executed) {
        qCWarning(logCore()) << "ScaleCommand: Already executed";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "ScaleCommand: No shapes to scale";
        return;
    }

    // Appliquer la mise à l'échelle à toutes les formes
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->scale(m_scaleX, m_scaleY, m_center);
        }
    }

    m_executed = true;

    qCInfo(logCore()) << "ScaleCommand: Scaled" << m_shapes.size() << "shape(s) by"
                      << "(" << m_scaleX << "," << m_scaleY << ")";
    emit executed();
}

void ScaleCommand::undo()
{
    if (!m_executed) {
        qCWarning(logCore()) << "ScaleCommand: Not executed, cannot undo";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "ScaleCommand: No shapes to undo";
        return;
    }

    // Restaurer les positions initiales
    // Note: Pour une vraie restauration, il faudrait aussi stocker les dimensions initiales
    // Pour simplifier, on applique l'inverse de la mise à l'échelle
    double inverseScaleX = 1.0 / m_scaleX;
    double inverseScaleY = 1.0 / m_scaleY;

    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->scale(inverseScaleX, inverseScaleY, m_center);

            // Corriger la position si nécessaire
            if (m_initialPositions.contains(shape)) {
                QPair<double, double> initialPos = m_initialPositions[shape];
                QRectF currentBbox = shape->getBoundingBox();

                double dx = initialPos.first - currentBbox.x();
                double dy = initialPos.second - currentBbox.y();

                shape->translate(dx, dy);
            }
        }
    }

    m_executed = false;

    qCInfo(logCore()) << "ScaleCommand: Undone scale of" << m_shapes.size() << "shape(s)";
    emit undone();
}

void ScaleCommand::redo()
{
    if (m_executed) {
        qCWarning(logCore()) << "ScaleCommand: Already executed, use undo() first";
        return;
    }

    if (m_shapes.isEmpty()) {
        qCWarning(logCore()) << "ScaleCommand: No shapes to redo";
        return;
    }

    // Réappliquer la mise à l'échelle
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->scale(m_scaleX, m_scaleY, m_center);
        }
    }

    m_executed = true;

    qCInfo(logCore()) << "ScaleCommand: Redone scale of" << m_shapes.size() << "shape(s)";
    emit redone();
}

bool ScaleCommand::isObsolete() const
{
    // La commande est obsolète si toutes les formes ont été détruites
    for (IShape* shape : m_shapes) {
        if (shape) {
            return false;  // Au moins une forme existe encore
        }
    }

    qCDebug(logCore()) << "ScaleCommand: Obsolete (all shapes destroyed)";
    return true;
}

bool ScaleCommand::canMerge() const
{
    // Permet la fusion de mises à l'échelle consécutives
    return true;
}

bool ScaleCommand::mergeWith(IEditorCommand* other)
{
    if (!other || other->getCommandId() != "ScaleShape") {
        return false;
    }

    ScaleCommand* otherScale = qobject_cast<ScaleCommand*>(other);
    if (!otherScale) {
        return false;
    }

    // Vérifier que ce sont les mêmes formes
    if (m_shapes.size() != otherScale->m_shapes.size()) {
        return false;
    }

    for (IShape* shape : m_shapes) {
        if (!otherScale->m_shapes.contains(shape)) {
            return false;
        }
    }

    // Vérifier que le centre est le même (à epsilon près)
    const double epsilon = 1e-6;
    if (std::abs(m_center.x - otherScale->m_center.x) > epsilon ||
        std::abs(m_center.y - otherScale->m_center.y) > epsilon) {
        return false;
    }

    // Fusionner : multiplier les facteurs d'échelle
    m_scaleX *= otherScale->m_scaleX;
    m_scaleY *= otherScale->m_scaleY;

    qCDebug(logCore()) << "ScaleCommand: Merged with scale, total factors:"
                       << "(" << m_scaleX << "," << m_scaleY << ")";

    return true;
}

QString ScaleCommand::getCommandId() const
{
    return "ScaleShape";
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
