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
    # Sources du module Core
    ../../core/interface/Interface.h
    ../../core/interface/Interface.cpp
    ../../core/types/Point2D.h
    ../../core/types/Point3D.h
    ../../core/types/Material.h
    ../../core/shapes/IShape.h
    ../../core/shapes/IShape.cpp
    ../../core/shapes/Rectangle.h
    ../../core/shapes/Rectangle.cpp
    ../../core/shapes/Circle.h
    ../../core/shapes/Circle.cpp
)

# Lie les bibliothèques Qt
target_link_libraries(LaserCutStudioBenchmarks
    PRIVATE
    Qt6::Test
    Qt6::Core
)

# Ajoute le benchmark à CTest
add_test(NAME LaserCutStudioBenchmarks COMMAND LaserCutStudioBenchmarks -iterations 100)

# Active le support Qt pour MOC si nécessaire
set_target_properties(LaserCutStudioBenchmarks PROPERTIES
    AUTOMOC ON
)
