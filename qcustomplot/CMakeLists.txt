cmake_minimum_required(VERSION 3.20)

project(qcustomplot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets OpenGL PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL PrintSupport REQUIRED)

set(SOURCES
  qcustomplot.cpp
)

set(HEADERS_INSTALL
  qcustomplot_global.h
  qcustomplot.h
)

# want on top CMAKELists.txt option(BUILD_SHARED_LIBS "Build using static libraries" OFF)
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
)
if (WIN32)
    if(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    endif()
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_LIBRARY)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
