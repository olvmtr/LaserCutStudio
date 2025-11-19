#ifndef CLONABLEMIXIN_H
#define CLONABLEMIXIN_H

// Bridge vers LibInterface
// Ce fichier contient uniquement des macros, donc simple include
#include <LibInterface/Patterns/ClonableMixin.h>

/**
 * @file ClonableMixin.h
 * @brief Bridge vers LibInterface pour les macros IMPLEMENT_CLONE
 *
 * Ce fichier est un bridge vers la bibliothèque LibInterface.
 * Les macros IMPLEMENT_CLONE et IMPLEMENT_CLONE_INLINE sont définies
 * dans LibInterface/Patterns/ClonableMixin.h
 *
 * Usage:
 * @code
 * // Dans le .cpp
 * IMPLEMENT_CLONE(Rectangle, IShape)
 *
 * // Dans le .h (inline)
 * class MyClass : public IShape {
 *     IMPLEMENT_CLONE_INLINE(MyClass, IShape)
 * };
 * @endcode
 */

#endif // CLONABLEMIXIN_H
