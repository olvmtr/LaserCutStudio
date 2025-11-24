#include "core/models/parts/Part.h"

namespace LaserCutStudio {
namespace Core {

Part::Part()
    : IPart()
    , m_name("")
    , m_shape(nullptr)
    , m_thickness(0.0)
    , m_material(Material())
    , m_joints()
{
}

Part::Part(const QString& name, IShape* shape, double thickness, const Material& material)
    : IPart(name, shape, thickness, material)
    , m_name(name)
    , m_shape(shape)
    , m_thickness(thickness)
    , m_material(material)
    , m_joints()
{
}

Part::Part(const Part& other)
    : IPart(other)
    , m_name(other.m_name)
    , m_shape(other.m_shape ? other.m_shape->clone() : nullptr)
    , m_thickness(other.m_thickness)
    , m_material(other.m_material)
    , m_joints(other.m_joints)
{
}

IMPLEMENT_CLONE(Part, IPart)

QString Part::getName() const
{
    return m_name;
}

void Part::setName(const QString& name)
{
    m_name = name;
}

IShape* Part::getShape() const
{
    return m_shape;
}

void Part::setShape(IShape* shape)
{
    m_shape = shape;
}

double Part::getThickness() const
{
    return m_thickness;
}

void Part::setThickness(double thickness)
{
    m_thickness = thickness;
}

Material Part::getMaterial() const
{
    return m_material;
}

void Part::setMaterial(const Material& material)
{
    m_material = material;
}

void Part::addJoint(IJoint* joint)
{
    if (joint && !m_joints.contains(joint)) {
        m_joints.append(joint);
    }
}

void Part::removeJoint(IJoint* joint)
{
    m_joints.removeAll(joint);
}

QList<IJoint*> Part::getJoints() const
{
    return m_joints;
}

double Part::getVolume() const
{
    if (!m_shape) return 0.0;
    return m_shape->getArea() * m_thickness;
}

double Part::getMass() const
{
    return getVolume() * m_material.getDensity() / 1000.0;
}

// Auto-enregistrement dans le Factory Pattern
const bool Part::s_registered = IPart::registerFactory<Part>();

} // namespace Core
} // namespace LaserCutStudio
