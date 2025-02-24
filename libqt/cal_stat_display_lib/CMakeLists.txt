cmake_minimum_required(VERSION 3.23)

project(cal_stat_display_lib VERSION 1.0 LANGUAGES CXX)

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


# find_package(Boost)

include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/base/msg_logger
${CMAKE_SOURCE_DIR}/base/prc_com
${Boost_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/qcustomplot
${CMAKE_SOURCE_DIR}/libqt/plotlib
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libxml/calstat
${CMAKE_SOURCE_DIR}/libqt/xmlcallib
${CMAKE_SOURCE_DIR}/libqt/gui_items
${CMAKE_SOURCE_DIR}/libvector/math_vector

)

set(SOURCES
  cal_stat_display_lib.cpp
  cal_stat_display_lib.ui
)

set(HEADERS_INSTALL
  cal_stat_display_lib.h
)


add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL})


target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
PRIVATE qcustomplot
PRIVATE plotlib
PRIVATE math_vector
PRIVATE tinyxml2
PRIVATE tinyxmlwriter
PRIVATE measdocxml
PRIVATE calibration
PRIVATE calstat
PRIVATE xmlcallib
PRIVATE gui_items
PRIVATE db_sensor_query
PRIVATE sql_base_tools
PRIVATE sql_vector
)

IF(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    IF(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    ENDIF()
ENDIF()


target_compile_definitions(${PROJECT_NAME} PRIVATE CAL_STAT_DISPLAY_LIB_LIBRARY)
# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
