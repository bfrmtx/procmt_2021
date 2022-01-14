cmake_minimum_required(VERSION 3.20)

project(atsfilename LANGUAGES CXX)

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
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libvector/math_vector
${CMAKE_SOURCE_DIR}/libqt/eqdatetime
${CMAKE_SOURCE_DIR}/procmt/atsheader
${CMAKE_SOURCE_DIR}/procmt/atmfile

)

set(SOURCES
  atsfilename.cpp
  )

set(HEADERS_INSTALL
  atsfilename.h
 )

add_library(atsfilename SHARED ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(atsfilename PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(atsfilename
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
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
)

target_compile_definitions(atsfilename PRIVATE ATSFILENAME_LIBRARY)

install(TARGETS atsfilename DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


