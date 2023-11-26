cmake_minimum_required(VERSION 3.14)

project(compute_shader LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui ShaderTools)

qt_standard_project_setup()

qt_add_executable(compute_shader
    main.cpp
    README.md
)

#add_executable(compute_shader
#  main.cpp
#  arrayAdd.glsl
#)
qt_add_shaders(compute_shader "shaders"
    GLSL "450"
    PREFIX "/"
    FILES
    arrayAdd.comp
)

target_link_libraries(compute_shader
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::GuiPrivate
)

include(GNUInstallDirs)
install(TARGETS compute_shader
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


