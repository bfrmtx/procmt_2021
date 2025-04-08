

project(atsfile_collector VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6 REQUIRED COMPONENTS Core Sql )

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_SOURCE_DIR}/include
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/base/prc_com
${CMAKE_SOURCE_DIR}/base/msg_logger
${CMAKE_SOURCE_DIR}/fftreal
${CMAKE_SOURCE_DIR}/libqt/
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libvector/math_vector
${CMAKE_SOURCE_DIR}/libqt/eqdatetime
${CMAKE_SOURCE_DIR}/procmt/atsheader
${CMAKE_SOURCE_DIR}/procmt/atmfile
${CMAKE_SOURCE_DIR}/procmt/atsfile
${CMAKE_SOURCE_DIR}/firfilter
${CMAKE_SOURCE_DIR}/libvector/fftreal_vector
${CMAKE_SOURCE_DIR}/procmt/ats_ascii_file


)

set(SOURCES
  atsfile_collector.cpp
)

set(HEADERS_INSTALL
  atsfile_collector.h
)

add_library(${PROJECT_NAME}   ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE math_vector
PRIVATE tinyxml2
PRIVATE tinyxmlwriter
PRIVATE measdocxml
PRIVATE calibration
PRIVATE db_sensor_query
PRIVATE sql_base_tools
PRIVATE sql_vector
PRIVATE prc_com
PRIVATE msg_logger
PRIVATE eqdatetime
PRIVATE atsheader
PRIVATE atsfile
PRIVATE ats_ascii_file
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ATSFILE_COLLECTOR_LIBRARY)

# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)


