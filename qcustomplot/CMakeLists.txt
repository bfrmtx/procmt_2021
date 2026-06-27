project(qcustomplot VERSION 2.1.1  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)

if(QCUSTOMPLOT_USE_OPENGL)
find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets OpenGL
    OPTIONAL_COMPONENTS PrintSupport
)
else()
find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets
    OPTIONAL_COMPONENTS PrintSupport
)
endif()

set(SOURCES
  qcustomplot.cpp
)

set(HEADERS_INSTALL
  qcustomplot_global.h
  qcustomplot.h
)

# MSVC can not link the DLL
if(WIN32)
    add_library(${PROJECT_NAME} STATIC
        ${SOURCES}
        ${HEADERS_INSTALL}
    )
else()
    add_library(${PROJECT_NAME} SHARED
        ${SOURCES}
        ${HEADERS_INSTALL}
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(TARGET Qt6::PrintSupport)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::PrintSupport)
endif()

if(QCUSTOMPLOT_USE_OPENGL)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    endif()
endif()

if(QCUSTOMPLOT_USE_OPENGL)
  message ("qcustomplot openGL")
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    QCUSTOMPLOT_LIBRARY
    QCUSTOMPLOT_USE_OPENGL
)
else()
    message ("qcustomplot default")
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    QCUSTOMPLOT_LIBRARY
)
endif()

# for libraries

include(${CMAKE_SOURCE_DIR}/cmake/lib.cmake)
