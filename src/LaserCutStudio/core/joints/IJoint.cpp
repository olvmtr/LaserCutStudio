#include "IJoint.h"
#include "../parts/IPart.h"

namespace LaserCutStudio {
namespace Core {

// Initialisation des listes statiques
QList<IJoint*> IJoint::s_joints;
QMap<QString, IJoint::FactoryFunc> IJoint::s_factories;

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

    // Ajoute ce joint aux pièces
    if (m_partA) {
        m_partA->addJoint(this);
    }
    if (m_partB) {
        m_partB->addJoint(this);
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
    // Retire le joint des anciennes pièces
    if (m_partA) {
        m_partA->removeJoint(this);
    }
    if (m_partB) {
        m_partB->removeJoint(this);
    }

    // Connecte aux nouvelles pièces
    m_partA = partA;
    m_partB = partB;

    if (m_partA) {
        m_partA->addJoint(this);
    }
    if (m_partB) {
        m_partB->addJoint(this);
    }
}

void IJoint::disconnect()
{
    if (m_partA) {
        m_partA->removeJoint(this);
        m_partA = nullptr;
    }
    if (m_partB) {
        m_partB->removeJoint(this);
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

void IJoint::notifyPartDestroyed(IPart* part)
{
    // Met à nullptr les pointeurs vers le Part détruit
    // sans essayer de manipuler le Part
    if (m_partA == part) {
        m_partA = nullptr;
    }
    if (m_partB == part) {
        m_partB = nullptr;
    }
}

// ===== Factory Pattern =====

IJoint* IJoint::create(const QVariantMap& config)
{
    QString type = config.value("type").toString();

    if (!s_factories.contains(type)) {
        qWarning() << "Unknown joint type:" << type;
        return nullptr;
    }

    return s_factories[type](config);
}

QStringList IJoint::availableTypes()
{
    return s_factories.keys();
}

QVariantMap IJoint::toVariant() const
{
    QVariantMap map;

    // Ajoute le type
    map["type"] = getTypeName();

    // Utilise le système Q_PROPERTY pour sérialiser automatiquement
    const QMetaObject* meta = metaObject();

    // Parcourt toutes les propriétés déclarées
    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);

        // Ne sérialise que les propriétés stockées (pas les calculées)
        if (prop.isStored()) {
            QString propName = QString::fromUtf8(prop.name());
            QVariant value = prop.read(this);

            // Exclut l'id (déjà géré par Interface) et objectName (interne Qt)
            if (propName != "id" && propName != "objectName") {
                map[propName] = value;
            }
        }
    }

    return map;
}

} // namespace Core
} // namespace LaserCutStudio
