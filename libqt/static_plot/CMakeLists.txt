cmake_minimum_required(VERSION 3.20)

project(static_plot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets OpenGL PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL PrintSupport REQUIRED)
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/qcustomplot)

set(SOURCES
  static_plot.cpp
  static_plot.ui
  )

set(HEADERS_INSTALL

  static_plot.h
 )


add_library(static_plot SHARED  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(static_plot
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
PRIVATE qcustomplot
)

target_compile_definitions(static_plot PRIVATE STATIC_PLOT_LIBRARY)
install(TARGETS static_plot DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
