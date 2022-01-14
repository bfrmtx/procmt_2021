#
cmake_minimum_required(VERSION 3.20)
project(procmt_2021 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
# set CMAKE_BUILD_TYPE RELEASE for CLANG
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  message ("UNIX LINUX Apple Clang build")
 IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message ("debug build")
    add_compile_options("-std=c++20 -march=native -pthread -DDEBUG -ggdb3 -O0 -Wsign-compare -Wall -Wextra -Werror -pedantic")
    set(CMAKE_VERBOSE_MAKEFILE ON)
 ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
 IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
    message ("release build")
    add_compile_options("-std=c++20 -march=native -pthread -DNDEBUG -O3 -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare")
 ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)
endif()
#
#
# set CMAKE_BUILD_TYPE RELEASE for MSVC
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
  message ( "WINDOWS MSVC build" )
  add_compile_definitions( _USE_MATH_DEFINES _HAS_STD_BYTE _msvc )
  IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message ("debug build")
    add_compile_options(" /std:c++20 /DEBUG ")
    set(CMAKE_VERBOSE_MAKEFILE ON)
 ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
 IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
    message ("release build")
    add_compile_options(" /std:c++20 /O2 ")
 ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)

endif()



message ( "adding subdirs" )
# the info.sql3 and others will be copied to your build dir - the software will look for SQLite databases
# DEFINES += PMTSQLPATH=\"\\\"$${PJH}/$${OSDIR}$${CPLDIR}\\\"\"
add_definitions(-DPMTSQLPATH="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DPMTSQLPATH_UNIX="${CMAKE_INSTALL_PREFIX}/bin")
add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
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
add_subdirectory(libqt/mest_plotlib) ## nor needed for adroid app?
add_subdirectory(libqt/prc_com_table)
add_subdirectory(libqt/static_plot) ## nor needed for adroid app?
add_subdirectory(libqt/gui_items)
add_subdirectory(libqt/qstring_utilities)
add_subdirectory(libqt/plotlib)
add_subdirectory(libqt/xy_regression_plot) ## nor needed for adroid app?
add_subdirectory(libqt/xmlcallib)
add_subdirectory(libqt/cal_stat_display_lib) ## nor needed for adroid app?
add_subdirectory(libqt/lineedit_file)
add_subdirectory(libqt/gen_trf_filename) ## nor needed for adroid app
add_subdirectory(libqt/cal2sql) ## nor needed for adroid app
add_subdirectory(libqt/xmlcal ) ## nor needed for adroid app
add_subdirectory(libqt/mk_master_cal) ## nor needed for adroid app
add_subdirectory(firfilter) ## nor needed for adroid app
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
add_subdirectory(pmt_gui/tsplotter)
add_subdirectory(pmt_gui/ediplotter)
add_subdirectory(pmt_gui/spectra_plotter_pro)
add_subdirectory(pmt_gui/procmt_mini)
