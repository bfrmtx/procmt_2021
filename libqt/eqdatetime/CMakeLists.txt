cmake_minimum_required(VERSION 3.20)

project(eqdatetime LANGUAGES CXX)

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
  eqdatetime.cpp
  )

set(HEADERS_INSTALL
  eqdatetime.h
 )

add_library(eqdatetime STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(eqdatetime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(eqdatetime PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(eqdatetime PRIVATE PRC_COM_LIBRARY)

install(TARGETS eqdatetime DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


