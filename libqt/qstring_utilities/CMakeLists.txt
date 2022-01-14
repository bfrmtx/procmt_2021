cmake_minimum_required(VERSION 3.20)

project(qstring_utilities LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)

set(SOURCES
  qstring_utilities.cpp
  )

set(HEADERS_INSTALL
  qstring_utilities.h
 )

add_library(qstring_utilities STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(qstring_utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(qstring_utilities PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(qstring_utilities PRIVATE QSTRING_UTILITIES_LIBRARY)

install(TARGETS qstring_utilities DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


