cmake_minimum_required(VERSION 3.16)

project(PolygonPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

# Créer la bibliothèque de plugin (shared library)
add_library(PolygonPlugin SHARED
    PolygonPlugin.h
    Polygon.h
    polygon.json
)

# Lier Qt
target_link_libraries(PolygonPlugin PRIVATE Qt6::Core)

# Inclure les répertoires parent pour accéder aux interfaces
target_include_directories(PolygonPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Définir la macro DEBUG en mode Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(PolygonPlugin PRIVATE DEBUG)
endif()

# Installation du plugin
install(TARGETS PolygonPlugin
    LIBRARY DESTINATION plugins
    RUNTIME DESTINATION plugins
)
