#!/bin/bash
# Script de migration des fichiers vers la nouvelle structure
# Utilise 'git mv' pour préserver l'historique Git

set -e  # Arrêter en cas d'erreur

CORE_DIR="src/LaserCutStudio/core"

echo "============================================"
echo "Migration des fichiers (préserve historique Git)"
echo "============================================"
echo ""

# Vérifier que nous sommes dans un dépôt Git
if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    echo "❌ ERREUR : Vous devez être dans un dépôt Git"
    exit 1
fi

# Vérifier qu'il n'y a pas de modifications non commitées
if ! git diff-index --quiet HEAD --; then
    echo "⚠️  ATTENTION : Vous avez des modifications non commitées"
    echo "Veuillez commiter ou stasher vos changements avant la migration"
    exit 1
fi

echo "✅ Dépôt Git propre, migration en cours..."
echo ""

# ============================================================
# COUCHE MODELS
# ============================================================

echo "📦 Migration de models/base/..."
git mv "${CORE_DIR}/interface/Interface.h" "${CORE_DIR}/models/base/"
git mv "${CORE_DIR}/interface/Interface.cpp" "${CORE_DIR}/models/base/"
git mv "${CORE_DIR}/types/Point2D.h" "${CORE_DIR}/models/base/types/"
git mv "${CORE_DIR}/types/Point3D.h" "${CORE_DIR}/models/base/types/"
git mv "${CORE_DIR}/types/Material.h" "${CORE_DIR}/models/base/types/"
git mv "${CORE_DIR}/types/JointType.h" "${CORE_DIR}/models/base/types/"

echo "📦 Migration de models/shapes/..."
git mv "${CORE_DIR}/shapes/IShape.h" "${CORE_DIR}/models/shapes/interfaces/"
git mv "${CORE_DIR}/shapes/IShape.cpp" "${CORE_DIR}/models/shapes/interfaces/"
git mv "${CORE_DIR}/shapes/Rectangle.h" "${CORE_DIR}/models/shapes/implementations/"
git mv "${CORE_DIR}/shapes/Rectangle.cpp" "${CORE_DIR}/models/shapes/implementations/"
git mv "${CORE_DIR}/shapes/Circle.h" "${CORE_DIR}/models/shapes/implementations/"
git mv "${CORE_DIR}/shapes/Circle.cpp" "${CORE_DIR}/models/shapes/implementations/"

echo "📦 Migration de models/parts/..."
git mv "${CORE_DIR}/parts/IPart.h" "${CORE_DIR}/models/parts/interfaces/"
git mv "${CORE_DIR}/parts/IPart.cpp" "${CORE_DIR}/models/parts/interfaces/"
git mv "${CORE_DIR}/parts/Part.h" "${CORE_DIR}/models/parts/implementations/"
git mv "${CORE_DIR}/parts/Part.cpp" "${CORE_DIR}/models/parts/implementations/"

echo "📦 Migration de models/joints/..."
git mv "${CORE_DIR}/joints/IJoint.h" "${CORE_DIR}/models/joints/interfaces/"
git mv "${CORE_DIR}/joints/IJoint.cpp" "${CORE_DIR}/models/joints/interfaces/"
git mv "${CORE_DIR}/joints/TabJoint.h" "${CORE_DIR}/models/joints/implementations/"
git mv "${CORE_DIR}/joints/TabJoint.cpp" "${CORE_DIR}/models/joints/implementations/"
git mv "${CORE_DIR}/joints/FingerJoint.h" "${CORE_DIR}/models/joints/implementations/"
git mv "${CORE_DIR}/joints/FingerJoint.cpp" "${CORE_DIR}/models/joints/implementations/"

echo "📦 Migration de models/projects/..."
git mv "${CORE_DIR}/projects/IProject.h" "${CORE_DIR}/models/projects/interfaces/"
git mv "${CORE_DIR}/projects/IProject.cpp" "${CORE_DIR}/models/projects/interfaces/"
git mv "${CORE_DIR}/projects/Project.h" "${CORE_DIR}/models/projects/implementations/"
git mv "${CORE_DIR}/projects/Project.cpp" "${CORE_DIR}/models/projects/implementations/"

# ============================================================
# COUCHE SERVICES
# ============================================================

echo "🔧 Migration de services/geometry/..."
git mv "${CORE_DIR}/utils/GeometryUtils.h" "${CORE_DIR}/services/geometry/"

# ============================================================
# COUCHE INFRASTRUCTURE
# ============================================================

echo "⚙️  Migration de infrastructure/patterns/..."
git mv "${CORE_DIR}/patterns/FactoryMixin.h" "${CORE_DIR}/infrastructure/patterns/factory/"
git mv "${CORE_DIR}/patterns/ClonableMixin.h" "${CORE_DIR}/infrastructure/patterns/prototype/"
git mv "${CORE_DIR}/patterns/PropertyMixin.h" "${CORE_DIR}/infrastructure/patterns/properties/"
git mv "${CORE_DIR}/patterns/ListManagerMixin.h" "${CORE_DIR}/infrastructure/patterns/lists/"

echo "⚙️  Migration de infrastructure/config/..."
git mv "${CORE_DIR}/config/ConfigManager.h" "${CORE_DIR}/infrastructure/config/"
git mv "${CORE_DIR}/config/ConfigManager.cpp" "${CORE_DIR}/infrastructure/config/"

echo "⚙️  Migration de infrastructure/logging/..."
git mv "${CORE_DIR}/logging/LogManager.h" "${CORE_DIR}/infrastructure/logging/"
git mv "${CORE_DIR}/logging/LogManager.cpp" "${CORE_DIR}/infrastructure/logging/"
git mv "${CORE_DIR}/logging/LogCategories.h" "${CORE_DIR}/infrastructure/logging/"
git mv "${CORE_DIR}/logging/LogCategories.cpp" "${CORE_DIR}/infrastructure/logging/"

echo "⚙️  Migration de infrastructure/di/..."
git mv "${CORE_DIR}/di/ServiceLocator.h" "${CORE_DIR}/infrastructure/di/"
git mv "${CORE_DIR}/di/ServiceLocator.cpp" "${CORE_DIR}/infrastructure/di/"

echo "⚙️  Migration de infrastructure/plugins/..."
git mv "${CORE_DIR}/plugins/PluginManager.h" "${CORE_DIR}/infrastructure/plugins/core/"
git mv "${CORE_DIR}/plugins/PluginManager.cpp" "${CORE_DIR}/infrastructure/plugins/core/"
git mv "${CORE_DIR}/plugins/IShapePlugin.h" "${CORE_DIR}/infrastructure/plugins/interfaces/"
git mv "${CORE_DIR}/plugins/IJointPlugin.h" "${CORE_DIR}/infrastructure/plugins/interfaces/"

# ============================================================
# UTILITAIRES
# ============================================================

echo "🛠️  Migration de utils/..."
git mv "${CORE_DIR}/DebugMacros.h" "${CORE_DIR}/utils_new/"
git mv "${CORE_DIR}/examples/DebugExample.cpp" "${CORE_DIR}/utils_new/examples/"

echo ""
echo "✅ Migration des fichiers terminée avec succès !"
echo ""

# Supprimer les anciens répertoires vides
echo "🧹 Nettoyage des répertoires vides..."
rmdir "${CORE_DIR}/interface" 2>/dev/null || true
rmdir "${CORE_DIR}/types" 2>/dev/null || true
rmdir "${CORE_DIR}/shapes" 2>/dev/null || true
rmdir "${CORE_DIR}/parts" 2>/dev/null || true
rmdir "${CORE_DIR}/joints" 2>/dev/null || true
rmdir "${CORE_DIR}/projects" 2>/dev/null || true
rmdir "${CORE_DIR}/patterns" 2>/dev/null || true
rmdir "${CORE_DIR}/config" 2>/dev/null || true
rmdir "${CORE_DIR}/logging" 2>/dev/null || true
rmdir "${CORE_DIR}/di" 2>/dev/null || true
rmdir "${CORE_DIR}/plugins" 2>/dev/null || true
rmdir "${CORE_DIR}/examples" 2>/dev/null || true
rmdir "${CORE_DIR}/utils" 2>/dev/null || true

# Renommer utils_new en utils
git mv "${CORE_DIR}/utils_new" "${CORE_DIR}/utils"

echo ""
echo "📊 État des fichiers :"
git status --short

echo ""
echo "Prochaine étape : ./scripts/03_update_includes.sh"
