#ifndef ISHAPE_H
#define ISHAPE_H

#include "../interface/Interface.h"
#include "../patterns/FactoryMixin.h"
#include "../types/Point2D.h"
#include <QList>
#include <QRectF>
#include <QMap>
#include <QVariantMap>
#include <QMetaObject>
#include <QMetaProperty>
#include <functional>
#include <memory>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
namespace Plugins {
    class PluginManager;
}

/**
 * @brief Interface pour les formes géométriques 2D
 *
 * Cette interface hérite du pattern Prototype et gère une liste
 * statique de toutes les formes créées.
 * Hérite de Interface (donc QObject) pour bénéficier des Signals/Slots.
 * Utilise FactoryMixin pour le Factory Pattern (élimine la duplication).
 */
class IShape : public Interface, protected Patterns::FactoryMixin<IShape>
{
    Q_OBJECT
    friend class Plugins::PluginManager;

signals:
    /**
     * @brief Signal émis lorsque la géométrie de la forme change
     */
    void geometryChanged();

public:
    /**
     * @brief Destructeur virtuel public (permet la destruction polymorphe)
     */
    virtual ~IShape();

    /**
     * @brief Clone la forme (Pattern Prototype)
     * @return Pointeur vers une nouvelle forme clonée
     */
    virtual IShape* clone() const override = 0;

    /**
     * @brief Calcule l'aire de la forme
     * @return Aire en unités carrées
     */
    virtual double getArea() const = 0;

    /**
     * @brief Calcule le rectangle englobant
     * @return Rectangle englobant la forme
     */
    virtual QRectF getBoundingBox() const = 0;

    /**
     * @brief Vérifie si un point est à l'intérieur de la forme
     * @param point Point à tester
     * @return true si le point est à l'intérieur
     */
    virtual bool containsPoint(const Point2D& point) const = 0;

    /**
     * @brief Obtient les points de la forme
     * @return Liste des points
     */
    virtual QList<Point2D> getPoints() const = 0;

    /**
     * @brief Translate la forme
     * @param dx Déplacement en X
     * @param dy Déplacement en Y
     */
    virtual void translate(double dx, double dy) = 0;

    /**
     * @brief Fait pivoter la forme autour d'un point
     * @param angle Angle en degrés
     * @param center Centre de rotation
     */
    virtual void rotate(double angle, const Point2D& center) = 0;

    /**
     * @brief Met à l'échelle la forme
     * @param scaleX Facteur d'échelle en X
     * @param scaleY Facteur d'échelle en Y
     * @param center Centre de mise à l'échelle
     */
    virtual void scale(double scaleX, double scaleY, const Point2D& center) = 0;

    // Factory Pattern fourni par FactoryMixin
    using FactoryMixin<IShape>::create;
    using FactoryMixin<IShape>::availableTypes;
    using FactoryMixin<IShape>::registerFactory;

    /**
     * @brief Retourne le nom du type (doit être implémenté par les classes concrètes)
     */
    virtual QString getTypeName() const override = 0;

    // Gestion de la liste statique
    /**
     * @brief Obtient toutes les formes créées
     * @return Liste de toutes les formes
     */
    static QList<IShape*> getAllShapes() { return s_shapes; }

    /**
     * @brief Ajoute une forme à la liste
     * @param shape Forme à ajouter
     */
    static void addShape(IShape* shape);

    /**
     * @brief Retire une forme de la liste
     * @param shape Forme à retirer
     */
    static void removeShape(IShape* shape);

    /**
     * @brief Vide la liste de toutes les formes
     */
    static void clearAllShapes();

protected:
    /**
     * @brief Constructeur
     */
    IShape();

    static QList<IShape*> s_shapes; ///< Liste statique de toutes les formes
};

} // namespace Core
} // namespace LaserCutStudio

#endif // ISHAPE_H
