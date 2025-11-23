/**
 * @file PanTool.cpp
 * @brief Implémentation de l'outil de navigation Pan
 */

#include "core/models/editor/tool/PanTool.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

// Auto-registration avec le Factory Pattern
namespace {
    Patterns::FactoryMixin<ITool>::AutoRegister<PanTool> g_panToolReg;
}

PanTool::PanTool(QObject* parent)
    : ITool(parent)
    , m_isActive(false)
    , m_isPanning(false)
    , m_lastPanPos(0, 0)
{
    // Enregistrement automatique dans la liste via ListManagerMixin
    registerInstance(this);

    qCInfo(logCore()) << "PanTool created";
}

PanTool::~PanTool()
{
    unregisterInstance(this);
    qCInfo(logCore()) << "PanTool destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(PanTool, ITool)

// ===== ITool interface =====

QString PanTool::getName() const
{
    return "Pan";
}

QString PanTool::getDescription() const
{
    return "Navigate by dragging the canvas";
}

QIcon PanTool::getIcon() const
{
    // TODO: Charger l'icône depuis les ressources
    return QIcon();
}

QCursor PanTool::getCursor() const
{
    return m_isPanning ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
}

void PanTool::setActive(bool active)
{
    if (m_isActive != active) {
        m_isActive = active;
        emit activeChanged(active);

        if (active) {
            qCInfo(logCore()) << "PanTool activated";
        } else {
            qCInfo(logCore()) << "PanTool deactivated";
        }
    }
}

void PanTool::activate()
{
    setActive(true);
    emit statusMessage("Click and drag to pan the view");
}

void PanTool::deactivate()
{
    // Arrêter le pan si en cours
    if (m_isPanning) {
        m_isPanning = false;
        emit panEnded();
    }

    setActive(false);
}

bool PanTool::handleMousePress(const Point2D& scenePos, Qt::MouseButton button)
{
    Q_UNUSED(scenePos)

    if (!m_isActive || button != Qt::LeftButton) {
        return false;
    }

    // PanTool just signals that panning should start
    // Canvas2DView will handle the actual panning in screen coordinates
    m_isPanning = true;

    emit panStarted();
    emit statusMessage("Panning...");

    qCDebug(logCore()) << "PanTool: Pan mode started";
    return true;  // Consume the event so Canvas knows we're active
}

bool PanTool::handleMouseMove(const Point2D& scenePos)
{
    Q_UNUSED(scenePos)

    if (!m_isActive || !m_isPanning) {
        return false;
    }

    // Canvas2DView handles the actual panning
    // This just indicates the tool is processing the event
    return true;
}

bool PanTool::handleMouseRelease(const Point2D& scenePos, Qt::MouseButton button)
{
    Q_UNUSED(scenePos)

    if (!m_isActive || button != Qt::LeftButton || !m_isPanning) {
        return false;
    }

    // Terminer le pan
    m_isPanning = false;

    emit panEnded();
    emit statusMessage("Pan completed");

    qCDebug(logCore()) << "PanTool: Pan mode ended";
    return true;
}

bool PanTool::handleKeyPress(int key, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)

    // Échap : annuler le pan en cours
    if (key == Qt::Key_Escape && m_isPanning) {
        m_isPanning = false;
        emit panEnded();
        emit statusMessage("Pan cancelled");

        qCDebug(logCore()) << "PanTool: Pan cancelled";
        return true;
    }

    return false;
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
