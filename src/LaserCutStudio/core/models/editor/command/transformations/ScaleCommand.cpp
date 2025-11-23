/**
 * @file ScaleCommand.cpp
 * @brief Implémentation de la commande de mise à l'échelle
 */

#include "core/models/editor/command/transformations/ScaleCommand.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

using namespace Transformations;

ScaleCommand::ScaleCommand(const QVector<IShape*>& shapes,
                         double scaleX,
                         double scaleY,
                         const Point2D& center,
                         QObject* parent)
    : ITransformationCommand(shapes, parent)
    , m_scaleX(scaleX)
    , m_scaleY(scaleY)
    , m_center(center)
{
    qCDebug(logCore()) << "ScaleCommand created:" << m_shapes.size() << "shape(s),"
                       << "scale(" << m_scaleX << "," << m_scaleY << ")";
}

ScaleCommand::~ScaleCommand()
{
    qCDebug(logCore()) << "ScaleCommand destroyed";
}

ScaleCommand::ScaleCommand(const QVariantMap& params, QObject* parent)
    : ScaleCommand(resolveShapesFromVariant(params),
                  params.value("scaleX").toDouble(),
                  params.value("scaleY").toDouble(),
                  Point2D(params.value("centerX").toDouble(),
                         params.value("centerY").toDouble()),
                  parent)
{
    // Le constructeur délègue au constructeur principal
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(ScaleCommand, IEditorCommand)

// ===== Helper privé pour résolution des shapes =====

QVector<IShape*> ScaleCommand::resolveShapesFromVariant(const QVariantMap& params)
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
        qCWarning(logCore()) << "ScaleCommand: No shapes found for IDs:" << shapeIds;
    }

    return shapes;
}

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
    if (!validateTransformState(false)) {
        return;
    }

    // Appliquer la mise à l'échelle à toutes les formes
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->scale(m_scaleX, m_scaleY, m_center);
        }
    }

    markExecuted();
}

void ScaleCommand::undo()
{
    if (!validateUndoState()) {
        return;
    }

    // Appliquer l'inverse de la mise à l'échelle
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

    markUndone();
}

void ScaleCommand::redo()
{
    if (!validateTransformState(true)) {
        return;
    }

    // Réappliquer la mise à l'échelle
    for (IShape* shape : m_shapes) {
        if (shape) {
            shape->scale(m_scaleX, m_scaleY, m_center);
        }
    }

    markExecuted();
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
    if (!shapesMatch(otherScale->m_shapes)) {
        return false;
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

// ===== Enregistrement automatique dans Factory =====

// Enregistre ScaleCommand dans le Factory de ITransformationCommand avec factory personnalisée
const bool ScaleCommand::s_registered = ITransformationCommand::registerCustomFactory(
    "ScaleCommand",
    [](const QVariantMap& params) -> ITransformationCommand* {
        return new ScaleCommand(params);
    }
);

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
