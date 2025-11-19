#!/bin/bash
# Script de création de la nouvelle structure de répertoires
# Architecture par couches : models / services / infrastructure / utils

set -e  # Arrêter en cas d'erreur

CORE_DIR="src/LaserCutStudio/core"

echo "==================================="
echo "Création de la nouvelle structure"
echo "==================================="

# Créer la structure models/
echo "Création de models/..."
mkdir -p "${CORE_DIR}/models/base/types"
mkdir -p "${CORE_DIR}/models/shapes/interfaces"
mkdir -p "${CORE_DIR}/models/shapes/implementations"
mkdir -p "${CORE_DIR}/models/parts/interfaces"
mkdir -p "${CORE_DIR}/models/parts/implementations"
mkdir -p "${CORE_DIR}/models/joints/interfaces"
mkdir -p "${CORE_DIR}/models/joints/implementations"
mkdir -p "${CORE_DIR}/models/projects/interfaces"
mkdir -p "${CORE_DIR}/models/projects/implementations"

# Créer la structure services/
echo "Création de services/..."
mkdir -p "${CORE_DIR}/services/geometry"
mkdir -p "${CORE_DIR}/services/validation"
mkdir -p "${CORE_DIR}/services/serialization"

# Créer la structure infrastructure/
echo "Création de infrastructure/..."
mkdir -p "${CORE_DIR}/infrastructure/patterns/factory"
mkdir -p "${CORE_DIR}/infrastructure/patterns/prototype"
mkdir -p "${CORE_DIR}/infrastructure/patterns/properties"
mkdir -p "${CORE_DIR}/infrastructure/patterns/lists"
mkdir -p "${CORE_DIR}/infrastructure/config"
mkdir -p "${CORE_DIR}/infrastructure/logging"
mkdir -p "${CORE_DIR}/infrastructure/di"
mkdir -p "${CORE_DIR}/infrastructure/plugins/core"
mkdir -p "${CORE_DIR}/infrastructure/plugins/interfaces"

# Créer la structure utils/
echo "Création de utils_new/..."
mkdir -p "${CORE_DIR}/utils_new/examples"

echo ""
echo "✅ Nouvelle structure créée avec succès !"
echo ""
echo "Arborescence créée :"
tree -d -L 4 "${CORE_DIR}/models" "${CORE_DIR}/services" "${CORE_DIR}/infrastructure" "${CORE_DIR}/utils_new" 2>/dev/null || find "${CORE_DIR}/models" "${CORE_DIR}/services" "${CORE_DIR}/infrastructure" "${CORE_DIR}/utils_new" -type d | sort

echo ""
echo "Prochaine étape : ./scripts/02_migrate_files.sh"
