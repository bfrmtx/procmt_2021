

project(gen_trf_filename VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)



find_package(Qt6 REQUIRED COMPONENTS Widgets)


set(SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_1.cmake)


include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)
