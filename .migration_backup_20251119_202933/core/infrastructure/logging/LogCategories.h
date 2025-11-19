#ifndef LOGCATEGORIES_H
#define LOGCATEGORIES_H

#include <QLoggingCategory>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Catégories de logging pour le module Core
 *
 * Utilisation :
 * @code
 * qCDebug(logShapes) << "Creating rectangle:" << width << "x" << height;
 * qCInfo(logParts) << "Part created:" << part->getName();
 * qCWarning(logJoints) << "Invalid joint connection";
 * qCCritical(logProjects) << "Failed to save project:" << error;
 * @endcode
 */

// Catégorie générale Core
Q_DECLARE_LOGGING_CATEGORY(logCore)

// Catégories par sous-module
Q_DECLARE_LOGGING_CATEGORY(logShapes)
Q_DECLARE_LOGGING_CATEGORY(logParts)
Q_DECLARE_LOGGING_CATEGORY(logJoints)
Q_DECLARE_LOGGING_CATEGORY(logProjects)
Q_DECLARE_LOGGING_CATEGORY(logConfig)

// Catégorie pour les performances
Q_DECLARE_LOGGING_CATEGORY(logPerformance)

} // namespace Core
} // namespace LaserCutStudio

#endif // LOGCATEGORIES_H
