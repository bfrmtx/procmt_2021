project(unsplit VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)

qt_standard_project_setup()
include(${CMAKE_SOURCE_DIR}/cmake/macos_app_icon.cmake)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    resources.qrc
)

if(WIN32)
    target_sources(${PROJECT_NAME} PRIVATE app_icon.rc)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(APPLE)
    configure_macos_app_icon(${PROJECT_NAME} icons/unsplit.png)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

install(FILES ${PROJECT_NAME}.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/desktop)
install(DIRECTORY icons/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/desktop)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Xml
)

include(GNUInstallDirs)

# for executables
include(${CMAKE_SOURCE_DIR}/cmake/qt_gui_2.cmake)
