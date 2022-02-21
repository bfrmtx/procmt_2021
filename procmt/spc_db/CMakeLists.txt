cmake_minimum_required(VERSION 3.20)

project(spc_db LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/base/prc_com
${CMAKE_SOURCE_DIR}/procmt/mt_data
${CMAKE_SOURCE_DIR}/libvector/math_vector
)

set(SOURCES
  spc_db.cpp
)

set(HEADERS_INSTALL
  spc_db.h
)

# want STATIC
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS_INSTALL} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
PRIVATE Qt${QT_VERSION_MAJOR}::Core
PRIVATE Qt${QT_VERSION_MAJOR}::Sql
PRIVATE prc_com
PRIVATE mt_data
PRIVATE math_vector

)

target_compile_definitions(${PROJECT_NAME} PRIVATE SPC_DB_LIBRARY)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


