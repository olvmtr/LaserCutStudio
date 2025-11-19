cmake_minimum_required(VERSION 3.16)
project(LibInterface VERSION 1.0.0 LANGUAGES CXX)

# Configuration C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core)

# Sources
set(SOURCES
    src/Interface.cpp
    src/DI/ServiceLocator.cpp
)

# Headers publics
set(PUBLIC_HEADERS
    include/LibInterface/Interface.h
    include/LibInterface/DebugMacros.h
    include/LibInterface/Patterns/FactoryMixin.h
    include/LibInterface/Patterns/ListManagerMixin.h
    include/LibInterface/Patterns/PropertyMixin.h
    include/LibInterface/Patterns/ClonableMixin.h
    include/LibInterface/DI/ServiceLocator.h
)

# Bibliothèque LibInterface
add_library(LibInterface STATIC ${SOURCES} ${PUBLIC_HEADERS})

# Alias pour usage moderne CMake
add_library(LibInterface::LibInterface ALIAS LibInterface)

# Include directories
target_include_directories(LibInterface
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Liaison Qt6
target_link_libraries(LibInterface
    PUBLIC
        Qt6::Core
)

# Propriétés
set_target_properties(LibInterface PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

# Installation
include(GNUInstallDirs)

install(TARGETS LibInterface
    EXPORT LibInterfaceTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LibInterface
)

# Installation headers (structure complète)
install(DIRECTORY include/LibInterface
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Export configuration
install(EXPORT LibInterfaceTargets
    FILE LibInterfaceTargets.cmake
    NAMESPACE LibInterface::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibInterface
)

# Config file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/LibInterfaceConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LibInterfaceConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibInterface
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/LibInterfaceConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LibInterfaceConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/LibInterfaceConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibInterface
)
