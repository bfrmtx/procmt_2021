cmake_minimum_required(VERSION 3.23)

project(csem_db VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)

find_package(Qt6
    REQUIRED COMPONENTS Core Sql
)


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


add_library(${PROJECT_NAME}    ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE prc_com
PRIVATE mt_data
PRIVATE math_vector

)

target_compile_definitions(${PROJECT_NAME}  PRIVATE CSEM_DB_LIBRARY)

install(TARGETS ${PROJECT_NAME}  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)


