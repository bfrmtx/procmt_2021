

project(mestplotlib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)

include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/qcustomplot
)

set(SOURCES
  mestplotlib.cpp
  mestplotlib.ui
)

set(HEADERS_INSTALL
  mestplotlib.h
)


add_library( ${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries( ${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE qcustomplot
)


target_compile_definitions( ${PROJECT_NAME} PRIVATE MESTPLOTLIB_LIBRARY)
install(TARGETS  ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
# for qcustomplot
include(${CMAKE_SOURCE_DIR}/cmake/qcustomplot_use.cmake)
