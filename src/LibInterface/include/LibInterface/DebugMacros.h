#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H

/**
 * @file DebugMacros.h
 * @brief Macros de debug conditionnelles pour optimiser les builds Release
 *
 * Ce fichier garantit que la macro DEBUG est définie en mode Debug
 * pour permettre l'utilisation de #if DEBUG dans tout le code.
 */

#include <QLoggingCategory>

// ============================================================================
// DÉTECTION ET DÉFINITION DE DEBUG
// ============================================================================

// Détection automatique du mode Debug/Release
// Ordre de priorité : DEBUG (défini par CMake) > QT_DEBUG > _DEBUG > !NDEBUG
#if defined(DEBUG) || defined(QT_DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    #define LASERCUTSTUDIO_DEBUG 1
#else
    #define LASERCUTSTUDIO_DEBUG 0
#endif

// S'assurer que DEBUG est défini en mode Debug
#ifndef DEBUG
    #if LASERCUTSTUDIO_DEBUG
        #define DEBUG 1
    #endif
#endif

// ============================================================================
// UTILISATION
// ============================================================================

/**
 * Usage dans le code :
 *
 * @code
 * #if DEBUG
 *     qDebug() << "Debug message";
 *     validateInternalState();
 * #endif
 * @endcode
 *
 * En Release : Le code entre #if DEBUG/#endif est COMPLÈTEMENT RETIRÉ
 *              par le préprocesseur (0 overhead, pas dans le binaire)
 */

// ============================================================================
// MACROS D'OPTIMISATION (indépendantes de DEBUG)
// ============================================================================

/**
 * @brief Indique au compilateur qu'une condition est probable
 *
 * Exemple:
 * @code
 * if (LIKELY(pointer != nullptr)) {
 *     // Cas normal (optimisé par le compilateur)
 * } else {
 *     // Cas rare
 * }
 * @endcode
 */
#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

/**
 * @brief Marque une fonction comme inline forcé
 */
#if defined(_MSC_VER)
    #define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

/**
 * @brief Marque une variable/fonction comme inutilisée (évite warnings)
 */
#define UNUSED(x) (void)(x)

#endif // DEBUGMACROS_H
