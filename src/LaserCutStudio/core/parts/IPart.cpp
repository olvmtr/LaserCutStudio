#include "IPart.h"
#include "../joints/IJoint.h"

namespace LaserCutStudio {
namespace Core {

// Initialisation de la liste statique
QList<IPart*> IPart::s_parts;
// Note: s_factories est maintenant dans FactoryMixin et initialisé automatiquement

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
    // Le signal aboutToBeDestroyed() de Interface est automatiquement émis
    // et les joints connectés via QObject::connect() reçoivent la notification
    // et nettoient leurs pointeurs automatiquement. Plus besoin de notification manuelle!

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
// Note: create(), availableTypes() et toVariant() sont maintenant fournis par FactoryMixin et Interface

} // namespace Core
} // namespace LaserCutStudio
