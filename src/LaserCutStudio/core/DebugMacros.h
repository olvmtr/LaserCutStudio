#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H

/**
 * @file DebugMacros.h
 * @brief Macros de debug conditionnelles pour optimiser les builds Release
 *
 * Ces macros permettent de retirer complètement du code de debug/validation
 * en mode Release pour optimiser les performances.
 */

// Détection automatique du mode Debug/Release
#if defined(QT_DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    #define LASERCUTSTUDIO_DEBUG 1
#else
    #define LASERCUTSTUDIO_DEBUG 0
#endif

// ============================================================================
// MACROS DE DEBUG
// ============================================================================

/**
 * @brief Exécute du code uniquement en Debug
 *
 * Exemple:
 * @code
 * DEBUG_ONLY(
 *     qDebug() << "Variable x =" << x;
 *     validateInternalState();
 * )
 * @endcode
 */
#if LASERCUTSTUDIO_DEBUG
    #define DEBUG_ONLY(code) code
#else
    #define DEBUG_ONLY(code)
#endif

/**
 * @brief Assertion qui ne s'exécute qu'en Debug
 *
 * Plus performant que Q_ASSERT car complètement retiré en Release.
 *
 * Exemple:
 * @code
 * DEBUG_ASSERT(pointer != nullptr, "Pointer must not be null");
 * DEBUG_ASSERT(width > 0 && height > 0, "Dimensions must be positive");
 * @endcode
 */
#if LASERCUTSTUDIO_DEBUG
    #define DEBUG_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                qCritical() << "ASSERTION FAILED:" << message \
                           << "\n  File:" << __FILE__ \
                           << "\n  Line:" << __LINE__ \
                           << "\n  Function:" << Q_FUNC_INFO; \
                Q_ASSERT(condition); \
            } \
        } while(0)
#else
    #define DEBUG_ASSERT(condition, message) ((void)0)
#endif

/**
 * @brief Validation étendue uniquement en Debug
 *
 * Exemple:
 * @code
 * DEBUG_VALIDATE({
 *     if (!isInternalStateValid()) {
 *         qWarning() << "Invalid internal state detected";
 *         return false;
 *     }
 * })
 * @endcode
 */
#if LASERCUTSTUDIO_DEBUG
    #define DEBUG_VALIDATE(code) code
#else
    #define DEBUG_VALIDATE(code)
#endif

/**
 * @brief Mesure de performance uniquement en Debug
 *
 * Exemple:
 * @code
 * DEBUG_MEASURE_TIME("Complex calculation") {
 *     // Code à mesurer
 *     complexCalculation();
 * }
 * @endcode
 */
#if LASERCUTSTUDIO_DEBUG
    #include <QElapsedTimer>
    #define DEBUG_MEASURE_TIME(label) \
        QElapsedTimer _debugTimer_##__LINE__; \
        _debugTimer_##__LINE__.start(); \
        auto _debugTimerGuard_##__LINE__ = qScopeGuard([&]() { \
            qDebug() << "[PERF]" << label << "took" \
                     << _debugTimer_##__LINE__.elapsed() << "ms"; \
        }); \
        if (true)
#else
    #define DEBUG_MEASURE_TIME(label) if (false)
#endif

// ============================================================================
// MACROS DE BENCHMARK
// ============================================================================

/**
 * @brief Exécute du code uniquement si les benchmarks sont activés
 *
 * Exemple:
 * @code
 * BENCHMARK_ONLY(
 *     BenchmarkShapes bench;
 *     bench.runAll();
 * )
 * @endcode
 */
#ifdef BUILD_BENCHMARKS
    #define BENCHMARK_ONLY(code) code
#else
    #define BENCHMARK_ONLY(code)
#endif

/**
 * @brief Fonction ou méthode disponible uniquement avec benchmarks
 *
 * Exemple:
 * @code
 * class MyClass {
 * public:
 *     BENCHMARK_METHOD void runBenchmark();
 * };
 * @endcode
 */
#ifdef BUILD_BENCHMARKS
    #define BENCHMARK_METHOD
#else
    #define BENCHMARK_METHOD [[maybe_unused]]
#endif

// ============================================================================
// MACROS D'OPTIMISATION
// ============================================================================

/**
 * @brief Indique au compilateur qu'une condition est probable
 *
 * Exemple:
 * @code
 * if (LIKELY(pointer != nullptr)) {
 *     // Cas normal
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
