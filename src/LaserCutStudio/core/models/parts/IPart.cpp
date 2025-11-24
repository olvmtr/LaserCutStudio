#include "core/models/parts/IPart.h"
#include "core/models/joints/IJoint.h"

namespace LaserCutStudio {
namespace Core {

IPart::IPart()
    : Interface()
{
    registerInstance(this);
}

IPart::IPart(const QString& name, IShape* shape, double thickness, const Material& material)
    : Interface()
{
    registerInstance(this);
}

IPart::IPart(const IPart& other)
    : Interface(other)
{
    registerInstance(this);
}

IPart::~IPart()
{
    unregisterInstance(this);
}

} // namespace Core
} // namespace LaserCutStudio
