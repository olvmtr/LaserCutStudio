cmake_minimum_required(VERSION 3.16)

# Active les tests
enable_testing()

# Trouve Qt Test
find_package(Qt6 REQUIRED COMPONENTS Test Core)

# Crée l'exécutable de benchmarks
add_executable(LaserCutStudioBenchmarks
    main_benchmarks.cpp
    BenchmarkShapes.h
    BenchmarkShapes.cpp
    # Sources du module Core (seulement ceux qui ne sont pas dans LibInterface)
    ../../core/models/base/types/Point2D.h
    ../../core/models/base/types/Point3D.h
    ../../core/models/base/types/Material.h
)

# Lie les bibliothèques Qt et LibInterface
target_link_libraries(LaserCutStudioBenchmarks
    PRIVATE
    Qt6::Test
    Qt6::Core
    LibInterface::LibInterface
)

# Include directories pour accès avec "core/..."
target_include_directories(LaserCutStudioBenchmarks PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Définir la macro BUILD_BENCHMARKS pour le code conditionnel
target_compile_definitions(LaserCutStudioBenchmarks PRIVATE
    BUILD_BENCHMARKS
)

# Ajoute le benchmark à CTest
add_test(NAME LaserCutStudioBenchmarks COMMAND LaserCutStudioBenchmarks -iterations 100)

# Active le support Qt pour MOC si nécessaire
set_target_properties(LaserCutStudioBenchmarks PROPERTIES
    AUTOMOC ON
)
