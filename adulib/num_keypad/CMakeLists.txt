cmake_minimum_required(VERSION 3.20)

project(num_keypad LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 COMPONENTS Widgets Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql REQUIRED)
include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_SOURCE_DIR}/include )

set(SOURCES
  num_keypad.cpp
  num_keypad.ui
  num_keypad_resources.qrc
)

set(HEADERS_INSTALL

  num_keypad.h
)

# want STATIC
add_library(${PROJECT_NAME} STATIC  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
)

target_compile_definitions(${PROJECT_NAME} PRIVATE NUM_KEYPAD_LIBRARY)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
