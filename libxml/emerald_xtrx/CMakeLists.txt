cmake_minimum_required(VERSION 3.20)

project(emerald_xtrx LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/libxml/tinyxml2
${CMAKE_SOURCE_DIR}/libxml/measdocxml
)

set(SOURCES
  emerald_xtrx.cpp
)

set(HEADERS_INSTALL
  emerald_xtrx.h
)

# want on top CMAKELists.txt option(BUILD_SHARED_LIBS "Build using static libraries" OFF)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE tinyxml2 PRIVATE measdocxml)

target_compile_definitions(${PROJECT_NAME} PRIVATE EMERALD_XTRX_LIBRARY)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


