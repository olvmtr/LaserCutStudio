/**
 * @file SelectionManager.h
 * @brief Gestionnaire de sélection d'objets dans l'éditeur
 *
 * SelectionManager implémente ISelection pour gérer la sélection
 * d'un ou plusieurs objets et effectuer des opérations groupées.
 */

#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include "core/models/editor/selection/ISelection.h"
#include "core/models/patterns/properties/PropertyMixin.h"
#include <QVector>
#include <QRectF>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class SelectionManager
 * @brief Implémentation concrète de la gestion de sélection
 *
 * SelectionManager gère l'ensemble des formes sélectionnées et fournit
 * des opérations groupées (transformation, alignement, distribution).
 *
 * ## Fonctionnalités
 * - Sélection simple/multiple
 * - Sélection rectangulaire (zone)
 * - Opérations groupées (move, rotate, scale)
 * - Alignement (left, right, top, bottom, center)
 * - Distribution (horizontal, vertical)
 * - Duplication et suppression
 *
 * ## Utilisation
 * ```cpp
 * SelectionManager* selection = new SelectionManager();
 * selection->addShape(myRectangle);
 * selection->addShape(myCircle);
 * selection->moveBy(10, 20);  // Déplace les deux formes
 * ```
 */
class SelectionManager : public ISelection,
                         protected Patterns::PropertyMixin<SelectionManager>
{
    Q_OBJECT

public:
    explicit SelectionManager(QObject* parent = nullptr);
    ~SelectionManager() override;

    // ===== Macro pour getTypeName() =====
    DECLARE_TYPE_NAME(SelectionManager)

    // ===== ISelection interface =====

    int getCount() const override { return m_selectedShapes.size(); }
    bool isEmpty() const override { return m_selectedShapes.isEmpty(); }
    QVector<IShape*> getSelectedShapes() const override { return m_selectedShapes; }

    QRectF getBoundingRect() const override;
    Point2D getCenter() const override;

    void addShape(IShape* shape) override;
    void removeShape(IShape* shape) override;
    void toggleShape(IShape* shape) override;
    void selectAll(const QVector<IShape*>& shapes) override;
    void clear() override;
    void selectInRect(const QRectF& rect, const QVector<IShape*>& shapes) override;

    bool isSelected(IShape* shape) const override;

    // Opérations groupées
    void moveBy(double dx, double dy) override;
    void rotateBy(double angleDegrees) override;
    void scaleBy(double factorX, double factorY) override;
    void deleteSelection() override;
    QVector<IShape*> duplicate() override;

    // Alignement
    void alignLeft() override;
    void alignRight() override;
    void alignTop() override;
    void alignBottom() override;
    void alignCenterH() override;
    void alignCenterV() override;

    // Distribution
    void distributeHorizontally() override;
    void distributeVertically() override;

    // ===== Prototype Pattern =====

    ISelection* clone() const override;

private:
    QVector<IShape*> m_selectedShapes;

    // Helpers
    void updateBoundingRect();
    QRectF m_cachedBoundingRect;
    bool m_boundingRectDirty = true;

    // Connexions aux signaux aboutToBeDestroyed
    void connectShapeSignals(IShape* shape);
    void disconnectShapeSignals(IShape* shape);
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // SELECTIONMANAGER_H
