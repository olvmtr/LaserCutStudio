#include "core/models/parts/implementations/Part.h"

namespace LaserCutStudio {
namespace Core {

Part::Part()
    : IPart()
{
}

Part::Part(const QString& name, IShape* shape, double thickness, const Material& material)
    : IPart(name, shape, thickness, material)
{
}

Part::Part(const Part& other)
    : IPart(other)
{
}

IPart* Part::clone() const
{
    return new Part(*this);
}

// Auto-enregistrement dans le Factory Pattern
const bool Part::s_registered = IPart::registerFactory<Part>();

} // namespace Core
} // namespace LaserCutStudio
