cmake_minimum_required(VERSION 3.23)

project(gmttools VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)


find_package(Qt6 REQUIRED COMPONENTS Core)

set(SOURCES
        main.cpp
        mtx_tmath.h
        std_colour_scale.h
        std_colour_scale.cpp
)


# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)

include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/base/prc_com)
target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE prc_com
)


# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)
