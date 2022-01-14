cmake_minimum_required(VERSION 3.20)

project(recording LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)

set(SOURCES
  recording.cpp
  )

set(HEADERS_INSTALL
  recording.h
 )

add_library(recording STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(recording PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(recording
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Widgets

)

target_compile_definitions(recording PRIVATE RECORDING_LIBRARY)

install(TARGETS recording DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


