cmake_minimum_required(VERSION 3.20)

project(calib_lab LANGUAGES CXX)

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
${CMAKE_SOURCE_DIR}/fftreal
${CMAKE_SOURCE_DIR}/base/prc_com
${CMAKE_SOURCE_DIR}/procmt/atsfile
${CMAKE_SOURCE_DIR}/procmt/atsheader
${CMAKE_SOURCE_DIR}/procmt/atmfile
${CMAKE_SOURCE_DIR}/procmt/ats_ascii_file
${CMAKE_SOURCE_DIR}/libqt/eqdatetime
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/tinyxml2
${CMAKE_SOURCE_DIR}/libxml/tinyxmlwriter
${CMAKE_SOURCE_DIR}/libxml/sql_base_tools
${CMAKE_SOURCE_DIR}/base/msg_logger
${CMAKE_SOURCE_DIR}/libxml/db_sensor_query
${CMAKE_SOURCE_DIR}/procmt/edi_file
${CMAKE_SOURCE_DIR}/procmt/mt_data
${CMAKE_SOURCE_DIR}/procmt/mc_data
${CMAKE_SOURCE_DIR}/procmt/mt_site
${CMAKE_SOURCE_DIR}/libqt/qstring_utilities
${CMAKE_SOURCE_DIR}/firfilter
${CMAKE_SOURCE_DIR}/libvector/math_vector
${CMAKE_SOURCE_DIR}/libvector/fftreal_vector

)
set(SOURCES
  calib_lab.cpp
  )

set(HEADERS_INSTALL
  calib_lab.h
 )

add_library(calib_lab SHARED ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(calib_lab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(calib_lab
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE prc_com
PRIVATE mt_site
PRIVATE mt_data
PRIVATE mc_data
PRIVATE eqdatetime
PRIVATE measdocxml
PRIVATE tinyxml2
PRIVATE tinyxmlwriter
PRIVATE db_sensor_query
PRIVATE sql_base_tools
PRIVATE calibration
PRIVATE db_sensor_query
PRIVATE geocoordinates
PRIVATE qstring_utilities
PRIVATE firfilter
PRIVATE atsfile


)

target_compile_definitions(calib_lab PRIVATE CALIB_LAB_LIBRARY)

install(TARGETS calib_lab DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


