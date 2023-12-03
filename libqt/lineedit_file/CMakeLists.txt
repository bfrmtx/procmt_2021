cmake_minimum_required(VERSION 3.23)

project(lineedit_file VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)


find_package( Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
  lineedit_file.cpp
)

set(HEADERS_INSTALL
  lineedit_file.h
)

# want STATIC - so added  STATIC here
add_library(${PROJECT_NAME} STATIC  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)

target_compile_definitions(${PROJECT_NAME} PRIVATE lineedit_file_LIBRARY)
# for libraries

include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
