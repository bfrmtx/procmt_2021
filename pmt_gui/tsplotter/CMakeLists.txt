cmake_minimum_required(VERSION 3.23)

project(tsplotter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)


IF(QCUSTOMPLOT_USE_OPENGL)
    find_package(Qt6
        REQUIRED COMPONENTS Core Gui Widgets Network Sql OpenGL
        OPTIONAL_COMPONENTS PrintSupport
    )
ELSE()
    find_package(Qt6
        REQUIRED COMPONENTS Core Gui Widgets Network Sql
        OPTIONAL_COMPONENTS PrintSupport
    )
ENDIF()


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
${CMAKE_SOURCE_DIR}/libqt/xmlcallib
${CMAKE_SOURCE_DIR}/libqt/plotlib
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
${CMAKE_SOURCE_DIR}/libvector/sql_vector
${CMAKE_SOURCE_DIR}/procmt/edi_file
${CMAKE_SOURCE_DIR}/procmt/mt_data
${CMAKE_SOURCE_DIR}/procmt/mc_data
${CMAKE_SOURCE_DIR}/procmt/mt_site
${CMAKE_SOURCE_DIR}/adulib/aduxml
${CMAKE_SOURCE_DIR}/adulib/phoenix_json
${CMAKE_SOURCE_DIR}/adulib/adu_json
${CMAKE_SOURCE_DIR}/pmt_adulib/group_atsfiles
${CMAKE_SOURCE_DIR}/qcustomplot

)
set(SOURCES
main.cpp
addressdialog.cpp
qextnetworkaccessmanager.cpp
qwebdav.cpp
qwebdavdialog.cpp
tsnetworkhandler.cpp
tsplotter.cpp
xmlreader.cpp
plot.cpp
qthelper.cpp
gotosampledialog.cpp
manualscalingdialog.cpp
tsplotter.h
addressdialog.h
namespace.h
qextnetworkaccessmanager.h
qwebdav.h
qwebdavdialog.h
tsnetworkhandler.h
xmlreader.h
plot.h
qthelper.h
gotosampledialog.h
manualscalingdialog.h
tsplotter.ui
addressdialog.ui
gotosampledialog.ui
manualscalingdialog.ui
qwebdavdialog.ui
)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)


target_link_libraries(${PROJECT_NAME} PRIVATE
 Qt6::Core
 Qt6::Sql
 Qt6::Gui
 Qt6::Widgets
 Qt6::Network
 prc_com
 prc_com_table
 atsfile
 atsheader
 atmfile
 eqdatetime
 calibration
 measdocxml
 tinyxml2
 tinyxmlwriter
 db_sensor_query
 sql_base_tools
 msg_logger
 fftreal_vector
 firfilter
 ats_ascii_file
 acsp_prz
 atsfile_collector
 calibration
 db_sensor_query
 math_vector
 mt_data
 geocoordinates
 gui_items
 mc_data
 aduxml
 adu_json
 phoenix_json
 mt_site
 plotlib
 lineedit_file
 group_atsfiles
 qstring_utilities
 qcustomplot
 xmlcallib
)

if(TARGET Qt6::PrintSupport)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::PrintSupport)
endif()


IF(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::OpenGL
    )
    IF(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    ENDIF()
ENDIF()

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)

