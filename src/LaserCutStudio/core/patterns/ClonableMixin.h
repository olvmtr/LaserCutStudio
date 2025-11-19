#ifndef CLONABLEMIXIN_H
#define CLONABLEMIXIN_H

namespace LaserCutStudio {
namespace Core {
namespace Patterns {

/**
 * @file ClonableMixin.h
 * @brief Macros pour simplifier l'implémentation de la méthode clone()
 *
 * Ce fichier fournit des macros pour implémenter automatiquement
 * la méthode clone() du pattern Prototype, éliminant la duplication
 * de code identique dans toutes les classes.
 */

/**
 * @brief Implémente la méthode clone() pour une classe
 *
 * Cette macro génère l'implémentation standard de clone() qui crée
 * une copie de l'objet via le constructeur de copie.
 *
 * Usage dans le .cpp :
 * @code
 * // Dans Rectangle.cpp
 * IMPLEMENT_CLONE(Rectangle, IShape)
 * @endcode
 *
 * Cette macro génère :
 * @code
 * IShape* Rectangle::clone() const {
 *     return new Rectangle(*this);
 * }
 * @endcode
 *
 * @param ClassName Nom de la classe concrète (ex: Rectangle)
 * @param BaseClass Nom de la classe de base retournée (ex: IShape)
 *
 * @note La classe doit avoir un constructeur de copie accessible
 * @note La macro doit être utilisée dans le namespace LaserCutStudio::Core
 */
#define IMPLEMENT_CLONE(ClassName, BaseClass) \
    BaseClass* ClassName::clone() const \
    { \
        return new ClassName(*this); \
    }

/**
 * @brief Implémente clone() inline dans le .h (pour classes template)
 *
 * Version inline de la macro IMPLEMENT_CLONE pour les cas où
 * l'implémentation doit être dans le header (classes template).
 *
 * Usage dans le .h :
 * @code
 * class MyClass : public IShape {
 *     IMPLEMENT_CLONE_INLINE(MyClass, IShape)
 * };
 * @endcode
 *
 * @param ClassName Nom de la classe concrète
 * @param BaseClass Nom de la classe de base retournée
 */
#define IMPLEMENT_CLONE_INLINE(ClassName, BaseClass) \
    BaseClass* clone() const override \
    { \
        return new ClassName(*this); \
    }

} // namespace Patterns
} // namespace Core
} // namespace LaserCutStudio

#endif // CLONABLEMIXIN_H
