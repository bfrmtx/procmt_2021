# Install script for directory: /home/bfr/devel/github_procmt_2021/procmt_2021/pmt_gui/ats_multi_edit

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/share/desktop/ats_multi_edit.desktop")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/share/desktop" TYPE FILE FILES "/home/bfr/devel/github_procmt_2021/procmt_2021/pmt_gui/ats_multi_edit/ats_multi_edit.desktop")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/share/desktop/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/share/desktop" TYPE DIRECTORY FILES "/home/bfr/devel/github_procmt_2021/procmt_2021/pmt_gui/ats_multi_edit/icons/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit"
         RPATH "\$ORIGIN/../lib:/usr/local/procmt/lib:\$ORIGIN")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin" TYPE EXECUTABLE FILES "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_gui/ats_multi_edit/ats_multi_edit")
  if(EXISTS "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit"
         OLD_RPATH "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/prc_com_table:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/ats_ascii_file:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libgeo/geocoordinates:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/qstring_utilities:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atsfile:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atmfile:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/fftreal_vector:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/firfilter:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/procmt/atsheader:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/base/prc_com:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/eqdatetime:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/calibration:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/measdocxml:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/tinyxml2:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/tinyxmlwriter:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/db_sensor_query:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/sql_base_tools:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/base/msg_logger:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/math_vector:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/sql_vector:"
         NEW_RPATH "\$ORIGIN/../lib:/usr/local/procmt/lib:\$ORIGIN")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip" "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/ats_multi_edit")
    endif()
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/pmt_gui/ats_multi_edit/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
