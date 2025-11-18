#include "IPart.h"
#include "../joints/IJoint.h"

namespace LaserCutStudio {
namespace Core {

// Initialisation des listes statiques
QList<IPart*> IPart::s_parts;
QMap<QString, IPart::FactoryFunc> IPart::s_factories;

IPart::IPart()
    : Interface()
    , m_name("Unnamed Part")
    , m_shape(nullptr)
    , m_thickness(3.0)
    , m_material(Material::Plywood())
{
    addPart(this);
}

IPart::IPart(const QString& name, IShape* shape, double thickness, const Material& material)
    : Interface()
    , m_name(name)
    , m_shape(shape)
    , m_thickness(thickness)
    , m_material(material)
{
    addPart(this);
}

IPart::IPart(const IPart& other)
    : Interface(other)
    , m_name(other.m_name)
    , m_shape(other.m_shape ? other.m_shape->clone() : nullptr)
    , m_thickness(other.m_thickness)
    , m_material(other.m_material)
    , m_joints(other.m_joints) // Copie la liste de joints
{
    addPart(this);
}

IPart::~IPart()
{
    // Notifie tous les joints connectés que ce Part va être détruit
    // On fait une copie de la liste car removeJoint() modifie m_joints
    QList<IJoint*> jointsCopy = m_joints;
    for (IJoint* joint : jointsCopy) {
        if (joint) {
            // Déconnecte ce part du joint sans appeler removeJoint
            // pour éviter de modifier m_joints pendant l'itération
            if (joint->getPartA() == this) {
                // Met le pointeur à nullptr dans le joint
                joint->connect(nullptr, joint->getPartB());
            }
            if (joint->getPartB() == this) {
                // Met le pointeur à nullptr dans le joint
                joint->connect(joint->getPartA(), nullptr);
            }
        }
    }
    m_joints.clear();

    removePart(this);
}

void IPart::addJoint(IJoint* joint)
{
    if (joint && !m_joints.contains(joint)) {
        m_joints.append(joint);
    }
}

void IPart::removeJoint(IJoint* joint)
{
    m_joints.removeAll(joint);
}

double IPart::getVolume() const
{
    if (!m_shape) {
        return 0.0;
    }
    // Volume = aire de la forme × épaisseur
    return m_shape->getArea() * m_thickness;
}

double IPart::getMass() const
{
    // Masse = volume × densité du matériau
    // Volume en mm³, densité en kg/m³
    // Conversion: 1 m³ = 1e9 mm³
    double volumeM3 = getVolume() / 1e9;
    return volumeM3 * m_material.getDensity();
}

void IPart::addPart(IPart* part)
{
    if (part && !s_parts.contains(part)) {
        s_parts.append(part);
    }
}

void IPart::removePart(IPart* part)
{
    s_parts.removeAll(part);
}

void IPart::clearAllParts()
{
    s_parts.clear();
}

// ===== Factory Pattern =====

IPart* IPart::create(const QVariantMap& config)
{
    QString type = config.value("type").toString();

    if (!s_factories.contains(type)) {
        qWarning() << "Unknown part type:" << type;
        return nullptr;
    }

    return s_factories[type](config);
}

QStringList IPart::availableTypes()
{
    return s_factories.keys();
}

QVariantMap IPart::toVariant() const
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
