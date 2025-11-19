cmake_minimum_required(VERSION 3.16)

# Active les tests
enable_testing()

# Trouve Qt Test
find_package(Qt6 REQUIRED COMPONENTS Test)

# Crée l'exécutable de tests
add_executable(LaserCutStudioTests
    main.cpp
    TestTypes.h
    TestTypes.cpp
    TestShapes.h
    TestShapes.cpp
    TestPart.h
    TestPart.cpp
    TestJoint.h
    TestJoint.cpp
    TestProject.h
    TestProject.cpp
    TestConfigManager.h
    TestConfigManager.cpp
    TestLogging.h
    TestLogging.cpp
    TestPluginManager.h
    TestPluginManager.cpp
    TestServiceLocator.h
    TestServiceLocator.cpp
    # Sources du module Core (seulement ceux qui ne sont pas dans LibInterface)
    ../core/types/Point2D.h
    ../core/types/Point3D.h
    ../core/types/Material.h
    ../core/types/JointType.h
    ../core/shapes/IShape.h
    ../core/shapes/IShape.cpp
    ../core/shapes/Rectangle.h
    ../core/shapes/Rectangle.cpp
    ../core/shapes/Circle.h
    ../core/shapes/Circle.cpp
    ../core/parts/IPart.h
    ../core/parts/IPart.cpp
    ../core/parts/Part.h
    ../core/parts/Part.cpp
    ../core/joints/IJoint.h
    ../core/joints/IJoint.cpp
    ../core/joints/TabJoint.h
    ../core/joints/TabJoint.cpp
    ../core/joints/FingerJoint.h
    ../core/joints/FingerJoint.cpp
    ../core/projects/IProject.h
    ../core/projects/IProject.cpp
    ../core/projects/Project.h
    ../core/projects/Project.cpp
    ../core/config/ConfigManager.h
    ../core/config/ConfigManager.cpp
    ../core/logging/LogCategories.h
    ../core/logging/LogCategories.cpp
    ../core/logging/LogManager.h
    ../core/logging/LogManager.cpp
    ../core/plugins/IShapePlugin.h
    ../core/plugins/IJointPlugin.h
    ../core/plugins/PluginManager.h
    ../core/plugins/PluginManager.cpp
)

# Lie les bibliothèques Qt et LibInterface
target_link_libraries(LaserCutStudioTests
    PRIVATE
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    LibInterface::LibInterface
)

# Définir la macro DEBUG en mode Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(LaserCutStudioTests PRIVATE DEBUG)
endif()

# Configuration des logs pour les tests (même que l'app principale)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(LaserCutStudioTests PRIVATE
        QT_NO_DEBUG_OUTPUT
        QT_NO_INFO_OUTPUT
    )
endif()

# Ajoute le test à CTest
add_test(NAME LaserCutStudioTests COMMAND LaserCutStudioTests)

# Active le support Qt pour MOC si nécessaire
set_target_properties(LaserCutStudioTests PROPERTIES
    AUTOMOC ON
)

# Option pour compiler les benchmarks (activée par défaut en Debug uniquement)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(BUILD_BENCHMARKS "Build performance benchmarks" ON)
else()
    option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
endif()

# Ajoute les benchmarks si l'option est activée
if(BUILD_BENCHMARKS)
    message(STATUS "Building benchmarks (BUILD_BENCHMARKS=ON)")
    add_subdirectory(benchmarks)
else()
    message(STATUS "Skipping benchmarks (BUILD_BENCHMARKS=OFF)")
endif()
