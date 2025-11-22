/**
 * @file ISelection.h
 * @brief Interface pour la gestion de la sélection d'objets
 *
 * ISelection définit le contrat pour gérer la sélection d'un ou plusieurs
 * objets dans l'éditeur 2D. Supporte sélection simple, multiple, et
 * opérations groupées (déplacement, rotation, etc.).
 */

#ifndef ISELECTION_H
#define ISELECTION_H

#include "core/models/base/Interface.h"
#include "core/infrastructure/patterns/factory/FactoryMixin.h"
#include "core/models/shapes/interfaces/IShape.h"
#include "core/models/base/types/Point2D.h"
#include <QVector>
#include <QRectF>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class ISelection
 * @brief Interface abstraite pour la gestion de la sélection
 *
 * ISelection gère l'ensemble des formes sélectionnées dans l'éditeur
 * et fournit des opérations groupées (move, rotate, scale, delete).
 *
 * ## Modes de sélection
 * - Simple : Un seul objet sélectionné
 * - Multiple : Plusieurs objets sélectionnés (Ctrl+clic)
 * - Rectangle : Sélection par zone rectangulaire
 *
 * ## Opérations supportées
 * - Transformation groupée (move, rotate, scale)
 * - Alignement (left, right, top, bottom, center)
 * - Distribution (horizontal, vertical)
 * - Grouping/Ungrouping
 * - Duplication
 * - Suppression
 *
 * @note Envoie des signaux pour notifications de changement
 */
class ISelection : public Interface, protected Patterns::FactoryMixin<ISelection>
{
    Q_OBJECT

    // Expose les méthodes du Factory Pattern
    using FactoryMixin<ISelection>::create;
    using FactoryMixin<ISelection>::availableTypes;
    using FactoryMixin<ISelection>::registerFactory;

    Q_PROPERTY(int count READ getCount NOTIFY countChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY countChanged)
    Q_PROPERTY(QRectF boundingRect READ getBoundingRect NOTIFY boundingRectChanged)

public:
    explicit ISelection(QObject* parent = nullptr) : Interface(parent) {}
    ~ISelection() override = default;

    /**
     * @brief Nombre d'objets sélectionnés
     */
    virtual int getCount() const = 0;

    /**
     * @brief Vérifie si la sélection est vide
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Liste des formes sélectionnées
     */
    virtual QVector<IShape*> getSelectedShapes() const = 0;

    /**
     * @brief Rectangle englobant de toute la sélection
     */
    virtual QRectF getBoundingRect() const = 0;

    /**
     * @brief Centre de la sélection (pour rotation/scale)
     */
    virtual Point2D getCenter() const = 0;

    /**
     * @brief Ajoute une forme à la sélection
     */
    Q_INVOKABLE virtual void addShape(IShape* shape) = 0;

    /**
     * @brief Retire une forme de la sélection
     */
    Q_INVOKABLE virtual void removeShape(IShape* shape) = 0;

    /**
     * @brief Bascule l'état de sélection d'une forme
     */
    Q_INVOKABLE virtual void toggleShape(IShape* shape) = 0;

    /**
     * @brief Sélectionne toutes les formes
     */
    virtual void selectAll(const QVector<IShape*>& shapes) = 0;

    /**
     * @brief Désélectionne tout
     */
    Q_INVOKABLE virtual void clear() = 0;

    /**
     * @brief Sélectionne les formes dans une zone rectangulaire
     */
    virtual void selectInRect(const QRectF& rect, const QVector<IShape*>& shapes) = 0;

    /**
     * @brief Vérifie si une forme est sélectionnée
     */
    Q_INVOKABLE virtual bool isSelected(IShape* shape) const = 0;

    // ===== Opérations groupées =====

    /**
     * @brief Déplace toute la sélection
     */
    virtual void moveBy(double dx, double dy) = 0;

    /**
     * @brief Rotation de la sélection autour de son centre
     */
    virtual void rotateBy(double angleDegrees) = 0;

    /**
     * @brief Mise à l'échelle de la sélection
     */
    virtual void scaleBy(double factorX, double factorY) = 0;

    /**
     * @brief Supprime toutes les formes sélectionnées
     */
    virtual void deleteSelection() = 0;

    /**
     * @brief Duplique la sélection
     * @return Nouvelles formes créées
     */
    virtual QVector<IShape*> duplicate() = 0;

    // ===== Alignement =====

    /**
     * @brief Aligne la sélection à gauche
     */
    virtual void alignLeft() = 0;

    /**
     * @brief Aligne la sélection à droite
     */
    virtual void alignRight() = 0;

    /**
     * @brief Aligne la sélection en haut
     */
    virtual void alignTop() = 0;

    /**
     * @brief Aligne la sélection en bas
     */
    virtual void alignBottom() = 0;

    /**
     * @brief Centre la sélection horizontalement
     */
    virtual void alignCenterH() = 0;

    /**
     * @brief Centre la sélection verticalement
     */
    virtual void alignCenterV() = 0;

    // ===== Distribution =====

    /**
     * @brief Distribue horizontalement avec espacement égal
     */
    virtual void distributeHorizontally() = 0;

    /**
     * @brief Distribue verticalement avec espacement égal
     */
    virtual void distributeVertically() = 0;

signals:
    /**
     * @brief Émis quand le nombre de sélections change
     */
    void countChanged(int count);

    /**
     * @brief Émis quand une forme est ajoutée
     */
    void shapeAdded(IShape* shape);

    /**
     * @brief Émis quand une forme est retirée
     */
    void shapeRemoved(IShape* shape);

    /**
     * @brief Émis quand la sélection est vidée
     */
    void cleared();

    /**
     * @brief Émis quand le bounding rect change
     */
    void boundingRectChanged(const QRectF& rect);

    /**
     * @brief Émis quand la sélection est transformée
     */
    void transformed();
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // ISELECTION_H
