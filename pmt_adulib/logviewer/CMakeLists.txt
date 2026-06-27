project(logviewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/qcustomplot

)
set(SOURCES
mainwindow.cpp
mainwindow.h
mainwindow.ui
main.cpp
)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE qcustomplot
)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)
# for qcustomplot
include(${CMAKE_SOURCE_DIR}/cmake/qcustomplot_use.cmake)
