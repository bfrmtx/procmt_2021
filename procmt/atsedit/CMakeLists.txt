cmake_minimum_required(VERSION 3.20)

project(atsedit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(QT NAMES Qt6 COMPONENTS Core Widgets Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Sql REQUIRED)

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
${CMAKE_SOURCE_DIR}/adulib/aduxml


)
set(PROJECT_SOURCES
atsedit.cpp
atsedit.h
atsedit.ui
overwrite_dialog.cpp
overwrite_dialog.h
overwrite_dialog.ui
main.cpp
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
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
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    # for GUI
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

