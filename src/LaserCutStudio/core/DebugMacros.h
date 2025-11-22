/**
 * @file DebugMacros.h
 * @brief Macros pour le code conditionnel Debug/Release
 *
 * Ce fichier fournit des macros pour activer du code uniquement en Debug
 * ou uniquement en Release, permettant d'optimiser les performances en
 * production tout en gardant la testabilité en développement.
 */

#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

// ===== Détection du mode Debug =====
#if !defined(DEBUG) && (defined(_DEBUG) || defined(QT_DEBUG) || !defined(NDEBUG))
    #define DEBUG 1
#endif

#ifndef DEBUG
    #define DEBUG 0
#endif

// ===== Macros Qt conditionnelles =====

/**
 * @brief Q_INVOKABLE conditionnel - expose la méthode à QML uniquement en Debug
 *
 * Utilisation:
 * @code
 * class MyClass : public QObject {
 *     Q_OBJECT
 * public:
 *     // Méthode exposée à QML seulement en Debug (pour tests QML)
 *     Q_INVOKABLE_DEBUG QString debugInfo() const;
 *
 *     // Méthode toujours exposée à QML (utilisée par l'interface en production)
 *     Q_INVOKABLE QString getName() const;
 * };
 * @endcode
 *
 * @warning N'utilisez Q_INVOKABLE_DEBUG que pour les méthodes de test/debug!
 *          Si l'interface QML appelle cette méthode en production, ça cassera.
 */
#if DEBUG
    #define Q_INVOKABLE_DEBUG Q_INVOKABLE
#else
    #define Q_INVOKABLE_DEBUG
#endif

/**
 * @brief Définit une propriété Qt exposée uniquement en Debug
 *
 * Utilisation:
 * @code
 * class MyClass : public QObject {
 *     Q_OBJECT
 *     Q_PROPERTY_DEBUG(int debugCounter READ getDebugCounter)
 * public:
 *     int getDebugCounter() const { return m_debugCounter; }
 * private:
 *     #if DEBUG
 *         int m_debugCounter = 0;
 *     #endif
 * };
 * @endcode
 */
#if DEBUG
    #define Q_PROPERTY_DEBUG(...) Q_PROPERTY(__VA_ARGS__)
#else
    #define Q_PROPERTY_DEBUG(...)
#endif

// ===== Macros d'optimisation =====

/**
 * @brief Indique au compilateur qu'une condition est très probable
 * Utilisé pour optimiser les branches chaudes
 */
#ifdef __GNUC__
    #define LIKELY(x) __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x) (x)
    #define UNLIKELY(x) (x)
#endif

/**
 * @brief Force l'inlining d'une fonction
 */
#ifdef __GNUC__
    #define FORCE_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define FORCE_INLINE __forceinline
#else
    #define FORCE_INLINE inline
#endif

/**
 * @brief Marque une variable comme inutilisée (évite les warnings)
 */
#define UNUSED(x) (void)(x)

// ===== Assertions personnalisées =====

/**
 * @brief Assertion qui n'existe qu'en Debug
 * Avantage sur Q_ASSERT: 0 overhead en Release (complètement retiré)
 */
#if DEBUG
    #include <QDebug>
    #define DEBUG_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                qCritical() << "ASSERTION FAILED:" << message \
                           << "\nFile:" << __FILE__ \
                           << "\nLine:" << __LINE__; \
                Q_ASSERT(condition); \
            } \
        } while(0)
#else
    #define DEBUG_ASSERT(condition, message) do { } while(0)
#endif

/**
 * @brief Exécute du code uniquement en Debug
 *
 * Utilisation:
 * @code
 * DEBUG_ONLY(
 *     qDebug() << "Variable value:" << myVar;
 *     validateInvariants();
 * )
 * @endcode
 */
#if DEBUG
    #define DEBUG_ONLY(code) code
#else
    #define DEBUG_ONLY(code)
#endif

// ===== Catégories de logging conditionnelles =====

/**
 * @brief Déclare une catégorie de logging active uniquement en Debug
 *
 * Utilisation:
 * @code
 * Q_LOGGING_CATEGORY_DEBUG(lcDebugShapes, "lasercutstudio.debug.shapes")
 *
 * // En Debug: logs actifs
 * qCDebug(lcDebugShapes) << "Shape created:" << shape->getTypeName();
 *
 * // En Release: complètement retiré du binaire
 * @endcode
 */
#if DEBUG
    #define Q_LOGGING_CATEGORY_DEBUG(name, string) Q_LOGGING_CATEGORY(name, string)
#else
    #define Q_LOGGING_CATEGORY_DEBUG(name, string) \
        const QLoggingCategory name(nullptr); \
        Q_UNUSED(name)
#endif

#endif // DEBUG_MACROS_H
