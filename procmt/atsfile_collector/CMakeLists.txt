cmake_minimum_required(VERSION 3.20)

project(atsfile_collector LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
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

add_library(atsfile_collector STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(atsfile_collector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(atsfile_collector
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
)

target_compile_definitions(atsfile_collector PRIVATE ATSFILE_COLLECTOR_LIBRARY)

install(TARGETS atsfile_collector DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


