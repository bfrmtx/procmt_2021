
project(mt_base VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(HEADERS_INSTALL doubles.h  iterator_complex_templates.h  iterator_templates.h  qtx_templates.h  statmaps.h  threadbuffer.h  vector_templates.h  vector_utils.h )

install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
set(DB_INSTALL info.sql3  master_calibration.sql3  procmt.sql3 )
install(FILES ${DB_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

file(COPY 5chanEHnoise DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
