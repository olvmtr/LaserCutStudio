/**
 * @file ConstraintCanvas2DView.cpp
 * @brief Implémentation du canvas de géométrie contrainte
 */

#include "ConstraintCanvas2DView.h"
#include "core/infrastructure/logging/LogCategories.h"
#include "core/models/constraints/DistanceConstraint.h"
#include "core/models/constraints/LengthConstraint.h"
#include "core/models/constraints/AngleConstraint.h"
#include "core/models/constraints/FixedPointConstraint.h"
#include "core/models/constraints/ParallelConstraint.h"
#include "core/models/constraints/PerpendicularConstraint.h"
#include "core/models/constraints/EqualLengthConstraint.h"
#include "core/models/constraints/CoincidentConstraint.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainterPath>
#include <QtMath>
#include <memory>
#include <functional>

namespace LaserCutStudio {
namespace Core {
namespace UI {

ConstraintCanvas2DView::ConstraintCanvas2DView(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setAntialiasing(true);

    qCInfo(logCore()) << "ConstraintCanvas2DView created";
}

ConstraintCanvas2DView::~ConstraintCanvas2DView()
{
    qCInfo(logCore()) << "ConstraintCanvas2DView destroyed";
}

// ===== QQuickPaintedItem interface =====

void ConstraintCanvas2DView::paint(QPainter* painter)
{
    if (!painter) return;

    // Background blanc
    painter->fillRect(0, 0, width(), height(), Qt::white);

    // Appliquer transformation zoom/pan
    painter->save();
    QTransform transform = getViewTransform();
    painter->setTransform(transform);

    // Dessiner dans l'ordre : grille -> segments -> points -> contraintes -> mesures -> sélection -> preview
    if (m_gridVisible) {
        drawGrid(painter);
    }

    if (m_sketch) {
        drawSegments(painter);
        drawPoints(painter);

        if (m_showConstraints) {
            drawConstraints(painter);
        }

        if (m_showMeasurements) {
            drawMeasurements(painter);
        }
    }

    drawSelection(painter);
    drawPreview(painter);

    painter->restore();
}

// ===== Setters =====

void ConstraintCanvas2DView::setSketch(ConstraintSketch* sketch)
{
    if (m_sketch == sketch) return;
    m_sketch = sketch;
    emit sketchChanged(sketch);
    update();
}

void ConstraintCanvas2DView::setEditMode(EditMode mode)
{
    if (m_editMode == mode) return;
    m_editMode = mode;

    // Reset état d'interaction
    m_segmentStartPoint = nullptr;
    m_constraintSelection.clear();

    emit editModeChanged(mode);
    update();
}

void ConstraintCanvas2DView::setConstraintType(ConstraintType type)
{
    if (m_constraintType == type) return;
    m_constraintType = type;
    emit constraintTypeChanged(type);
}

void ConstraintCanvas2DView::setGridVisible(bool visible)
{
    if (m_gridVisible == visible) return;
    m_gridVisible = visible;
    emit gridVisibleChanged(visible);
    update();
}

void ConstraintCanvas2DView::setGridSize(double size)
{
    if (qFuzzyCompare(m_gridSize, size)) return;
    m_gridSize = size;
    emit gridSizeChanged(size);
    update();
}

void ConstraintCanvas2DView::setSnapToGrid(bool snap)
{
    if (m_snapToGrid == snap) return;
    m_snapToGrid = snap;
    emit snapToGridChanged(snap);
}

void ConstraintCanvas2DView::setZoomLevel(double zoom)
{
    double clampedZoom = qBound(0.1, zoom, 10.0);
    if (qFuzzyCompare(m_zoomLevel, clampedZoom)) return;
    m_zoomLevel = clampedZoom;
    emit zoomLevelChanged(clampedZoom);
    update();
}

void ConstraintCanvas2DView::setPanOffset(const QPointF& offset)
{
    if (m_panOffset == offset) return;
    m_panOffset = offset;
    emit panOffsetChanged(offset);
    update();
}

void ConstraintCanvas2DView::setShowConstraints(bool show)
{
    if (m_showConstraints == show) return;
    m_showConstraints = show;
    emit showConstraintsChanged(show);
    update();
}

void ConstraintCanvas2DView::setShowMeasurements(bool show)
{
    if (m_showMeasurements == show) return;
    m_showMeasurements = show;
    emit showMeasurementsChanged(show);
    update();
}

void ConstraintCanvas2DView::setAutoSolve(bool autoSolve)
{
    if (m_autoSolve == autoSolve) return;
    m_autoSolve = autoSolve;
    emit autoSolveChanged(autoSolve);
}

// ===== Actions QML =====

void ConstraintCanvas2DView::solve()
{
    if (!m_sketch) return;

    bool success = m_sketch->solve();
    qCInfo(logCore()) << "Manual solve:" << (success ? "converged" : "failed");
    update();
}

void ConstraintCanvas2DView::clear()
{
    m_segmentStartPoint = nullptr;
    m_hoveredPoint = nullptr;
    m_hoveredSegment = nullptr;
    m_selectedElement = nullptr;
    m_constraintSelection.clear();
    update();
}

void ConstraintCanvas2DView::resetView()
{
    setZoomLevel(1.0);
    setPanOffset(QPointF(0, 0));
}

QPointF ConstraintCanvas2DView::screenToScene(const QPointF& screenPos) const
{
    QTransform transform = getViewTransform();
    QTransform inverted = transform.inverted();
    return inverted.map(screenPos);
}

QPointF ConstraintCanvas2DView::sceneToScreen(const QPointF& scenePos) const
{
    QTransform transform = getViewTransform();
    return transform.map(scenePos);
}

// ===== Événements Qt =====

void ConstraintCanvas2DView::mousePressEvent(QMouseEvent* event)
{
    if (!event) return;

    QPointF screenPos = event->position();
    QPointF scenePos = screenToScene(screenPos);

    // Bouton milieu : Pan
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        m_lastPanPos = screenPos;
        return;
    }

    // Bouton gauche : Interaction selon mode
    if (event->button() == Qt::LeftButton) {
        switch (m_editMode) {
        case EditMode::PlacePoint:
            handlePointPlacement(scenePos);
            break;
        case EditMode::DrawSegment:
            handleSegmentDrawing(scenePos);
            break;
        case EditMode::Select:
            handleSelection(scenePos);
            break;
        case EditMode::AddConstraint:
            handleSelection(scenePos);  // Sélectionner éléments pour contrainte
            break;
        }
    }

    update();
}

void ConstraintCanvas2DView::mouseMoveEvent(QMouseEvent* event)
{
    if (!event) return;

    QPointF screenPos = event->position();
    QPointF scenePos = screenToScene(screenPos);

    // Pan en cours
    if (m_isPanning) {
        QPointF delta = screenPos - m_lastPanPos;
        setPanOffset(m_panOffset + delta);
        m_lastPanPos = screenPos;
        return;
    }

    // Mise à jour hover
    m_hoveredPoint = findPointNear(scenePos);
    m_hoveredSegment = findSegmentNear(scenePos);

    update();
}

void ConstraintCanvas2DView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!event) return;

    if (event->button() == Qt::MiddleButton) {
        m_isPanning = false;
    }
}

void ConstraintCanvas2DView::wheelEvent(QWheelEvent* event)
{
    if (!event) return;

    // Zoom avec molette
    double delta = event->angleDelta().y();
    double zoomFactor = (delta > 0) ? 1.1 : 0.9;

    QPointF screenCenter = event->position();
    QPointF sceneCenter = screenToScene(screenCenter);

    // Zoom
    double newZoom = m_zoomLevel * zoomFactor;
    setZoomLevel(newZoom);

    // Ajuster pan pour zoomer sur la position de la souris
    QPointF newSceneCenter = screenToScene(screenCenter);
    QPointF sceneDelta = sceneCenter - newSceneCenter;
    setPanOffset(m_panOffset + sceneDelta * m_zoomLevel);
}

// ===== Fonctions de rendu =====

void ConstraintCanvas2DView::drawGrid(QPainter* painter)
{
    if (!painter || m_gridSize <= 0) return;

    painter->save();
    painter->setPen(QPen(QColor(220, 220, 220), 0));  // 0 = cosmetic (pas de scaling)

    // Calculer zone visible en coordonnées scène
    QPointF topLeft = screenToScene(QPointF(0, 0));
    QPointF bottomRight = screenToScene(QPointF(width(), height()));
    QRectF visibleRect(topLeft, bottomRight);

    // Dessiner lignes verticales
    double startX = qFloor(visibleRect.left() / m_gridSize) * m_gridSize;
    for (double x = startX; x <= visibleRect.right(); x += m_gridSize) {
        painter->drawLine(QPointF(x, visibleRect.top()), QPointF(x, visibleRect.bottom()));
    }

    // Dessiner lignes horizontales
    double startY = qFloor(visibleRect.top() / m_gridSize) * m_gridSize;
    for (double y = startY; y <= visibleRect.bottom(); y += m_gridSize) {
        painter->drawLine(QPointF(visibleRect.left(), y), QPointF(visibleRect.right(), y));
    }

    painter->restore();
}

void ConstraintCanvas2DView::drawPoints(QPainter* painter)
{
    if (!painter || !m_sketch) return;

    const QList<IGeometricElement*>& elements = m_sketch->elements();
    for (IGeometricElement* elem : elements) {
        GeometricPoint* point = qobject_cast<GeometricPoint*>(elem);
        if (point) {
            drawPoint(painter, point);
        }
    }
}

void ConstraintCanvas2DView::drawSegments(QPainter* painter)
{
    if (!painter || !m_sketch) return;

    const QList<IGeometricElement*>& elements = m_sketch->elements();
    for (IGeometricElement* elem : elements) {
        GeometricSegment* segment = qobject_cast<GeometricSegment*>(elem);
        if (segment) {
            drawSegment(painter, segment);
        }
    }
}

void ConstraintCanvas2DView::drawConstraints(QPainter* painter)
{
    if (!painter || !m_sketch) return;

    const QList<IConstraint*>& constraints = m_sketch->constraints();
    for (IConstraint* constraint : constraints) {
        drawConstraintIcon(painter, constraint);
    }
}

void ConstraintCanvas2DView::drawMeasurements(QPainter* painter)
{
    if (!painter || !m_sketch) return;

    // TODO: Implémenter affichage des mesures
    // Pour l'instant, on affiche les longueurs des segments
    const QList<IGeometricElement*>& elements = m_sketch->elements();
    for (IGeometricElement* elem : elements) {
        GeometricSegment* segment = qobject_cast<GeometricSegment*>(elem);
        if (segment && segment->isValid()) {
            QPointF p1 = sceneToScreen(QPointF(segment->startPoint()->x(), segment->startPoint()->y()));
            QPointF p2 = sceneToScreen(QPointF(segment->endPoint()->x(), segment->endPoint()->y()));
            double length = segment->length();
            drawDistanceMeasurement(painter, p1, p2, length);
        }
    }
}

void ConstraintCanvas2DView::drawSelection(QPainter* painter)
{
    if (!painter || !m_selectedElement) return;

    painter->save();
    painter->setPen(QPen(QColor(0, 120, 215), 2.0 / m_zoomLevel));  // Bleu sélection

    // Dessiner contour autour élément sélectionné
    QRectF bbox = m_selectedElement->getBoundingBox();
    painter->drawRect(bbox.adjusted(-2, -2, 2, 2));

    painter->restore();
}

void ConstraintCanvas2DView::drawPreview(QPainter* painter)
{
    if (!painter) return;

    // Preview segment en cours de dessin
    if (m_editMode == EditMode::DrawSegment && m_segmentStartPoint && m_hoveredPoint) {
        painter->save();
        painter->setPen(QPen(QColor(100, 100, 100), 1.0 / m_zoomLevel, Qt::DashLine));

        QPointF p1(m_segmentStartPoint->x(), m_segmentStartPoint->y());
        QPointF p2(m_hoveredPoint->x(), m_hoveredPoint->y());
        painter->drawLine(p1, p2);

        painter->restore();
    }
}

// ===== Helpers de rendu =====

void ConstraintCanvas2DView::drawPoint(QPainter* painter, GeometricPoint* point)
{
    if (!painter || !point) return;

    painter->save();

    // Couleur selon état
    QColor color = point->isLocked() ? QColor(200, 50, 50) : QColor(50, 50, 200);
    if (point == m_hoveredPoint) {
        color = QColor(0, 200, 0);  // Vert si hover
    }

    // Dessiner cercle (taille fixe en pixels)
    double radius = 4.0 / m_zoomLevel;
    painter->setPen(QPen(color.darker(), 1.0 / m_zoomLevel));
    painter->setBrush(color);
    painter->drawEllipse(QPointF(point->x(), point->y()), radius, radius);

    painter->restore();
}

void ConstraintCanvas2DView::drawSegment(QPainter* painter, GeometricSegment* segment)
{
    if (!painter || !segment || !segment->isValid()) return;

    painter->save();

    // Couleur selon état
    QColor color = QColor(50, 50, 50);
    if (segment == m_hoveredSegment) {
        color = QColor(0, 200, 0);  // Vert si hover
    }

    painter->setPen(QPen(color, 1.5 / m_zoomLevel));

    GeometricPoint* p1 = segment->startPoint();
    GeometricPoint* p2 = segment->endPoint();
    painter->drawLine(QPointF(p1->x(), p1->y()), QPointF(p2->x(), p2->y()));

    painter->restore();
}

void ConstraintCanvas2DView::drawConstraintIcon(QPainter* painter, IConstraint* constraint)
{
    if (!painter || !constraint) return;

    painter->save();

    // Couleur selon état verrouillé
    QColor color = constraint->isLocked() ? QColor(200, 50, 50) : QColor(100, 100, 200);
    painter->setPen(QPen(color, 1.0 / m_zoomLevel));

    // TODO: Dessiner icône spécifique selon type de contrainte
    // Pour l'instant, dessiner un petit carré au centre des éléments affectés
    QList<GeometricPoint*> points = constraint->affectedPoints();
    if (!points.isEmpty()) {
        QPointF center(0, 0);
        for (GeometricPoint* p : points) {
            center += QPointF(p->x(), p->y());
        }
        center /= points.size();

        double size = 3.0 / m_zoomLevel;
        painter->drawRect(QRectF(center.x() - size, center.y() - size, size * 2, size * 2));
    }

    painter->restore();
}

void ConstraintCanvas2DView::drawDistanceMeasurement(QPainter* painter, const QPointF& p1, const QPointF& p2, double distance)
{
    if (!painter) return;

    painter->save();
    painter->setPen(QPen(QColor(100, 100, 100), 1));
    painter->setFont(QFont("Arial", 10));

    // Position du texte au milieu
    QPointF midpoint = (p1 + p2) / 2.0;

    // Afficher distance avec l'unité du sketch
    QString text;
    if (m_sketch) {
        text = m_sketch->formatValue(distance, m_sketch->unit());
    } else {
        text = QString::number(distance, 'f', 1) + " mm";
    }
    painter->drawText(midpoint + QPointF(5, -5), text);

    painter->restore();
}

void ConstraintCanvas2DView::drawAngleMeasurement(QPainter* painter, const QPointF& p1, const QPointF& p2, const QPointF& p3, double angle)
{
    // TODO: Implémenter affichage angle
    Q_UNUSED(painter);
    Q_UNUSED(p1);
    Q_UNUSED(p2);
    Q_UNUSED(p3);
    Q_UNUSED(angle);
}

// ===== Helpers d'interaction =====

GeometricPoint* ConstraintCanvas2DView::findPointNear(const QPointF& scenePos, double tolerance)
{
    if (!m_sketch) return nullptr;

    double minDist = tolerance / m_zoomLevel;
    GeometricPoint* nearest = nullptr;

    const QList<IGeometricElement*>& elements = m_sketch->elements();
    for (IGeometricElement* elem : elements) {
        GeometricPoint* point = qobject_cast<GeometricPoint*>(elem);
        if (point) {
            double dx = point->x() - scenePos.x();
            double dy = point->y() - scenePos.y();
            double dist = qSqrt(dx * dx + dy * dy);

            if (dist < minDist) {
                minDist = dist;
                nearest = point;
            }
        }
    }

    return nearest;
}

GeometricSegment* ConstraintCanvas2DView::findSegmentNear(const QPointF& scenePos, double tolerance)
{
    if (!m_sketch) return nullptr;

    double minDist = tolerance / m_zoomLevel;
    GeometricSegment* nearest = nullptr;

    const QList<IGeometricElement*>& elements = m_sketch->elements();
    for (IGeometricElement* elem : elements) {
        GeometricSegment* segment = qobject_cast<GeometricSegment*>(elem);
        if (segment && segment->isValid()) {
            Point2D p(scenePos.x(), scenePos.y());
            double dist = segment->distanceToPoint(p);

            if (dist < minDist) {
                minDist = dist;
                nearest = segment;
            }
        }
    }

    return nearest;
}

void ConstraintCanvas2DView::handlePointPlacement(const QPointF& scenePos)
{
    if (!m_sketch) return;

    // Snap to grid si activé
    QPointF adjustedPos = m_snapToGrid ? snapToGridInternal(scenePos) : scenePos;

    // Capturer le pointeur créé (shared_ptr pour gestion mémoire sûre)
    std::shared_ptr<GeometricPoint*> pointPtr = std::make_shared<GeometricPoint*>(nullptr);

    pushCommand("Add Point",
        // Execute: ajouter point
        [this, adjustedPos, pointPtr]() {
            *pointPtr = m_sketch->addPoint(adjustedPos.x(), adjustedPos.y());
            qCInfo(logCore()) << "Point placed at" << adjustedPos;
            emit pointCreated(*pointPtr);
            triggerSolveIfEnabled();
        },
        // Undo: supprimer point
        [this, pointPtr]() {
            if (*pointPtr && m_sketch) {
                m_sketch->removeElement(*pointPtr);
                update();
            }
        }
    );
}

void ConstraintCanvas2DView::handleSegmentDrawing(const QPointF& scenePos)
{
    if (!m_sketch) return;

    // Snap to grid si activé
    QPointF adjustedPos = m_snapToGrid ? snapToGridInternal(scenePos) : scenePos;

    // Chercher un point existant près du clic (tolérance: 10 pixels)
    GeometricPoint* clickedPoint = findPointNear(scenePos, 10.0);

    if (!m_segmentStartPoint) {
        // Premier clic : créer ou réutiliser point de départ
        if (!clickedPoint) {
            // Créer nouveau point automatiquement
            std::shared_ptr<GeometricPoint*> pointPtr = std::make_shared<GeometricPoint*>(nullptr);

            pushCommand("Add Point",
                [this, adjustedPos, pointPtr]() {
                    *pointPtr = m_sketch->addPoint(adjustedPos.x(), adjustedPos.y());
                    qCInfo(logCore()) << "Auto-created start point at" << adjustedPos;
                    emit pointCreated(*pointPtr);
                    triggerSolveIfEnabled();
                },
                [this, pointPtr]() {
                    if (*pointPtr && m_sketch) {
                        m_sketch->removeElement(*pointPtr);
                        update();
                    }
                }
            );

            m_segmentStartPoint = *pointPtr;
        } else {
            // Réutiliser point existant
            m_segmentStartPoint = clickedPoint;
        }

        qCInfo(logCore()) << "Segment start point set";
        update(); // Afficher le point de départ

    } else {
        // Second clic : créer point final et segment
        GeometricPoint* endPoint = clickedPoint;

        if (!endPoint) {
            // Créer nouveau point final
            std::shared_ptr<GeometricPoint*> pointPtr = std::make_shared<GeometricPoint*>(nullptr);

            pushCommand("Add Point",
                [this, adjustedPos, pointPtr]() {
                    *pointPtr = m_sketch->addPoint(adjustedPos.x(), adjustedPos.y());
                    qCInfo(logCore()) << "Auto-created end point at" << adjustedPos;
                    emit pointCreated(*pointPtr);
                    triggerSolveIfEnabled();
                },
                [this, pointPtr]() {
                    if (*pointPtr && m_sketch) {
                        m_sketch->removeElement(*pointPtr);
                        update();
                    }
                }
            );

            endPoint = *pointPtr;
        }

        // Créer segment entre les deux points
        if (endPoint != m_segmentStartPoint) {
            GeometricPoint* startPt = m_segmentStartPoint;
            GeometricPoint* endPt = endPoint;

            std::shared_ptr<GeometricSegment*> segmentPtr = std::make_shared<GeometricSegment*>(nullptr);

            pushCommand("Add Segment",
                [this, startPt, endPt, segmentPtr]() {
                    *segmentPtr = m_sketch->addSegment(startPt, endPt);
                    qCInfo(logCore()) << "Segment created";
                    emit segmentCreated(*segmentPtr);
                    triggerSolveIfEnabled();
                },
                [this, segmentPtr]() {
                    if (*segmentPtr && m_sketch) {
                        m_sketch->removeElement(*segmentPtr);
                        update();
                    }
                }
            );
        }

        // Reset pour prochain segment
        m_segmentStartPoint = nullptr;
        update();
    }
}

void ConstraintCanvas2DView::handleSelection(const QPointF& scenePos)
{
    // Chercher point ou segment près du clic
    GeometricPoint* point = findPointNear(scenePos);
    if (point) {
        m_selectedElement = point;
        qCInfo(logCore()) << "Point selected";
        emit elementSelected(point);

        // Si on est en mode AddConstraint, ajouter à la sélection
        if (m_editMode == EditMode::AddConstraint) {
            m_constraintSelection.append(point);
            if (m_constraintSelection.size() >= 2) {
                handleConstraintAddition();
            }
        }
        return;
    }

    GeometricSegment* segment = findSegmentNear(scenePos);
    if (segment) {
        m_selectedElement = segment;
        qCInfo(logCore()) << "Segment selected";
        emit elementSelected(segment);

        // Si on est en mode AddConstraint, ajouter à la sélection
        if (m_editMode == EditMode::AddConstraint) {
            m_constraintSelection.append(segment);
            if (m_constraintSelection.size() >= 2) {
                handleConstraintAddition();
            }
        }
        return;
    }

    // Rien trouvé
    m_selectedElement = nullptr;
}

void ConstraintCanvas2DView::handleConstraintAddition()
{
    if (!m_sketch || m_constraintSelection.size() < 2) return;

    IConstraint* constraint = nullptr;

    // Créer contrainte selon type
    switch (m_constraintType) {
    case ConstraintType::Coincident: {
        GeometricPoint* p1 = qobject_cast<GeometricPoint*>(m_constraintSelection[0]);
        GeometricPoint* p2 = qobject_cast<GeometricPoint*>(m_constraintSelection[1]);
        if (p1 && p2) {
            constraint = new CoincidentConstraint(p1, p2, false);
            m_sketch->addConstraint(constraint);
        }
        break;
    }
    case ConstraintType::Parallel: {
        GeometricSegment* s1 = qobject_cast<GeometricSegment*>(m_constraintSelection[0]);
        GeometricSegment* s2 = qobject_cast<GeometricSegment*>(m_constraintSelection[1]);
        if (s1 && s2) {
            constraint = new ParallelConstraint(s1, s2, false);
            m_sketch->addConstraint(constraint);
        }
        break;
    }
    case ConstraintType::Perpendicular: {
        GeometricSegment* s1 = qobject_cast<GeometricSegment*>(m_constraintSelection[0]);
        GeometricSegment* s2 = qobject_cast<GeometricSegment*>(m_constraintSelection[1]);
        if (s1 && s2) {
            constraint = new PerpendicularConstraint(s1, s2, false);
            m_sketch->addConstraint(constraint);
        }
        break;
    }
    case ConstraintType::EqualLength: {
        GeometricSegment* s1 = qobject_cast<GeometricSegment*>(m_constraintSelection[0]);
        GeometricSegment* s2 = qobject_cast<GeometricSegment*>(m_constraintSelection[1]);
        if (s1 && s2) {
            constraint = new EqualLengthConstraint(s1, s2, false);
            m_sketch->addConstraint(constraint);
        }
        break;
    }
    default:
        qCWarning(logCore()) << "Constraint type not yet implemented:" << static_cast<int>(m_constraintType);
        break;
    }

    if (constraint) {
        qCInfo(logCore()) << "Constraint created:" << constraint->getTypeName();
        emit constraintCreated(constraint);
        triggerSolveIfEnabled();
    }

    // Reset sélection
    m_constraintSelection.clear();
}

// ===== Utilitaires =====

QPointF ConstraintCanvas2DView::snapToGridInternal(const QPointF& pos) const
{
    if (m_gridSize <= 0) return pos;

    double x = qRound(pos.x() / m_gridSize) * m_gridSize;
    double y = qRound(pos.y() / m_gridSize) * m_gridSize;
    return QPointF(x, y);
}

QTransform ConstraintCanvas2DView::getViewTransform() const
{
    QTransform transform;
    transform.translate(m_panOffset.x(), m_panOffset.y());
    transform.scale(m_zoomLevel, m_zoomLevel);
    return transform;
}

void ConstraintCanvas2DView::triggerSolveIfEnabled()
{
    if (m_autoSolve && m_sketch) {
        m_sketch->solve();
        update();
    }
}

// ===== Undo/Redo =====

void ConstraintCanvas2DView::pushCommand(const QString& description,
                                        std::function<void()> executeFunc,
                                        std::function<void()> undoFunc)
{
    // Exécuter la commande immédiatement
    executeFunc();

    // Ajouter la commande à la pile d'annulation
    m_undoStack.append(new CanvasCommand(description, executeFunc, undoFunc));

    // Vider la pile de rétablissement (on ne peut plus refaire après une nouvelle action)
    qDeleteAll(m_redoStack);
    m_redoStack.clear();

    // Limiter la taille de la pile (ex: 50 commandes max)
    const int MAX_HISTORY = 50;
    while (m_undoStack.size() > MAX_HISTORY) {
        delete m_undoStack.takeFirst();
    }
}

void ConstraintCanvas2DView::undo()
{
    if (!canUndo()) return;

    // Récupérer la dernière commande
    CanvasCommand* cmd = m_undoStack.takeLast();

    // Exécuter l'annulation
    cmd->undoFunc();

    // Déplacer vers pile de rétablissement
    m_redoStack.append(cmd);

    // Mettre à jour l'affichage
    update();
}

void ConstraintCanvas2DView::redo()
{
    if (!canRedo()) return;

    // Récupérer la commande à refaire
    CanvasCommand* cmd = m_redoStack.takeLast();

    // Ré-exécuter la commande
    cmd->executeFunc();

    // Déplacer vers pile d'annulation
    m_undoStack.append(cmd);

    // Mettre à jour l'affichage
    update();
}

bool ConstraintCanvas2DView::canUndo() const
{
    return !m_undoStack.isEmpty();
}

bool ConstraintCanvas2DView::canRedo() const
{
    return !m_redoStack.isEmpty();
}

void ConstraintCanvas2DView::clearHistory()
{
    qDeleteAll(m_undoStack);
    m_undoStack.clear();

    qDeleteAll(m_redoStack);
    m_redoStack.clear();
}

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio
