cmake_minimum_required(VERSION 3.20)

project(db_sensor_query LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/base/msg_logger )

set(SOURCES
  db_sensor_query.cpp
  )

set(HEADERS_INSTALL
  db_sensor_query.h
 )

add_library(db_sensor_query STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(db_sensor_query PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(db_sensor_query PRIVATE Qt${QT_VERSION_MAJOR}::Core PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(db_sensor_query PRIVATE DB_SENSOR_QUERY_LIBRARY)

install(TARGETS db_sensor_query DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


