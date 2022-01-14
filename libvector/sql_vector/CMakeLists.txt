cmake_minimum_required(VERSION 3.20)

project(sql_vector LANGUAGES CXX)

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
  sql_vector.cpp
  )

set(HEADERS_INSTALL
  sql_vector.h
 )

add_library(sql_vector STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(sql_vector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sql_vector PRIVATE Qt${QT_VERSION_MAJOR}::Core PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(sql_vector PRIVATE MATH_VECTOR_LIBRARY)

install(TARGETS sql_vector DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


