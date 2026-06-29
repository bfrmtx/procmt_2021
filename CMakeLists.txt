#
cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(procmt_2021 VERSION 3.1 LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local/procmt" CACHE PATH "Install path prefix" FORCE)
endif()

### Require out-of-source builds
# I want this setting because my source directory is a rsync drive, and I I don't want
# the builds to be synced all day long. I want to keep the source directory clean.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

message("compiler used / detected " ${CMAKE_CXX_COMPILER_ID})

# uncomment the line below to use OpenGL
set(QCUSTOMPLOT_USE_OPENGL TRUE)

if(NOT DEFINED QCUSTOMPLOT_USE_OPENGL)
    message ("BUILD WITHOUT OpenGL               <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
else()
    message ("BUILD WITH OpenGL                  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
    # make it availabe in header and sources
    # this also switches OpenGL on in general
    #ifdef QCUSTOMPLOT_USE_OPENGL checks the line below
    add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
endif()

if(NOT DEFINED USE_SPECTRAL_PLOTTER_PRO)
    message ("BUILD WITHOUT SPECTRAL_PLOTTER_PRO <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
else()
    message ("BUILD WITH SPECTRAL_PLOTTER_PRO    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
    # make it availabe in header and sources
    #ifdef USE_SPECTRAL_PLOTTER_PRO
    add_definitions(-DUSE_SPECTRAL_PLOTTER_PRO)

endif()
# test packages first
# Qt6
find_package(Qt6
    REQUIRED COMPONENTS Concurrent Core Gui Widgets Sql Network Xml
    OPTIONAL_COMPONENTS PrintSupport
)
if (NOT Qt6_FOUND)
    message(FATAL_ERROR "Package Qt6 required, but not found!")
endif()

# Use CMake's platform-aware thread abstraction instead of raw -pthread flags.
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

#
# Compiler / build-type flags for non-Windows toolchains (Linux + macOS).
# Matches Clang, AppleClang (both report "...Clang") and GNU (g++).
if(NOT WIN32 AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
    message ("UNIX / Linux / macOS build (${CMAKE_CXX_COMPILER_ID})")
    #find_package(Boost REQUIRED)
    # on Linux and macOS we can build shared libs for all not explicitly marked as STATIC
    option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
    if(CMAKE_BUILD_TYPE MATCHES DEBUG)
        message ("debug build ---------------------")
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # -glldb / -fstandalone-debug are Clang/LLDB specific
            add_compile_options(-glldb -fstandalone-debug -O0 -Wno-unused-parameter -Wno-sign-compare)
        else()
            # GNU g++
            add_compile_options(-ggdb3 -O0 -Wno-unused-parameter -Wno-sign-compare)
        endif()
        set(CMAKE_VERBOSE_MAKEFILE ON)
    endif()
    if(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message ("release build ---------------------")
        add_compile_options(-march=native -O2 -DNDEBUG -Wno-unused-parameter -Wno-sign-compare)
    endif()
    message ("starting UNIX / Linux / macOS build")
endif()
#
#
# set CMAKE_BUILD_TYPE RELEASE for g++ on mingw WINDOWS
if(WIN32)
    message ( "WINDOWS build, msvc or msys2" )
    # check that a compiler is found
    if(NOT CMAKE_CXX_COMPILER)
        message(FATAL_ERROR "No C++ compiler found. Please set CMAKE_CXX_COMPILER to your compiler executable.")
    endif()
    # This tree only uses Boost headers, so avoid find_package(Boost) because
    # CMake CMP0167 removed the legacy FindBoost module.
    # Keep Boost_ROOT as an optional hint for Windows include paths.
    set(Boost_ROOT "C:/devel/boost")
    set(Boost_INCLUDE_DIRS "${Boost_ROOT}")
    if(MSVC)
        # _USE_MATH_DEFINES is only needed for MSVC to expose math constants in <cmath>
        message("Detected MSVC compiler ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
        # Uncomment if you need MSVC-specific defines
        add_compile_definitions(_USE_MATH_DEFINES _msvc)
        # my vulkan
        set(Vulkan_INCLUDE_DIR "C:/devel/VulkanSDK/")
        # /arch:AVX2 is supported by Intel and AMD processors that support AVX2 instructions.
        # It is supported by MSVC on Windows for both Intel and AMD CPUs with AVX2.
        add_compile_options(/arch:AVX2)
        # Force static build for all libraries
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
        # Thread support is provided through CMake's Threads::Threads target.
        # MSVC uses native Win32 threading underneath std::thread.
        add_compile_options(/EHsc)
    elseif(MINGW)
        message("Detected MinGW / clang compiler")
        # Add MinGW/MSYS2 specific options here if needed
        # we need static libs for the clang++ compiler !! shared libs are not working
        add_compile_options(-DBUILD_SHARED_LIBS=OFF -static)
        if(CMAKE_BUILD_TYPE MATCHES DEBUG)
            message ("debug build ---------------------")
            add_compile_options(-DDEBUG -g -Wno-unused-parameter -Wno-sign-compare)
            set(CMAKE_VERBOSE_MAKEFILE ON)
        endif()
        if(CMAKE_BUILD_TYPE MATCHES RELEASE)
            message ("release build ---------------------")
            add_compile_options(-DNDEBUG -O2 -march=x86-64-v3 -Wno-unused-parameter -Wno-sign-compare)
        endif()
    else()
        message("Unknown Windows compiler")
    endif()
    message ( "starting WINDOWS build <<<<<<<<<<<<<<<<<<<<<<" )
endif()
#
message ( "adding subdirs" )
# the info.sql3 and others will be copied to your build dir - the software will look for SQLite databases
# for this old fashioned program, the sql databases are in the bin (executable) dir in the cpp files we add "/bin/"
add_definitions(-DPMTSQLPATH="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DPMTSQLPATH_UNIX="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DPMTSQLPATH_APPLE="${CMAKE_INSTALL_PREFIX}")
#
## templates FIRST
add_subdirectory(include)
add_subdirectory(base/prc_com)
add_subdirectory(base/msg_logger)
add_subdirectory(boost_student)
add_subdirectory(qcustomplot)
add_subdirectory(xz2gmt)
add_subdirectory(libvector/fftreal_vector)
add_subdirectory(libvector/math_vector)
add_subdirectory(libvector/sql_vector)
add_subdirectory(libxml/tinyxmlwriter)
add_subdirectory(libxml/tinyxml2)
add_subdirectory(libxml/sql_base_tools)
add_subdirectory(libxml/measdocxml)
add_subdirectory(libxml/prc_com_xml)
add_subdirectory(libxml/db_sensor_query)
add_subdirectory(libxml/calibration)
add_subdirectory(libxml/emerald_xtrx)
add_subdirectory(libxml/calstat)
add_subdirectory(libgeo/geocoordinates)
add_subdirectory(libgeo/gmttools)
add_subdirectory(libqt/eqdatetime)
add_subdirectory(libqt/mest_plotlib)
add_subdirectory(libqt/prc_com_table)
add_subdirectory(libqt/static_plot)
add_subdirectory(libqt/gui_items)
add_subdirectory(libqt/qstring_utilities)
add_subdirectory(libqt/plotlib)
add_subdirectory(libqt/xy_regression_plot)
add_subdirectory(libqt/xmlcallib)
add_subdirectory(libqt/cal_stat_display_lib)
add_subdirectory(libqt/lineedit_file)
add_subdirectory(libqt/gen_trf_filename)
add_subdirectory(libqt/xmlcal )
add_subdirectory(libqt/mk_master_cal)
add_subdirectory(firfilter)
add_subdirectory(adulib/num_keypad)
add_subdirectory(adulib/select_items_from_db_sensors)
add_subdirectory(adulib/aduxml)
add_subdirectory(adulib/adu_json)
add_subdirectory(adulib/recording)
add_subdirectory(procmt/atmfile)
add_subdirectory(procmt/atsheader)
add_subdirectory(procmt/atsfile)
add_subdirectory(procmt/atsfilename)
add_subdirectory(procmt/ats_ascii_file)
add_subdirectory(procmt/mt_data)
add_subdirectory(procmt/atsfile_collector)
add_subdirectory(procmt/acsp_prz)
add_subdirectory(procmt/mc_data)
add_subdirectory(procmt/edi_file)
add_subdirectory(procmt/mt_site)
add_subdirectory(procmt/emerald)
add_subdirectory(procmt/procmt_lib)
add_subdirectory(procmt/spc_db)
add_subdirectory(procmt/csem_db)
add_subdirectory(procmt/filter)
add_subdirectory(procmt/atsedit)
add_subdirectory(procmt/mt_gensign)
add_subdirectory(procmt/procmt)
add_subdirectory(procmt/atstool)
add_subdirectory(procmt/tsdata2ats)
add_subdirectory(pmt_adulib/group_atsfiles)
add_subdirectory(pmt_adulib/logviewer)
add_subdirectory(pmt_adulib/measdoc_from_mc_data)
add_subdirectory(pmt_adulib/phoenix_json)
add_subdirectory(adulib/adu_demonstrator)
add_subdirectory(pmt_adulib/ts_tools)
add_subdirectory(pmt_gui/ats_multi_edit)
add_subdirectory(pmt_gui/tsplotter)
add_subdirectory(pmt_gui/ediplotter)
add_subdirectory(libqt/cal2sql)
if(USE_SPECTRAL_PLOTTER_PRO)
    message ( "ADDING spectra plotter pro" )
    add_subdirectory(pmt_gui/spectra_plotter_pro)
else()
    message ( "ADDING spectra plotter standard" )
    add_subdirectory(pmt_gui/spectra_plotter)
endif()
add_subdirectory(pmt_gui/procmt_mini)
add_subdirectory(pmt_adulib/calib_lab)
add_subdirectory(CoilFieldTestTool)
add_subdirectory(InSituCoilCal)
add_subdirectory(unsplit)
add_subdirectory(split)
add_subdirectory(tsmp)
