

project(spectra_plotter_pro LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets Network Sql OpenGL OpenGLWidgets
    OPTIONAL_COMPONENTS PrintSupport
)

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
    ${CMAKE_SOURCE_DIR}/spectra_plotter_pro/default_assets
    ${CMAKE_SOURCE_DIR}/spectra_plotter_pro/glm
    ${CMAKE_SOURCE_DIR}/spectra_plotter_pro/ygfx
    ${CMAKE_SOURCE_DIR}/spectra_plotter_pro/ygfxqt
)

set(SOURCES

    progressdialog.cpp
    ygfx/gfxarraybasecontainer.cpp
    ygfx/gfxfloatbufferset.cpp
    ygfx/gfxshader.cpp
    ygfx/gfxuniforms.cpp
    ygfxqt/gfx_qt_math_selection.cpp
    ygfxqt/gfx_qt_math_selector.cpp
    ygfxqt/gfx_qt_mathplot.cpp
    ygfxqt/gfx_qt_mathplot_set.cpp
    ygfxqt/gfx_qt_widget_maths.cpp
    ygfxqt/gfx_qt_helper.cpp
    ygfxqt/gfx_qt_assetmanager.cpp
    spectra_plotter_pro.cpp

    progressdialog.h
    ygfx.h
    ygfx/gfxarray.h
    ygfx/gfxarraybasecontainer.h
    ygfx/gfxconstexpr.h
    ygfx/gfxfloatbufferset.h
    ygfx/gfxincludes.h
    ygfx/gfxshader.h
    ygfx/gfxuniforms.h
    ygfx/gfxvector.h
    ygfx/gfxvertex.h
    ygfxqt/gfx_qt_math_selection.h
    ygfxqt/gfx_qt_math_selector.h
    ygfxqt/gfx_qt_mathplot.h
    ygfxqt/gfx_qt_mathplot_set.h
    ygfxqt/gfx_qt_widget_maths.h
    ygfxqt/gfx_qt_helper.h
    ygfxqt/gfx_qt_assetmanager.h

    spectra_plotter.ui
    progressdialog.ui

    default_resources.qrc

)
set(HEADERS_INSTALL
    spectra_plotter_pro.h
)



add_library(${PROJECT_NAME} SHARED ${SOURCES} )

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::OpenGL
    Qt6::Sql
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
    mt_site
    qcustomplot
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE SPECTRA_PLOTTER_PRO_LIBRARY)
# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
