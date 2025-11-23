#ifndef IPART_H
#define IPART_H

#include "core/models/base/Interface.h"
#include "core/infrastructure/patterns/factory/FactoryMixin.h"
#include "core/infrastructure/patterns/lists/ListManagerMixin.h"
#include "core/models/shapes/IShape.h"
#include "core/models/base/types/Material.h"
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
 * @brief Interface pour les pièces physiques à découper au laser
 *
 * Une pièce (Part) représente un élément physique 2D à découper, composé de :
 * - Une forme géométrique 2D (IShape) - Le contour de découpe
 * - Un matériau (Material) - Bois, acrylique, MDF, etc.
 * - Une épaisseur (thickness) - en mm
 * - Des joints (IJoint) - Connexions avec d'autres pièces
 *
 * ## Patterns Architecturaux
 *
 * - **Composite Pattern** : Une IPart contient UNE IShape (relation de composition)
 * - **Prototype Pattern** : Hérite de Interface pour clonage polymorphe
 * - **Factory Pattern** : Utilise FactoryMixin pour création depuis QVariantMap
 * - **Signals/Slots** : Hérite de QObject pour notifications de changements
 *
 * ## Factory Pattern - Utilisation
 *
 * ### Création depuis QVariantMap
 *
 * @code
 * // Créer une pièce rectangulaire en bois
 * QVariantMap partData;
 * partData["type"] = "Part";
 * partData["name"] = "Façade Boîte";
 *
 * // Forme de la pièce (imbriquée)
 * QVariantMap shapeData;
 * shapeData["type"] = "Rectangle";
 * shapeData["x"] = 0.0;
 * shapeData["y"] = 0.0;
 * shapeData["width"] = 200.0;
 * shapeData["height"] = 150.0;
 * partData["shape"] = shapeData;
 *
 * // Propriétés physiques
 * partData["thickness"] = 3.0;          // 3mm
 * partData["material_name"] = "Plywood";
 * partData["material_density"] = 550.0;
 *
 * IPart* part = IPart::create(partData);
 * if (part) {
 *     qDebug() << "Volume:" << part->calculateVolume() << "mm³";
 *     qDebug() << "Masse:" << part->getMass() << "g";
 * }
 * @endcode
 *
 * ### Composition avec IShape
 *
 * @code
 * // ⚠️ Architecture : UNE shape par part
 * IPart* part = IPart::create(data);
 * IShape* shape = part->getShape();  // UNE shape (singular)
 *
 * // ❌ INCORRECT : getShapes() n'existe pas !
 * // for (IShape* s : part->getShapes()) { }  // Ne compile pas
 * @endcode
 *
 * ### Lister Types Disponibles
 *
 * @code
 * QStringList partTypes = IPart::availableTypes();
 * // => ["Part"] (actuellement 1 type, extensible via plugins)
 * @endcode
 *
 * ### Round-Trip Sérialisation
 *
 * @code
 * // Créer, sérialiser, désérialiser
 * Part* original = new Part("Box Side", rectangleShape, 3.0, Material::Wood());
 * QVariantMap data = original->toVariant();  // Sérialisation automatique
 * IPart* clone = IPart::create(data);        // Désérialisation
 *
 * // clone contient une copie de la shape originale
 * assert(clone->getShape()->getArea() == original->getShape()->getArea());
 * @endcode
 *
 * @note Architecture : Chaque IPart contient **UNE** IShape via getShape() (pas getShapes au pluriel)
 * @note Ownership : IPart possède sa IShape (destructeur nettoie automatiquement)
 * @note Joints : Les IJoint référencent les IPart mais ne les possèdent pas
 *
 * @see FactoryMixin, IShape, IJoint, Material, Interface::toVariant()
 */
class IPart : public Interface,
              protected Patterns::FactoryMixin<IPart>,
              protected Patterns::ListManagerMixin<IPart>
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

    // Gestion de la liste statique (fournie par ListManagerMixin)
    using ListManagerMixin<IPart>::getAllInstances;
    using ListManagerMixin<IPart>::clearAllInstances;
    using ListManagerMixin<IPart>::instanceCount;

    static QList<IPart*> getAllParts() { return getAllInstances(); }
    static void clearAllParts() { clearAllInstances(); }

protected:
    IPart();
    IPart(const QString& name, IShape* shape, double thickness, const Material& material);
    IPart(const IPart& other);

    QString m_name;             ///< Nom de la pièce
    IShape* m_shape;            ///< Forme 2D de la pièce
    double m_thickness;         ///< Épaisseur en mm
    Material m_material;        ///< Matériau de la pièce
    QList<IJoint*> m_joints;    ///< Joints connectés à cette pièce
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IPART_H
