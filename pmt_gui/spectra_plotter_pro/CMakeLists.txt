cmake_minimum_required(VERSION 3.20)

project(spectra_plotter_pro LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets Gui OpenGL OpenGLWidgets PrintSupport Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets OpenGL OpenGLWidgets PrintSupport Sql REQUIRED)


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



# want STATIC
add_library(${PROJECT_NAME} STATIC ${SOURCES} )

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Gui
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
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
PRIVATE mt_site
PRIVATE qcustomplot
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE SPECTRA_PLOTTER_PRO_LIBRARY)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
