/**
 * @file PanTool.h
 * @brief Outil de navigation pour déplacer la vue (pan)
 */

#ifndef PANTOOL_H
#define PANTOOL_H

#include "core/models/editor/interfaces/ITool.h"
#include <QPointF>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class PanTool
 * @brief Outil permettant de déplacer la vue du canvas par drag
 *
 * PanTool permet à l'utilisateur de naviguer dans le canvas en cliquant
 * et en déplaçant la souris. Il affiche un curseur en forme de main.
 *
 * ## Utilisation
 * - Activer l'outil
 * - Cliquer et déplacer pour panner
 * - Relâcher pour terminer
 *
 * ## Signaux
 * - `panDelta(QPointF)` : Émis lors du déplacement (delta en pixels écran)
 * - `panStarted()` : Émis au début du pan
 * - `panEnded()` : Émis à la fin du pan
 */
class PanTool : public ITool
{
    Q_OBJECT

public:
    explicit PanTool(QObject* parent = nullptr);
    ~PanTool() override;

    // ===== Type name (Factory Pattern) =====
    DECLARE_TYPE_NAME(PanTool)

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

signals:
    /**
     * @brief Émis quand l'utilisateur déplace la vue
     * @param delta Déplacement en pixels écran (coordonnées widget)
     */
    void panDelta(QPointF delta);

    /**
     * @brief Émis au début du pan
     */
    void panStarted();

    /**
     * @brief Émis à la fin du pan
     */
    void panEnded();

private:
    bool m_isActive;
    bool m_isPanning;
    Point2D m_lastPanPos;  // Position de départ du pan
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // PANTOOL_H
