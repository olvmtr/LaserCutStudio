#!/bin/bash
# Script de mise à jour des #include dans tous les fichiers sources
# Remplace les anciens chemins relatifs par les nouveaux chemins absolus

set -e  # Arrêter en cas d'erreur

CORE_DIR="src/LaserCutStudio/core"
TEST_DIR="src/LaserCutStudio/tests"

echo "============================================"
echo "Mise à jour des #include"
echo "============================================"
echo ""

# Créer une sauvegarde avant modification
BACKUP_DIR=".migration_backup_$(date +%Y%m%d_%H%M%S)"
echo "📦 Création d'une sauvegarde dans ${BACKUP_DIR}..."
mkdir -p "${BACKUP_DIR}"
cp -r "${CORE_DIR}" "${BACKUP_DIR}/"
cp -r "${TEST_DIR}" "${BACKUP_DIR}/" 2>/dev/null || true
echo "✅ Sauvegarde créée"
echo ""

# Fonction pour remplacer les includes dans un fichier
update_includes() {
    local file="$1"
    echo "  Traitement : ${file}"

    # Modèle → Models
    sed -i 's|#include "../interface/Interface.h"|#include "core/models/base/Interface.h"|g' "$file"
    sed -i 's|#include "interface/Interface.h"|#include "core/models/base/Interface.h"|g' "$file"

    sed -i 's|#include "../types/Point2D.h"|#include "core/models/base/types/Point2D.h"|g' "$file"
    sed -i 's|#include "../types/Point3D.h"|#include "core/models/base/types/Point3D.h"|g' "$file"
    sed -i 's|#include "../types/Material.h"|#include "core/models/base/types/Material.h"|g' "$file"
    sed -i 's|#include "../types/JointType.h"|#include "core/models/base/types/JointType.h"|g' "$file"

    sed -i 's|#include "IShape.h"|#include "core/models/shapes/interfaces/IShape.h"|g' "$file"
    sed -i 's|#include "../shapes/IShape.h"|#include "core/models/shapes/interfaces/IShape.h"|g' "$file"
    sed -i 's|#include "Rectangle.h"|#include "core/models/shapes/implementations/Rectangle.h"|g' "$file"
    sed -i 's|#include "Circle.h"|#include "core/models/shapes/implementations/Circle.h"|g' "$file"

    sed -i 's|#include "IPart.h"|#include "core/models/parts/interfaces/IPart.h"|g' "$file"
    sed -i 's|#include "../parts/IPart.h"|#include "core/models/parts/interfaces/IPart.h"|g' "$file"
    sed -i 's|#include "Part.h"|#include "core/models/parts/implementations/Part.h"|g' "$file"

    sed -i 's|#include "IJoint.h"|#include "core/models/joints/interfaces/IJoint.h"|g' "$file"
    sed -i 's|#include "../joints/IJoint.h"|#include "core/models/joints/interfaces/IJoint.h"|g' "$file"
    sed -i 's|#include "TabJoint.h"|#include "core/models/joints/implementations/TabJoint.h"|g' "$file"
    sed -i 's|#include "FingerJoint.h"|#include "core/models/joints/implementations/FingerJoint.h"|g' "$file"

    sed -i 's|#include "IProject.h"|#include "core/models/projects/interfaces/IProject.h"|g' "$file"
    sed -i 's|#include "../projects/IProject.h"|#include "core/models/projects/interfaces/IProject.h"|g' "$file"
    sed -i 's|#include "Project.h"|#include "core/models/projects/implementations/Project.h"|g' "$file"

    # Services
    sed -i 's|#include "../utils/GeometryUtils.h"|#include "core/services/geometry/GeometryUtils.h"|g' "$file"
    sed -i 's|#include "utils/GeometryUtils.h"|#include "core/services/geometry/GeometryUtils.h"|g' "$file"

    # Infrastructure - Patterns
    sed -i 's|#include "../patterns/FactoryMixin.h"|#include "core/infrastructure/patterns/factory/FactoryMixin.h"|g' "$file"
    sed -i 's|#include "patterns/FactoryMixin.h"|#include "core/infrastructure/patterns/factory/FactoryMixin.h"|g' "$file"

    sed -i 's|#include "../patterns/ClonableMixin.h"|#include "core/infrastructure/patterns/prototype/ClonableMixin.h"|g' "$file"
    sed -i 's|#include "patterns/ClonableMixin.h"|#include "core/infrastructure/patterns/prototype/ClonableMixin.h"|g' "$file"

    sed -i 's|#include "../patterns/PropertyMixin.h"|#include "core/infrastructure/patterns/properties/PropertyMixin.h"|g' "$file"
    sed -i 's|#include "patterns/PropertyMixin.h"|#include "core/infrastructure/patterns/properties/PropertyMixin.h"|g' "$file"

    sed -i 's|#include "../patterns/ListManagerMixin.h"|#include "core/infrastructure/patterns/lists/ListManagerMixin.h"|g' "$file"
    sed -i 's|#include "patterns/ListManagerMixin.h"|#include "core/infrastructure/patterns/lists/ListManagerMixin.h"|g' "$file"

    # Infrastructure - Config
    sed -i 's|#include "../config/ConfigManager.h"|#include "core/infrastructure/config/ConfigManager.h"|g' "$file"
    sed -i 's|#include "config/ConfigManager.h"|#include "core/infrastructure/config/ConfigManager.h"|g' "$file"

    # Infrastructure - Logging
    sed -i 's|#include "../logging/LogManager.h"|#include "core/infrastructure/logging/LogManager.h"|g' "$file"
    sed -i 's|#include "../logging/LogCategories.h"|#include "core/infrastructure/logging/LogCategories.h"|g' "$file"
    sed -i 's|#include "logging/LogManager.h"|#include "core/infrastructure/logging/LogManager.h"|g' "$file"
    sed -i 's|#include "logging/LogCategories.h"|#include "core/infrastructure/logging/LogCategories.h"|g' "$file"

    # Infrastructure - DI
    sed -i 's|#include "../di/ServiceLocator.h"|#include "core/infrastructure/di/ServiceLocator.h"|g' "$file"
    sed -i 's|#include "di/ServiceLocator.h"|#include "core/infrastructure/di/ServiceLocator.h"|g' "$file"

    # Infrastructure - Plugins
    sed -i 's|#include "../plugins/PluginManager.h"|#include "core/infrastructure/plugins/core/PluginManager.h"|g' "$file"
    sed -i 's|#include "plugins/PluginManager.h"|#include "core/infrastructure/plugins/core/PluginManager.h"|g' "$file"
    sed -i 's|#include "../plugins/IShapePlugin.h"|#include "core/infrastructure/plugins/interfaces/IShapePlugin.h"|g' "$file"
    sed -i 's|#include "plugins/IShapePlugin.h"|#include "core/infrastructure/plugins/interfaces/IShapePlugin.h"|g' "$file"
    sed -i 's|#include "../plugins/IJointPlugin.h"|#include "core/infrastructure/plugins/interfaces/IJointPlugin.h"|g' "$file"
    sed -i 's|#include "plugins/IJointPlugin.h"|#include "core/infrastructure/plugins/interfaces/IJointPlugin.h"|g' "$file"

    # Utils
    sed -i 's|#include "DebugMacros.h"|#include "core/utils/DebugMacros.h"|g' "$file"
    sed -i 's|#include "../DebugMacros.h"|#include "core/utils/DebugMacros.h"|g' "$file"
    sed -i 's|#include "core/DebugMacros.h"|#include "core/utils/DebugMacros.h"|g' "$file"
}

# Traiter tous les fichiers .h et .cpp du core/
echo "🔄 Mise à jour des includes dans core/..."
find "${CORE_DIR}" -type f \( -name "*.h" -o -name "*.cpp" \) | while read -r file; do
    update_includes "$file"
done

# Traiter tous les fichiers de tests
echo ""
echo "🔄 Mise à jour des includes dans tests/..."
if [ -d "${TEST_DIR}" ]; then
    find "${TEST_DIR}" -type f \( -name "*.h" -o -name "*.cpp" \) | while read -r file; do
        update_includes "$file"
    done
fi

# Traiter main.cpp
echo ""
echo "🔄 Mise à jour de main.cpp..."
if [ -f "src/LaserCutStudio/main.cpp" ]; then
    update_includes "src/LaserCutStudio/main.cpp"
fi

echo ""
echo "✅ Mise à jour des includes terminée !"
echo ""
echo "📋 Résumé :"
echo "  - Sauvegarde : ${BACKUP_DIR}"
echo "  - Fichiers modifiés : $(find ${CORE_DIR} ${TEST_DIR} -type f \( -name "*.h" -o -name "*.cpp" \) 2>/dev/null | wc -l)"
echo ""
echo "Prochaine étape : ./scripts/04_update_cmake.sh"
