#include "core/models/joints/interfaces/IJoint.h"
#include "core/models/parts/interfaces/IPart.h"

namespace LaserCutStudio {
namespace Core {

// Note: s_factories et s_instances sont maintenant dans FactoryMixin et ListManagerMixin

IJoint::IJoint()
    : Interface()
    , m_type(JointType::TAB)
    , m_partA(nullptr)
    , m_partB(nullptr)
    , m_position(Point3D())
    , m_angle(90.0)
{
    registerInstance(this);
}

IJoint::IJoint(JointType type, IPart* partA, IPart* partB, const Point3D& position, double angle)
    : Interface()
    , m_type(type)
    , m_partA(partA)
    , m_partB(partB)
    , m_position(position)
    , m_angle(angle)
{
    registerInstance(this);

    // Utilise les helpers pour connexion avec gestion aboutToBeDestroyed
    // Note: m_partA et m_partB sont initialisés dans la liste d'initialisation
    IPart* tempA = m_partA;
    IPart* tempB = m_partB;
    m_partA = nullptr;
    m_partB = nullptr;
    connectToPart(m_partA, tempA);
    connectToPart(m_partB, tempB);
}

IJoint::IJoint(const IJoint& other)
    : Interface(other)
    , m_type(other.m_type)
    , m_partA(other.m_partA)
    , m_partB(other.m_partB)
    , m_position(other.m_position)
    , m_angle(other.m_angle)
{
    registerInstance(this);
}

IJoint::~IJoint()
{
    disconnect();
    unregisterInstance(this);
}

void IJoint::connect(IPart* partA, IPart* partB)
{
    // Déconnecte des anciennes pièces (si présentes)
    disconnectFromPart(m_partA);
    disconnectFromPart(m_partB);

    // Connecte aux nouvelles pièces avec gestion aboutToBeDestroyed
    connectToPart(m_partA, partA);
    connectToPart(m_partB, partB);
}

void IJoint::connectToPart(IPart*& partMember, IPart* newPart)
{
    partMember = newPart;  // Assigne le pointeur de membre
    if (newPart) {
        newPart->addJoint(this);
        // Connecte au signal aboutToBeDestroyed pour nettoyage automatique
        QObject::connect(newPart, &Interface::aboutToBeDestroyed,
                        this, [this, &partMember](Interface* destroyedPart) {
            if (partMember == destroyedPart) {
                partMember = nullptr;
            }
        });
    }
}

void IJoint::disconnectFromPart(IPart*& partMember)
{
    if (partMember) {
        partMember->removeJoint(this);
        // Déconnecte tous les signaux de cette pièce vers ce joint
        QObject::disconnect(partMember, nullptr, this, nullptr);
        partMember = nullptr;
    }
}

void IJoint::disconnect()
{
    disconnectFromPart(m_partA);
    disconnectFromPart(m_partB);
}

bool IJoint::isValid() const
{
    return m_partA != nullptr && m_partB != nullptr && m_partA != m_partB;
}

// ===== Factory Pattern =====
// Note: create(), availableTypes() et toVariant() sont maintenant fournis par FactoryMixin et Interface
// Note: Gestion de liste (add/remove/clear) maintenant fournie par ListManagerMixin

} // namespace Core
} // namespace LaserCutStudio
