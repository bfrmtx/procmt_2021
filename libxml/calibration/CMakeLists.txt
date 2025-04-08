

project(calibration VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6 REQUIRED COMPONENTS Core Sql )

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/libxml/tinyxml2
${CMAKE_SOURCE_DIR}/libxml/tinyxmlwriter
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/sql_base_tools
${CMAKE_SOURCE_DIR}/libxml/db_sensor_query
${CMAKE_SOURCE_DIR}/libxml/math_vector
${CMAKE_SOURCE_DIR}/base/msg_logger
)

set(SOURCES
  calibration.cpp
)

set(HEADERS_INSTALL
  calibration.h
)


add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE tinyxml2
PRIVATE tinyxmlwriter
PRIVATE measdocxml
PRIVATE sql_base_tools
PRIVATE db_sensor_query
PRIVATE math_vector
)

target_compile_definitions(${PROJECT_NAME} PRIVATE CALIBRATION_LIBRARY)

# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)


