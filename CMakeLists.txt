#
cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(procmt_2021 VERSION 2.1 LANGUAGES CXX)

IF (NOT DEFINED QCUSTOMPLOT_USE_OPENGL)
    message ("BUILD WITHOUT OpenGL               <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
ELSE()
    message ("BUILD WITH OpenGL                  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
    # make it availabe in header and sources
    # this also switches OpenGL on in general
    #ifdef QCUSTOMPLOT_USE_OPENGL checks the line below
    add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
ENDIF()

IF (NOT DEFINED USE_SPECTRAL_PLOTTER_PRO)
    message ("BUILD WITHOUT SPECTRAL_PLOTTER_PRO <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
ELSE()
    message ("BUILD WITH SPECTRAL_PLOTTER_PRO    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
    # make it availabe in header and sources
    #ifdef USE_SPECTRAL_PLOTTER_PRO
    add_definitions(-DUSE_SPECTRAL_PLOTTER_PRO)

ENDIF()
# test packages first
# Qt6
find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets Sql Network Xml
    OPTIONAL_COMPONENTS PrintSupport
)
if (NOT Qt6_FOUND) 
    message(FATAL_ERROR "Package Qt6 required, but not found!")
endif()

# boost
find_package(Boost REQUIRED)
if (NOT Boost_FOUND) 
    message(FATAL_ERROR "Package Boost required, but not found!")
endif()




#
# set CMAKE_BUILD_TYPE RELEASE for CLANG
IF(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32 )
# using regular Clang or AppleClang
    message ("UNIX LINUX Apple Clang build")
    # on Linux and MacOS we can build shared libs for all not explicitly marked as STATIC
    option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
    IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
        message ("debug build ---------------------")
        #add_compile_options(-pthread -ggdb3 -O0 -Wsign-compare -Wall -Wextra -Werror -pedantic)
        add_compile_options(-g -O0 -pthread -Wno-unused-parameter -Wno-sign-compare)
        set(CMAKE_VERBOSE_MAKEFILE ON)
    ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message ("release build ---------------------")
        add_compile_options(-pthread -Wno-unused-parameter -Wno-sign-compare)
    ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)
ENDIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32)
#
#
# set CMAKE_BUILD_TYPE RELEASE for g++ on mingw WINDOWS
IF(WIN32)
    message ( "WINDOWS build" )
    # on WINDOWS it is easier to make STATIC libs
    # some libs will by dynamic (dll); add PATH to the lib folder
    option(BUILD_SHARED_LIBS "Build using static libraries" OFF)
    add_compile_definitions( _USE_MATH_DEFINES _msvc )
    IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
        message ("debug build ---------------------")
        add_compile_options(-DDEBUG)
        set(CMAKE_VERBOSE_MAKEFILE ON)
    ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message ("release build ---------------------")
        add_compile_options(-DNDEBUG)
    ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)
ENDIF(WIN32)

IF (WIN32)
    message ( "starting WINDOWS build" )
# add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
# set(QCUSTOMPLOT_USE_OPENGL TRUE)
ELSE()
# add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
# set(QCUSTOMPLOT_USE_OPENGL TRUE)
    message ("starting UNIX LINUX Apple Clang build")
ENDIF()
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
IF(USE_SPECTRAL_PLOTTER_PRO)
    message ( "ADDING spectra plotter pro" )
    add_subdirectory(pmt_gui/spectra_plotter_pro)
ELSE()
    message ( "ADDING spectra plotter standard" )
    add_subdirectory(pmt_gui/spectra_plotter)
ENDIF()
add_subdirectory(pmt_gui/procmt_mini)
add_subdirectory(pmt_adulib/calib_lab)
#IF (NOT WIN32)
#message ( "skipping CoilFieldTestTool for Windows" )
add_subdirectory(CoilFieldTestTool)
add_subdirectory(InSituCoilCal)

#ENDIF()


