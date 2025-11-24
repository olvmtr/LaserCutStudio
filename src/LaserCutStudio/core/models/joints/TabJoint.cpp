#include "core/models/joints/TabJoint.h"

namespace LaserCutStudio {
namespace Core {

TabJoint::TabJoint()
    : IJoint()
    , m_type(JointType::TAB)
    , m_partA(nullptr)
    , m_partB(nullptr)
    , m_position()
    , m_angle(0.0)
    , m_tabWidth(10.0)
    , m_tabDepth(3.0)
{
}

TabJoint::TabJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
                   double tabWidth, double tabDepth)
    : IJoint(JointType::TAB, partA, partB, position, angle)
    , m_type(JointType::TAB)
    , m_partA(nullptr)
    , m_partB(nullptr)
    , m_position(position)
    , m_angle(angle)
    , m_tabWidth(tabWidth)
    , m_tabDepth(tabDepth)
{
    connect(partA, partB);
}

TabJoint::TabJoint(const TabJoint& other)
    : IJoint(other)
    , m_type(other.m_type)
    , m_partA(other.m_partA)
    , m_partB(other.m_partB)
    , m_position(other.m_position)
    , m_angle(other.m_angle)
    , m_tabWidth(other.m_tabWidth)
    , m_tabDepth(other.m_tabDepth)
{
}

IMPLEMENT_CLONE(TabJoint, IJoint)

JointType TabJoint::getType() const
{
    return m_type;
}

void TabJoint::setType(JointType type)
{
    m_type = type;
}

IPart* TabJoint::getPartA() const
{
    return m_partA;
}

IPart* TabJoint::getPartB() const
{
    return m_partB;
}

void TabJoint::connect(IPart* partA, IPart* partB)
{
    disconnectFromPart(m_partA);
    disconnectFromPart(m_partB);
    connectToPart(m_partA, partA);
    connectToPart(m_partB, partB);
}

void TabJoint::disconnect()
{
    disconnectFromPart(m_partA);
    disconnectFromPart(m_partB);
}

Point3D TabJoint::getPosition() const
{
    return m_position;
}

void TabJoint::setPosition(const Point3D& position)
{
    m_position = position;
}

double TabJoint::getAngle() const
{
    return m_angle;
}

void TabJoint::setAngle(double angle)
{
    m_angle = angle;
}

bool TabJoint::isValid() const
{
    return m_partA != nullptr && m_partB != nullptr &&
           m_tabWidth > 0.0 && m_tabDepth > 0.0;
}

void TabJoint::setTabWidth(double width)
{
    updateProperty(m_tabWidth, width, &TabJoint::tabWidthChanged);
}

void TabJoint::setTabDepth(double depth)
{
    updateProperty(m_tabDepth, depth, &TabJoint::tabDepthChanged);
}

void TabJoint::connectToPart(IPart*& partMember, IPart* newPart)
{
    partMember = newPart;
    if (newPart) {
        newPart->addJoint(this);
        QObject::connect(newPart, &Interface::aboutToBeDestroyed,
                        this, [this, &partMember](Interface* destroyedPart) {
            if (partMember == destroyedPart) {
                partMember = nullptr;
            }
        });
    }
}

void TabJoint::disconnectFromPart(IPart*& partMember)
{
    if (partMember) {
        partMember->removeJoint(this);
        QObject::disconnect(partMember, nullptr, this, nullptr);
        partMember = nullptr;
    }
}

// Auto-enregistrement dans le Factory Pattern
const bool TabJoint::s_registered = IJoint::registerFactory<TabJoint>();

} // namespace Core
} // namespace LaserCutStudio
