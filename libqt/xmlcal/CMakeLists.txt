project(xmlcal VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)

include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/base/msg_logger
${CMAKE_SOURCE_DIR}/base/prc_com
${Boost_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/qcustomplot
${CMAKE_SOURCE_DIR}/libqt/plotlib
${CMAKE_SOURCE_DIR}/libqt/gui_items
${CMAKE_SOURCE_DIR}/libxml/measdocxml
${CMAKE_SOURCE_DIR}/libxml/calibration
${CMAKE_SOURCE_DIR}/libxml/calstat
${CMAKE_SOURCE_DIR}/boost_student
${CMAKE_SOURCE_DIR}/libvector/math_vector
${CMAKE_SOURCE_DIR}/libqt/xmlcallib

)

set(SOURCES
        main.cpp
                resources.qrc
)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/macos_app_icon.cmake)

if(WIN32)
        target_sources(${PROJECT_NAME} PRIVATE app_icon.rc)
endif()

if(APPLE)
                configure_macos_app_icon(${PROJECT_NAME} icons/xmlcal.png)
endif()

install(FILES ${PROJECT_NAME}.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/desktop)
install(DIRECTORY icons/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/desktop)

include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE qcustomplot
PRIVATE plotlib
PRIVATE gui_items
PRIVATE boost_student
PRIVATE math_vector
PRIVATE tinyxml2
PRIVATE tinyxmlwriter
PRIVATE measdocxml
PRIVATE calibration
PRIVATE db_sensor_query
PRIVATE sql_base_tools
PRIVATE sql_vector
PRIVATE xmlcallib
)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)
# for qcustomplot
include(${CMAKE_SOURCE_DIR}/cmake/qcustomplot_use.cmake)
