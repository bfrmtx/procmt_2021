

project(geocoordinates VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6 REQUIRED COMPONENTS Core)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include)

set(SOURCES
  geocoordinates.cpp
)

set(HEADERS_INSTALL
  geocoordinates.h
)


add_library(${PROJECT_NAME}   ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(${PROJECT_NAME} PRIVATE GEOCOORDINATES_LIBRARY)

# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)


