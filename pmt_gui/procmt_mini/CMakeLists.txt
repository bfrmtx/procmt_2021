cmake_minimum_required(VERSION 3.20)

project(procmt_mini VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------RPATH for loading .so from ../lib ------------------------------------------
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# below bin we have lib, so bin/lib where I put re-locatable so libs or dlls
if (APPLE OR UNIX)
set(CMAKE_INSTALL_RPATH "/usr/local/procmt/lib")
endif()

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
if (APPLE OR UNIX)
 set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# ------------------------------------------------------------

find_package(QT NAMES Qt6 COMPONENTS Core Sql Gui Widgets Network OpenGL OpenGLWidgets PrintSupport  REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Gui  Widgets Network OpenGL OpenGLWidgets PrintSupport  REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(

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
${CMAKE_SOURCE_DIR}/libqt/static_plot
${CMAKE_SOURCE_DIR}/libqt/qstring_utilities
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/tinyxml2
${CMAKE_SOURCE_DIR}/libxml/tinyxmlwriter
${CMAKE_SOURCE_DIR}/libxml/sql_base_tools
${CMAKE_SOURCE_DIR}/libxml/prc_com_xml
${CMAKE_SOURCE_DIR}/base/msg_logger
${CMAKE_SOURCE_DIR}/libvector/fftreal_vector
${CMAKE_SOURCE_DIR}/firfilter
${CMAKE_SOURCE_DIR}/procmt/ats_ascii_file
${CMAKE_SOURCE_DIR}/procmt/acsp_prz
${CMAKE_SOURCE_DIR}/procmt/spc_db
${CMAKE_SOURCE_DIR}/procmt/atsfile_collector
${CMAKE_SOURCE_DIR}/libxml/db_sensor_query
${CMAKE_SOURCE_DIR}/libvector/math_vector
${CMAKE_SOURCE_DIR}/procmt/edi_file
${CMAKE_SOURCE_DIR}/procmt/mt_data
${CMAKE_SOURCE_DIR}/procmt/mc_data
${CMAKE_SOURCE_DIR}/procmt/mt_site
${CMAKE_SOURCE_DIR}/procmt/procmt_lib
${CMAKE_SOURCE_DIR}/adulib/aduxml
${CMAKE_SOURCE_DIR}/adulib/phoenix_json
${CMAKE_SOURCE_DIR}/adulib/adu_json
${CMAKE_SOURCE_DIR}/pmt_adulib/group_atsfiles
${CMAKE_SOURCE_DIR}/qcustomplot
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/pmt_gui/spectra_plotter_pro

)
set(PROJECT_SOURCES
main.cpp
procmt_mini.cpp
dir_dialog.cpp
procmt_mini.h
dir_dialog.h
procmt_mini.ui
dir_dialog.ui
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(procmt_mini
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET procmt_mini APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(procmt_mini SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(procmt_mini
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(procmt_mini
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE Qt${QT_VERSION_MAJOR}::Network

PRIVATE spectra_plotter_pro
PRIVATE static_plot
PRIVATE qcustomplot

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
PRIVATE edi_file
PRIVATE lineedit_file
PRIVATE group_atsfiles
PRIVATE qstring_utilities


PRIVATE procmt_lib
PRIVATE prc_com_xml
PRIVATE spc_db
)

set_target_properties(procmt_mini PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(procmt_mini)
endif()

install(TARGETS procmt_mini DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

