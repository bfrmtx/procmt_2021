cmake_minimum_required(VERSION 3.20)

project(calstat LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/libvector/math_vector)

set(SOURCES
  calstat.cpp
  )

set(HEADERS_INSTALL
  calstat.h
 )

add_library(calstat SHARED ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(calstat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(calstat PRIVATE Qt${QT_VERSION_MAJOR}::Core PRIVATE math_vector)

target_compile_definitions(calstat PRIVATE CALSTAT_LIBRARY)

install(TARGETS calstat DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


