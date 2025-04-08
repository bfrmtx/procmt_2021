

project(ts_tools VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets Network)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/fftreal
${CMAKE_SOURCE_DIR}/base/prc_com
${CMAKE_SOURCE_DIR}/libqt/prc_com_table
${CMAKE_SOURCE_DIR}/procmt/atsfile
${CMAKE_SOURCE_DIR}/procmt/atsheader
${CMAKE_SOURCE_DIR}/procmt/atmfile
${CMAKE_SOURCE_DIR}/libqt/eqdatetime
${CMAKE_SOURCE_DIR}/libqt/gui_items
${CMAKE_SOURCE_DIR}/libqt/lineedit_file
${CMAKE_SOURCE_DIR}/libqt/qstring_utilities
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/tinyxml2
${CMAKE_SOURCE_DIR}/libxml/tinyxmlwriter
${CMAKE_SOURCE_DIR}/libxml/sql_base_tools
${CMAKE_SOURCE_DIR}/base/msg_logger
${CMAKE_SOURCE_DIR}/libvector/fftreal_vector
${CMAKE_SOURCE_DIR}/firfilter
${CMAKE_SOURCE_DIR}/procmt/ats_ascii_file
${CMAKE_SOURCE_DIR}/procmt/acsp_prz
${CMAKE_SOURCE_DIR}/procmt/atsfile_collector
${CMAKE_SOURCE_DIR}/libxml/db_sensor_query
${CMAKE_SOURCE_DIR}/libvector/math_vector
${CMAKE_SOURCE_DIR}/procmt/edi_file
${CMAKE_SOURCE_DIR}/procmt/mt_data
${CMAKE_SOURCE_DIR}/procmt/mc_data
${CMAKE_SOURCE_DIR}/procmt/mt_site
${CMAKE_SOURCE_DIR}/adulib/aduxml
${CMAKE_SOURCE_DIR}/adulib/phoenix_json
${CMAKE_SOURCE_DIR}/adulib/adu_json
${CMAKE_SOURCE_DIR}/pmt_adulib/group_atsfiles


)
set(SOURCES
main.cpp
mainwindow.cpp
mainwindow.h
mainwindow.ui
)

include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Network
PRIVATE prc_com
PRIVATE prc_com_table
PRIVATE atsfile
PRIVATE atsheader
PRIVATE atmfile
PRIVATE eqdatetime
PRIVATE calibration
PRIVATE measdocxml
PRIVATE tinyxml2
PRIVATE tinyxmlwriter
PRIVATE db_sensor_query
PRIVATE sql_base_tools
PRIVATE msg_logger
PRIVATE fftreal_vector
PRIVATE firfilter
PRIVATE ats_ascii_file
PRIVATE acsp_prz
PRIVATE atsfile_collector
PRIVATE calibration
PRIVATE db_sensor_query
PRIVATE math_vector
PRIVATE mt_data
PRIVATE geocoordinates
PRIVATE gui_items
PRIVATE mc_data
PRIVATE aduxml
PRIVATE adu_json
PRIVATE phoenix_json
PRIVATE mt_site
PRIVATE lineedit_file
PRIVATE group_atsfiles
PRIVATE qstring_utilities
)

include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)

