#!/bin/bash
# Script master pour orchestrer toute la migration
# Exécute tous les scripts dans l'ordre

set -e  # Arrêter en cas d'erreur

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║  Migration vers Architecture en Couches - LaserCutStudio      ║"
echo "║  models / services / infrastructure / utils                   ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Vérifier que tous les scripts existent
SCRIPTS=(
    "01_create_new_structure.sh"
    "02_migrate_files.sh"
    "03_update_includes.sh"
    "04_update_cmake.sh"
)

for script in "${SCRIPTS[@]}"; do
    if [ ! -f "${SCRIPT_DIR}/${script}" ]; then
        echo "❌ ERREUR : Script ${script} introuvable"
        exit 1
    fi
    chmod +x "${SCRIPT_DIR}/${script}"
done

echo "✅ Tous les scripts sont présents"
echo ""

# Sauvegarder l'état actuel avec Git
echo "📸 Création d'un commit de sauvegarde..."
git add -A
git commit -m "chore: sauvegarde avant migration architecture en couches" || echo "Rien à commiter"
BACKUP_COMMIT=$(git rev-parse HEAD)
echo "✅ Commit de sauvegarde : ${BACKUP_COMMIT}"
echo "   Rollback possible avec : git reset --hard ${BACKUP_COMMIT}"
echo ""

# Demander confirmation
read -p "⚠️  Voulez-vous continuer la migration ? (o/N) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Oo]$ ]]; then
    echo "Migration annulée"
    exit 0
fi

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Étape 1/4 : Création de la nouvelle structure"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
"${SCRIPT_DIR}/01_create_new_structure.sh"

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Étape 2/4 : Migration des fichiers (git mv)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
"${SCRIPT_DIR}/02_migrate_files.sh"

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Étape 3/4 : Mise à jour des #include"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
"${SCRIPT_DIR}/03_update_includes.sh"

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Étape 4/4 : Mise à jour CMakeLists.txt et compilation"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
"${SCRIPT_DIR}/04_update_cmake.sh"

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                     MIGRATION TERMINÉE                         ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "✅ Migration réussie !"
echo ""
echo "📋 Prochaines étapes :"
echo "  1. Compiler : cd build/Desktop-Debug && cmake --build ."
echo "  2. Tester : ctest --output-on-failure"
echo "  3. Benchmarks : ./tests/benchmarks/LaserCutStudioBenchmarks"
echo "  4. Commit : git add -A && git commit -m 'refactor: migration architecture en couches'"
echo ""
echo "🔄 Rollback si nécessaire : git reset --hard ${BACKUP_COMMIT}"
echo ""
