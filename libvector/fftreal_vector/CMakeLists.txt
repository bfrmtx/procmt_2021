cmake_minimum_required(VERSION 3.20)

project(fftreal_vector LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/fftreal)

set(SOURCES
  fftreal_vector.cpp
  )

set(HEADERS_INSTALL
  fftreal_vector.h
 )

add_library(fftreal_vector STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(fftreal_vector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(fftreal_vector PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(fftreal_vector PRIVATE FFTREAL_VECTOR_LIBRARY)

install(TARGETS fftreal_vector DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


