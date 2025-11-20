#ifndef ISHAPE_H
#define ISHAPE_H

#include "core/models/base/Interface.h"
#include "core/infrastructure/patterns/factory/FactoryMixin.h"
#include "core/infrastructure/patterns/lists/ListManagerMixin.h"
#include "core/infrastructure/patterns/properties/PropertyMixin.h"
#include "core/models/base/types/Point2D.h"
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
 * Cette interface définit le contrat pour toutes les formes géométriques
 * 2D du système (Rectangle, Circle, Triangle, etc.).
 *
 * ## Patterns Architecturaux
 *
 * - **Prototype Pattern** : Hérite de Interface pour clonage polymorphe
 * - **Factory Pattern** : Utilise FactoryMixin pour création depuis QVariantMap
 * - **List Manager** : Utilise ListManagerMixin pour liste statique de toutes les instances
 * - **Signals/Slots** : Hérite de QObject pour notifications de changements
 *
 * ## Factory Pattern - Utilisation
 *
 * IShape utilise le Factory Pattern via FactoryMixin pour créer des instances
 * depuis des données sérialisées (QVariantMap).
 *
 * ### Enregistrement Automatique
 *
 * Chaque classe concrète s'enregistre automatiquement au démarrage :
 *
 * @code
 * // Rectangle.h
 * class Rectangle : public IShape {
 *     Q_OBJECT
 *     DECLARE_TYPE_NAME(Rectangle)  // Déclare staticTypeName() et getTypeName()
 * private:
 *     static const bool s_registered;  // Déclaration
 * };
 *
 * // Rectangle.cpp
 * const bool Rectangle::s_registered = IShape::registerFactory<Rectangle>();
 * @endcode
 *
 * ### Création depuis QVariantMap
 *
 * @code
 * // Créer un rectangle
 * QVariantMap rectData;
 * rectData["type"] = "Rectangle";
 * rectData["x"] = 10.0;
 * rectData["y"] = 20.0;
 * rectData["width"] = 100.0;
 * rectData["height"] = 50.0;
 *
 * IShape* rect = IShape::create(rectData);
 * if (rect) {
 *     qDebug() << "Created:" << rect->getTypeName();
 *     qDebug() << "Area:" << rect->getArea();  // 5000.0
 * }
 *
 * // Créer un cercle
 * QVariantMap circleData;
 * circleData["type"] = "Circle";
 * circleData["centerX"] = 50.0;
 * circleData["centerY"] = 50.0;
 * circleData["radius"] = 25.0;
 *
 * IShape* circle = IShape::create(circleData);
 * @endcode
 *
 * ### Lister Types Disponibles
 *
 * @code
 * // Obtenir tous les types enregistrés
 * QStringList types = IShape::availableTypes();
 * // => ["Rectangle", "Circle", "Triangle"]
 *
 * // Créer dynamiquement depuis UI
 * QString selectedType = ui->shapeComboBox->currentText();
 * QVariantMap defaultData;
 * defaultData["type"] = selectedType;
 * IShape* shape = IShape::create(defaultData);
 * @endcode
 *
 * ### Round-Trip Sérialisation
 *
 * @code
 * // Objet → QVariantMap → Objet (clone exact)
 * Rectangle* original = new Rectangle(10, 20, 100, 50);
 * QVariantMap data = original->toVariant();  // Sérialisation automatique
 * IShape* clone = IShape::create(data);      // Désérialisation
 *
 * // clone est une copie exacte de original !
 * assert(clone->getTypeName() == "Rectangle");
 * assert(clone->getArea() == original->getArea());
 * @endcode
 *
 * @note Zéro duplication : Le Factory Pattern est fourni par FactoryMixin (CRTP)
 * @note Type-safe : Vérification à la compilation via template
 * @note Extensible : Nouveaux types via plugins sans recompilation
 *
 * @see FactoryMixin, DECLARE_TYPE_NAME, Interface::toVariant()
 */
class IShape : public Interface,
               protected Patterns::FactoryMixin<IShape>,
               protected Patterns::ListManagerMixin<IShape>
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

    // Gestion de la liste statique (fournie par ListManagerMixin)
    using ListManagerMixin<IShape>::getAllInstances;
    using ListManagerMixin<IShape>::clearAllInstances;
    using ListManagerMixin<IShape>::instanceCount;

    /**
     * @brief Obtient toutes les formes créées (alias pour compatibilité)
     * @return Liste de toutes les formes
     */
    static QList<IShape*> getAllShapes() { return getAllInstances(); }

    /**
     * @brief Vide la liste de toutes les formes (alias pour compatibilité)
     */
    static void clearAllShapes() { clearAllInstances(); }

protected:
    /**
     * @brief Constructeur
     */
    IShape();
};

} // namespace Core
} // namespace LaserCutStudio

#endif // ISHAPE_H
