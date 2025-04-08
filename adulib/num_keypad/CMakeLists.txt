

project(num_keypad  VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)


find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets
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


add_library(${PROJECT_NAME} SHARED  ${SOURCES} ${HEADERS_INSTALL})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)



target_compile_definitions(${PROJECT_NAME} PRIVATE NUM_KEYPAD_LIBRARY)
# for libraries
include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
