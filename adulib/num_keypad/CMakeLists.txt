cmake_minimum_required(VERSION 3.20)

project(num_keypad LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/out_of_tree_build.cmake)

find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets Sql
)


include_directories(${CMAKE_INSTALL_PREFIX}/include)

set(SOURCES
  num_keypad.cpp
  num_keypad.ui
  num_keypad_resources.qrc
)

set(HEADERS_INSTALL
  num_keypad.h
)

# want STATIC
add_library(${PROJECT_NAME} SHARED  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)



target_compile_definitions(${PROJECT_NAME} PRIVATE NUM_KEYPAD_LIBRARY)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
