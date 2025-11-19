#ifndef LISTMANAGERMIXIN_H
#define LISTMANAGERMIXIN_H

// Bridge vers LibInterface
#include <LibInterface/Patterns/ListManagerMixin.h>

namespace LaserCutStudio {
namespace Core {
namespace Patterns {

/**
 * @brief Alias vers LibInterface::Patterns::ListManagerMixin
 *
 * Ce fichier est un bridge vers la bibliothèque LibInterface.
 * Toute l'implémentation se trouve dans LibInterface.
 */
template<typename T>
using ListManagerMixin = ::LibInterface::Patterns::ListManagerMixin<T>;

} // namespace Patterns
} // namespace Core
} // namespace LaserCutStudio

#endif // LISTMANAGERMIXIN_H
