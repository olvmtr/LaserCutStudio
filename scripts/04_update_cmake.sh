#!/bin/bash
# Script de mise à jour des CMakeLists.txt pour la nouvelle structure

set -e  # Arrêter en cas d'erreur

echo "============================================"
echo "Mise à jour des CMakeLists.txt"
echo "============================================"
echo ""

CORE_CMAKE="src/LaserCutStudio/core/CMakeLists.txt"

echo "📝 Génération du nouveau CMakeLists.txt pour core/..."

# Sauvegarder l'ancien
cp "${CORE_CMAKE}" "${CORE_CMAKE}.backup"

# Générer le nouveau CMakeLists.txt
cat > "${CORE_CMAKE}" << 'EOF'
# CMakeLists.txt - Module Core LaserCutStudio
# Architecture par couches : models / services / infrastructure / utils

# ==============================================================================
# COUCHE MODELS (Domaine métier)
# ==============================================================================

# Models - Base
set(CORE_MODELS_BASE_SOURCES
    models/base/Interface.cpp
)

set(CORE_MODELS_BASE_HEADERS
    models/base/Interface.h
    models/base/types/Point2D.h
    models/base/types/Point3D.h
    models/base/types/Material.h
    models/base/types/JointType.h
)

# Models - Shapes
set(CORE_MODELS_SHAPES_SOURCES
    models/shapes/interfaces/IShape.cpp
    models/shapes/implementations/Rectangle.cpp
    models/shapes/implementations/Circle.cpp
)

set(CORE_MODELS_SHAPES_HEADERS
    models/shapes/interfaces/IShape.h
    models/shapes/implementations/Rectangle.h
    models/shapes/implementations/Circle.h
)

# Models - Parts
set(CORE_MODELS_PARTS_SOURCES
    models/parts/interfaces/IPart.cpp
    models/parts/implementations/Part.cpp
)

set(CORE_MODELS_PARTS_HEADERS
    models/parts/interfaces/IPart.h
    models/parts/implementations/Part.h
)

# Models - Joints
set(CORE_MODELS_JOINTS_SOURCES
    models/joints/interfaces/IJoint.cpp
    models/joints/implementations/TabJoint.cpp
    models/joints/implementations/FingerJoint.cpp
)

set(CORE_MODELS_JOINTS_HEADERS
    models/joints/interfaces/IJoint.h
    models/joints/implementations/TabJoint.h
    models/joints/implementations/FingerJoint.h
)

# Models - Projects
set(CORE_MODELS_PROJECTS_SOURCES
    models/projects/interfaces/IProject.cpp
    models/projects/implementations/Project.cpp
)

set(CORE_MODELS_PROJECTS_HEADERS
    models/projects/interfaces/IProject.h
    models/projects/implementations/Project.h
)

# ==============================================================================
# COUCHE SERVICES (Logique applicative)
# ==============================================================================

# Services - Geometry
set(CORE_SERVICES_GEOMETRY_HEADERS
    services/geometry/GeometryUtils.h
)

# ==============================================================================
# COUCHE INFRASTRUCTURE (Technique)
# ==============================================================================

# Infrastructure - Patterns
set(CORE_INFRASTRUCTURE_PATTERNS_HEADERS
    infrastructure/patterns/factory/FactoryMixin.h
    infrastructure/patterns/prototype/ClonableMixin.h
    infrastructure/patterns/properties/PropertyMixin.h
    infrastructure/patterns/lists/ListManagerMixin.h
)

# Infrastructure - Config
set(CORE_INFRASTRUCTURE_CONFIG_SOURCES
    infrastructure/config/ConfigManager.cpp
)

set(CORE_INFRASTRUCTURE_CONFIG_HEADERS
    infrastructure/config/ConfigManager.h
)

# Infrastructure - Logging
set(CORE_INFRASTRUCTURE_LOGGING_SOURCES
    infrastructure/logging/LogManager.cpp
    infrastructure/logging/LogCategories.cpp
)

set(CORE_INFRASTRUCTURE_LOGGING_HEADERS
    infrastructure/logging/LogManager.h
    infrastructure/logging/LogCategories.h
)

# Infrastructure - Dependency Injection
set(CORE_INFRASTRUCTURE_DI_SOURCES
    infrastructure/di/ServiceLocator.cpp
)

set(CORE_INFRASTRUCTURE_DI_HEADERS
    infrastructure/di/ServiceLocator.h
)

# Infrastructure - Plugins
set(CORE_INFRASTRUCTURE_PLUGINS_SOURCES
    infrastructure/plugins/core/PluginManager.cpp
)

set(CORE_INFRASTRUCTURE_PLUGINS_HEADERS
    infrastructure/plugins/core/PluginManager.h
    infrastructure/plugins/interfaces/IShapePlugin.h
    infrastructure/plugins/interfaces/IJointPlugin.h
)

# ==============================================================================
# UTILITAIRES
# ==============================================================================

set(CORE_UTILS_HEADERS
    utils/DebugMacros.h
)

# ==============================================================================
# AGGREGATION DES SOURCES
# ==============================================================================

set(CORE_SOURCES
    # Models
    ${CORE_MODELS_BASE_SOURCES}
    ${CORE_MODELS_SHAPES_SOURCES}
    ${CORE_MODELS_PARTS_SOURCES}
    ${CORE_MODELS_JOINTS_SOURCES}
    ${CORE_MODELS_PROJECTS_SOURCES}

    # Services
    # (Pas de sources pour geometry pour l'instant)

    # Infrastructure
    ${CORE_INFRASTRUCTURE_CONFIG_SOURCES}
    ${CORE_INFRASTRUCTURE_LOGGING_SOURCES}
    ${CORE_INFRASTRUCTURE_DI_SOURCES}
    ${CORE_INFRASTRUCTURE_PLUGINS_SOURCES}
)

set(CORE_HEADERS
    # Models
    ${CORE_MODELS_BASE_HEADERS}
    ${CORE_MODELS_SHAPES_HEADERS}
    ${CORE_MODELS_PARTS_HEADERS}
    ${CORE_MODELS_JOINTS_HEADERS}
    ${CORE_MODELS_PROJECTS_HEADERS}

    # Services
    ${CORE_SERVICES_GEOMETRY_HEADERS}

    # Infrastructure
    ${CORE_INFRASTRUCTURE_PATTERNS_HEADERS}
    ${CORE_INFRASTRUCTURE_CONFIG_HEADERS}
    ${CORE_INFRASTRUCTURE_LOGGING_HEADERS}
    ${CORE_INFRASTRUCTURE_DI_HEADERS}
    ${CORE_INFRASTRUCTURE_PLUGINS_HEADERS}

    # Utils
    ${CORE_UTILS_HEADERS}
)

# Créer la bibliothèque Core
add_library(CoreLib STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

# Lien avec Qt6
target_link_libraries(CoreLib PUBLIC
    Qt6::Core
)

# Include directories (accès avec "core/...")
target_include_directories(CoreLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # src/LaserCutStudio/
)

# Exposer les sources et headers pour les autres modules
set(CORE_SOURCES ${CORE_SOURCES} PARENT_SCOPE)
set(CORE_HEADERS ${CORE_HEADERS} PARENT_SCOPE)

# Afficher un résumé
message(STATUS "")
message(STATUS "=== LaserCutStudio Core Module (Architecture en couches) ===")
message(STATUS "Models       : ${CORE_MODELS_BASE_SOURCES} + ${CORE_MODELS_SHAPES_SOURCES} + ${CORE_MODELS_PARTS_SOURCES} + ${CORE_MODELS_JOINTS_SOURCES} + ${CORE_MODELS_PROJECTS_SOURCES}")
message(STATUS "Services     : Geometry (header-only)")
message(STATUS "Infrastructure : Config, Logging, DI, Plugins, Patterns")
message(STATUS "Utils        : DebugMacros")
message(STATUS "Total sources: ${CORE_SOURCES}")
message(STATUS "")
EOF

echo "✅ CMakeLists.txt pour core/ généré"
echo ""

# Tester la configuration CMake
echo "🔍 Test de configuration CMake..."
cd build/Desktop-Debug
if cmake ../.. -DCMAKE_BUILD_TYPE=Debug; then
    echo "✅ Configuration CMake réussie !"
else
    echo "❌ Erreur de configuration CMake"
    echo "Restauration de l'ancien CMakeLists.txt..."
    cp "${CORE_CMAKE}.backup" "${CORE_CMAKE}"
    exit 1
fi

echo ""
echo "Prochaine étape : cmake --build . pour compiler"
