/**
 * @file SelectionManager.cpp
 * @brief Implémentation du gestionnaire de sélection
 */

#include "core/models/editor/selection/SelectionManager.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>
#include <QtMath>
#include <algorithm>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

// Auto-registration avec le Factory Pattern
namespace {
    Patterns::FactoryMixin<ISelection>::AutoRegister<SelectionManager> g_selectionManagerReg;
}

SelectionManager::SelectionManager(QObject* parent)
    : ISelection(parent)
    , m_boundingRectDirty(true)
{
    qCInfo(logCore()) << "SelectionManager created";
}

SelectionManager::~SelectionManager()
{
    // Déconnecter tous les signaux
    for (IShape* shape : m_selectedShapes) {
        disconnectShapeSignals(shape);
    }

    qCInfo(logCore()) << "SelectionManager destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(SelectionManager, ISelection)

// ===== Sélection =====

QRectF SelectionManager::getBoundingRect() const
{
    if (m_boundingRectDirty) {
        const_cast<SelectionManager*>(this)->updateBoundingRect();
    }
    return m_cachedBoundingRect;
}

Point2D SelectionManager::getCenter() const
{
    QRectF rect = getBoundingRect();
    return Point2D(rect.center().x(), rect.center().y());
}

void SelectionManager::addShape(IShape* shape)
{
    if (!shape) {
        qCWarning(logCore()) << "Cannot add null shape to selection";
        return;
    }

    if (m_selectedShapes.contains(shape)) {
        qCDebug(logCore()) << "Shape already in selection";
        return;
    }

    m_selectedShapes.append(shape);
    connectShapeSignals(shape);
    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Shape added to selection:" << shape->getTypeName();
    emit shapeAdded(shape);
    emit countChanged(m_selectedShapes.size());
}

void SelectionManager::removeShape(IShape* shape)
{
    if (!shape) {
        qCWarning(logCore()) << "Cannot remove null shape from selection";
        return;
    }

    int index = m_selectedShapes.indexOf(shape);
    if (index == -1) {
        qCDebug(logCore()) << "Shape not in selection";
        return;
    }

    m_selectedShapes.removeAt(index);
    disconnectShapeSignals(shape);
    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Shape removed from selection:" << shape->getTypeName();
    emit shapeRemoved(shape);
    emit countChanged(m_selectedShapes.size());
}

void SelectionManager::toggleShape(IShape* shape)
{
    if (isSelected(shape)) {
        removeShape(shape);
    } else {
        addShape(shape);
    }
}

void SelectionManager::selectAll(const QVector<IShape*>& shapes)
{
    clear();

    for (IShape* shape : shapes) {
        if (shape) {
            m_selectedShapes.append(shape);
            connectShapeSignals(shape);
        }
    }

    m_boundingRectDirty = true;

    qCInfo(logCore()) << "Selected all shapes:" << m_selectedShapes.size();
    emit countChanged(m_selectedShapes.size());
}

void SelectionManager::clear()
{
    if (m_selectedShapes.isEmpty()) {
        return;
    }

    // Déconnecter tous les signaux
    for (IShape* shape : m_selectedShapes) {
        disconnectShapeSignals(shape);
    }

    int oldCount = m_selectedShapes.size();
    m_selectedShapes.clear();
    m_boundingRectDirty = true;

    qCInfo(logCore()) << "Selection cleared:" << oldCount << "shapes";
    emit cleared();
    emit countChanged(0);
}

void SelectionManager::selectInRect(const QRectF& rect, const QVector<IShape*>& shapes)
{
    clear();

    for (IShape* shape : shapes) {
        if (!shape) continue;

        QRectF shapeBounds = shape->getBoundingBox();

        // Sélectionner si le rectangle de la forme intersecte ou est contenu
        if (rect.intersects(shapeBounds)) {
            m_selectedShapes.append(shape);
            connectShapeSignals(shape);
        }
    }

    m_boundingRectDirty = true;

    qCInfo(logCore()) << "Selected" << m_selectedShapes.size() << "shapes in rect";
    emit countChanged(m_selectedShapes.size());
}

bool SelectionManager::isSelected(IShape* shape) const
{
    return m_selectedShapes.contains(shape);
}

// ===== Opérations groupées =====

void SelectionManager::moveBy(double dx, double dy)
{
    if (m_selectedShapes.isEmpty()) {
        return;
    }

    for (IShape* shape : m_selectedShapes) {
        shape->translate(dx, dy);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Moved" << m_selectedShapes.size() << "shapes by" << dx << "," << dy;
    emit transformed();
}

void SelectionManager::rotateBy(double angleDegrees)
{
    if (m_selectedShapes.isEmpty()) {
        return;
    }

    Point2D center = getCenter();

    for (IShape* shape : m_selectedShapes) {
        shape->rotate(angleDegrees, center);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Rotated" << m_selectedShapes.size() << "shapes by" << angleDegrees;
    emit transformed();
}

void SelectionManager::scaleBy(double factorX, double factorY)
{
    if (m_selectedShapes.isEmpty()) {
        return;
    }

    Point2D center = getCenter();

    for (IShape* shape : m_selectedShapes) {
        shape->scale(factorX, factorY, center);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Scaled" << m_selectedShapes.size() << "shapes by" << factorX << "," << factorY;
    emit transformed();
}

void SelectionManager::deleteSelection()
{
    if (m_selectedShapes.isEmpty()) {
        return;
    }

    int count = m_selectedShapes.size();

    // Supprimer toutes les formes sélectionnées
    for (IShape* shape : m_selectedShapes) {
        disconnectShapeSignals(shape);
        delete shape;
    }

    m_selectedShapes.clear();
    m_boundingRectDirty = true;

    qCInfo(logCore()) << "Deleted" << count << "shapes from selection";
    emit cleared();
    emit countChanged(0);
}

QVector<IShape*> SelectionManager::duplicate()
{
    if (m_selectedShapes.isEmpty()) {
        return QVector<IShape*>();
    }

    QVector<IShape*> duplicates;

    for (IShape* shape : m_selectedShapes) {
        IShape* clone = shape->clone();
        if (clone) {
            // Décaler légèrement la copie
            clone->translate(10.0, 10.0);
            duplicates.append(clone);
        }
    }

    qCInfo(logCore()) << "Duplicated" << duplicates.size() << "shapes";
    return duplicates;
}

// ===== Alignement =====

void SelectionManager::alignLeft()
{
    if (m_selectedShapes.size() < 2) {
        return;
    }

    QRectF bounds = getBoundingRect();
    double leftX = bounds.left();

    for (IShape* shape : m_selectedShapes) {
        QRectF shapeBounds = shape->getBoundingBox();
        double dx = leftX - shapeBounds.left();
        shape->translate(dx, 0);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Aligned" << m_selectedShapes.size() << "shapes to left";
    emit transformed();
}

void SelectionManager::alignRight()
{
    if (m_selectedShapes.size() < 2) {
        return;
    }

    QRectF bounds = getBoundingRect();
    double rightX = bounds.right();

    for (IShape* shape : m_selectedShapes) {
        QRectF shapeBounds = shape->getBoundingBox();
        double dx = rightX - shapeBounds.right();
        shape->translate(dx, 0);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Aligned" << m_selectedShapes.size() << "shapes to right";
    emit transformed();
}

void SelectionManager::alignTop()
{
    if (m_selectedShapes.size() < 2) {
        return;
    }

    QRectF bounds = getBoundingRect();
    double topY = bounds.top();

    for (IShape* shape : m_selectedShapes) {
        QRectF shapeBounds = shape->getBoundingBox();
        double dy = topY - shapeBounds.top();
        shape->translate(0, dy);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Aligned" << m_selectedShapes.size() << "shapes to top";
    emit transformed();
}

void SelectionManager::alignBottom()
{
    if (m_selectedShapes.size() < 2) {
        return;
    }

    QRectF bounds = getBoundingRect();
    double bottomY = bounds.bottom();

    for (IShape* shape : m_selectedShapes) {
        QRectF shapeBounds = shape->getBoundingBox();
        double dy = bottomY - shapeBounds.bottom();
        shape->translate(0, dy);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Aligned" << m_selectedShapes.size() << "shapes to bottom";
    emit transformed();
}

void SelectionManager::alignCenterH()
{
    if (m_selectedShapes.size() < 2) {
        return;
    }

    QRectF bounds = getBoundingRect();
    double centerX = bounds.center().x();

    for (IShape* shape : m_selectedShapes) {
        QRectF shapeBounds = shape->getBoundingBox();
        double dx = centerX - shapeBounds.center().x();
        shape->translate(dx, 0);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Aligned" << m_selectedShapes.size() << "shapes to center horizontally";
    emit transformed();
}

void SelectionManager::alignCenterV()
{
    if (m_selectedShapes.size() < 2) {
        return;
    }

    QRectF bounds = getBoundingRect();
    double centerY = bounds.center().y();

    for (IShape* shape : m_selectedShapes) {
        QRectF shapeBounds = shape->getBoundingBox();
        double dy = centerY - shapeBounds.center().y();
        shape->translate(0, dy);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Aligned" << m_selectedShapes.size() << "shapes to center vertically";
    emit transformed();
}

// ===== Distribution =====

void SelectionManager::distributeHorizontally()
{
    if (m_selectedShapes.size() < 3) {
        qCDebug(logCore()) << "Need at least 3 shapes to distribute";
        return;
    }

    // Trier les formes par position X
    QVector<IShape*> sorted = m_selectedShapes;
    std::sort(sorted.begin(), sorted.end(), [](IShape* a, IShape* b) {
        return a->getBoundingBox().center().x() < b->getBoundingBox().center().x();
    });

    // Calculer l'espacement
    double leftX = sorted.first()->getBoundingBox().center().x();
    double rightX = sorted.last()->getBoundingBox().center().x();
    double totalSpace = rightX - leftX;
    double spacing = totalSpace / (sorted.size() - 1);

    // Distribuer
    for (int i = 1; i < sorted.size() - 1; ++i) {
        double targetX = leftX + i * spacing;
        double currentX = sorted[i]->getBoundingBox().center().x();
        double dx = targetX - currentX;
        sorted[i]->translate(dx, 0);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Distributed" << sorted.size() << "shapes horizontally";
    emit transformed();
}

void SelectionManager::distributeVertically()
{
    if (m_selectedShapes.size() < 3) {
        qCDebug(logCore()) << "Need at least 3 shapes to distribute";
        return;
    }

    // Trier les formes par position Y
    QVector<IShape*> sorted = m_selectedShapes;
    std::sort(sorted.begin(), sorted.end(), [](IShape* a, IShape* b) {
        return a->getBoundingBox().center().y() < b->getBoundingBox().center().y();
    });

    // Calculer l'espacement
    double topY = sorted.first()->getBoundingBox().center().y();
    double bottomY = sorted.last()->getBoundingBox().center().y();
    double totalSpace = bottomY - topY;
    double spacing = totalSpace / (sorted.size() - 1);

    // Distribuer
    for (int i = 1; i < sorted.size() - 1; ++i) {
        double targetY = topY + i * spacing;
        double currentY = sorted[i]->getBoundingBox().center().y();
        double dy = targetY - currentY;
        sorted[i]->translate(0, dy);
    }

    m_boundingRectDirty = true;

    qCDebug(logCore()) << "Distributed" << sorted.size() << "shapes vertically";
    emit transformed();
}

// ===== Helpers privés =====

void SelectionManager::updateBoundingRect()
{
    if (m_selectedShapes.isEmpty()) {
        m_cachedBoundingRect = QRectF();
        m_boundingRectDirty = false;
        return;
    }

    // Calculer le rectangle englobant de toutes les formes
    QRectF united;
    for (IShape* shape : m_selectedShapes) {
        if (united.isNull()) {
            united = shape->getBoundingBox();
        } else {
            united = united.united(shape->getBoundingBox());
        }
    }

    m_cachedBoundingRect = united;
    m_boundingRectDirty = false;

    emit boundingRectChanged(m_cachedBoundingRect);
}

void SelectionManager::connectShapeSignals(IShape* shape)
{
    if (!shape) return;

    // Connecter au signal aboutToBeDestroyed pour nettoyage automatique
    connect(shape, &Interface::aboutToBeDestroyed,
            this, [this, shape](Interface* destroyed) {
        if (destroyed == shape) {
            removeShape(shape);
        }
    });

    // Connecter au signal geometryChanged pour invalider le bounding rect
    connect(shape, &IShape::geometryChanged,
            this, [this]() {
        m_boundingRectDirty = true;
    });
}

void SelectionManager::disconnectShapeSignals(IShape* shape)
{
    if (!shape) return;
    disconnect(shape, nullptr, this, nullptr);
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
