/**
 * @file ZoomTool.cpp
 * @brief Implémentation de l'outil de zoom par clic
 */

#include "core/models/editor/tool/ZoomTool.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

// Auto-registration avec le Factory Pattern
namespace {
    Patterns::FactoryMixin<ITool>::AutoRegister<ZoomTool> g_zoomToolReg;
}

ZoomTool::ZoomTool(QObject* parent)
    : ITool(parent)
    , m_isActive(false)
    , m_zoomFactor(1.25)  // Zoom in par défaut de 25%
{
    // Enregistrement automatique dans la liste via ListManagerMixin
    registerInstance(this);

    qCInfo(logCore()) << "ZoomTool created";
}

ZoomTool::~ZoomTool()
{
    unregisterInstance(this);
    qCInfo(logCore()) << "ZoomTool destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(ZoomTool, ITool)

// ===== ITool interface =====

QString ZoomTool::getName() const
{
    return "Zoom";
}

QString ZoomTool::getDescription() const
{
    return "Zoom in/out by clicking on the canvas";
}

QIcon ZoomTool::getIcon() const
{
    // TODO: Charger l'icône depuis les ressources
    return QIcon();
}

QCursor ZoomTool::getCursor() const
{
    // Curseur loupe avec "+"
    return Qt::CrossCursor;  // TODO: Créer curseur personnalisé avec loupe
}

void ZoomTool::setActive(bool active)
{
    if (m_isActive != active) {
        m_isActive = active;
        emit activeChanged(active);

        if (active) {
            qCInfo(logCore()) << "ZoomTool activated";
        } else {
            qCInfo(logCore()) << "ZoomTool deactivated";
        }
    }
}

void ZoomTool::activate()
{
    setActive(true);
    emit statusMessage("Left-click to zoom in, Right-click to zoom out");
}

void ZoomTool::deactivate()
{
    setActive(false);
}

bool ZoomTool::handleMousePress(const Point2D& scenePos, Qt::MouseButton button)
{
    Q_UNUSED(scenePos)

    if (!m_isActive) {
        return false;
    }

    // Zoom in avec clic gauche, zoom out avec clic droit
    // Note: On émet juste le signal, Canvas2DView gérera le zoom réel
    if (button == Qt::LeftButton) {
        emit statusMessage(QString("Zooming in (%1%)").arg(m_zoomFactor * 100));
        qCDebug(logCore()) << "ZoomTool: Zoom in requested";
        return true;  // Consommer l'événement
    } else if (button == Qt::RightButton) {
        emit statusMessage(QString("Zooming out (%1%)").arg((1.0 / m_zoomFactor) * 100));
        qCDebug(logCore()) << "ZoomTool: Zoom out requested";
        return true;  // Consommer l'événement
    }

    return false;
}

bool ZoomTool::handleMouseMove(const Point2D& scenePos)
{
    Q_UNUSED(scenePos)

    // ZoomTool ne fait rien lors du mouvement de souris
    return false;
}

bool ZoomTool::handleMouseRelease(const Point2D& scenePos, Qt::MouseButton button)
{
    Q_UNUSED(scenePos)
    Q_UNUSED(button)

    // Le zoom est instantané au clic, rien à faire au release
    return false;
}

bool ZoomTool::handleKeyPress(int key, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)

    // Optionnel : + et - pour zoomer sans souris
    if (m_isActive) {
        if (key == Qt::Key_Plus || key == Qt::Key_Equal) {
            emit statusMessage("Zooming in (keyboard)");
            qCDebug(logCore()) << "ZoomTool: Zoom in via keyboard";
            return true;
        } else if (key == Qt::Key_Minus) {
            emit statusMessage("Zooming out (keyboard)");
            qCDebug(logCore()) << "ZoomTool: Zoom out via keyboard";
            return true;
        }
    }

    return false;
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
