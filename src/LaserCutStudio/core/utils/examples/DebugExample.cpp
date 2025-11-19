/**
 * @file DebugExample.cpp
 * @brief Exemples d'utilisation de #if DEBUG
 */

#include "core/utils/DebugMacros.h"
#include "../shapes/Rectangle.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Examples {

/**
 * @brief Exemple simple avec logs debug
 */
double calculateArea(double width, double height)
{
    #if DEBUG
        // Validations en debug uniquement
        if (width <= 0 || height <= 0) {
            qCritical() << "Invalid dimensions:" << width << "x" << height;
            return 0.0;
        }

        if (width > 10000 || height > 10000) {
            qWarning() << "Unusually large dimensions:" << width << "x" << height;
        }
    #endif

    double area = width * height;

    #if DEBUG
        qDebug() << "Calculated area:" << area
                 << "from dimensions" << width << "x" << height;
    #endif

    return area;
}

/**
 * @brief Exemple avec fonction debug
 */
void processComplexShape(Rectangle* shape)
{
    #if DEBUG
        qDebug() << "Processing shape...";
        auto startTime = QDateTime::currentMSecsSinceEpoch();
    #endif

    // Traitement
    double area = shape->getArea();
    QRectF bbox = shape->getBoundingBox();

    #if DEBUG
        auto elapsed = QDateTime::currentMSecsSinceEpoch() - startTime;
        qDebug() << "Shape area:" << area;
        qDebug() << "Bounding box:" << bbox;
        qDebug() << "Processing took" << elapsed << "ms";
    #endif
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
        #if DEBUG
            qWarning() << "Null pointer detected";
        #endif
        return false;
    }
}

/**
 * @brief Classe avec méthodes debug
 */
class ExampleClass
{
public:
    ExampleClass(int value) : m_value(value)
    {
        #if DEBUG
            if (value < 0) {
                qCritical() << "Value must be non-negative:" << value;
            }
            qDebug() << "ExampleClass created with value:" << value;
        #endif
    }

    int getValue() const
    {
        #if DEBUG
            if (m_value < 0) {
                qCritical() << "Invalid internal state: m_value =" << m_value;
            }
        #endif
        return m_value;
    }

    // Méthodes disponibles uniquement en Debug
    #if DEBUG
        void validateInternalState() const
        {
            if (m_value < 0) {
                qCritical() << "Invalid state detected!";
            }
        }

        void dumpDebugInfo() const
        {
            qDebug() << "ExampleClass debug info:";
            qDebug() << "  - m_value =" << m_value;
        }
    #endif

private:
    int m_value;
};

/**
 * @brief Variables conditionnelles
 */
void statisticsExample()
{
    #if DEBUG
        int debugCounter = 0;
        int errorCount = 0;
    #endif

    for (int i = 0; i < 100; ++i) {
        bool success = processItem(i);

        #if DEBUG
            debugCounter++;
            if (!success) {
                errorCount++;
            }
        #endif
    }

    #if DEBUG
        qDebug() << "Processed" << debugCounter << "items";
        qDebug() << "Errors:" << errorCount;
    #endif
}

/**
 * @brief Code complexe uniquement en debug
 */
class DebugTracker
{
    #if DEBUG
    public:
        static DebugTracker& instance()
        {
            static DebugTracker tracker;
            return tracker;
        }

        void trackAllocation(void* ptr, size_t size)
        {
            m_allocations[ptr] = size;
            qDebug() << "Allocated" << size << "bytes at" << ptr;
        }

        void trackDeallocation(void* ptr)
        {
            auto it = m_allocations.find(ptr);
            if (it != m_allocations.end()) {
                qDebug() << "Deallocated" << it->second << "bytes at" << ptr;
                m_allocations.erase(it);
            }
        }

        void dumpLeaks()
        {
            if (!m_allocations.empty()) {
                qWarning() << "Memory leaks detected:";
                for (const auto& [ptr, size] : m_allocations) {
                    qWarning() << "  -" << size << "bytes at" << ptr;
                }
            }
        }

    private:
        std::map<void*, size_t> m_allocations;
    #endif
};

/**
 * @brief Impact performance
 */
void performanceCriticalFunction()
{
    // Ce code est identique en Debug et Release
    for (int i = 0; i < 1000000; ++i) {
        processData(i);
    }

    #if DEBUG
        // Ce code est COMPLÈTEMENT RETIRÉ en Release
        // 0 overhead, pas dans le binaire, pas exécuté
        qDebug() << "Processed 1M items";
        validateAllData();
    #endif
}

} // namespace Examples
} // namespace Core
} // namespace LaserCutStudio
