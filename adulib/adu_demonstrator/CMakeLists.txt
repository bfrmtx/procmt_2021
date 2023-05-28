cmake_minimum_required(VERSION 3.20)

project(adu_demonstrator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)

find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets Sql Network
)


#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/adulib/adu_json
${CMAKE_SOURCE_DIR}/adulib/recording
)

set(SOURCES
adu_gui.cpp
adu_gui.h
adu_gui.ui
main.cpp
)

include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)


target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    adu_json
    recording
)


include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)



