cmake_minimum_required(VERSION 3.23)

project(xy_regression_plot VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)


IF(QCUSTOMPLOT_USE_OPENGL)
  find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL Sql PrintSupport)
ELSE()
  find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)
ENDIF()

include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${Boost_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/qcustomplot
${CMAKE_SOURCE_DIR}/libqt/plotlib
${CMAKE_SOURCE_DIR}/libqt/gui_items
${CMAKE_SOURCE_DIR}/boost_student
${CMAKE_SOURCE_DIR}/libvector/math_vector
)

set(SOURCES
  xy_regression_plot.cpp
)

set(HEADERS_INSTALL

  xy_regression_plot.h
)


add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
PRIVATE qcustomplot
PRIVATE plotlib
PRIVATE gui_items
PRIVATE boost_student
PRIVATE math_vector
)

IF(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    IF(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    ENDIF()
ENDIF()


target_compile_definitions(${PROJECT_NAME} PRIVATE XY_REGRESSION_PLOT_LIBRARY)
# for libraries

include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
