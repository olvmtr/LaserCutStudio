/**
 * @file Canvas2DView.cpp
 * @brief Implémentation du canvas de dessin 2D
 */

#include "core/ui/canvas/Canvas2DView.h"
#include "core/infrastructure/logging/LogCategories.h"
#include "core/infrastructure/config/ConfigManager.h"
#include "core/models/editor/interfaces/ITool.h"
#include "core/models/shapes/interfaces/IShape.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainterPath>
#include <QtMath>

namespace LaserCutStudio {
namespace Core {
namespace UI {

Canvas2DView::Canvas2DView(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setAntialiasing(true);

    // Charger les préférences depuis ConfigManager
    ConfigManager& config = ConfigManager::instance();
    config.load();  // S'assurer que les préférences sont chargées

    m_gridSize = config.getEditorGridSize();
    m_gridVisible = config.getEditorGridVisible();
    m_snapToGrid = config.getEditorSnapToGrid();
    m_zoomLevel = config.getEditorDefaultZoom();

    qCInfo(logCore()) << "Canvas2DView created (grid:" << m_gridSize
                      << "mm, visible:" << m_gridVisible
                      << ", snap:" << m_snapToGrid
                      << ", zoom:" << (m_zoomLevel * 100) << "%)";
}

Canvas2DView::~Canvas2DView()
{
    // Sauvegarder les préférences actuelles avant destruction
    ConfigManager& config = ConfigManager::instance();
    config.setEditorGridSize(m_gridSize);
    config.setEditorGridVisible(m_gridVisible);
    config.setEditorSnapToGrid(m_snapToGrid);
    config.setEditorDefaultZoom(m_zoomLevel);
    config.save();

    disconnectEditorServiceSignals();
    qCInfo(logCore()) << "Canvas2DView destroyed (preferences saved)";
}

// ===== QQuickPaintedItem interface =====

void Canvas2DView::paint(QPainter* painter)
{
    if (!painter) return;

    // Background blanc
    painter->fillRect(0, 0, width(), height(), Qt::white);

    // Appliquer transformation zoom/pan
    painter->save();
    QTransform transform = getViewTransform();
    painter->setTransform(transform);

    // Dessiner dans l'ordre : grille -> formes -> sélection -> preview
    if (m_gridVisible) {
        drawGrid(painter);
    }

    drawShapes(painter);
    drawSelection(painter);
    drawToolPreview(painter);

    painter->restore();

    // Dessiner rulers (sans transformation, en coordonnées widget)
    if (m_rulersVisible) {
        drawRulers(painter);
    }
}

// ===== Setters =====

void Canvas2DView::setEditorService(Services::EditorService* service)
{
    if (m_editorService == service) {
        return;
    }

    disconnectEditorServiceSignals();
    m_editorService = service;
    connectEditorServiceSignals();

    qCInfo(logCore()) << "Canvas2DView: EditorService set";
    emit editorServiceChanged(service);
    update();
}

void Canvas2DView::setGridVisible(bool visible)
{
    if (m_gridVisible == visible) return;
    m_gridVisible = visible;
    emit gridVisibleChanged(visible);
    update();
}

void Canvas2DView::setGridSize(double size)
{
    if (qFuzzyCompare(m_gridSize, size)) return;
    m_gridSize = size;
    emit gridSizeChanged(size);
    update();
}

void Canvas2DView::setSnapToGrid(bool snap)
{
    if (m_snapToGrid == snap) return;
    m_snapToGrid = snap;
    emit snapToGridChanged(snap);
}

void Canvas2DView::setZoomLevel(double zoom)
{
    double clampedZoom = qBound(0.1, zoom, 10.0);
    if (qFuzzyCompare(m_zoomLevel, clampedZoom)) return;
    m_zoomLevel = clampedZoom;
    emit zoomLevelChanged(clampedZoom);
    update();
}

void Canvas2DView::setPanOffset(const QPointF& offset)
{
    if (m_panOffset == offset) return;
    m_panOffset = offset;
    emit panOffsetChanged(offset);
    update();
}

void Canvas2DView::setRulersVisible(bool visible)
{
    if (m_rulersVisible == visible) return;
    m_rulersVisible = visible;
    emit rulersVisibleChanged(visible);
    update();
}

// ===== Utilitaires publics =====

Point2D Canvas2DView::screenToScene(const QPointF& screenPos) const
{
    QTransform transform = getViewTransform();
    QTransform inverted = transform.inverted();
    QPointF scenePos = inverted.map(screenPos);
    return Point2D(scenePos.x(), scenePos.y());
}

QPointF Canvas2DView::sceneToScreen(const Point2D& scenePos) const
{
    QTransform transform = getViewTransform();
    return transform.map(QPointF(scenePos.x, scenePos.y));
}

Point2D Canvas2DView::snapToGridIfEnabled(const Point2D& pos) const
{
    if (m_snapToGrid) {
        return snapToGridInternal(pos);
    }
    return pos;
}

void Canvas2DView::zoomAt(double zoomFactor, const QPointF& centerScreen)
{
    // Convertir le centre en coordonnées scène avant zoom
    Point2D centerScene = screenToScene(centerScreen);

    // Appliquer le nouveau zoom
    double newZoom = m_zoomLevel * zoomFactor;
    setZoomLevel(newZoom);

    // Ajuster le pan pour que le point reste au même endroit à l'écran
    QPointF newCenterScreen = sceneToScreen(centerScene);
    QPointF deltaPan = centerScreen - newCenterScreen;
    setPanOffset(m_panOffset + deltaPan);
}

void Canvas2DView::resetView()
{
    setZoomLevel(1.0);
    setPanOffset(QPointF(0, 0));
    qCInfo(logCore()) << "Canvas2DView: View reset";
}

// ===== Événements Qt =====

void Canvas2DView::mousePressEvent(QMouseEvent* event)
{
    if (!event) return;

    Point2D scenePos = screenToScene(event->position());
    scenePos = snapToGridIfEnabled(scenePos);

    m_lastMousePos = scenePos;

    // Clic milieu = pan
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        m_lastPanPos = event->position();
        event->accept();
        return;
    }

    // Déléguer à l'outil actif
    if (m_editorService && m_editorService->getActiveTool()) {
        bool handled = m_editorService->getActiveTool()->handleMousePress(scenePos, event->button());
        if (handled) {
            m_isDrawing = true;
            event->accept();
            update();
            return;
        }
    }

    event->ignore();
}

void Canvas2DView::mouseMoveEvent(QMouseEvent* event)
{
    if (!event) return;

    Point2D scenePos = screenToScene(event->position());
    scenePos = snapToGridIfEnabled(scenePos);

    // Pan avec clic milieu
    if (m_isPanning) {
        QPointF delta = event->position() - m_lastPanPos;
        setPanOffset(m_panOffset + delta);
        m_lastPanPos = event->position();
        event->accept();
        return;
    }

    // Déléguer à l'outil actif
    if (m_editorService && m_editorService->getActiveTool()) {
        bool handled = m_editorService->getActiveTool()->handleMouseMove(scenePos);
        if (handled) {
            m_lastMousePos = scenePos;
            event->accept();
            update();
            return;
        }
    }

    m_lastMousePos = scenePos;
    event->ignore();
}

void Canvas2DView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!event) return;

    Point2D scenePos = screenToScene(event->position());
    scenePos = snapToGridIfEnabled(scenePos);

    // Fin du pan
    if (event->button() == Qt::MiddleButton && m_isPanning) {
        m_isPanning = false;
        event->accept();
        return;
    }

    // Déléguer à l'outil actif
    if (m_editorService && m_editorService->getActiveTool()) {
        bool handled = m_editorService->getActiveTool()->handleMouseRelease(scenePos, event->button());
        if (handled) {
            m_isDrawing = false;
            event->accept();
            update();
            return;
        }
    }

    m_isDrawing = false;
    event->ignore();
}

void Canvas2DView::wheelEvent(QWheelEvent* event)
{
    if (!event) return;

    // Zoom avec molette (Ctrl+Molette pour zoom, sinon pan vertical)
    if (event->modifiers() & Qt::ControlModifier) {
        double zoomFactor = 1.0 + (event->angleDelta().y() / 1200.0);
        zoomAt(zoomFactor, event->position());
        event->accept();
    } else {
        // Pan vertical/horizontal
        QPointF delta(event->angleDelta().x() / 8.0, event->angleDelta().y() / 8.0);
        setPanOffset(m_panOffset + delta);
        event->accept();
    }
}

// ===== Fonctions de rendu =====

void Canvas2DView::drawGrid(QPainter* painter)
{
    if (!painter || m_gridSize <= 0) return;

    painter->save();

    QPen gridPen(QColor(220, 220, 220), 1.0 / m_zoomLevel);
    painter->setPen(gridPen);

    // Calculer la zone visible en coordonnées scène
    QRectF visibleRect = painter->transform().inverted().mapRect(QRectF(0, 0, width(), height()));

    double startX = qFloor(visibleRect.left() / m_gridSize) * m_gridSize;
    double startY = qFloor(visibleRect.top() / m_gridSize) * m_gridSize;

    // Dessiner lignes verticales
    for (double x = startX; x <= visibleRect.right(); x += m_gridSize) {
        painter->drawLine(QPointF(x, visibleRect.top()), QPointF(x, visibleRect.bottom()));
    }

    // Dessiner lignes horizontales
    for (double y = startY; y <= visibleRect.bottom(); y += m_gridSize) {
        painter->drawLine(QPointF(visibleRect.left(), y), QPointF(visibleRect.right(), y));
    }

    painter->restore();
}

void Canvas2DView::drawRulers(QPainter* painter)
{
    if (!painter) return;

    painter->save();

    const int rulerSize = 30;
    QColor rulerColor(240, 240, 240);
    QColor textColor(100, 100, 100);

    // Ruler horizontal (haut)
    painter->fillRect(0, 0, width(), rulerSize, rulerColor);
    painter->setPen(QPen(textColor, 1));
    painter->drawLine(0, rulerSize - 1, width(), rulerSize - 1);

    // Ruler vertical (gauche)
    painter->fillRect(0, 0, rulerSize, height(), rulerColor);
    painter->drawLine(rulerSize - 1, 0, rulerSize - 1, height());

    // TODO: Dessiner graduations et texte

    painter->restore();
}

void Canvas2DView::drawShapes(QPainter* painter)
{
    if (!painter || !m_editorService) return;

    painter->save();

    const QVector<IShape*>& shapes = m_editorService->getShapes();

    for (IShape* shape : shapes) {
        if (!shape) continue;

        QPen pen(Qt::black, 2.0 / m_zoomLevel);
        QBrush brush(QColor(200, 220, 255, 128));
        painter->setPen(pen);
        painter->setBrush(brush);

        // Dessiner selon le type de forme
        QString typeName = shape->getTypeName();

        if (typeName == "Rectangle") {
            QRectF bounds = shape->getBoundingBox();
            painter->drawRect(bounds);
        }
        else if (typeName == "Circle") {
            QRectF bounds = shape->getBoundingBox();
            painter->drawEllipse(bounds);
        }
        else if (typeName == "Triangle") {
            QList<Point2D> points = shape->getPoints();
            if (points.size() >= 3) {
                QPolygonF polygon;
                for (const Point2D& pt : points) {
                    polygon << QPointF(pt.x, pt.y);
                }
                painter->drawPolygon(polygon);
            }
        }
        else {
            // Type inconnu : dessiner le bounding box par défaut
            QRectF bounds = shape->getBoundingBox();
            painter->drawRect(bounds);
        }
    }

    painter->restore();
}

void Canvas2DView::drawSelection(QPainter* painter)
{
    if (!painter || !m_editorService) return;

    Editor::ISelection* selection = m_editorService->getSelection();
    if (!selection || selection->getCount() == 0) return;

    painter->save();

    // Dessiner le bounding rect de la sélection
    QRectF bounds = selection->getBoundingRect();
    QPen pen(Qt::blue, 2.0 / m_zoomLevel, Qt::DashLine);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(bounds);

    // Dessiner les poignées de redimensionnement
    const double handleSize = 8.0 / m_zoomLevel;
    QBrush handleBrush(Qt::blue);
    painter->setBrush(handleBrush);
    painter->setPen(Qt::NoPen);

    QPointF corners[4] = {
        bounds.topLeft(),
        bounds.topRight(),
        bounds.bottomLeft(),
        bounds.bottomRight()
    };

    for (const QPointF& corner : corners) {
        painter->drawRect(QRectF(corner.x() - handleSize / 2, corner.y() - handleSize / 2,
                                  handleSize, handleSize));
    }

    painter->restore();
}

void Canvas2DView::drawToolPreview(QPainter* painter)
{
    if (!painter || !m_editorService || !m_isDrawing) return;

    // L'outil actif peut dessiner sa prévisualisation
    // Pour l'instant, on ne fait rien (à implémenter dans ITool si besoin)
}

// ===== Helpers =====

void Canvas2DView::connectEditorServiceSignals()
{
    if (!m_editorService) return;

    // Redessiner quand une forme est ajoutée/supprimée
    connect(m_editorService, &Services::EditorService::shapeAdded,
            this, [this]() { update(); });

    connect(m_editorService, &Services::EditorService::shapeRemoved,
            this, [this]() { update(); });

    // Redessiner quand la sélection change
    connect(m_editorService, &Services::EditorService::selectionChanged,
            this, [this]() {
        emit selectionChanged();
        update();
    });

    // Redessiner quand l'outil actif change
    connect(m_editorService, &Services::EditorService::activeToolChanged,
            this, [this]() { update(); });

    qCDebug(logCore()) << "Canvas2DView: Connected to EditorService signals";
}

void Canvas2DView::disconnectEditorServiceSignals()
{
    if (!m_editorService) return;
    disconnect(m_editorService, nullptr, this, nullptr);
}

Point2D Canvas2DView::snapToGridInternal(const Point2D& pos) const
{
    if (m_gridSize <= 0) return pos;

    double snappedX = qRound(pos.x / m_gridSize) * m_gridSize;
    double snappedY = qRound(pos.y / m_gridSize) * m_gridSize;

    return Point2D(snappedX, snappedY);
}

QTransform Canvas2DView::getViewTransform() const
{
    QTransform transform;
    transform.translate(m_panOffset.x(), m_panOffset.y());
    transform.scale(m_zoomLevel, m_zoomLevel);
    return transform;
}

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio
