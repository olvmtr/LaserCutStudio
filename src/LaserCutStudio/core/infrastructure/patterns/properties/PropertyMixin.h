#ifndef PROPERTYMIXIN_H
#define PROPERTYMIXIN_H

// Bridge vers LibInterface
#include <LibInterface/Patterns/PropertyMixin.h>

namespace LaserCutStudio {
namespace Core {
namespace Patterns {

/**
 * @brief Alias vers LibInterface::Patterns::PropertyMixin
 *
 * Ce fichier est un bridge vers la bibliothèque LibInterface.
 * Toute l'implémentation se trouve dans LibInterface.
 */
template<typename Derived>
using PropertyMixin = ::LibInterface::Patterns::PropertyMixin<Derived>;

} // namespace Patterns
} // namespace Core
} // namespace LaserCutStudio

#endif // PROPERTYMIXIN_H
