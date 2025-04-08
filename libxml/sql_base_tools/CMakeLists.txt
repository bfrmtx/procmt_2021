

project(sql_base_tools VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6 REQUIRED COMPONENTS Core Sql )

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)

set(SOURCES
  sql_base_tools.cpp
)

set(HEADERS_INSTALL
  sql_base_tools.h
)


add_library(${PROJECT_NAME}   ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(${PROJECT_NAME} PRIVATE SQL_BASE_TOOLS_LIBRARY)

# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)


