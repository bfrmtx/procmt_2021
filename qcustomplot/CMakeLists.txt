project(qcustomplot VERSION 2.1.1  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)

IF(QCUSTOMPLOT_USE_OPENGL)
find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets OpenGL
    OPTIONAL_COMPONENTS PrintSupport
)
ELSE()
find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets
    OPTIONAL_COMPONENTS PrintSupport
)
ENDIF()

set(SOURCES
  qcustomplot.cpp
)

set(HEADERS_INSTALL
  qcustomplot_global.h
  qcustomplot.h
)

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${HEADERS_INSTALL}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(TARGET Qt6::PrintSupport)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::PrintSupport)
endif()

IF(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    IF(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    ENDIF()
ENDIF()

IF (QCUSTOMPLOT_USE_OPENGL)
  message ("qcustomplot openGL")
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    QCUSTOMPLOT_LIBRARY
    QCUSTOMPLOT_USE_OPENGL
)
ELSE()
    message ("qcustomplot default")
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    QCUSTOMPLOT_LIBRARY
)
ENDIF()

# for libraries

include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
