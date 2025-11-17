#include "TabJoint.h"

namespace LaserCutStudio {
namespace Core {

TabJoint::TabJoint()
    : IJoint()
    , m_tabWidth(10.0)
    , m_tabDepth(3.0)
{
    m_type = JointType::TAB;
}

TabJoint::TabJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
                   double tabWidth, double tabDepth)
    : IJoint(JointType::TAB, partA, partB, position, angle)
    , m_tabWidth(tabWidth)
    , m_tabDepth(tabDepth)
{
}

TabJoint::TabJoint(const TabJoint& other)
    : IJoint(other)
    , m_tabWidth(other.m_tabWidth)
    , m_tabDepth(other.m_tabDepth)
{
}

IJoint* TabJoint::clone() const
{
    return new TabJoint(*this);
}

} // namespace Core
} // namespace LaserCutStudio
