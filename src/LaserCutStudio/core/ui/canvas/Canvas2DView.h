/**
 * @file Canvas2DView.h
 * @brief Canvas de dessin 2D pour Qt Quick
 *
 * Canvas2DView est un QQuickPaintedItem qui intègre l'éditeur 2D
 * dans l'interface QML. Il gère le rendu des formes, les interactions
 * souris, le zoom/pan, et la grille.
 */

#ifndef CANVAS2DVIEW_H
#define CANVAS2DVIEW_H

#include <QQuickPaintedItem>
#include <QPainter>
#include <QTransform>
#include "core/services/editor/EditorService.h"
#include "core/models/base/types/Point2D.h"

namespace LaserCutStudio {
namespace Core {
namespace UI {

/**
 * @class Canvas2DView
 * @brief Canvas de dessin 2D intégré dans Qt Quick
 *
 * Cette classe fournit un canvas de dessin 2D utilisable dans QML.
 * Elle intègre l'EditorService pour gérer les outils, formes et sélection.
 *
 * ## Fonctionnalités
 * - Rendu des formes avec QPainter
 * - Gestion des outils (ShapeCreationTool, sélection)
 * - Zoom et pan (molette + clic milieu)
 * - Grille avec magnétisme optionnel
 * - Règles (rulers) horizontales et verticales
 *
 * ## Usage QML
 * @code
 * Canvas2DView {
 *     id: canvas
 *     anchors.fill: parent
 *     gridVisible: true
 *     gridSize: 10
 *     snapToGrid: true
 *     zoomLevel: 1.0
 *     onShapeCreated: console.log("Shape created")
 * }
 * @endcode
 */
class Canvas2DView : public QQuickPaintedItem
{
    Q_OBJECT

    // ===== Propriétés QML =====
    Q_PROPERTY(Services::EditorService* editorService READ getEditorService WRITE setEditorService NOTIFY editorServiceChanged)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(double gridSize READ getGridSize WRITE setGridSize NOTIFY gridSizeChanged)
    Q_PROPERTY(bool snapToGrid READ isSnapToGrid WRITE setSnapToGrid NOTIFY snapToGridChanged)
    Q_PROPERTY(double zoomLevel READ getZoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QPointF panOffset READ getPanOffset WRITE setPanOffset NOTIFY panOffsetChanged)
    Q_PROPERTY(bool rulersVisible READ areRulersVisible WRITE setRulersVisible NOTIFY rulersVisibleChanged)

public:
    explicit Canvas2DView(QQuickItem* parent = nullptr);
    ~Canvas2DView() override;

    // ===== QQuickPaintedItem interface =====
    void paint(QPainter* painter) override;

    // ===== Getters =====
    Services::EditorService* getEditorService() const { return m_editorService; }
    bool isGridVisible() const { return m_gridVisible; }
    double getGridSize() const { return m_gridSize; }
    bool isSnapToGrid() const { return m_snapToGrid; }
    double getZoomLevel() const { return m_zoomLevel; }
    QPointF getPanOffset() const { return m_panOffset; }
    bool areRulersVisible() const { return m_rulersVisible; }

    // ===== Setters =====
    void setEditorService(Services::EditorService* service);
    void setGridVisible(bool visible);
    void setGridSize(double size);
    void setSnapToGrid(bool snap);
    void setZoomLevel(double zoom);
    void setPanOffset(const QPointF& offset);
    void setRulersVisible(bool visible);

    // ===== Utilitaires publics =====

    /**
     * @brief Convertit coordonnées écran → coordonnées scène
     * @param screenPos Position en coordonnées widget
     * @return Position en coordonnées scène (avec zoom/pan)
     */
    Q_INVOKABLE Point2D screenToScene(const QPointF& screenPos) const;

    /**
     * @brief Convertit coordonnées scène → coordonnées écran
     * @param scenePos Position en coordonnées scène
     * @return Position en coordonnées widget
     */
    Q_INVOKABLE QPointF sceneToScreen(const Point2D& scenePos) const;

    /**
     * @brief Ajuste une position à la grille (si snapToGrid activé)
     * @param pos Position à ajuster
     * @return Position ajustée sur la grille
     */
    Q_INVOKABLE Point2D snapToGridIfEnabled(const Point2D& pos) const;

    /**
     * @brief Zoom sur un point spécifique
     * @param zoomFactor Facteur de zoom (ex: 1.2 pour +20%)
     * @param centerScreen Point de zoom en coordonnées écran
     */
    Q_INVOKABLE void zoomAt(double zoomFactor, const QPointF& centerScreen);

    /**
     * @brief Reset le zoom et le pan
     */
    Q_INVOKABLE void resetView();

signals:
    void editorServiceChanged(Services::EditorService* service);
    void gridVisibleChanged(bool visible);
    void gridSizeChanged(double size);
    void snapToGridChanged(bool snap);
    void zoomLevelChanged(double zoom);
    void panOffsetChanged(const QPointF& offset);
    void rulersVisibleChanged(bool visible);

    void shapeCreated(IShape* shape);
    void selectionChanged();

protected:
    // ===== Événements Qt =====
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    // ===== Fonctions de rendu =====
    void drawGrid(QPainter* painter);
    void drawRulers(QPainter* painter);
    void drawShapes(QPainter* painter);
    void drawSelection(QPainter* painter);
    void drawToolPreview(QPainter* painter);

    // ===== Helpers =====
    void connectEditorServiceSignals();
    void disconnectEditorServiceSignals();
    Point2D snapToGridInternal(const Point2D& pos) const;
    QTransform getViewTransform() const;

    // ===== Membres =====
    Services::EditorService* m_editorService = nullptr;

    // Grille
    bool m_gridVisible = true;
    double m_gridSize = 10.0;  // mm
    bool m_snapToGrid = false;

    // Zoom/Pan
    double m_zoomLevel = 1.0;
    QPointF m_panOffset = QPointF(0, 0);
    bool m_isPanning = false;
    QPointF m_lastPanPos;

    // Rulers
    bool m_rulersVisible = true;

    // État interaction
    bool m_isDrawing = false;
    Point2D m_lastMousePos;
};

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio

#endif // CANVAS2DVIEW_H
