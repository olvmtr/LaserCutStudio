#include "core/models/joints/IJoint.h"
#include "core/models/parts/IPart.h"

namespace LaserCutStudio {
namespace Core {

IJoint::IJoint()
    : Interface()
{
    registerInstance(this);
}

IJoint::IJoint(JointType type, IPart* partA, IPart* partB, const Point3D& position, double angle)
    : Interface()
{
    registerInstance(this);
}

IJoint::IJoint(const IJoint& other)
    : Interface(other)
{
    registerInstance(this);
}

IJoint::~IJoint()
{
    unregisterInstance(this);
}

} // namespace Core
} // namespace LaserCutStudio
