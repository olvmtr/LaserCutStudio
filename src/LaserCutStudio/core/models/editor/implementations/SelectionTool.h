/**
 * @file SelectionTool.h
 * @brief Outil de sélection de formes au clic
 */

#ifndef SELECTIONTOOL_H
#define SELECTIONTOOL_H

#include "core/models/editor/interfaces/ITool.h"

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class SelectionTool
 * @brief Outil permettant de sélectionner des formes par clic
 *
 * SelectionTool permet de sélectionner une ou plusieurs formes :
 * - **Clic simple** : Sélectionne une forme (remplace la sélection)
 * - **Ctrl+Clic** : Ajoute/retire de la sélection (multi-sélection)
 * - **Clic sur fond** : Désélectionne tout
 *
 * ## Utilisation
 * 1. Activer l'outil
 * 2. Cliquer sur une forme pour la sélectionner
 * 3. Les formes sélectionnées sont affichées avec un rectangle bleu
 *
 * ## Architecture
 * - Utilise `IShape::containsPoint()` pour le hit testing
 * - Utilise `SelectionManager` pour gérer la sélection
 * - Envoie des signaux pour notification de changements
 */
class SelectionTool : public ITool
{
    Q_OBJECT

public:
    explicit SelectionTool(QObject* parent = nullptr);
    ~SelectionTool() override;

    // ===== Type name (Factory Pattern) =====
    DECLARE_TYPE_NAME(SelectionTool)

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

private:
    /**
     * @brief Trouve la forme sous le curseur
     * @param scenePos Position en coordonnées scène
     * @return Forme trouvée ou nullptr
     */
    IShape* findShapeAt(const Point2D& scenePos);

    bool m_isActive;
    bool m_potentialDrag;  // Souris pressée, peut devenir un drag si mouvement
    bool m_isDragging;  // Drag confirmé (mouvement > threshold)
    Point2D m_pressStartPos;  // Position initiale du clic
    Point2D m_dragStartPos;  // Position de début du drag (après threshold)
    Point2D m_lastDragPos;  // Dernière position du drag

    static constexpr double DRAG_THRESHOLD = 5.0;  // Threshold en pixels pour démarrer un drag
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // SELECTIONTOOL_H
