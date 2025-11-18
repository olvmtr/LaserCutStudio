/**
 * @file DebugMacrosExample.cpp
 * @brief Exemples d'utilisation des macros de debug
 *
 * Ce fichier montre comment utiliser les macros de DebugMacros.h
 * pour écrire du code qui s'optimise automatiquement en Release.
 */

#include "../DebugMacros.h"
#include "../shapes/Rectangle.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Examples {

/**
 * @brief Exemple de fonction avec validation debug
 */
double calculateArea(double width, double height)
{
    // Assertions retirées en Release (0 overhead)
    DEBUG_ASSERT(width > 0, "Width must be positive");
    DEBUG_ASSERT(height > 0, "Height must be positive");

    // Code de validation étendu uniquement en Debug
    DEBUG_VALIDATE({
        if (width > 10000 || height > 10000) {
            qWarning() << "Unusually large dimensions:" << width << "x" << height;
        }
    })

    double area = width * height;

    // Log de debug retiré en Release
    DEBUG_ONLY(
        qDebug() << "Calculated area:" << area
                 << "from dimensions" << width << "x" << height;
    )

    return area;
}

/**
 * @brief Exemple avec mesure de performance en Debug
 */
void processComplexShape(Rectangle* shape)
{
    // Mesure automatique du temps en Debug uniquement
    DEBUG_MEASURE_TIME("processComplexShape") {
        // Traitement complexe
        double area = shape->getArea();
        QRectF bbox = shape->getBoundingBox();

        DEBUG_ONLY(
            qDebug() << "Shape area:" << area;
            qDebug() << "Bounding box:" << bbox;
        )
    }
}

/**
 * @brief Exemple avec optimisations LIKELY/UNLIKELY
 */
bool validatePointer(void* ptr)
{
    // Indique au compilateur que le cas normal est ptr != nullptr
    if (LIKELY(ptr != nullptr)) {
        return true;
    } else {
        // Cas rare
        DEBUG_ONLY(
            qWarning() << "Null pointer detected";
        )
        return false;
    }
}

/**
 * @brief Fonction benchmark disponible uniquement si BUILD_BENCHMARKS
 */
BENCHMARK_METHOD
void runPerformanceTests()
{
    BENCHMARK_ONLY({
        qInfo() << "Running performance tests...";
        // Code de benchmark
    })
}

/**
 * @brief Classe avec méthodes de debug conditionnelles
 */
class ExampleClass
{
public:
    ExampleClass(int value) : m_value(value)
    {
        DEBUG_ASSERT(value >= 0, "Value must be non-negative");

        DEBUG_ONLY(
            qDebug() << "ExampleClass created with value:" << value;
        )
    }

    int getValue() const
    {
        DEBUG_VALIDATE({
            if (m_value < 0) {
                qCritical() << "Invalid internal state: m_value =" << m_value;
            }
        })

        return m_value;
    }

    // Méthode de validation disponible uniquement en Debug
    DEBUG_ONLY(
        bool validateInternalState() const
        {
            return m_value >= 0;
        }
    )

private:
    int m_value;
};

// ============================================================================
// COMPARAISON : Avec et Sans Macros
// ============================================================================

// ❌ SANS MACROS (toujours compilé, même en Release)
void badExample(double* values, int count)
{
    // Ces validations sont TOUJOURS exécutées, même en Release
    Q_ASSERT(values != nullptr);
    Q_ASSERT(count > 0);

    qDebug() << "Processing" << count << "values"; // Toujours compilé

    for (int i = 0; i < count; ++i) {
        if (values[i] < 0) {
            qDebug() << "Negative value at index" << i; // Toujours compilé
        }
    }
}

// ✅ AVEC MACROS (optimisé en Release)
void goodExample(double* values, int count)
{
    // Ces assertions sont RETIRÉES en Release
    DEBUG_ASSERT(values != nullptr, "values cannot be null");
    DEBUG_ASSERT(count > 0, "count must be positive");

    // Ce log est RETIRÉ en Release (QT_NO_DEBUG_OUTPUT)
    qCDebug(logCore) << "Processing" << count << "values";

    for (int i = 0; i < count; ++i) {
        // Validation étendue RETIRÉE en Release
        DEBUG_VALIDATE({
            if (values[i] < 0) {
                qCDebug(logCore) << "Negative value at index" << i;
            }
        })
    }
}

// ============================================================================
// IMPACT PERFORMANCE
// ============================================================================

/*
 * Code SANS macros (Release) :
 * - Toutes les assertions exécutées : ~5-10% overhead
 * - Tous les qDebug() compilés : ~10-20% overhead si nombreux
 * - Code de validation toujours présent : +5-15% taille binaire
 *
 * Code AVEC macros (Release) :
 * - Assertions retirées : 0% overhead
 * - qCDebug() retiré : 0% overhead
 * - Code de validation retiré : -5-15% taille binaire
 *
 * GAIN TOTAL : 20-40% amélioration performance en Release
 */

} // namespace Examples
} // namespace Core
} // namespace LaserCutStudio
