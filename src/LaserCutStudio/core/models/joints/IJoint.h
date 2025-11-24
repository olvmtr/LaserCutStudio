#ifndef IJOINT_H
#define IJOINT_H

#include "core/models/base/Interface.h"
#include "core/models/patterns/factory/FactoryMixin.h"
#include "core/models/patterns/lists/ListManagerMixin.h"
#include "core/models/base/types/JointType.h"
#include "core/models/base/types/Point3D.h"
#include <QList>
#include <QMap>
#include <QVariantMap>
#include <QMetaObject>
#include <QMetaProperty>
#include <functional>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class IPart;

namespace Plugins {
    class PluginManager;
}

/**
 * @brief Interface pour les connexions d'assemblage entre pièces
 *
 * Un joint (Joint) définit comment deux pièces (IPart) s'assemblent entre elles
 * dans l'espace 3D. Types disponibles : Tab, Finger, Mortise-Tenon, Edge-to-Edge.
 *
 * ## Caractéristiques
 *
 * - **Connexion bi-directionnelle** : Connecte partA et partB
 * - **Position 3D** : Coordonnées (x, y, z) du point d'assemblage
 * - **Angle** : Rotation en degrés pour orientation relative
 * - **Type** : JointType (TAB, FINGER, MORTISE_TENON, EDGE_TO_EDGE)
 * - **Gestion lifecycle** : Écoute aboutToBeDestroyed pour nettoyage automatique
 *
 * ## Patterns Architecturaux
 *
 * - **Observer Pattern** : Écoute les signaux aboutToBeDestroyed des pièces
 * - **Prototype Pattern** : Hérite de Interface pour clonage polymorphe
 * - **Factory Pattern** : Utilise FactoryMixin pour création depuis QVariantMap
 * - **Bidirectional Association** : Les pièces référencent leurs joints
 *
 * ## Factory Pattern - Utilisation
 *
 * ### Création depuis QVariantMap
 *
 * @code
 * // Créer un finger joint entre deux pièces
 * QVariantMap jointData;
 * jointData["type"] = "FingerJoint";
 *
 * // Position 3D du joint
 * QVariantMap posData;
 * posData["x"] = 100.0;
 * posData["y"] = 0.0;
 * posData["z"] = 0.0;
 * jointData["position"] = posData;
 *
 * // Paramètres du finger joint
 * jointData["angle"] = 90.0;           // Perpendiculaire
 * jointData["fingerCount"] = 5;        // 5 doigts
 * jointData["fingerWidth"] = 10.0;     // 10mm de large
 *
 * IJoint* joint = IJoint::create(jointData);
 *
 * // Connecter aux pièces (après création)
 * joint->connect(partA, partB);
 * @endcode
 *
 * ### Connexion Bidirectionnelle
 *
 * @code
 * // Connecter joint ↔ pièces (relation bidirectionnelle)
 * IJoint* joint = new FingerJoint();
 * joint->connect(partA, partB);
 *
 * // Les pièces connaissent maintenant leurs joints
 * QList<IJoint*> jointsA = partA->getJoints();
 * assert(jointsA.contains(joint));  // true
 *
 * // Si une pièce est détruite, le joint se déconnecte automatiquement
 * delete partA;  // Émet aboutToBeDestroyed
 * // → joint->getPartA() == nullptr (nettoyage automatique)
 * @endcode
 *
 * ### Lister Types Disponibles
 *
 * @code
 * QStringList jointTypes = IJoint::availableTypes();
 * // => ["TabJoint", "FingerJoint"]
 * // (Extensible via plugins pour Mortise-Tenon, Dovetail, etc.)
 * @endcode
 *
 * ### Round-Trip Sérialisation
 *
 * @code
 * FingerJoint* original = new FingerJoint(partA, partB, pos, 90.0, 5, 10.0);
 * QVariantMap data = original->toVariant();
 * IJoint* clone = IJoint::create(data);
 *
 * // clone a les mêmes propriétés (sauf connexions pièces)
 * assert(clone->getTypeName() == "FingerJoint");
 * assert(clone->getAngle() == 90.0);
 * @endcode
 *
 * @warning Ownership : IJoint ne possède PAS les IPart (pointeurs non-owning)
 * @note Lifecycle : Utilise aboutToBeDestroyed pour éviter dangling pointers
 * @note Thread-safety : Connexion/déconnexion doivent être faites dans le thread Qt
 *
 * @see FactoryMixin, IPart, JointType, Point3D, Interface::toVariant()
 */
class IJoint : public Interface,
               protected Patterns::FactoryMixin<IJoint>,
               protected Patterns::ListManagerMixin<IJoint>
{
    Q_OBJECT
    friend class Plugins::PluginManager;

signals:
    /**
     * @brief Signal émis lorsque partA change
     */
    void partAChanged(IPart* newPart);

    /**
     * @brief Signal émis lorsque partB change
     */
    void partBChanged(IPart* newPart);

    /**
     * @brief Signal émis lorsque la position change
     */
    void positionChanged(const Point3D& newPosition);

    /**
     * @brief Signal émis lorsque l'angle change
     */
    void angleChanged(double newAngle);

public:
    /**
     * @brief Destructeur virtuel public (permet la destruction polymorphe)
     */
    virtual ~IJoint();

    /**
     * @brief Clone le joint
     */
    virtual IJoint* clone() const override = 0;

    /**
     * @brief Obtient le type de joint
     */
    virtual JointType getType() const = 0;

    /**
     * @brief Définit le type de joint
     */
    virtual void setType(JointType type) = 0;

    /**
     * @brief Obtient la première pièce
     */
    virtual IPart* getPartA() const = 0;

    /**
     * @brief Obtient la deuxième pièce
     */
    virtual IPart* getPartB() const = 0;

    /**
     * @brief Connecte deux pièces
     */
    virtual void connect(IPart* partA, IPart* partB) = 0;

    /**
     * @brief Déconnecte les pièces
     */
    virtual void disconnect() = 0;

    /**
     * @brief Obtient la position du joint dans l'espace 3D
     */
    virtual Point3D getPosition() const = 0;

    /**
     * @brief Définit la position du joint
     */
    virtual void setPosition(const Point3D& position) = 0;

    /**
     * @brief Obtient l'angle du joint (en degrés)
     */
    virtual double getAngle() const = 0;

    /**
     * @brief Définit l'angle du joint
     */
    virtual void setAngle(double angle) = 0;

    /**
     * @brief Vérifie si le joint est valide
     */
    virtual bool isValid() const = 0;

    // Factory Pattern fourni par FactoryMixin
    using FactoryMixin<IJoint>::create;
    using FactoryMixin<IJoint>::availableTypes;
    using FactoryMixin<IJoint>::registerFactory;

    /**
     * @brief Retourne le nom du type (doit être implémenté par les classes concrètes)
     */
    virtual QString getTypeName() const override = 0;

    // Gestion de la liste statique (fournie par ListManagerMixin)
    using ListManagerMixin<IJoint>::getAllInstances;
    using ListManagerMixin<IJoint>::clearAllInstances;
    using ListManagerMixin<IJoint>::instanceCount;

    static QList<IJoint*> getAllJoints() { return getAllInstances(); }
    static void clearAllJoints() { clearAllInstances(); }

protected:
    IJoint();
    IJoint(JointType type, IPart* partA, IPart* partB, const Point3D& position, double angle);
    IJoint(const IJoint& other);
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IJOINT_H
