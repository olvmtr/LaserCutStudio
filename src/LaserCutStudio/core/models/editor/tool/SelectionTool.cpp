/**
 * @file SelectionTool.cpp
 * @brief Implémentation de l'outil de sélection
 */

#include "core/models/editor/tool/SelectionTool.h"
#include "core/services/editor/EditorService.h"
#include "core/models/editor/command/transformations/MoveCommand.h"
#include "core/models/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>
#include <QtMath>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

// Auto-registration avec le Factory Pattern
namespace {
    Patterns::FactoryMixin<ITool>::AutoRegister<SelectionTool> g_selectionToolReg;
}

SelectionTool::SelectionTool(QObject* parent)
    : ITool(parent)
    , m_isActive(false)
    , m_potentialDrag(false)
    , m_isDragging(false)
    , m_pressStartPos(0, 0)
    , m_dragStartPos(0, 0)
    , m_lastDragPos(0, 0)
{
    // Enregistrement automatique dans la liste via ListManagerMixin
    registerInstance(this);

    qCInfo(logCore()) << "SelectionTool created";
}

SelectionTool::~SelectionTool()
{
    unregisterInstance(this);
    qCInfo(logCore()) << "SelectionTool destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(SelectionTool, ITool)

// ===== ITool interface =====

QString SelectionTool::getName() const
{
    return "Selection";
}

QString SelectionTool::getDescription() const
{
    return "Select shapes by clicking (Ctrl to add/remove)";
}

QIcon SelectionTool::getIcon() const
{
    // TODO: Charger l'icône depuis les ressources
    return QIcon();
}

QCursor SelectionTool::getCursor() const
{
    return Qt::ArrowCursor;  // Curseur standard pour sélection
}

void SelectionTool::setActive(bool active)
{
    if (m_isActive != active) {
        m_isActive = active;
        emit activeChanged(active);

        if (active) {
            qCInfo(logCore()) << "SelectionTool activated";
        } else {
            qCInfo(logCore()) << "SelectionTool deactivated";
        }
    }
}

void SelectionTool::activate()
{
    setActive(true);
    emit statusMessage("Click to select shapes (Ctrl to add/remove)");
}

void SelectionTool::deactivate()
{
    setActive(false);
}

bool SelectionTool::handleMousePress(const Point2D& scenePos, Qt::MouseButton button)
{
    if (!m_isActive || button != Qt::LeftButton) {
        return false;
    }

    // Accéder à EditorService via parent
    auto* editorService = qobject_cast<Services::EditorService*>(parent());
    if (!editorService) {
        qCWarning(logCore()) << "SelectionTool: No EditorService parent";
        return false;
    }

    // Récupérer le SelectionManager
    Editor::ISelection* selection = editorService->getSelection();
    if (!selection) {
        qCWarning(logCore()) << "SelectionTool: No SelectionManager";
        return false;
    }

    // Trouver la forme sous le curseur
    IShape* clickedShape = findShapeAt(scenePos);

    if (clickedShape) {
        // Sélectionner la forme (remplace la sélection actuelle)
        // Note : Si la forme était déjà sélectionnée, on la garde sélectionnée
        if (!selection->isSelected(clickedShape)) {
            selection->clear();
            selection->addShape(clickedShape);
            qCInfo(logCore()) << "SelectionTool: Selected shape" << clickedShape->getTypeName();
        }

        // Démarrer un drag potentiel (sera confirmé si mouvement > threshold)
        m_potentialDrag = true;
        m_pressStartPos = scenePos;
        m_lastDragPos = scenePos;

        emit statusMessage(QString("Selected: %1 (drag to move)").arg(clickedShape->getTypeName()));
        qCDebug(logCore()) << "SelectionTool: Press at" << scenePos.x << "," << scenePos.y;
    } else {
        // Clic sur le fond : désélectionner tout
        selection->clear();
        m_potentialDrag = false;
        emit statusMessage("Selection cleared");
        qCDebug(logCore()) << "SelectionTool: Cleared selection";
    }

    return true;  // Événement consommé
}

bool SelectionTool::handleMouseMove(const Point2D& scenePos)
{
    if (!m_isActive) {
        return false;
    }

    // Si on a un drag potentiel mais pas encore confirmé
    if (m_potentialDrag && !m_isDragging) {
        // Calculer la distance depuis le clic initial
        double dx = scenePos.x - m_pressStartPos.x;
        double dy = scenePos.y - m_pressStartPos.y;
        double distance = qSqrt(dx * dx + dy * dy);

        // Si mouvement > threshold → démarrer vraiment le drag
        if (distance > DRAG_THRESHOLD) {
            m_isDragging = true;
            m_dragStartPos = m_pressStartPos;  // Le drag démarre depuis la position initiale du clic
            m_lastDragPos = m_pressStartPos;
            emit statusMessage("Dragging...");
            qCDebug(logCore()) << "SelectionTool: Drag started (moved" << distance << "px)";
        } else {
            // Mouvement trop petit, on attend
            return false;
        }
    }

    // Si drag confirmé, déplacer la sélection
    if (m_isDragging) {
        // Accéder à EditorService via parent
        auto* editorService = qobject_cast<Services::EditorService*>(parent());
        if (!editorService) {
            return false;
        }

        Editor::ISelection* selection = editorService->getSelection();
        if (!selection) {
            return false;
        }

        // Calculer le delta depuis la dernière position
        double dx = scenePos.x - m_lastDragPos.x;
        double dy = scenePos.y - m_lastDragPos.y;

        // Déplacer la sélection en temps réel (sans commande pour l'instant)
        selection->moveBy(dx, dy);

        // Mettre à jour la dernière position
        m_lastDragPos = scenePos;

        // Afficher les coordonnées dans la barre de statut
        double totalDx = scenePos.x - m_dragStartPos.x;
        double totalDy = scenePos.y - m_dragStartPos.y;
        emit statusMessage(QString("Moving: %1, %2")
                           .arg(totalDx, 0, 'f', 1)
                           .arg(totalDy, 0, 'f', 1));

        return true;
    }

    return false;
}

bool SelectionTool::handleMouseRelease(const Point2D& scenePos, Qt::MouseButton button)
{
    if (!m_isActive || button != Qt::LeftButton) {
        return false;
    }

    bool handled = false;

    // Si on était en train de déplacer, créer une MoveCommand
    if (m_isDragging) {
        auto* editorService = qobject_cast<Services::EditorService*>(parent());
        if (editorService && editorService->getSelection()) {
            // D'abord, appliquer le dernier déplacement incrémental depuis m_lastDragPos
            // (car il n'y a pas forcément eu de handleMouseMove() entre le dernier move et le release)
            double lastDx = scenePos.x - m_lastDragPos.x;
            double lastDy = scenePos.y - m_lastDragPos.y;

            if (qAbs(lastDx) > 0.01 || qAbs(lastDy) > 0.01) {
                editorService->getSelection()->moveBy(lastDx, lastDy);
            }

            // Maintenant calculer le déplacement total
            double totalDx = scenePos.x - m_dragStartPos.x;
            double totalDy = scenePos.y - m_dragStartPos.y;

            qCDebug(logCore()) << "SelectionTool: Release with total displacement"
                               << totalDx << "," << totalDy;

            // Seulement si mouvement significatif (> 1 pixel)
            if (qAbs(totalDx) > 1.0 || qAbs(totalDy) > 1.0) {
                // Récupérer les formes sélectionnées
                QVector<IShape*> selectedShapes = editorService->getSelection()->getSelectedShapes();

                if (!selectedShapes.isEmpty()) {
                    // Annuler le déplacement temps réel (sera refait par la commande)
                    editorService->getSelection()->moveBy(-totalDx, -totalDy);

                    // Créer la commande de déplacement
                    auto* moveCmd = new Editor::MoveCommand(selectedShapes, totalDx, totalDy);
                    editorService->pushCommand(moveCmd);

                    qCInfo(logCore()) << "SelectionTool: Created MoveCommand for"
                                      << selectedShapes.size() << "shapes with dx/dy ="
                                      << totalDx << "," << totalDy;
                }
            }
        }

        emit statusMessage("Move completed");
        handled = true;
    } else if (m_potentialDrag) {
        // C'était juste un clic de sélection (pas de mouvement suffisant)
        emit statusMessage("Shape selected");
        handled = true;
    }

    // Réinitialiser les états de drag
    m_potentialDrag = false;
    m_isDragging = false;

    return handled;
}

bool SelectionTool::handleKeyPress(int key, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)

    // Raccourcis clavier pour sélection
    if (m_isActive) {
        // Ctrl+A : Sélectionner tout
        if (key == Qt::Key_A && (modifiers & Qt::ControlModifier)) {
            auto* editorService = qobject_cast<Services::EditorService*>(parent());
            if (editorService && editorService->getSelection()) {
                editorService->getSelection()->selectAll(editorService->getShapes());
                emit statusMessage("Selected all shapes");
                qCDebug(logCore()) << "SelectionTool: Select All";
                return true;
            }
        }

        // Escape : Désélectionner tout
        if (key == Qt::Key_Escape) {
            auto* editorService = qobject_cast<Services::EditorService*>(parent());
            if (editorService && editorService->getSelection()) {
                editorService->getSelection()->clear();
                emit statusMessage("Selection cleared");
                qCDebug(logCore()) << "SelectionTool: Cleared via Escape";
                return true;
            }
        }
    }

    return false;
}

// ===== Helpers privés =====

IShape* SelectionTool::findShapeAt(const Point2D& scenePos)
{
    auto* editorService = qobject_cast<Services::EditorService*>(parent());
    if (!editorService) {
        return nullptr;
    }

    // Parcourir toutes les formes (du dessus vers le bas)
    const QVector<IShape*>& shapes = editorService->getShapes();

    // Parcourir en ordre inverse pour avoir les formes du dessus en premier
    for (int i = shapes.size() - 1; i >= 0; --i) {
        IShape* shape = shapes[i];
        if (shape && shape->containsPoint(scenePos)) {
            return shape;
        }
    }

    return nullptr;  // Aucune forme trouvée
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
