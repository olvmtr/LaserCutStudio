#include "core/models/joints/FingerJoint.h"
#include "core/models/parts/IPart.h"

namespace LaserCutStudio {
namespace Core {

FingerJoint::FingerJoint()
    : IJoint()
    , m_type(JointType::FINGER)
    , m_partA(nullptr)
    , m_partB(nullptr)
    , m_position()
    , m_angle(0.0)
    , m_fingerCount(5)
    , m_fingerWidth(5.0)
{
}

FingerJoint::FingerJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
                         int fingerCount, double fingerWidth)
    : IJoint(JointType::FINGER, partA, partB, position, angle)
    , m_type(JointType::FINGER)
    , m_partA(nullptr)
    , m_partB(nullptr)
    , m_position(position)
    , m_angle(angle)
    , m_fingerCount(fingerCount)
    , m_fingerWidth(fingerWidth)
{
    connect(partA, partB);
}

FingerJoint::FingerJoint(const FingerJoint& other)
    : IJoint(other)
    , m_type(other.m_type)
    , m_partA(other.m_partA)
    , m_partB(other.m_partB)
    , m_position(other.m_position)
    , m_angle(other.m_angle)
    , m_fingerCount(other.m_fingerCount)
    , m_fingerWidth(other.m_fingerWidth)
{
}

IMPLEMENT_CLONE(FingerJoint, IJoint)

JointType FingerJoint::getType() const
{
    return m_type;
}

void FingerJoint::setType(JointType type)
{
    m_type = type;
}

IPart* FingerJoint::getPartA() const
{
    return m_partA;
}

IPart* FingerJoint::getPartB() const
{
    return m_partB;
}

void FingerJoint::connect(IPart* partA, IPart* partB)
{
    disconnectFromPart(m_partA);
    disconnectFromPart(m_partB);
    connectToPart(m_partA, partA);
    connectToPart(m_partB, partB);
}

void FingerJoint::disconnect()
{
    disconnectFromPart(m_partA);
    disconnectFromPart(m_partB);
}

Point3D FingerJoint::getPosition() const
{
    return m_position;
}

void FingerJoint::setPosition(const Point3D& position)
{
    m_position = position;
}

double FingerJoint::getAngle() const
{
    return m_angle;
}

void FingerJoint::setAngle(double angle)
{
    m_angle = angle;
}

bool FingerJoint::isValid() const
{
    return m_partA != nullptr && m_partB != nullptr &&
           m_fingerCount > 0 && m_fingerWidth > 0.0;
}

void FingerJoint::setFingerCount(int count)
{
    updateProperty(m_fingerCount, count, &FingerJoint::fingerCountChanged);
}

void FingerJoint::setFingerWidth(double width)
{
    updateProperty(m_fingerWidth, width, &FingerJoint::fingerWidthChanged);
}

void FingerJoint::connectToPart(IPart*& partMember, IPart* newPart)
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

void FingerJoint::disconnectFromPart(IPart*& partMember)
{
    if (partMember) {
        partMember->removeJoint(this);
        QObject::disconnect(partMember, nullptr, this, nullptr);
        partMember = nullptr;
    }
}

// Auto-enregistrement dans le Factory Pattern
const bool FingerJoint::s_registered = IJoint::registerFactory<FingerJoint>();

} // namespace Core
} // namespace LaserCutStudio
