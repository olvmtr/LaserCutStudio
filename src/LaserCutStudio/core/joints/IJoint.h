#ifndef IJOINT_H
#define IJOINT_H

#include "../interface/Interface.h"
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

// Forward declaration
class IPart;

/**
 * @brief Interface pour les assemblages entre pièces
 *
 * Un joint définit comment deux pièces s'assemblent entre elles
 * dans l'espace 3D.
 * Hérite de Interface (donc QObject) pour bénéficier des Signals/Slots.
 */
class IJoint : public Interface
{
    Q_OBJECT

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

    // Factory Pattern avec QVariant
    /**
     * @brief Crée un joint depuis une configuration QVariant
     * @param config Configuration avec au minimum la clé "type"
     * @return Nouveau joint ou nullptr si type inconnu
     */
    static IJoint* create(const QVariantMap& config);

    /**
     * @brief Liste tous les types de joints disponibles
     */
    static QStringList availableTypes();

    /**
     * @brief Sérialise le joint en QVariantMap pour sauvegarde/réseau
     */
    virtual QVariantMap toVariant() const;

    /**
     * @brief Retourne le nom du type (doit être implémenté par les classes concrètes)
     */
    virtual QString getTypeName() const = 0;

    // Gestion de la liste statique
    static QList<IJoint*> getAllJoints() { return s_joints; }
    static void addJoint(IJoint* joint);
    static void removeJoint(IJoint* joint);
    static void clearAllJoints();

    /**
     * @brief Notification qu'un Part va être détruit
     * Appelé par le destructeur de IPart
     */
    void notifyPartDestroyed(IPart* part);

protected:
    IJoint();
    IJoint(JointType type, IPart* partA, IPart* partB, const Point3D& position, double angle);
    IJoint(const IJoint& other);

    JointType m_type;       ///< Type de joint
    IPart* m_partA;         ///< Première pièce
    IPart* m_partB;         ///< Deuxième pièce
    Point3D m_position;     ///< Position dans l'espace 3D
    double m_angle;         ///< Angle d'assemblage en degrés

    static QList<IJoint*> s_joints; ///< Liste statique de tous les joints

    // Factory Pattern infrastructure
    using FactoryFunc = std::function<IJoint*(const QVariantMap&)>;
    static QMap<QString, FactoryFunc> s_factories;

    /**
     * @brief Enregistre une classe concrète dans le Factory Pattern
     * Utilisé par les classes dérivées pour s'auto-enregistrer
     */
    template<typename T>
    static bool registerFactory() {
        s_factories[T::staticTypeName()] = [](const QVariantMap& params) {
            auto* obj = new T();
            // Utilise Q_PROPERTY pour configurer l'objet depuis params
            const QMetaObject* meta = obj->metaObject();
            for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
                int propIndex = meta->indexOfProperty(it.key().toUtf8().constData());
                if (propIndex >= 0) {
                    QMetaProperty prop = meta->property(propIndex);
                    if (prop.isWritable()) {
                        prop.write(obj, it.value());
                    }
                }
            }
            return obj;
        };
        return true;
    }
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IJOINT_H
