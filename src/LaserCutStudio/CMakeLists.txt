cmake_minimum_required(VERSION 3.16)

project(LaserCutStudio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Générer compile_commands.json pour clang-tidy et autres outils d'analyse
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

# Ajouter LibInterface (bibliothèque réutilisable)
add_subdirectory(../LibInterface ${CMAKE_CURRENT_BINARY_DIR}/LibInterface)

qt_standard_project_setup()

qt_add_executable(appLaserCutStudio
    main.cpp
    # Core Module - Models Base
    core/models/base/Interface.h
    core/models/base/Interface.cpp
    # Core Module - Models Base
    core/models/base/types/Point2D.h
    core/models/base/types/Point3D.h
    core/models/base/types/Material.h
    core/models/base/types/JointType.h
    # Core Module - Models Shapes
    core/models/shapes/interfaces/IShape.h
    core/models/shapes/interfaces/IShape.cpp
    core/models/shapes/implementations/Rectangle.h
    core/models/shapes/implementations/Rectangle.cpp
    core/models/shapes/implementations/Circle.h
    core/models/shapes/implementations/Circle.cpp
    core/models/shapes/implementations/Triangle.h
    core/models/shapes/implementations/Triangle.cpp
    # Core Module - Models Parts
    core/models/parts/interfaces/IPart.h
    core/models/parts/interfaces/IPart.cpp
    core/models/parts/implementations/Part.h
    core/models/parts/implementations/Part.cpp
    # Core Module - Models Joints
    core/models/joints/interfaces/IJoint.h
    core/models/joints/interfaces/IJoint.cpp
    core/models/joints/implementations/TabJoint.h
    core/models/joints/implementations/TabJoint.cpp
    core/models/joints/implementations/FingerJoint.h
    core/models/joints/implementations/FingerJoint.cpp
    # Core Module - Models Projects
    core/models/projects/interfaces/IProject.h
    core/models/projects/interfaces/IProject.cpp
    core/models/projects/implementations/Project.h
    core/models/projects/implementations/Project.cpp
    # Core Module - Models Editor
    core/models/editor/interfaces/ITool.h
    core/models/editor/interfaces/ITool.cpp
    core/models/editor/interfaces/IEditorCommand.h
    core/models/editor/interfaces/IEditorCommand.cpp
    core/models/editor/interfaces/ISelection.h
    core/models/editor/interfaces/ISelection.cpp
    core/models/editor/implementations/ShapeCreationTool.h
    core/models/editor/implementations/ShapeCreationTool.cpp
    core/models/editor/implementations/PanTool.h
    core/models/editor/implementations/PanTool.cpp
    core/models/editor/implementations/ZoomTool.h
    core/models/editor/implementations/ZoomTool.cpp
    core/models/editor/implementations/SelectionTool.h
    core/models/editor/implementations/SelectionTool.cpp
    core/models/editor/implementations/SelectionManager.h
    core/models/editor/implementations/SelectionManager.cpp
    core/models/editor/implementations/CreateShapeCommand.h
    core/models/editor/implementations/CreateShapeCommand.cpp
    core/models/editor/implementations/MoveCommand.h
    core/models/editor/implementations/MoveCommand.cpp
    core/models/editor/implementations/DeleteShapeCommand.h
    core/models/editor/implementations/DeleteShapeCommand.cpp
    core/models/editor/implementations/RotateCommand.h
    core/models/editor/implementations/RotateCommand.cpp
    core/models/editor/implementations/ScaleCommand.h
    core/models/editor/implementations/ScaleCommand.cpp
    core/models/editor/implementations/CommandStack.h
    core/models/editor/implementations/CommandStack.cpp
    # Core Module - Services Editor
    core/services/editor/EditorService.h
    core/services/editor/EditorService.cpp
    # Core Module - UI Canvas
    core/ui/canvas/Canvas2DView.h
    core/ui/canvas/Canvas2DView.cpp
    # Core Module - UI ViewModels
    core/ui/viewmodels/GenericPropertiesViewModel.h
    core/ui/viewmodels/GenericPropertiesViewModel.cpp
    # Core Module - Infrastructure Config
    core/infrastructure/config/ConfigManager.h
    core/infrastructure/config/ConfigManager.cpp
    # Core Module - Infrastructure Logging
    core/infrastructure/logging/LogCategories.h
    core/infrastructure/logging/LogCategories.cpp
    core/infrastructure/logging/LogManager.h
    core/infrastructure/logging/LogManager.cpp
    # Core Module - Infrastructure Plugins
    core/infrastructure/plugins/interfaces/IShapePlugin.h
    core/infrastructure/plugins/interfaces/IJointPlugin.h
    core/infrastructure/plugins/core/PluginManager.h
    core/infrastructure/plugins/core/PluginManager.cpp
    # Core Module - Infrastructure DI
    core/infrastructure/di/ServiceLocator.h
    core/infrastructure/di/ServiceLocator.cpp
)

qt_add_qml_module(appLaserCutStudio
    URI LaserCutStudio
    VERSION 1.0
    QML_FILES
        Main.qml
        EditorView.qml
        ToolPalette.qml
        ShapesListPanel.qml
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

# Include directories pour accès avec "core/..."
target_include_directories(appLaserCutStudio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
