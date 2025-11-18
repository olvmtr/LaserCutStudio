#ifndef IJOINT_H
#define IJOINT_H

#include "../interface/Interface.h"
#include "../patterns/FactoryMixin.h"
#include "../patterns/ListManagerMixin.h"
#include "../types/JointType.h"
#include "../types/Point3D.h"
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
 * @brief Interface pour les assemblages entre pièces
 *
 * Un joint définit comment deux pièces s'assemblent entre elles
 * dans l'espace 3D.
 * Hérite de Interface (donc QObject) pour bénéficier des Signals/Slots.
 * Utilise FactoryMixin pour le Factory Pattern (élimine la duplication).
 * Utilise ListManagerMixin pour la gestion de la liste statique.
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
    JointType getType() const { return m_type; }

    /**
     * @brief Définit le type de joint
     */
    void setType(JointType type) { m_type = type; }

    /**
     * @brief Obtient la première pièce
     */
    IPart* getPartA() const { return m_partA; }

    /**
     * @brief Obtient la deuxième pièce
     */
    IPart* getPartB() const { return m_partB; }

    /**
     * @brief Connecte deux pièces
     */
    void connect(IPart* partA, IPart* partB);

    /**
     * @brief Déconnecte les pièces
     */
    void disconnect();

    /**
     * @brief Obtient la position du joint dans l'espace 3D
     */
    Point3D getPosition() const { return m_position; }

    /**
     * @brief Définit la position du joint
     */
    void setPosition(const Point3D& position) { m_position = position; }

    /**
     * @brief Obtient l'angle du joint (en degrés)
     */
    double getAngle() const { return m_angle; }

    /**
     * @brief Définit l'angle du joint
     */
    void setAngle(double angle) { m_angle = angle; }

    /**
     * @brief Vérifie si le joint est valide
     */
    virtual bool isValid() const;

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

    JointType m_type;       ///< Type de joint
    IPart* m_partA;         ///< Première pièce
    IPart* m_partB;         ///< Deuxième pièce
    Point3D m_position;     ///< Position dans l'espace 3D
    double m_angle;         ///< Angle d'assemblage en degrés
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IJOINT_H
