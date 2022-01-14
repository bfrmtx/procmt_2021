cmake_minimum_required(VERSION 3.20)

project(csem_db LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/base/prc_com
${CMAKE_SOURCE_DIR}/procmt/mt_data
${CMAKE_SOURCE_DIR}/libvector/math_vector

)

set(SOURCES
  csem_db.cpp
  )

set(HEADERS_INSTALL
  csem_db.h
 )

add_library(csem_db STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(csem_db PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(csem_db
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE prc_com
PRIVATE mt_data
PRIVATE math_vector

)

target_compile_definitions(csem_db PRIVATE CSEM_DB_LIBRARY)

install(TARGETS csem_db DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


