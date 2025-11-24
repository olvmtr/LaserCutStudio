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
    TestEditorService.h
    TestEditorService.cpp
    TestArchitectureInterfaces.h
    TestArchitectureInterfaces.cpp
    TestGeometry.h
    TestGeometry.cpp
    # Sources du module Core (seulement ceux qui ne sont pas dans LibInterface)
    ../core/models/base/types/Point2D.h
    ../core/models/base/types/Point3D.h
    ../core/models/base/types/Material.h
    ../core/models/base/types/JointType.h
    ../core/models/shapes/IShape.h
    ../core/models/shapes/IShape.cpp
    ../core/models/shapes/Rectangle.h
    ../core/models/shapes/Rectangle.cpp
    ../core/models/shapes/Circle.h
    ../core/models/shapes/Circle.cpp
    ../core/models/parts/IPart.h
    ../core/models/parts/IPart.cpp
    ../core/models/parts/Part.h
    ../core/models/parts/Part.cpp
    ../core/models/joints/IJoint.h
    ../core/models/joints/IJoint.cpp
    ../core/models/joints/TabJoint.h
    ../core/models/joints/TabJoint.cpp
    ../core/models/joints/FingerJoint.h
    ../core/models/joints/FingerJoint.cpp
    ../core/models/projects/IProject.h
    ../core/models/projects/IProject.cpp
    ../core/models/projects/Project.h
    ../core/models/projects/Project.cpp
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
    ../core/models/editor/tool/ITool.h
    ../core/models/editor/selection/ISelection.h
    ../core/models/editor/command/IEditorCommand.h
    ../core/models/editor/command/transformations/ITransformationCommand.h
    ../core/models/editor/command/transformations/ITransformationCommand.cpp
    ../core/models/editor/command/CommandStack.h
    ../core/models/editor/command/CommandStack.cpp
    ../core/models/editor/command/CreateShapeCommand.h
    ../core/models/editor/command/CreateShapeCommand.cpp
    ../core/models/editor/command/transformations/MoveCommand.h
    ../core/models/editor/command/transformations/MoveCommand.cpp
    ../core/models/editor/command/DeleteShapeCommand.h
    ../core/models/editor/command/DeleteShapeCommand.cpp
    ../core/models/editor/command/transformations/RotateCommand.h
    ../core/models/editor/command/transformations/RotateCommand.cpp
    ../core/models/editor/command/transformations/ScaleCommand.h
    ../core/models/editor/command/transformations/ScaleCommand.cpp
    ../core/models/editor/selection/SelectionManager.h
    ../core/models/editor/selection/SelectionManager.cpp
    ../core/models/editor/tool/SelectionTool.h
    ../core/models/editor/tool/SelectionTool.cpp
    ../core/models/editor/tool/ShapeCreationTool.h
    ../core/models/editor/tool/ShapeCreationTool.cpp
    # New constraint-based geometry system
    ../core/models/geometry/IGeometricElement.h
    ../core/models/geometry/IGeometricElement.cpp
    ../core/models/geometry/GeometricPoint.h
    ../core/models/geometry/GeometricPoint.cpp
    ../core/models/geometry/GeometricSegment.h
    ../core/models/geometry/GeometricSegment.cpp
    ../core/models/geometry/GeometricArc.h
    ../core/models/geometry/GeometricArc.cpp
    ../core/models/constraints/IConstraint.h
    ../core/models/constraints/IConstraint.cpp
    ../core/models/constraints/DistanceConstraint.h
    ../core/models/constraints/DistanceConstraint.cpp
    ../core/models/constraints/LengthConstraint.h
    ../core/models/constraints/LengthConstraint.cpp
    ../core/models/constraints/AngleConstraint.h
    ../core/models/constraints/AngleConstraint.cpp
    ../core/models/constraints/FixedPointConstraint.h
    ../core/models/constraints/FixedPointConstraint.cpp
    ../core/models/sketch/ConstraintSolver.h
    ../core/models/sketch/ConstraintSolver.cpp
    ../core/models/sketch/ConstraintSketch.h
    ../core/models/sketch/ConstraintSketch.cpp
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
