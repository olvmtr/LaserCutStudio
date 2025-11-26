/**
 * @file ConstraintCanvas2DView.h
 * @brief Canvas de dessin 2D pour géométrie contrainte
 *
 * Canvas spécialisé pour l'édition avec le système de géométrie contrainte.
 * Permet de placer des points, dessiner des segments, appliquer des contraintes,
 * et voir le solveur en action temps réel.
 */

#ifndef CONSTRAINTCANVAS2DVIEW_H
#define CONSTRAINTCANVAS2DVIEW_H

#include <QQuickPaintedItem>
#include <QPainter>
#include <QTransform>
#include "core/models/sketch/ConstraintSketch.h"
#include "core/models/geometry/IGeometricPoint.h"
#include "core/models/geometry/IGeometricSegment.h"
#include "core/models/constraints/IConstraint.h"

// Forward declarations
namespace LaserCutStudio {
namespace Core {
    class GeometricPoint;
    class GeometricSegment;
}
}

namespace LaserCutStudio {
namespace Core {
namespace UI {

/**
 * @class ConstraintCanvas2DView
 * @brief Canvas pour édition avec géométrie contrainte
 *
 * Cette classe fournit un canvas dédié au système de géométrie contrainte.
 *
 * ## Fonctionnalités
 * - Placement interactif de points (clic gauche)
 * - Dessin de segments entre points
 * - Visualisation des contraintes (icônes, couleurs)
 * - Application de contraintes via menu contextuel
 * - Résolution en temps réel avec ConstraintSolver
 * - Zoom, pan, grille magnétique
 * - Affichage des mesures (distances, angles)
 *
 * ## Usage QML
 * @code
 * ConstraintCanvas2DView {
 *     id: constraintCanvas
 *     anchors.fill: parent
 *     editMode: ConstraintCanvas2DView.PlacePoint
 *     gridVisible: true
 *     snapToGrid: true
 * }
 * @endcode
 */
class ConstraintCanvas2DView : public QQuickPaintedItem
{
    Q_OBJECT

public:
    /**
     * @brief Mode d'édition actif
     */
    enum class EditMode {
        Select,          ///< Sélection d'éléments
        PlacePoint,      ///< Placement de points
        DrawSegment,     ///< Dessin de segments
        AddConstraint    ///< Ajout de contraintes
    };
    Q_ENUM(EditMode)

    /**
     * @brief Type de contrainte à ajouter
     */
    enum class ConstraintType {
        None,
        Distance,
        Length,
        Angle,
        FixedPoint,
        Parallel,
        Perpendicular,
        EqualLength,
        Coincident
    };
    Q_ENUM(ConstraintType)

    // ===== Propriétés QML =====
    Q_PROPERTY(ConstraintSketch* sketch READ sketch WRITE setSketch NOTIFY sketchChanged)
    Q_PROPERTY(EditMode editMode READ editMode WRITE setEditMode NOTIFY editModeChanged)
    Q_PROPERTY(ConstraintType constraintType READ constraintType WRITE setConstraintType NOTIFY constraintTypeChanged)

    // Grille et affichage
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(double gridSize READ gridSize WRITE setGridSize NOTIFY gridSizeChanged)
    Q_PROPERTY(bool snapToGrid READ isSnapToGrid WRITE setSnapToGrid NOTIFY snapToGridChanged)

    // Zoom/Pan
    Q_PROPERTY(double zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QPointF panOffset READ panOffset WRITE setPanOffset NOTIFY panOffsetChanged)

    // Affichage contraintes
    Q_PROPERTY(bool showConstraints READ showConstraints WRITE setShowConstraints NOTIFY showConstraintsChanged)
    Q_PROPERTY(bool showMeasurements READ showMeasurements WRITE setShowMeasurements NOTIFY showMeasurementsChanged)
    Q_PROPERTY(bool autoSolve READ autoSolve WRITE setAutoSolve NOTIFY autoSolveChanged)

    explicit ConstraintCanvas2DView(QQuickItem* parent = nullptr);
    ~ConstraintCanvas2DView() override;

    // ===== QQuickPaintedItem interface =====
    void paint(QPainter* painter) override;

    // ===== Getters =====
    ConstraintSketch* sketch() const { return m_sketch; }
    EditMode editMode() const { return m_editMode; }
    ConstraintType constraintType() const { return m_constraintType; }

    bool isGridVisible() const { return m_gridVisible; }
    double gridSize() const { return m_gridSize; }
    bool isSnapToGrid() const { return m_snapToGrid; }

    double zoomLevel() const { return m_zoomLevel; }
    QPointF panOffset() const { return m_panOffset; }

    bool showConstraints() const { return m_showConstraints; }
    bool showMeasurements() const { return m_showMeasurements; }
    bool autoSolve() const { return m_autoSolve; }

    // ===== Setters =====
    void setSketch(ConstraintSketch* sketch);
    void setEditMode(EditMode mode);
    void setConstraintType(ConstraintType type);

    void setGridVisible(bool visible);
    void setGridSize(double size);
    void setSnapToGrid(bool snap);

    void setZoomLevel(double zoom);
    void setPanOffset(const QPointF& offset);

    void setShowConstraints(bool show);
    void setShowMeasurements(bool show);
    void setAutoSolve(bool autoSolve);

    // ===== Actions QML =====

    /**
     * @brief Résout les contraintes manuellement
     */
    Q_INVOKABLE void solve();

    /**
     * @brief Efface tous les éléments
     */
    Q_INVOKABLE void clear();

    /**
     * @brief Reset le zoom et le pan
     */
    Q_INVOKABLE void resetView();

    /**
     * @brief Convertit coordonnées écran → coordonnées scène
     */
    Q_INVOKABLE QPointF screenToScene(const QPointF& screenPos) const;

    /**
     * @brief Convertit coordonnées scène → coordonnées écran
     */
    Q_INVOKABLE QPointF sceneToScreen(const QPointF& scenePos) const;

    // ===== Undo/Redo =====

    /**
     * @brief Annule la dernière action
     */
    Q_INVOKABLE void undo();

    /**
     * @brief Refait la dernière action annulée
     */
    Q_INVOKABLE void redo();

    /**
     * @brief Vérifie si un undo est possible
     */
    Q_INVOKABLE bool canUndo() const;

    /**
     * @brief Vérifie si un redo est possible
     */
    Q_INVOKABLE bool canRedo() const;

    /**
     * @brief Efface l'historique Undo/Redo
     */
    Q_INVOKABLE void clearHistory();

signals:
    void sketchChanged(ConstraintSketch* sketch);
    void editModeChanged(EditMode mode);
    void constraintTypeChanged(ConstraintType type);

    void gridVisibleChanged(bool visible);
    void gridSizeChanged(double size);
    void snapToGridChanged(bool snap);

    void zoomLevelChanged(double zoom);
    void panOffsetChanged(const QPointF& offset);

    void showConstraintsChanged(bool show);
    void showMeasurementsChanged(bool show);
    void autoSolveChanged(bool autoSolve);

    // Notifications d'événements
    void pointCreated(IGeometricPoint* point);
    void segmentCreated(IGeometricSegment* segment);
    void constraintCreated(IConstraint* constraint);
    void elementSelected(IGeometricElement* element);

protected:
    // ===== Événements Qt =====
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    // ===== Fonctions de rendu =====
    void drawGrid(QPainter* painter);
    void drawPoints(QPainter* painter);
    void drawSegments(QPainter* painter);
    void drawConstraints(QPainter* painter);
    void drawMeasurements(QPainter* painter);
    void drawSelection(QPainter* painter);
    void drawPreview(QPainter* painter);

    // ===== Helpers de rendu =====
    void drawPoint(QPainter* painter, IGeometricPoint* point);
    void drawSegment(QPainter* painter, IGeometricSegment* segment);
    void drawConstraintIcon(QPainter* painter, IConstraint* constraint);
    void drawDistanceMeasurement(QPainter* painter, const QPointF& p1, const QPointF& p2, double distance);
    void drawAngleMeasurement(QPainter* painter, const QPointF& p1, const QPointF& p2, const QPointF& p3, double angle);

    // ===== Helpers d'interaction =====
    IGeometricPoint* findPointNear(const QPointF& scenePos, double tolerance = 10.0);
    IGeometricSegment* findSegmentNear(const QPointF& scenePos, double tolerance = 10.0);
    void handlePointPlacement(const QPointF& scenePos);
    void handleSegmentDrawing(const QPointF& scenePos);
    void handleSelection(const QPointF& scenePos);
    void handleConstraintAddition();

    // ===== Utilitaires =====
    QPointF snapToGridInternal(const QPointF& pos) const;
    QTransform getViewTransform() const;
    void triggerSolveIfEnabled();

    // ===== Membres =====
    ConstraintSketch* m_sketch = nullptr;
    EditMode m_editMode = EditMode::DrawSegment;
    ConstraintType m_constraintType = ConstraintType::None;

    // Grille
    bool m_gridVisible = true;
    double m_gridSize = 10.0;  // mm
    bool m_snapToGrid = true;

    // Zoom/Pan
    double m_zoomLevel = 1.0;
    QPointF m_panOffset = QPointF(0, 0);
    bool m_isPanning = false;
    QPointF m_lastPanPos;

    // Affichage
    bool m_showConstraints = true;
    bool m_showMeasurements = true;
    bool m_autoSolve = true;

    // État d'interaction
    IGeometricPoint* m_segmentStartPoint = nullptr;  // Pour dessin de segment en cours
    IGeometricPoint* m_hoveredPoint = nullptr;
    IGeometricSegment* m_hoveredSegment = nullptr;
    IGeometricElement* m_selectedElement = nullptr;

    // Sélection pour contraintes (peut nécessiter 2 éléments)
    QList<IGeometricElement*> m_constraintSelection;

    // ===== Undo/Redo =====

    /**
     * @brief Structure de commande simple pour Undo/Redo
     */
    struct CanvasCommand {
        QString description;                      ///< Description de la commande
        std::function<void()> executeFunc;        ///< Fonction à appeler pour faire
        std::function<void()> undoFunc;           ///< Fonction à appeler pour annuler

        CanvasCommand(const QString& desc,
                     std::function<void()> exec,
                     std::function<void()> undo)
            : description(desc), executeFunc(exec), undoFunc(undo) {}
    };

    QVector<CanvasCommand*> m_undoStack;  ///< Pile d'annulation
    QVector<CanvasCommand*> m_redoStack;  ///< Pile de rétablissement

    /**
     * @brief Ajoute une commande à l'historique
     * @param description Description de l'action
     * @param executeFunc Fonction pour exécuter l'action
     * @param undoFunc Fonction pour annuler l'action
     */
    void pushCommand(const QString& description,
                    std::function<void()> executeFunc,
                    std::function<void()> undoFunc);
};

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio

#endif // CONSTRAINTCANVAS2DVIEW_H
