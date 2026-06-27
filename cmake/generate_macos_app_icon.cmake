function(run_sips width height output_name)
    execute_process(
        COMMAND "${SIPS_EXECUTABLE}" -z "${width}" "${height}" "${ICON_PNG}" --out "${ICONSET_DIR}/${output_name}"
        RESULT_VARIABLE command_result
        OUTPUT_QUIET
        OUTPUT_VARIABLE command_stdout
        ERROR_VARIABLE command_stderr
    )

    if(NOT command_result EQUAL 0)
        string(STRIP "${command_stdout}" command_stdout)
        string(STRIP "${command_stderr}" command_stderr)
        if(command_stderr)
            message(FATAL_ERROR "${command_stderr}")
        endif()

        if(command_stdout)
            message(FATAL_ERROR "${command_stdout}")
        endif()

        message(FATAL_ERROR "Command failed: ${SIPS_EXECUTABLE}")
    endif()
endfunction()

function(run_iconutil)
    execute_process(
        COMMAND "${ICONUTIL_EXECUTABLE}" -c icns "${ICONSET_DIR}" -o "${GENERATED_ICNS}"
        RESULT_VARIABLE command_result
        OUTPUT_QUIET
        OUTPUT_VARIABLE command_stdout
        ERROR_VARIABLE command_stderr
    )

    if(NOT command_result EQUAL 0)
        string(STRIP "${command_stdout}" command_stdout)
        string(STRIP "${command_stderr}" command_stderr)
        if(command_stderr)
            message(FATAL_ERROR "${command_stderr}")
        endif()

        if(command_stdout)
            message(FATAL_ERROR "${command_stdout}")
        endif()

        message(FATAL_ERROR "Command failed: ${ICONUTIL_EXECUTABLE}")
    endif()
endfunction()

file(REMOVE_RECURSE "${ICONSET_DIR}")
file(MAKE_DIRECTORY "${ICONSET_DIR}")

run_sips(16 16 icon_16x16.png)
run_sips(32 32 icon_16x16@2x.png)
run_sips(32 32 icon_32x32.png)
run_sips(64 64 icon_32x32@2x.png)
run_sips(128 128 icon_128x128.png)
run_sips(256 256 icon_128x128@2x.png)
run_sips(256 256 icon_256x256.png)
run_sips(512 512 icon_256x256@2x.png)
run_sips(512 512 icon_512x512.png)
run_sips(1024 1024 icon_512x512@2x.png)
run_iconutil()