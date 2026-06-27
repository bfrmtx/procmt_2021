if(QCUSTOMPLOT_USE_OPENGL)
    find_package(Qt6
        REQUIRED COMPONENTS Core Gui Widgets Network Sql OpenGL
        OPTIONAL_COMPONENTS PrintSupport
    )
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL
    )
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
    endif()
else()
    find_package(Qt6
        REQUIRED COMPONENTS Core Gui Widgets Network Sql
        OPTIONAL_COMPONENTS PrintSupport
    )
endif()
if(TARGET Qt6::PrintSupport)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::PrintSupport)
endif()
