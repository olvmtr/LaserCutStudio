#include "LogCategories.h"

namespace LaserCutStudio {
namespace Core {

/**
 * Définition des catégories de logging
 *
 * Format: Q_LOGGING_CATEGORY(nom, "identifiant.categorie")
 *
 * Contrôle depuis l'environnement :
 * export QT_LOGGING_RULES="lasercutstudio.core.shapes.debug=true"
 * export QT_LOGGING_RULES="lasercutstudio.core.*=true"
 * export QT_LOGGING_RULES="*.debug=false"
 */

Q_LOGGING_CATEGORY(logCore, "lasercutstudio.core")
Q_LOGGING_CATEGORY(logShapes, "lasercutstudio.core.shapes")
Q_LOGGING_CATEGORY(logParts, "lasercutstudio.core.parts")
Q_LOGGING_CATEGORY(logJoints, "lasercutstudio.core.joints")
Q_LOGGING_CATEGORY(logProjects, "lasercutstudio.core.projects")
Q_LOGGING_CATEGORY(logConfig, "lasercutstudio.core.config")
Q_LOGGING_CATEGORY(logPerformance, "lasercutstudio.performance")

} // namespace Core
} // namespace LaserCutStudio
