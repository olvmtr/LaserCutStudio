#!/bin/bash
# Script de mise à jour du CMakeLists.txt principal pour la nouvelle structure

set -e  # Arrêter en cas d'erreur

echo "============================================"
echo "Mise à jour du CMakeLists.txt principal"
echo "============================================"
echo ""

CMAKE_FILE="src/LaserCutStudio/CMakeLists.txt"

if [ ! -f "${CMAKE_FILE}" ]; then
    echo "❌ ERREUR : ${CMAKE_FILE} introuvable"
    exit 1
fi

echo "📝 Sauvegarde du CMakeLists.txt actuel..."
cp "${CMAKE_FILE}" "${CMAKE_FILE}.backup"
echo "✅ Sauvegarde créée : ${CMAKE_FILE}.backup"
echo ""

echo "📝 Mise à jour des chemins de fichiers..."

# Remplacer tous les anciens chemins par les nouveaux
sed -i 's|core/types/Point2D.h|core/models/base/types/Point2D.h|g' "${CMAKE_FILE}"
sed -i 's|core/types/Point3D.h|core/models/base/types/Point3D.h|g' "${CMAKE_FILE}"
sed -i 's|core/types/Material.h|core/models/base/types/Material.h|g' "${CMAKE_FILE}"
sed -i 's|core/types/JointType.h|core/models/base/types/JointType.h|g' "${CMAKE_FILE}"

sed -i 's|core/shapes/IShape.h|core/models/shapes/interfaces/IShape.h|g' "${CMAKE_FILE}"
sed -i 's|core/shapes/IShape.cpp|core/models/shapes/interfaces/IShape.cpp|g' "${CMAKE_FILE}"
sed -i 's|core/shapes/Rectangle.h|core/models/shapes/implementations/Rectangle.h|g' "${CMAKE_FILE}"
sed -i 's|core/shapes/Rectangle.cpp|core/models/shapes/implementations/Rectangle.cpp|g' "${CMAKE_FILE}"
sed -i 's|core/shapes/Circle.h|core/models/shapes/implementations/Circle.h|g' "${CMAKE_FILE}"
sed -i 's|core/shapes/Circle.cpp|core/models/shapes/implementations/Circle.cpp|g' "${CMAKE_FILE}"

sed -i 's|core/parts/IPart.h|core/models/parts/interfaces/IPart.h|g' "${CMAKE_FILE}"
sed -i 's|core/parts/IPart.cpp|core/models/parts/interfaces/IPart.cpp|g' "${CMAKE_FILE}"
sed -i 's|core/parts/Part.h|core/models/parts/implementations/Part.h|g' "${CMAKE_FILE}"
sed -i 's|core/parts/Part.cpp|core/models/parts/implementations/Part.cpp|g' "${CMAKE_FILE}"

sed -i 's|core/joints/IJoint.h|core/models/joints/interfaces/IJoint.h|g' "${CMAKE_FILE}"
sed -i 's|core/joints/IJoint.cpp|core/models/joints/interfaces/IJoint.cpp|g' "${CMAKE_FILE}"
sed -i 's|core/joints/TabJoint.h|core/models/joints/implementations/TabJoint.h|g' "${CMAKE_FILE}"
sed -i 's|core/joints/TabJoint.cpp|core/models/joints/implementations/TabJoint.cpp|g' "${CMAKE_FILE}"
sed -i 's|core/joints/FingerJoint.h|core/models/joints/implementations/FingerJoint.h|g' "${CMAKE_FILE}"
sed -i 's|core/joints/FingerJoint.cpp|core/models/joints/implementations/FingerJoint.cpp|g' "${CMAKE_FILE}"

sed -i 's|core/projects/IProject.h|core/models/projects/interfaces/IProject.h|g' "${CMAKE_FILE}"
sed -i 's|core/projects/IProject.cpp|core/models/projects/interfaces/IProject.cpp|g' "${CMAKE_FILE}"
sed -i 's|core/projects/Project.h|core/models/projects/implementations/Project.h|g' "${CMAKE_FILE}"
sed -i 's|core/projects/Project.cpp|core/models/projects/implementations/Project.cpp|g' "${CMAKE_FILE}"

sed -i 's|core/config/ConfigManager.h|core/infrastructure/config/ConfigManager.h|g' "${CMAKE_FILE}"
sed -i 's|core/config/ConfigManager.cpp|core/infrastructure/config/ConfigManager.cpp|g' "${CMAKE_FILE}"

sed -i 's|core/logging/LogCategories.h|core/infrastructure/logging/LogCategories.h|g' "${CMAKE_FILE}"
sed -i 's|core/logging/LogCategories.cpp|core/infrastructure/logging/LogCategories.cpp|g' "${CMAKE_FILE}"
sed -i 's|core/logging/LogManager.h|core/infrastructure/logging/LogManager.h|g' "${CMAKE_FILE}"
sed -i 's|core/logging/LogManager.cpp|core/infrastructure/logging/LogManager.cpp|g' "${CMAKE_FILE}"

sed -i 's|core/plugins/IShapePlugin.h|core/infrastructure/plugins/interfaces/IShapePlugin.h|g' "${CMAKE_FILE}"
sed -i 's|core/plugins/IJointPlugin.h|core/infrastructure/plugins/interfaces/IJointPlugin.h|g' "${CMAKE_FILE}"
sed -i 's|core/plugins/PluginManager.h|core/infrastructure/plugins/core/PluginManager.h|g' "${CMAKE_FILE}"
sed -i 's|core/plugins/PluginManager.cpp|core/infrastructure/plugins/core/PluginManager.cpp|g' "${CMAKE_FILE}"

echo "✅ CMakeLists.txt mis à jour"
echo ""

# Mettre à jour les commentaires pour refléter la nouvelle architecture
echo "📝 Mise à jour des commentaires..."
sed -i 's|# Core Module - Types|# Core Module - Models Base|g' "${CMAKE_FILE}"
sed -i 's|# Core Module - Shapes|# Core Module - Models Shapes|g' "${CMAKE_FILE}"
sed -i 's|# Core Module - Parts|# Core Module - Models Parts|g' "${CMAKE_FILE}"
sed -i 's|# Core Module - Joints|# Core Module - Models Joints|g' "${CMAKE_FILE}"
sed -i 's|# Core Module - Projects|# Core Module - Models Projects|g' "${CMAKE_FILE}"
sed -i 's|# Core Module - Config|# Core Module - Infrastructure Config|g' "${CMAKE_FILE}"
sed -i 's|# Core Module - Logging|# Core Module - Infrastructure Logging|g' "${CMAKE_FILE}"
sed -i 's|# Core Module - Plugins|# Core Module - Infrastructure Plugins|g' "${CMAKE_FILE}"

echo "✅ Commentaires mis à jour"
echo ""

# Ajouter les fichiers Interface.h/cpp et ServiceLocator.h/cpp qui manquent
echo "📝 Ajout des fichiers manquants (Interface, ServiceLocator)..."
# Trouver la ligne où ajouter Interface (après main.cpp, avant les types)
sed -i '/main.cpp/a\    # Core Module - Models Base\n    core/models/base/Interface.h\n    core/models/base/Interface.cpp' "${CMAKE_FILE}"

# Ajouter ServiceLocator après les plugins
sed -i '/core\/infrastructure\/plugins\/core\/PluginManager.cpp/a\    # Core Module - Infrastructure DI\n    core/infrastructure/di/ServiceLocator.h\n    core/infrastructure/di/ServiceLocator.cpp' "${CMAKE_FILE}"

echo "✅ Fichiers manquants ajoutés"
echo ""

# Tester la configuration CMake
echo "🔍 Test de configuration CMake..."
cd build/Desktop-Debug
if cmake ../.. -DCMAKE_BUILD_TYPE=Debug >/dev/null 2>&1; then
    echo "✅ Configuration CMake réussie !"
    echo ""
    echo "Prochaine étape : cmake --build . pour compiler"
else
    echo "❌ Erreur de configuration CMake"
    echo "Restauration de l'ancien CMakeLists.txt..."
    cp "${CMAKE_FILE}.backup" "${CMAKE_FILE}"
    echo "Affichage de l'erreur :"
    cmake ../.. -DCMAKE_BUILD_TYPE=Debug
    exit 1
fi
