

project(firfilter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6 REQUIRED COMPONENTS Core)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/base/prc_com)

set(SOURCES
  firfilter.cpp
)

set(HEADERS_INSTALL
  firfilter.h
)


add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE prc_com
)

target_compile_definitions(${PROJECT_NAME} PRIVATE FIRFILTER_LIBRARY)

# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)


