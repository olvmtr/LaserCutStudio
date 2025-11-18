cmake_minimum_required(VERSION 3.16)

project(LaserCutStudio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(appLaserCutStudio
    main.cpp
    # Core Module - Interface
    core/interface/Interface.h
    core/interface/Interface.cpp
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
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appLaserCutStudio
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Ajoute les tests
add_subdirectory(tests)
