/**
 * @file ZoomTool.h
 * @brief Outil de zoom par clic
 */

#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H

#include "core/models/editor/interfaces/ITool.h"
#include <QPointF>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class ZoomTool
 * @brief Outil permettant de zoomer par clic sur le canvas
 *
 * ZoomTool permet à l'utilisateur de zoomer sur un point spécifique :
 * - Clic gauche : Zoom avant (×1.25)
 * - Clic droit : Zoom arrière (×0.8)
 *
 * ## Utilisation
 * - Activer l'outil
 * - Cliquer sur un point pour zoomer à cet endroit
 * - Le zoom se centre sur le point cliqué
 *
 * ## Signaux
 * - `zoomRequested(double factor, QPointF center)` : Émis lors d'un clic
 */
class ZoomTool : public ITool
{
    Q_OBJECT

public:
    explicit ZoomTool(QObject* parent = nullptr);
    ~ZoomTool() override;

    // ===== Type name (Factory Pattern) =====
    DECLARE_TYPE_NAME(ZoomTool)

    // ===== Prototype Pattern =====
    ITool* clone() const override;

    // ===== ITool interface =====
    QString getName() const override;
    QString getDescription() const override;
    QIcon getIcon() const override;
    QCursor getCursor() const override;

    bool isActive() const override { return m_isActive; }
    void setActive(bool active) override;

    void activate() override;
    void deactivate() override;

    bool handleMousePress(const Point2D& scenePos, Qt::MouseButton button) override;
    bool handleMouseMove(const Point2D& scenePos) override;
    bool handleMouseRelease(const Point2D& scenePos, Qt::MouseButton button) override;
    bool handleKeyPress(int key, Qt::KeyboardModifiers modifiers) override;

    /**
     * @brief Facteur de zoom par défaut
     */
    double getZoomFactor() const { return m_zoomFactor; }
    void setZoomFactor(double factor) { m_zoomFactor = factor; }

signals:
    /**
     * @brief Émis quand l'utilisateur demande un zoom
     * @param factor Facteur de zoom (>1 = zoom in, <1 = zoom out)
     * @param center Point central du zoom (coordonnées écran)
     */
    void zoomRequested(double factor, QPointF center);

private:
    bool m_isActive;
    double m_zoomFactor;  // Facteur de zoom par défaut (1.25)
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // ZOOMTOOL_H
