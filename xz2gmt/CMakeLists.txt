

project(xz2gmt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)


find_package(Qt6
    REQUIRED COMPONENTS Core Widgets
)

set(SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)
# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)

