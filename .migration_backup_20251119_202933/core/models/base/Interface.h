#ifndef INTERFACE_H
#define INTERFACE_H

// Bridge vers LibInterface: LaserCutStudio::Core::Interface est un alias de LibInterface::Interface
#include <LibInterface/Interface.h>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Alias vers LibInterface::Interface
 *
 * Cette classe est un alias vers LibInterface::Interface pour maintenir la compatibilité
 * avec le code existant dans le namespace LaserCutStudio::Core.
 *
 * L'implémentation réelle vient de LibInterface (bibliothèque réutilisable).
 */
using Interface = ::LibInterface::Interface;

// DECLARE_TYPE_NAME est définie dans LibInterface/Interface.h

} // namespace Core
} // namespace LaserCutStudio

#endif // INTERFACE_H
