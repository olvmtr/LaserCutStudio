#include "FingerJoint.h"

namespace LaserCutStudio {
namespace Core {

FingerJoint::FingerJoint()
    : IJoint()
    , m_fingerCount(5)
    , m_fingerWidth(5.0)
{
    m_type = JointType::FINGER;
}

FingerJoint::FingerJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
                         int fingerCount, double fingerWidth)
    : IJoint(JointType::FINGER, partA, partB, position, angle)
    , m_fingerCount(fingerCount)
    , m_fingerWidth(fingerWidth)
{
}

FingerJoint::FingerJoint(const FingerJoint& other)
    : IJoint(other)
    , m_fingerCount(other.m_fingerCount)
    , m_fingerWidth(other.m_fingerWidth)
{
}

IJoint* FingerJoint::clone() const
{
    return new FingerJoint(*this);
}

} // namespace Core
} // namespace LaserCutStudio
