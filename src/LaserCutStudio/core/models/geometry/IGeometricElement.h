#ifndef IGEOMETRICELEMENT_H
#define IGEOMETRICELEMENT_H

#include "core/models/base/Interface.h"
#include "core/infrastructure/patterns/factory/FactoryMixin.h"
#include "core/infrastructure/patterns/lists/ListManagerMixin.h"
#include "core/models/base/types/Point2D.h"
#include <QPainterPath>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Interface de base pour tous les éléments géométriques
 *
 * Les éléments géométriques sont les briques de base du système de contraintes :
 * - Points (libres ou contraints)
 * - Segments (entre 2 points)
 * - Arcs (centre, rayon, angles)
 *
 * ## Patterns Architecturaux
 *
 * - **Prototype Pattern** : Hérite de Interface pour clonage polymorphe
 * - **Factory Pattern** : Utilise FactoryMixin pour création depuis QVariantMap
 * - **List Manager** : Utilise ListManagerMixin pour liste statique de toutes les instances
 * - **Signals/Slots** : Hérite de QObject pour notifications de changements
 *
 * @note Tous les éléments géométriques sont modifiables par le solveur de contraintes
 */
class IGeometricElement : public Interface,
                          protected Patterns::FactoryMixin<IGeometricElement>,
                          protected Patterns::ListManagerMixin<IGeometricElement>
{
    Q_OBJECT

signals:
    /**
     * @brief Signal émis lorsque la géométrie de l'élément change
     */
    void geometryChanged();

public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IGeometricElement();

    /**
     * @brief Clone l'élément (Pattern Prototype)
     * @return Pointeur vers un nouvel élément cloné
     */
    virtual IGeometricElement* clone() const override = 0;

    /**
     * @brief Obtient le type de l'élément (Point, Segment, Arc)
     * @return Nom du type
     */
    virtual QString getTypeName() const override = 0;

    /**
     * @brief Calcule le rectangle englobant
     * @return Rectangle englobant l'élément
     */
    virtual QRectF getBoundingBox() const = 0;

    /**
     * @brief Vérifie si un point est proche de l'élément (pour sélection)
     * @param point Point à tester
     * @param tolerance Distance maximale (en unités)
     * @return true si le point est proche
     */
    virtual bool isNear(const Point2D& point, double tolerance = 5.0) const = 0;

    /**
     * @brief Convertit l'élément en QPainterPath pour affichage
     * @return Chemin de dessin
     */
    virtual QPainterPath toPainterPath() const = 0;

    /**
     * @brief Obtient tous les points de l'élément (pour export polyline)
     * @param resolution Résolution pour arcs (nombre de segments)
     * @return Liste des points
     */
    virtual QList<Point2D> getPoints(int resolution = 32) const = 0;

    // Factory Pattern fourni par FactoryMixin
    using FactoryMixin<IGeometricElement>::create;
    using FactoryMixin<IGeometricElement>::availableTypes;
    using FactoryMixin<IGeometricElement>::registerFactory;

    // Gestion de la liste statique (fournie par ListManagerMixin)
    using ListManagerMixin<IGeometricElement>::getAllInstances;
    using ListManagerMixin<IGeometricElement>::clearAllInstances;
    using ListManagerMixin<IGeometricElement>::instanceCount;

protected:
    /**
     * @brief Constructeur
     */
    IGeometricElement();
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IGEOMETRICELEMENT_H
