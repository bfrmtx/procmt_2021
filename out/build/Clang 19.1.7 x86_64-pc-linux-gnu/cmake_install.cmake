# Install script for directory: /home/bfr/devel/github_procmt_2021/procmt_2021

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/include/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/base/prc_com/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/base/msg_logger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/boost_student/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/qcustomplot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/xz2gmt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/fftreal_vector/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/math_vector/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/sql_vector/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/tinyxmlwriter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/tinyxml2/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/sql_base_tools/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/measdocxml/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/prc_com_xml/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/db_sensor_query/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/calibration/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/emerald_xtrx/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/calstat/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libgeo/geocoordinates/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libgeo/gmttools/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/eqdatetime/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/mest_plotlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/prc_com_table/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/static_plot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/gui_items/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/qstring_utilities/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/plotlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/xy_regression_plot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/xmlcallib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/cal_stat_display_lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/lineedit_file/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/gen_trf_filename/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/xmlcal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/mk_master_cal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/firfilter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/num_keypad/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/select_items_from_db_sensors/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/aduxml/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/adu_json/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/recording/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atmfile/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atsheader/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atsfile/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atsfilename/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/ats_ascii_file/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/mt_data/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atsfile_collector/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/acsp_prz/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/mc_data/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/edi_file/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/mt_site/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/emerald/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/procmt_lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/spc_db/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/csem_db/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/filter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atsedit/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/mt_gensign/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/procmt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atstool/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/tsdata2ats/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_adulib/group_atsfiles/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_adulib/logviewer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_adulib/measdoc_from_mc_data/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_adulib/phoenix_json/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/adu_demonstrator/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_adulib/ts_tools/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_gui/ats_multi_edit/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_gui/tsplotter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_gui/ediplotter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/cal2sql/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_gui/spectra_plotter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_gui/procmt_mini/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_adulib/calib_lab/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/CoilFieldTestTool/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/InSituCoilCal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/unsplit/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/tsmp/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
