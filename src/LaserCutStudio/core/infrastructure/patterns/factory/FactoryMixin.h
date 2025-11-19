#ifndef FACTORYMIXIN_H
#define FACTORYMIXIN_H

// Bridge vers LibInterface: LaserCutStudio::Core::Patterns::FactoryMixin est un alias de LibInterface::Patterns::FactoryMixin
#include <LibInterface/Patterns/FactoryMixin.h>

namespace LaserCutStudio {
namespace Core {
namespace Patterns {

/**
 * @brief Mixin CRTP pour fournir le Factory Pattern automatiquement
 *
 * Ce template est un alias vers LibInterface::Patterns::FactoryMixin.
 * L'implémentation réelle se trouve dans LibInterface (bibliothèque réutilisable).
 *
 * Usage:
 * @code
 * class IShape : public Interface, protected Patterns::FactoryMixin<IShape> {
 * public:
 *     using FactoryMixin<IShape>::create;
 *     using FactoryMixin<IShape>::availableTypes;
 *     using FactoryMixin<IShape>::registerFactory;
 * };
 * @endcode
 */
template<typename Base>
using FactoryMixin = ::LibInterface::Patterns::FactoryMixin<Base>;

} // namespace Patterns
} // namespace Core
} // namespace LaserCutStudio

#endif // FACTORYMIXIN_H
