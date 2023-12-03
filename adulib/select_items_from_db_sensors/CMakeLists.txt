cmake_minimum_required(VERSION 3.23)

project(select_items_from_db_sensors VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)

find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets Sql
)


include_directories(
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libxml/db_sensor_query
    ${CMAKE_SOURCE_DIR}/base/msg_logger
 )

set(SOURCES
    select_items_from_db_sensors.cpp
    select_items_from_db_sensors.ui
)

set(HEADERS_INSTALL
    select_items_from_db_sensors.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    db_sensor_query
    msg_logger
 )

target_compile_definitions(${PROJECT_NAME} PRIVATE SELECT_ITEMS_FROM_DB_SENSORS_LIBRARY)
# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
