cmake_minimum_required(VERSION 3.20)
project(mt_base LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS_INSTALL doubles.h  iterator_complex_templates.h  iterator_templates.h  qtx_templates.h  statmaps.h  threadbuffer.h  vector_templates.h  vector_utils.h )


install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

set(DB_INSTALL info.sql3  master_calibration.sql3  procmt.sql3 )
install(FILES ${DB_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

file(COPY 5chanEHnoise DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
