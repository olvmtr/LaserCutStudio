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
    TestEditorCommands.h
    TestEditorCommands.cpp
    # Sources du module Core (seulement ceux qui ne sont pas dans LibInterface)
    ../core/models/base/types/Point2D.h
    ../core/models/base/types/Point3D.h
    ../core/models/base/types/Material.h
    ../core/models/base/types/JointType.h
    ../core/models/shapes/interfaces/IShape.h
    ../core/models/shapes/interfaces/IShape.cpp
    ../core/models/shapes/implementations/Rectangle.h
    ../core/models/shapes/implementations/Rectangle.cpp
    ../core/models/shapes/implementations/Circle.h
    ../core/models/shapes/implementations/Circle.cpp
    ../core/models/parts/interfaces/IPart.h
    ../core/models/parts/interfaces/IPart.cpp
    ../core/models/parts/implementations/Part.h
    ../core/models/parts/implementations/Part.cpp
    ../core/models/joints/interfaces/IJoint.h
    ../core/models/joints/interfaces/IJoint.cpp
    ../core/models/joints/implementations/TabJoint.h
    ../core/models/joints/implementations/TabJoint.cpp
    ../core/models/joints/implementations/FingerJoint.h
    ../core/models/joints/implementations/FingerJoint.cpp
    ../core/models/projects/interfaces/IProject.h
    ../core/models/projects/interfaces/IProject.cpp
    ../core/models/projects/implementations/Project.h
    ../core/models/projects/implementations/Project.cpp
    ../core/infrastructure/config/ConfigManager.h
    ../core/infrastructure/config/ConfigManager.cpp
    ../core/infrastructure/logging/LogCategories.h
    ../core/infrastructure/logging/LogCategories.cpp
    ../core/infrastructure/logging/LogManager.h
    ../core/infrastructure/logging/LogManager.cpp
    ../core/infrastructure/plugins/interfaces/IShapePlugin.h
    ../core/infrastructure/plugins/interfaces/IJointPlugin.h
    ../core/infrastructure/plugins/core/PluginManager.h
    ../core/infrastructure/plugins/core/PluginManager.cpp
    # Editor components
    ../core/services/editor/EditorService.h
    ../core/services/editor/EditorService.cpp
    ../core/models/editor/interfaces/ITool.h
    ../core/models/editor/interfaces/ISelection.h
    ../core/models/editor/interfaces/IEditorCommand.h
    ../core/models/editor/implementations/CommandStack.h
    ../core/models/editor/implementations/CommandStack.cpp
    ../core/models/editor/implementations/CreateShapeCommand.h
    ../core/models/editor/implementations/CreateShapeCommand.cpp
    ../core/models/editor/implementations/MoveCommand.h
    ../core/models/editor/implementations/MoveCommand.cpp
    ../core/models/editor/implementations/SelectionManager.h
    ../core/models/editor/implementations/SelectionManager.cpp
    ../core/models/editor/implementations/SelectionTool.h
    ../core/models/editor/implementations/SelectionTool.cpp
    ../core/models/editor/implementations/ShapeCreationTool.h
    ../core/models/editor/implementations/ShapeCreationTool.cpp
)

# Lie les bibliothèques Qt et LibInterface
target_link_libraries(LaserCutStudioTests
    PRIVATE
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    LibInterface::LibInterface
)

# Include directories pour accès avec "core/..."
target_include_directories(LaserCutStudioTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
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
