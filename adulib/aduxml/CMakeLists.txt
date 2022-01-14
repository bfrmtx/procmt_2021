cmake_minimum_required(VERSION 3.20)

project(aduxml LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Sql Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Widgets Network)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/base/prc_com
${CMAKE_SOURCE_DIR}/base/msg_logger
${CMAKE_SOURCE_DIR}/libgeo/geocoordinates
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libqt/eqdatetime
${CMAKE_SOURCE_DIR}/libxml/tinyxml2
${CMAKE_SOURCE_DIR}/libxml/tinyxmlwriter

)

set(SOURCES
    adulib.cpp
    aduslot.cpp
    channel.cpp
    adbboard.cpp
    cable.cpp
    gps.cpp
    sensor.cpp
    adutable.cpp
    adujoblist.cpp
    stoptime_dialog.cpp
    adunetwork.cpp
    adurt.cpp
    txm.cpp
    stoptime_dialog.ui
  )

set(HEADERS_INSTALL
    adulib.h
    aduslot.h
    channel.h
    adbboard.h
    cable.h
    gps.h
    sensor.h
    adutable.h
    adujoblist.h
    stoptime_dialog.h
    adunetwork.h
    adurt.h
    txm.h
 )

add_library(aduxml SHARED ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(aduxml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(aduxml
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Network

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
PRIVATE geocoordinates

)

target_compile_definitions(aduxml PRIVATE ADUXML_LIBRARY)

install(TARGETS aduxml DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


