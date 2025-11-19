#!/bin/bash
# Script de validation de la migration
# Vérifie que tout fonctionne correctement après migration

set -e

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║             Validation de la Migration                        ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

BUILD_DIR="build/Desktop-Debug"
SUCCESS_COUNT=0
TOTAL_CHECKS=7

# Fonction pour afficher le résultat
check() {
    local name="$1"
    local cmd="$2"

    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "✓ Vérification : ${name}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    if eval "$cmd"; then
        echo "✅ PASS : ${name}"
        ((SUCCESS_COUNT++))
    else
        echo "❌ FAIL : ${name}"
    fi
    echo ""
}

# 1. Vérifier l'existence des répertoires
check "Structure des répertoires" \
    "[ -d src/LaserCutStudio/core/models ] && \
     [ -d src/LaserCutStudio/core/services ] && \
     [ -d src/LaserCutStudio/core/infrastructure ] && \
     [ -d src/LaserCutStudio/core/utils ]"

# 2. Vérifier que les anciens répertoires sont supprimés
check "Nettoyage des anciens répertoires" \
    "[ ! -d src/LaserCutStudio/core/interface ] && \
     [ ! -d src/LaserCutStudio/core/shapes ] && \
     [ ! -d src/LaserCutStudio/core/patterns ]"

# 3. Vérifier que tous les fichiers ont été migrés
check "Migration des fichiers" \
    "[ -f src/LaserCutStudio/core/models/base/Interface.h ] && \
     [ -f src/LaserCutStudio/core/models/shapes/implementations/Rectangle.cpp ] && \
     [ -f src/LaserCutStudio/core/infrastructure/patterns/factory/FactoryMixin.h ]"

# 4. Compiler en Debug
check "Compilation Debug" \
    "cd ${BUILD_DIR} && cmake --build . --clean-first"

# 5. Exécuter les tests unitaires
check "Tests unitaires" \
    "cd ${BUILD_DIR} && ctest --output-on-failure"

# 6. Compiler en Release
check "Compilation Release" \
    "[ -d build/Desktop-Release ] || mkdir -p build/Desktop-Release && \
     cd build/Desktop-Release && \
     cmake ../.. -DCMAKE_BUILD_TYPE=Release && \
     cmake --build ."

# 7. Vérifier que les benchmarks peuvent se compiler
check "Compilation benchmarks" \
    "cd ${BUILD_DIR} && \
     [ -f tests/benchmarks/LaserCutStudioBenchmarks ] || \
     (cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DBUILD_BENCHMARKS=ON && \
      cmake --build . --target LaserCutStudioBenchmarks)"

# Résumé
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                       RÉSUMÉ                                   ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "Checks réussis : ${SUCCESS_COUNT}/${TOTAL_CHECKS}"
echo ""

if [ ${SUCCESS_COUNT} -eq ${TOTAL_CHECKS} ]; then
    echo "✅ ✅ ✅  VALIDATION COMPLÈTE RÉUSSIE ! ✅ ✅ ✅"
    echo ""
    echo "La migration est terminée avec succès."
    echo "Vous pouvez maintenant commiter les changements :"
    echo ""
    echo "  git add -A"
    echo "  git commit -m 'refactor: migration architecture en couches (models/services/infrastructure)'"
    echo ""
    exit 0
else
    echo "❌ ❌ ❌  VALIDATION ÉCHOUÉE ❌ ❌ ❌"
    echo ""
    echo "Certains checks ont échoué. Vérifiez les erreurs ci-dessus."
    echo ""
    exit 1
fi
