cmake_minimum_required(VERSION 3.23)

project(aduxml VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)

find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets Sql Network
)

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


add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    tinyxml2
    tinyxmlwriter
    measdocxml
    calibration
    db_sensor_query
    sql_base_tools
    sql_vector
    prc_com
    msg_logger
    eqdatetime
    geocoordinates
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ADUXML_LIBRARY)

# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)


