cmake_minimum_required(VERSION 3.20)

project(select_items_from_db_sensors LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql REQUIRED)
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

# want STATIC
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
    PRIVATE db_sensor_query
    PRIVATE msg_logger
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE SELECT_ITEMS_FROM_DB_SENSORS_LIBRARY)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
