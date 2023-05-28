cmake_minimum_required(VERSION 3.20)

project(spectra_plotter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)

IF(QCUSTOMPLOT_USE_OPENGL)
    find_package(QT NAMES Qt6 COMPONENTS Core Sql Gui Widgets Network OpenGL PrintSupport  REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Gui Widgets Network OpenGL PrintSupport  REQUIRED)
ELSE()
    find_package(QT NAMES Qt6 COMPONENTS Core Sql Gui Widgets Network PrintSupport  REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Gui Widgets Network PrintSupport  REQUIRED)
ENDIF()


#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/qcustomplot
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
${CMAKE_SOURCE_DIR}/procmt/mt_site
${CMAKE_SOURCE_DIR}/adulib/aduxml

)
# take all files from this directory
# does not work with QTCreator file(GLOB SOURCES "*.h" "*.cpp" ".ui")
set(SOURCES
graph_selector.cpp
graph_selector.h
# main.cpp
plotdata.cpp
plotdata.h
plotwidget.cpp
plotwidget.h
progressdialog.cpp
progressdialog.h
progressdialog.ui
spectra_plotter.cpp

spectra_plotter.ui
)

set(HEADERS_INSTALL
    spectra_plotter.h
)

# want STATIC
add_library(${PROJECT_NAME}   ${SOURCES} )

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE Qt${QT_VERSION_MAJOR}::Gui
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Network
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
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
PRIVATE mt_site
PRIVATE qcustomplot
)

IF(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    IF(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    ENDIF()
ENDIF()


target_compile_definitions(${PROJECT_NAME} PRIVATE SPECTRA_PLOTTER_LIBRARY)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
