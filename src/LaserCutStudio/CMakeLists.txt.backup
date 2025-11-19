cmake_minimum_required(VERSION 3.16)

project(LaserCutStudio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

# Ajouter LibInterface (bibliothèque réutilisable)
add_subdirectory(../LibInterface ${CMAKE_CURRENT_BINARY_DIR}/LibInterface)

qt_standard_project_setup()

qt_add_executable(appLaserCutStudio
    main.cpp
    # Core Module - Types
    core/types/Point2D.h
    core/types/Point3D.h
    core/types/Material.h
    core/types/JointType.h
    # Core Module - Shapes
    core/shapes/IShape.h
    core/shapes/IShape.cpp
    core/shapes/Rectangle.h
    core/shapes/Rectangle.cpp
    core/shapes/Circle.h
    core/shapes/Circle.cpp
    # Core Module - Parts
    core/parts/IPart.h
    core/parts/IPart.cpp
    core/parts/Part.h
    core/parts/Part.cpp
    # Core Module - Joints
    core/joints/IJoint.h
    core/joints/IJoint.cpp
    core/joints/TabJoint.h
    core/joints/TabJoint.cpp
    core/joints/FingerJoint.h
    core/joints/FingerJoint.cpp
    # Core Module - Projects
    core/projects/IProject.h
    core/projects/IProject.cpp
    core/projects/Project.h
    core/projects/Project.cpp
    # Core Module - Config
    core/config/ConfigManager.h
    core/config/ConfigManager.cpp
    # Core Module - Logging
    core/logging/LogCategories.h
    core/logging/LogCategories.cpp
    core/logging/LogManager.h
    core/logging/LogManager.cpp
    # Core Module - Plugins
    core/plugins/IShapePlugin.h
    core/plugins/IJointPlugin.h
    core/plugins/PluginManager.h
    core/plugins/PluginManager.cpp
)

qt_add_qml_module(appLaserCutStudio
    URI LaserCutStudio
    VERSION 1.0
    QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appLaserCutStudio PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appLaserCutStudio
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appLaserCutStudio
    PRIVATE
        Qt6::Quick
        LibInterface::LibInterface
)

# Définir la macro DEBUG en mode Debug (pour #if DEBUG dans le code)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(appLaserCutStudio PRIVATE DEBUG)
endif()

# Configuration des niveaux de logging selon le build type
# En Release, désactiver debug et info par défaut (optimisation performance)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    option(ENABLE_DEBUG_LOGS_IN_RELEASE "Enable qCDebug() in Release builds" OFF)

    if(NOT ENABLE_DEBUG_LOGS_IN_RELEASE)
        # Désactiver les logs de debug en Release
        target_compile_definitions(appLaserCutStudio PRIVATE
            QT_NO_DEBUG_OUTPUT      # Désactive qDebug() et qCDebug()
            QT_NO_INFO_OUTPUT       # Désactive qInfo() et qCInfo()
        )
        message(STATUS "Release build: Debug and Info logging disabled")
        message(STATUS "  - qDebug()/qCDebug() will be compiled out")
        message(STATUS "  - qInfo()/qCInfo() will be compiled out")
        message(STATUS "  - qWarning()/qCWarning() remain active")
        message(STATUS "  - qCritical() remains active")
    else()
        message(STATUS "Release build: Debug logging ENABLED (ENABLE_DEBUG_LOGS_IN_RELEASE=ON)")
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build: All logging levels enabled")
    message(STATUS "  - qDebug()/qCDebug() active")
    message(STATUS "  - qInfo()/qCInfo() active")
    message(STATUS "  - qWarning()/qCWarning() active")
    message(STATUS "  - qCritical() active")
endif()

include(GNUInstallDirs)
install(TARGETS appLaserCutStudio
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Documentation avec Doxygen
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/../../Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../../
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
    message(STATUS "Documentation target 'doc' available (use: cmake --build . --target doc)")
else()
    message(STATUS "Doxygen not found. Install with: sudo apt-get install doxygen graphviz")
    message(STATUS "Documentation target 'doc' will not be available")
endif()

# Ajoute les tests
add_subdirectory(tests)
