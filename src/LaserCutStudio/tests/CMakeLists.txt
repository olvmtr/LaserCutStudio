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
    TestConfigManager.h
    TestConfigManager.cpp
    TestLogging.h
    TestLogging.cpp
    TestServiceLocator.h
    TestServiceLocator.cpp
    TestArchitectureInterfaces.h
    TestArchitectureInterfaces.cpp
    TestGeometry.h
    TestGeometry.cpp
    TestConstraints.h
    TestConstraints.cpp
    TestConstraintSketch.h
    TestConstraintSketch.cpp
    # Sources du module Core (seulement ceux qui ne sont pas dans LibInterface)
    ../core/models/base/types/Point2D.h
    ../core/models/base/types/Point3D.h
    ../core/models/base/types/Material.h
    ../core/models/base/types/JointType.h
    ../core/infrastructure/config/ConfigManager.h
    ../core/infrastructure/config/ConfigManager.cpp
    ../core/infrastructure/logging/LogCategories.h
    ../core/infrastructure/logging/LogCategories.cpp
    ../core/infrastructure/logging/LogManager.h
    ../core/infrastructure/logging/LogManager.cpp
    # Constraint-based geometry system
    ../core/models/geometry/IGeometricElement.h
    ../core/models/geometry/IGeometricElement.cpp
    ../core/models/geometry/implementation/GeometricPoint.h
    ../core/models/geometry/implementation/GeometricPoint.cpp
    ../core/models/geometry/implementation/GeometricSegment.h
    ../core/models/geometry/implementation/GeometricSegment.cpp
    ../core/models/geometry/implementation/GeometricArc.h
    ../core/models/geometry/implementation/GeometricArc.cpp
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
    ../core/models/constraints/ParallelConstraint.h
    ../core/models/constraints/ParallelConstraint.cpp
    ../core/models/constraints/PerpendicularConstraint.h
    ../core/models/constraints/PerpendicularConstraint.cpp
    ../core/models/constraints/EqualLengthConstraint.h
    ../core/models/constraints/EqualLengthConstraint.cpp
    ../core/models/constraints/CoincidentConstraint.h
    ../core/models/constraints/CoincidentConstraint.cpp
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
