cmake_minimum_required(VERSION 3.20)

project(mestplotlib LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets OpenGL PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL PrintSupport REQUIRED)
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/qcustomplot
)

set(SOURCES
  mestplotlib.cpp
  mestplotlib.ui
  )

set(HEADERS_INSTALL

  mestplotlib.h
 )


add_library(mestplotlib SHARED  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(mestplotlib
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
PRIVATE qcustomplot
)

target_compile_definitions(mestplotlib PRIVATE MESTPLOTLIB_LIBRARY)
install(TARGETS mestplotlib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
