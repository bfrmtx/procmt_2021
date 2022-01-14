cmake_minimum_required(VERSION 3.20)

project(lineedit_file LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
# include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include )

set(SOURCES
  lineedit_file.cpp
  )

set(HEADERS_INSTALL

  lineedit_file.h
 )


add_library(lineedit_file STATIC  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(lineedit_file PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)

target_compile_definitions(lineedit_file PRIVATE lineedit_file_LIBRARY)
install(TARGETS lineedit_file DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
