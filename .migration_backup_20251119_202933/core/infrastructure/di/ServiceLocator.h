#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

// Bridge vers LibInterface
#include <LibInterface/DI/ServiceLocator.h>

namespace LaserCutStudio {
namespace Core {
namespace DI {

/**
 * @brief Alias vers LibInterface::DI::ServiceLocator
 *
 * Ce fichier est un bridge vers la bibliothèque LibInterface.
 * Toute l'implémentation se trouve dans LibInterface.
 *
 * Usage:
 * @code
 * // Enregistrer un service Singleton
 * ServiceLocator::instance().registerSingleton<IConfig>([]() {
 *     return new ConfigManager();
 * });
 *
 * // Résoudre une dépendance
 * IConfig* config = ServiceLocator::instance().resolve<IConfig>();
 * @endcode
 */
using ServiceLocator = ::LibInterface::DI::ServiceLocator;

} // namespace DI
} // namespace Core
} // namespace LaserCutStudio

#endif // SERVICELOCATOR_H
