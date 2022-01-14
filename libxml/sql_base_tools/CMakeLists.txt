cmake_minimum_required(VERSION 3.20)

project(sql_base_tools LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)

set(SOURCES
  sql_base_tools.cpp
  )

set(HEADERS_INSTALL
  sql_base_tools.h
 )

add_library(sql_base_tools STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(sql_base_tools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sql_base_tools PRIVATE Qt${QT_VERSION_MAJOR}::Core PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(sql_base_tools PRIVATE SQL_BASE_TOOLS_LIBRARY)

install(TARGETS sql_base_tools DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


