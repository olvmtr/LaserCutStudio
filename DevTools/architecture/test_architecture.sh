#!/bin/bash
# Script pour tester l'architecture SOLID de LaserCutStudio

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "=================================="
echo "Tests d'Architecture SOLID"
echo "=================================="
echo ""

# 1. Tests Python (analyse statique des dépendances)
echo "📋 Étape 1/3: Tests Python (analyse statique)..."
cd "$PROJECT_ROOT"
python3 DevTools/architecture/test_architecture_solid.py
PYTHON_EXIT_CODE=$?

echo ""
echo "=================================="
echo ""

# 2. clang-tidy (analyse du code C++)
if command -v clang-tidy &> /dev/null; then
    echo "📋 Étape 2/3: clang-tidy (analyse du code C++)..."

    # Vérifier si compile_commands.json existe
    BUILD_DIR="$PROJECT_ROOT/src/LaserCutStudio/build/Desktop-Debug"
    COMPILE_COMMANDS="$BUILD_DIR/compile_commands.json"

    if [ -f "$COMPILE_COMMANDS" ]; then
        echo "✅ Utilisation de: $COMPILE_COMMANDS"

        # Analyser tous les fichiers source
        find "$PROJECT_ROOT/src/LaserCutStudio/core" -name "*.cpp" | while read -r file; do
            echo "  Analyse: $(basename "$file")"
            clang-tidy "$file" -p "$BUILD_DIR" --quiet 2>&1 | grep -E "(warning|error)" || true
        done

        echo "✅ Analyse clang-tidy terminée"
    else
        echo "⚠️  compile_commands.json introuvable"
        echo "   Exécutez: cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .."
        echo "   Puis: make"
    fi
else
    echo "⚠️  clang-tidy non installé (ignoré)"
    echo "   Installation: sudo apt-get install clang-tidy"
fi

echo ""
echo "=================================="
echo ""

# 3. Résumé
echo "📋 Étape 3/3: Résumé..."
echo ""

if [ $PYTHON_EXIT_CODE -eq 0 ]; then
    echo "✅ Tous les tests d'architecture SOLID passés !"
    echo ""
    exit 0
else
    echo "❌ Des violations d'architecture ont été détectées"
    echo ""
    echo "Règles SOLID pour LaserCutStudio :"
    echo "  1. Pas de dépendances entre classes concrètes"
    echo "  2. Seules les interfaces peuvent être utilisées"
    echo "  3. Toutes les implémentations héritent d'une interface"
    echo "  4. Tous les patterns sont utilisés (Mixins, Macros)"
    echo "  5. Hiérarchie des packages respectée"
    echo "  6. Pas de dépendances circulaires"
    echo "  7. Mixins totalement indépendants"
    echo ""
    exit 1
fi
