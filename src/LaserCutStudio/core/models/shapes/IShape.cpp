#include "core/models/shapes/IShape.h"

namespace LaserCutStudio {
namespace Core {

// Note: s_factories et s_instances sont maintenant dans FactoryMixin et ListManagerMixin
// et initialisés automatiquement

IShape::IShape()
    : Interface()
{
    // Enregistre automatiquement cette forme dans la liste via ListManagerMixin
    registerInstance(this);
}

IShape::~IShape()
{
    // Désenregistre automatiquement cette forme de la liste via ListManagerMixin
    unregisterInstance(this);
}

// ===== Factory Pattern =====
// Note: create(), availableTypes() et toVariant() sont maintenant fournis par FactoryMixin et Interface

} // namespace Core
} // namespace LaserCutStudio
