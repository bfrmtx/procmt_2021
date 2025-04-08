

project(logviewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


IF(QCUSTOMPLOT_USE_OPENGL)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL Sql PrintSupport REQUIRED)
ELSE()
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql PrintSupport REQUIRED)
ENDIF()


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

IF(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    IF(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    ENDIF()
ENDIF()

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)


