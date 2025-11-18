#ifndef IPART_H
#define IPART_H

#include "../interface/Interface.h"
#include "../patterns/FactoryMixin.h"
#include "../shapes/IShape.h"
#include "../types/Material.h"
#include <QString>
#include <QList>
#include <QMap>
#include <QVariantMap>
#include <QMetaObject>
#include <QMetaProperty>
#include <functional>

namespace LaserCutStudio {
namespace Core {

// Forward declaration
class IJoint;

/**
 * @brief Interface pour les pièces à découper
 *
 * Une pièce représente un élément physique à découper,
 * avec une forme 2D, une épaisseur et un matériau.
 * Hérite de Interface (donc QObject) pour bénéficier des Signals/Slots.
 * Utilise FactoryMixin pour le Factory Pattern (élimine la duplication).
 */
class IPart : public Interface, protected Patterns::FactoryMixin<IPart>
{
    Q_OBJECT

signals:
    /**
     * @brief Signal émis lorsque la forme de la pièce change
     */
    void shapeChanged(IShape* newShape);

    /**
     * @brief Signal émis lorsque le matériau change
     */
    void materialChanged(const Material& newMaterial);

    /**
     * @brief Signal émis lorsque l'épaisseur change
     */
    void thicknessChanged(double newThickness);

public:
    /**
     * @brief Destructeur virtuel public (permet la destruction polymorphe)
     */
    virtual ~IPart();

    /**
     * @brief Clone la pièce
     */
    virtual IPart* clone() const override = 0;

    /**
     * @brief Obtient le nom de la pièce
     */
    QString getName() const { return m_name; }

    /**
     * @brief Définit le nom de la pièce
     */
    void setName(const QString& name) { m_name = name; }

    /**
     * @brief Obtient la forme de la pièce
     */
    IShape* getShape() const { return m_shape; }

    /**
     * @brief Définit la forme de la pièce
     */
    void setShape(IShape* shape) { m_shape = shape; }

    /**
     * @brief Obtient l'épaisseur
     */
    double getThickness() const { return m_thickness; }

    /**
     * @brief Définit l'épaisseur
     */
    void setThickness(double thickness) { m_thickness = thickness; }

    /**
     * @brief Obtient le matériau
     */
    Material getMaterial() const { return m_material; }

    /**
     * @brief Définit le matériau
     */
    void setMaterial(const Material& material) { m_material = material; }

    /**
     * @brief Ajoute un joint à la pièce
     */
    void addJoint(IJoint* joint);

    /**
     * @brief Retire un joint de la pièce
     */
    void removeJoint(IJoint* joint);

    /**
     * @brief Obtient tous les joints de la pièce
     */
    QList<IJoint*> getJoints() const { return m_joints; }

    /**
     * @brief Calcule le volume de la pièce
     */
    double getVolume() const;

    /**
     * @brief Calcule la masse de la pièce
     */
    double getMass() const;

    // Factory Pattern fourni par FactoryMixin
    using FactoryMixin<IPart>::create;
    using FactoryMixin<IPart>::availableTypes;
    using FactoryMixin<IPart>::registerFactory;

    /**
     * @brief Retourne le nom du type (doit être implémenté par les classes concrètes)
     */
    virtual QString getTypeName() const override = 0;

    // Gestion de la liste statique
    static QList<IPart*> getAllParts() { return s_parts; }
    static void addPart(IPart* part);
    static void removePart(IPart* part);
    static void clearAllParts();

protected:
    IPart();
    IPart(const QString& name, IShape* shape, double thickness, const Material& material);
    IPart(const IPart& other);

    QString m_name;             ///< Nom de la pièce
    IShape* m_shape;            ///< Forme 2D de la pièce
    double m_thickness;         ///< Épaisseur en mm
    Material m_material;        ///< Matériau de la pièce
    QList<IJoint*> m_joints;    ///< Joints connectés à cette pièce

    static QList<IPart*> s_parts; ///< Liste statique de toutes les pièces
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IPART_H
