# Install script for directory: /home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml

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
  if(EXISTS "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so"
         RPATH "\$ORIGIN/../lib:/usr/local/procmt/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib" TYPE SHARED_LIBRARY FILES "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/aduxml/libaduxml.so")
  if(EXISTS "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so"
         OLD_RPATH "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/calibration:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/db_sensor_query:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/sql_base_tools:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/sql_vector:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/base/prc_com:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/base/msg_logger:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libqt/eqdatetime:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libgeo/geocoordinates:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/tinyxmlwriter:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/measdocxml:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libxml/tinyxml2:/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/libvector/math_vector:"
         NEW_RPATH "\$ORIGIN/../lib:/usr/local/procmt/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip" "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/lib/libaduxml.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/adulib.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/aduslot.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/channel.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/adbboard.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/cable.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/gps.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/sensor.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/adutable.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/adujoblist.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/stoptime_dialog.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/adunetwork.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/adurt.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/txm.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include" TYPE FILE FILES
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/adulib.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/aduslot.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/channel.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/adbboard.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/cable.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/gps.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/sensor.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/adutable.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/adujoblist.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/stoptime_dialog.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/adunetwork.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/adurt.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/adulib/aduxml/txm.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/adulib/aduxml/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
