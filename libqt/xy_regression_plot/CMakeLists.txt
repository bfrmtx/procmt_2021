cmake_minimum_required(VERSION 3.20)

project(xy_regression_plot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets OpenGL Sql PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL Sql PrintSupport REQUIRED)
find_package(Boost REQUIRED)

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


add_library(xy_regression_plot SHARED  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(xy_regression_plot PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
PRIVATE qcustomplot
PRIVATE plotlib
PRIVATE gui_items
PRIVATE boost_student
PRIVATE math_vector
)

target_compile_definitions(xy_regression_plot PRIVATE XY_REGRESSION_PLOT_LIBRARY)
install(TARGETS xy_regression_plot DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
