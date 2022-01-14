cmake_minimum_required(VERSION 3.20)

project(prc_com_table LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include {CMAKE_SOURCE_DIR}/include/base/prc_com)

set(SOURCES
  prc_com_table.cpp
  )

set(HEADERS_INSTALL
  prc_com_table.h
 )

add_library(prc_com_table STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(prc_com_table PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(prc_com_table PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(prc_com_table PRIVATE PRC_COM_TABLE_LIBRARY)

install(TARGETS prc_com_table DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


