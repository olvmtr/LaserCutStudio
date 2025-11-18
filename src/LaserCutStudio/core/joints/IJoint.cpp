#include "IJoint.h"
#include "../parts/IPart.h"

namespace LaserCutStudio {
namespace Core {

// Initialisation de la liste statique
QList<IJoint*> IJoint::s_joints;
// Note: s_factories est maintenant dans FactoryMixin et initialisé automatiquement

IJoint::IJoint()
    : Interface()
    , m_type(JointType::TAB)
    , m_partA(nullptr)
    , m_partB(nullptr)
    , m_position(Point3D())
    , m_angle(90.0)
{
    addJoint(this);
}

IJoint::IJoint(JointType type, IPart* partA, IPart* partB, const Point3D& position, double angle)
    : Interface()
    , m_type(type)
    , m_partA(partA)
    , m_partB(partB)
    , m_position(position)
    , m_angle(angle)
{
    addJoint(this);

    // Ajoute ce joint aux pièces et connecte aux signaux
    if (m_partA) {
        m_partA->addJoint(this);
        // Connecte au signal aboutToBeDestroyed pour nettoyage automatique
        QObject::connect(m_partA, &Interface::aboutToBeDestroyed,
                        this, [this](Interface* destroyedPart) {
            if (m_partA == destroyedPart) {
                m_partA = nullptr;
            }
        });
    }
    if (m_partB) {
        m_partB->addJoint(this);
        // Connecte au signal aboutToBeDestroyed pour nettoyage automatique
        QObject::connect(m_partB, &Interface::aboutToBeDestroyed,
                        this, [this](Interface* destroyedPart) {
            if (m_partB == destroyedPart) {
                m_partB = nullptr;
            }
        });
    }
}

IJoint::IJoint(const IJoint& other)
    : Interface(other)
    , m_type(other.m_type)
    , m_partA(other.m_partA)
    , m_partB(other.m_partB)
    , m_position(other.m_position)
    , m_angle(other.m_angle)
{
    addJoint(this);
}

IJoint::~IJoint()
{
    disconnect();
    removeJoint(this);
}

void IJoint::connect(IPart* partA, IPart* partB)
{
    // Retire le joint des anciennes pièces et déconnecte les signaux
    if (m_partA) {
        m_partA->removeJoint(this);
        // Déconnecte tous les signaux de l'ancienne pièce vers ce joint
        QObject::disconnect(m_partA, nullptr, this, nullptr);
    }
    if (m_partB) {
        m_partB->removeJoint(this);
        // Déconnecte tous les signaux de l'ancienne pièce vers ce joint
        QObject::disconnect(m_partB, nullptr, this, nullptr);
    }

    // Connecte aux nouvelles pièces
    m_partA = partA;
    m_partB = partB;

    if (m_partA) {
        m_partA->addJoint(this);
        // Connecte au signal aboutToBeDestroyed pour nettoyage automatique
        QObject::connect(m_partA, &Interface::aboutToBeDestroyed,
                        this, [this](Interface* destroyedPart) {
            if (m_partA == destroyedPart) {
                m_partA = nullptr;
            }
        });
    }
    if (m_partB) {
        m_partB->addJoint(this);
        // Connecte au signal aboutToBeDestroyed pour nettoyage automatique
        QObject::connect(m_partB, &Interface::aboutToBeDestroyed,
                        this, [this](Interface* destroyedPart) {
            if (m_partB == destroyedPart) {
                m_partB = nullptr;
            }
        });
    }
}

void IJoint::disconnect()
{
    if (m_partA) {
        m_partA->removeJoint(this);
        // Déconnecte les signaux
        QObject::disconnect(m_partA, nullptr, this, nullptr);
        m_partA = nullptr;
    }
    if (m_partB) {
        m_partB->removeJoint(this);
        // Déconnecte les signaux
        QObject::disconnect(m_partB, nullptr, this, nullptr);
        m_partB = nullptr;
    }
}

bool IJoint::isValid() const
{
    return m_partA != nullptr && m_partB != nullptr && m_partA != m_partB;
}

void IJoint::addJoint(IJoint* joint)
{
    if (joint && !s_joints.contains(joint)) {
        s_joints.append(joint);
    }
}

void IJoint::removeJoint(IJoint* joint)
{
    s_joints.removeAll(joint);
}

void IJoint::clearAllJoints()
{
    s_joints.clear();
}

// ===== Factory Pattern =====
// Note: create(), availableTypes() et toVariant() sont maintenant fournis par FactoryMixin et Interface

} // namespace Core
} // namespace LaserCutStudio
