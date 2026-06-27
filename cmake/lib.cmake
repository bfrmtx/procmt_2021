# Ensure .lib file is generated for DLL on Windows
# @todo: This is not needed anymore with CMake 3.13 and above
# See https://cmake.org/cmake/help/latest/prop_tgt/WINDOWS_EXPORT_ALL_SYMBOLS.html
# Global data symbols must be explicitly marked with __declspec(dllimport) in order to link to data in the .dll
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()
#include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}  # Library files (*.so)
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}  # Runtime binaries
)

if(NOT HEADERS_INSTALL)
message ("no headers to install")
else()
install(FILES ${HEADERS_INSTALL} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
endif()
