cmake_minimum_required(VERSION 3.20)

project(mt_data LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/base/prc_com)

set(SOURCES
  mt_data.cpp
  )

set(HEADERS_INSTALL
  mt_data.h
 )

add_library(mt_data STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(mt_data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mt_data PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(mt_data PRIVATE MT_DATA_LIBRARY)

install(TARGETS mt_data DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


