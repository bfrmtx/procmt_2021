cmake_minimum_required(VERSION 3.20)

project(mestplotlib LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)


IF(QCUSTOMPLOT_USE_OPENGL)
  find_package(QT NAMES Qt6 COMPONENTS Widgets OpenGL PrintSupport REQUIRED)
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL PrintSupport REQUIRED)
ELSE()
  find_package(QT NAMES Qt6 COMPONENTS Widgets PrintSupport REQUIRED)
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport REQUIRED)
ENDIF()


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

# want on top CMAKELists.txt option(BUILD_SHARED_LIBS "Build using static libraries" OFF)
add_library( ${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries( ${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
PRIVATE qcustomplot
)

IF(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    IF(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    ENDIF()
ENDIF()


target_compile_definitions( ${PROJECT_NAME} PRIVATE MESTPLOTLIB_LIBRARY)
install(TARGETS  ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
