cmake_minimum_required(VERSION 3.20)

project(gui_items LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)


find_package(QT NAMES Qt6 COMPONENTS Widgets Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql REQUIRED)
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include )

set(SOURCES
  gui_items.cpp
)

set(HEADERS_INSTALL

  gui_items.h
)


# want on top CMAKELists.txt option(BUILD_SHARED_LIBS "Build using static libraries" OFF)
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GUI_ITEMS_LIBRARY)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
