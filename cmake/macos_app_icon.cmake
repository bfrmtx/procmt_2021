function(configure_macos_app_icon target icon_png)
    if(NOT APPLE)
        return()
    endif()

    find_program(SIPS_EXECUTABLE sips REQUIRED)
    find_program(ICONUTIL_EXECUTABLE iconutil REQUIRED)

    get_filename_component(icon_png_abs "${icon_png}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    get_filename_component(icon_basename "${icon_png}" NAME_WE)

    set(iconset_dir "${CMAKE_CURRENT_BINARY_DIR}/${icon_basename}.iconset")
    set(generated_icns "${CMAKE_CURRENT_BINARY_DIR}/${icon_basename}.icns")
    set(generate_icon_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/generate_macos_app_icon.cmake")

    add_custom_command(
        OUTPUT "${generated_icns}"
        COMMAND ${CMAKE_COMMAND}
                -DICON_PNG=${icon_png_abs}
                -DICONSET_DIR=${iconset_dir}
                -DGENERATED_ICNS=${generated_icns}
                -DSIPS_EXECUTABLE=${SIPS_EXECUTABLE}
                -DICONUTIL_EXECUTABLE=${ICONUTIL_EXECUTABLE}
                -P ${generate_icon_script}
        DEPENDS "${icon_png_abs}" "${generate_icon_script}"
        COMMENT " "
        VERBATIM
    )

    target_sources(${target} PRIVATE "${generated_icns}")
    set_source_files_properties("${generated_icns}" PROPERTIES GENERATED TRUE MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_ICON_FILE "${icon_basename}.icns")
endfunction()
