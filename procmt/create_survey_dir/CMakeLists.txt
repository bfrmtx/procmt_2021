cmake_minimum_required(VERSION 3.20)

project(create_survey_dir VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------RPATH for loading .so from ../lib ------------------------------------------
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# below bin we have lib, so bin/lib where I put re-locatable so libs or dlls
if (APPLE OR UNIX)
set(CMAKE_INSTALL_RPATH "/usr/local/procmt/lib")
endif()

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
if (APPLE OR UNIX)
 set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()
# ------------------------------------------------------------

find_package(QT NAMES Qt6 COMPONENTS Core  REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#top tree dir is CMAKE_SOURCE_DIR
include_directories(
${CMAKE_INSTALL_PREFIX}/include

)
set(PROJECT_SOURCES

main.cpp
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(create_survey_dir
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET create_survey_dir APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(create_survey_dir SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(create_survey_dir
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(create_survey_dir
PRIVATE Qt${QT_VERSION_MAJOR}::Core

)

set_target_properties(create_survey_dir PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(create_survey_dir)
endif()

install(TARGETS create_survey_dir DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

