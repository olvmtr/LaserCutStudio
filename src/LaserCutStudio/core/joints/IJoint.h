#ifndef IJOINT_H
#define IJOINT_H

#include "../interface/Interface.h"
#include "../types/JointType.h"
#include "../types/Point3D.h"
#include <QList>

namespace LaserCutStudio {
namespace Core {

// Forward declaration
class IPart;

/**
 * @brief Interface pour les assemblages entre pièces
 *
 * Un joint définit comment deux pièces s'assemblent entre elles
 * dans l'espace 3D.
 */
class IJoint : public Interface
{
public:
    IJoint();
    IJoint(JointType type, IPart* partA, IPart* partB, const Point3D& position, double angle);
    IJoint(const IJoint& other);
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

    // Gestion de la liste statique
    static QList<IJoint*> getAllJoints() { return s_joints; }
    static void addJoint(IJoint* joint);
    static void removeJoint(IJoint* joint);
    static void clearAllJoints();

protected:
    JointType m_type;       ///< Type de joint
    IPart* m_partA;         ///< Première pièce
    IPart* m_partB;         ///< Deuxième pièce
    Point3D m_position;     ///< Position dans l'espace 3D
    double m_angle;         ///< Angle d'assemblage en degrés

    static QList<IJoint*> s_joints; ///< Liste statique de tous les joints
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IJOINT_H
