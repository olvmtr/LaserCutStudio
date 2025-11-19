#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H

// Bridge vers LibInterface
// Ce fichier contient uniquement des macros, donc simple include
#include <LibInterface/DebugMacros.h>

/**
 * @file DebugMacros.h
 * @brief Bridge vers LibInterface pour les macros de debug
 *
 * Ce fichier est un bridge vers la bibliothèque LibInterface.
 * Les macros DEBUG, LIKELY, UNLIKELY, FORCE_INLINE, etc. sont définies
 * dans LibInterface/DebugMacros.h
 *
 * Usage:
 * @code
 * #if DEBUG
 *     qDebug() << "Debug message";
 * #endif
 *
 * if (LIKELY(pointer != nullptr)) {
 *     // Code optimisé
 * }
 * @endcode
 */

#endif // DEBUGMACROS_H
